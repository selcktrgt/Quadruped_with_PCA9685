/*
 * rs422_selector.c
 *
 * 2 girisli / 1 cikisli RS422 Kaynak Secici Birimi - uygulama mantigi.
 * Tasarim notlari:
 *
 *  - RS422 tam dupleks noktadan-noktaya bir hat oldugundan (RS485'in
 *    aksine) surucu/alici yonu yazilimla degistirilmez; DE/RE pin
 *    kontrolune gerek yoktur. Giris UART'lari "Receive only", cikis
 *    UART'i "Transmit only" olarak calisir.
 *  - Veri, protokol icerigi (NMEA0183 / SMASH binary) yorumlanmadan
 *    byte-seviyesinde saydam olarak aktarilir (Md.2, Md.6). Boylece
 *    hem ASCII NMEA cumleleri hem de 100 Hz / 1 Hz binary SMASH
 *    mesajlari ayni mekanizma ile tasinabilir.
 *  - Her giris UART'i kesme (interrupt) tabanli, byte byte alinir.
 *    Alinan her byte icin ilgili kaynagin "son veri zamani" guncellenir.
 *    Sadece o an aktif olan kaynagin byte'lari cikis halka tamponuna
 *    yazilip UART kesmesiyle gonderilir.
 *  - RS422_Selector_Process(), ana donguden periyodik cagrilarak:
 *      1) Manuel secim anahtarini ADC'den okur (donebble/hysteresis ile),
 *      2) AUTO modda Kaynak Onceliklendirme Mekanizmasini uygular:
 *         SRC1 canliysa (son 100 ms icinde veri geldiyse) SRC1 secilir;
 *         degilse ve SRC2 canliysa SRC2'ye gecilir (Md.4, Md.5);
 *         SRC1 tekrar canlanirsa oncelikli oldugu icin geri donulur.
 */

#include "rs422_selector.h"

/* ------------------------------------------------------------------ */

typedef struct {
    UART_HandleTypeDef *huart;
    volatile uint32_t    lastRxTick;
    uint8_t               rxByte;      /* IT tek-byte alim tamponu */
} RS422_InputCh_t;

static RS422_InputCh_t s_in[RS422_SRC_COUNT];
static UART_HandleTypeDef *s_huartOut;
static ADC_HandleTypeDef  *s_hadcSelect;

static volatile RS422_Source_t  s_activeSource = RS422_SRC_1;
static volatile RS422_SelMode_t s_selMode      = RS422_SEL_AUTO;

/* Cikis TX halka tamponu */
static uint8_t  s_txBuf[RS422_TX_RINGBUF_SIZE];
static volatile uint16_t s_txHead = 0;   /* yazma ucu */
static volatile uint16_t s_txTail = 0;   /* okuma ucu */
static volatile uint8_t  s_txBusy = 0;

static uint32_t s_lastAdcPollTick = 0;

/* ------------------------------------------------------------------ */
/* Yardimci: TX halka tamponuna byte ekle, gerekiyorsa gonderimi baslat */
static void RS422_TxEnqueue(uint8_t b)
{
    uint16_t next = (uint16_t)((s_txHead + 1U) % RS422_TX_RINGBUF_SIZE);

    if (next == s_txTail) {
        /* Tampon dolu: cikis hattinin girisi yetistiremedigi anlamina
         * gelir (ornegin baud rate uyumsuzlugu). Baytin dusurulmesi,
         * tum sistemin kilitlenmesinden tercih edilir. */
        return;
    }

    s_txBuf[s_txHead] = b;
    s_txHead = next;

    if (!s_txBusy) {
        s_txBusy = 1;
        uint8_t out = s_txBuf[s_txTail];
        s_txTail = (uint16_t)((s_txTail + 1U) % RS422_TX_RINGBUF_SIZE);
        HAL_UART_Transmit_IT(s_huartOut, &out, 1);
    }
}

static bool SourceAlive(RS422_Source_t s)
{
    if (s_in[s].lastRxTick == 0U) {
        return false; /* hic veri gelmedi */
    }
    return (HAL_GetTick() - s_in[s].lastRxTick) < RS422_SOURCE_TIMEOUT_MS;
}

/* ------------------------------------------------------------------ */

void RS422_Selector_Init(UART_HandleTypeDef *huartSrc1,
                          UART_HandleTypeDef *huartSrc2,
                          UART_HandleTypeDef *huartOut,
                          ADC_HandleTypeDef  *hadcSelect)
{
    s_in[RS422_SRC_1].huart      = huartSrc1;
    s_in[RS422_SRC_1].lastRxTick = 0;
    s_in[RS422_SRC_2].huart      = huartSrc2;
    s_in[RS422_SRC_2].lastRxTick = 0;

    s_huartOut    = huartOut;
    s_hadcSelect  = hadcSelect;

    s_activeSource = RS422_SRC_1;
    s_selMode      = RS422_SEL_AUTO;
    s_txHead = s_txTail = 0;
    s_txBusy = 0;

    /* Her iki giris hattinda da kesmeli tek-byte alimi baslat. */
    HAL_UART_Receive_IT(s_in[RS422_SRC_1].huart, &s_in[RS422_SRC_1].rxByte, 1);
    HAL_UART_Receive_IT(s_in[RS422_SRC_2].huart, &s_in[RS422_SRC_2].rxByte, 1);

    s_lastAdcPollTick = HAL_GetTick();
}

/* ------------------------------------------------------------------ */
/* Kesme baglamindan cagrilir (HAL_UART_RxCpltCallback icinden) */
void RS422_Selector_OnRxByte(UART_HandleTypeDef *huart)
{
    for (uint32_t i = 0; i < RS422_SRC_COUNT; i++) {
        if (s_in[i].huart->Instance == huart->Instance) {
            s_in[i].lastRxTick = HAL_GetTick();

            if ((RS422_Source_t)i == s_activeSource) {
                RS422_TxEnqueue(s_in[i].rxByte);
            }

            /* Sonraki byte icin tekrar kolla (IT alimi otomatik
             * tekrarlanmaz, her seferinde yeniden basvurulmali). */
            HAL_UART_Receive_IT(s_in[i].huart, &s_in[i].rxByte, 1);
            return;
        }
    }
}

/* Kesme baglamindan cagrilir (HAL_UART_TxCpltCallback icinden) */
void RS422_Selector_OnTxDone(UART_HandleTypeDef *huart)
{
    if (huart->Instance != s_huartOut->Instance) {
        return;
    }

    if (s_txTail == s_txHead) {
        s_txBusy = 0; /* tampon bos, gonderim durdu */
        return;
    }

    uint8_t out = s_txBuf[s_txTail];
    s_txTail = (uint16_t)((s_txTail + 1U) % RS422_TX_RINGBUF_SIZE);
    HAL_UART_Transmit_IT(s_huartOut, &out, 1);
}

/* Kesme baglamindan cagrilir (HAL_UART_ErrorCallback icinden).
 * Hat gurultusu / framing / overrun hatalarinda alimin kilitlenmemesi
 * icin ilgili giris kanali yeniden kollanir. */
void RS422_Selector_OnError(UART_HandleTypeDef *huart)
{
    for (uint32_t i = 0; i < RS422_SRC_COUNT; i++) {
        if (s_in[i].huart->Instance == huart->Instance) {
            __HAL_UART_CLEAR_PEFLAG(s_in[i].huart);
            HAL_UART_Receive_IT(s_in[i].huart, &s_in[i].rxByte, 1);
            return;
        }
    }

    if (huart->Instance == s_huartOut->Instance) {
        __HAL_UART_CLEAR_PEFLAG(s_huartOut);
    }
}

/* ------------------------------------------------------------------ */
/* Manuel secim anahtarini ADC'den okuyup moda cevirir (histerezisli). */
static void UpdateSelModeFromAdc(void)
{
    HAL_ADC_Start(s_hadcSelect);
    if (HAL_ADC_PollForConversion(s_hadcSelect, 5) != HAL_OK) {
        HAL_ADC_Stop(s_hadcSelect);
        return; /* olcum basarisiz: mevcut modu koru */
    }
    uint32_t adcVal = HAL_ADC_GetValue(s_hadcSelect);
    HAL_ADC_Stop(s_hadcSelect);

    if (adcVal <= RS422_ADC_TH_SRC1_MAX) {
        s_selMode = RS422_SEL_FORCE_SRC1;
    } else if (adcVal >= RS422_ADC_TH_AUTO_MIN && adcVal <= RS422_ADC_TH_AUTO_MAX) {
        s_selMode = RS422_SEL_AUTO;
    } else if (adcVal >= RS422_ADC_TH_SRC2_MIN) {
        s_selMode = RS422_SEL_FORCE_SRC2;
    }
    /* Esikler arasindaki "olu bolge" (anahtarin gecis ani / gurultu):
     * bir onceki secili mod bilerek korunur. */
}

/* AUTO moddaki Kaynak Onceliklendirme Mekanizmasi: SRC1 birincil
 * oncelikli kaynaktir; canli oldugu surece secilir. SRC1 100 ms'den
 * uzun sure sessiz kalirsa SRC2'ye gecilir; SRC1 tekrar veri gondermeye
 * basladigi an oncelik geregi tekrar SRC1'e donulur. */
static RS422_Source_t ResolveAutoSource(void)
{
    if (SourceAlive(RS422_SRC_1)) {
        return RS422_SRC_1;
    }
    if (SourceAlive(RS422_SRC_2)) {
        return RS422_SRC_2;
    }
    /* Iki kaynak da sessiz: son aktif kaynakta kal (veri gelince
     * dogal olarak tekrar canlanacaktir). */
    return s_activeSource;
}

void RS422_Selector_Process(void)
{
    uint32_t now = HAL_GetTick();

    if ((now - s_lastAdcPollTick) >= RS422_ADC_POLL_PERIOD_MS) {
        s_lastAdcPollTick = now;
        UpdateSelModeFromAdc();
    }

    RS422_Source_t desired;
    switch (s_selMode) {
        case RS422_SEL_FORCE_SRC1: desired = RS422_SRC_1; break;
        case RS422_SEL_FORCE_SRC2: desired = RS422_SRC_2; break;
        case RS422_SEL_AUTO:
        default:                   desired = ResolveAutoSource(); break;
    }

    if (desired != s_activeSource) {
        s_activeSource = desired;

        /* Kaynak degisiminde gosterge LED'leri (opsiyonel donanim,
         * mevcut degilse bu satirlar CubeMX'te pin tanimlanmadan da
         * derlenir; kullanmiyorsan main.h'den GPIO tanimlarini kaldir). */
#if defined(SRC1_ACTIVE_LED_GPIO_Port) && defined(SRC2_ACTIVE_LED_GPIO_Port)
        HAL_GPIO_WritePin(SRC1_ACTIVE_LED_GPIO_Port, SRC1_ACTIVE_LED_Pin,
                           (s_activeSource == RS422_SRC_1) ? GPIO_PIN_SET : GPIO_PIN_RESET);
        HAL_GPIO_WritePin(SRC2_ACTIVE_LED_GPIO_Port, SRC2_ACTIVE_LED_Pin,
                           (s_activeSource == RS422_SRC_2) ? GPIO_PIN_SET : GPIO_PIN_RESET);
#endif
    }
}

RS422_Source_t RS422_GetActiveSource(void)
{
    return s_activeSource;
}

RS422_SelMode_t RS422_GetSelMode(void)
{
    return s_selMode;
}

uint32_t RS422_GetLastRxTick(RS422_Source_t src)
{
    if (src >= RS422_SRC_COUNT) {
        return 0;
    }
    return s_in[src].lastRxTick;
}
