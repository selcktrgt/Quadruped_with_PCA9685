/*
 * rs422_selector.h
 *
 * 2 girisli / 1 cikisli RS422 Kaynak Secici Birimi mantigi.
 * STM32F103 + STM32CubeIDE HAL icin.
 *
 * Gereksinim referanslari (Ek-A Teknik Gereksinimler):
 *  - Md.2 : Iki farkli kaynaktan NMEA0183 ve/veya binary veri alinabilir.
 *  - Md.3 : 4800..115200 bps, 1..100 Hz veri guncelleme destegi.
 *  - Md.4 : Kaynaklar arasi gecis suresi 100 ms'yi asmaz.
 *  - Md.5 : Kaynak Onceliklendirme Mekanizmasi (SRC1 > SRC2, oncelikli
 *           kaynak canlandiginda otomatik olarak geri donulur).
 *  - Md.6 : Secilen kaynak veriyi cikis hattina iletir (byte-seviyesinde
 *           saydam aktarim; protokolun icerigi yorumlanmaz).
 *  - Md.7 : Kaynaklar arasinda manuel gecis (bkz. RS422_SelMode_t).
 */

#ifndef RS422_SELECTOR_H
#define RS422_SELECTOR_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include <stdint.h>
#include <stdbool.h>

/* ------------------------------------------------------------------ */
/*  Yapilandirma (kullanicinin donanimina gore ayarlanmasi gerekenler) */
/* ------------------------------------------------------------------ */

/* Kaynak sessiz kalirsa yedek kaynaga gecis suresi (Md.4). */
#define RS422_SOURCE_TIMEOUT_MS      100U

/* Manuel secim anahtarinin ADC ile okunma periyodu. */
#define RS422_ADC_POLL_PERIOD_MS     20U

/* Cikis UART TX halka tampon boyutu (byte). 115200 bps'de ~87us/byte,
 * 100 Hz*27 byte'lik SMASH mesaji icin fazlasiyla yeterli. */
#define RS422_TX_RINGBUF_SIZE        256U

/* ADC 12-bit (0..4095, Vref=3.3V) esik degerleri. Manuel secim anahtari
 * 3 konumlu olacak sekilde bir gerilim bolucu ile ADC pinine baglanir:
 *   Konum SRC1  -> ~0.0 V  (GND'ye kisa devre)
 *   Konum AUTO  -> ~1.65 V (ortadan, direnc koprusu ile)
 *   Konum SRC2  -> ~3.3 V  (VCC'ye kisa devre)
 * Esikler arasindaki bolgeler "olu bolge" olarak kabul edilir; anahtar
 * gecis anindaki ara degerlerde bir onceki mod korunur (titresim/gurultu
 * bagisikligi icin). Kendi anahtar/direnc degerlerine gore kalibre et. */
#define RS422_ADC_TH_SRC1_MAX        1000U   /* 0        .. 1000 -> FORCE SRC1 */
#define RS422_ADC_TH_AUTO_MIN        1600U   /* 1600     .. 2500 -> AUTO       */
#define RS422_ADC_TH_AUTO_MAX        2500U
#define RS422_ADC_TH_SRC2_MIN        3100U   /* 3100     .. 4095 -> FORCE SRC2 */

/* ------------------------------------------------------------------ */

typedef enum {
    RS422_SRC_1 = 0,
    RS422_SRC_2 = 1,
    RS422_SRC_COUNT
} RS422_Source_t;

typedef enum {
    RS422_SEL_AUTO = 0,     /* Kaynak Onceliklendirme Mekanizmasi (otomatik) */
    RS422_SEL_FORCE_SRC1,   /* Kol/anahtar ile manuel SRC1 zorlama */
    RS422_SEL_FORCE_SRC2    /* Kol/anahtar ile manuel SRC2 zorlama */
} RS422_SelMode_t;

/* Baslatma: giris/cikis UART handle'lari ve secim anahtarinin okundugu
 * ADC handle'i verilir. UART'lar CubeMX'te su sekilde ayarlanmis olmali:
 *   huartSrc1 / huartSrc2 : Mode = Receive only  (RS422 giris hatlari)
 *   huartOut              : Mode = Transmit only (RS422 cikis hatti)
 * Baud rate / kelime formati (8N1, akis kontrolsuz) her uc UART'ta ayni
 * ve SDDS arayuz dokumanina uygun (varsayilan 115200) olmalidir. */
void RS422_Selector_Init(UART_HandleTypeDef *huartSrc1,
                          UART_HandleTypeDef *huartSrc2,
                          UART_HandleTypeDef *huartOut,
                          ADC_HandleTypeDef  *hadcSelect);

/* Ana dongude (while(1) icinde) surekli cagirilmalidir. ADC okuma,
 * mod/kaynak karari ve LED guncellemesi burada yapilir. Bloklamaz. */
void RS422_Selector_Process(void);

/* Su an aktif olarak cikisa aktarilan kaynak. */
RS422_Source_t RS422_GetActiveSource(void);

/* Su an gecerli olan secim modu (AUTO / FORCE_SRC1 / FORCE_SRC2). */
RS422_SelMode_t RS422_GetSelMode(void);

/* Bir kaynaktan en son ne zaman (HAL_GetTick tabanli) veri geldigini
 * verir; kaynak hic veri gondermemisse 0 doner. Tanilama/LED icin. */
uint32_t RS422_GetLastRxTick(RS422_Source_t src);

/* HAL callback'lerinden cagrilmasi gereken kancalar. main.c / stm32f1xx_it.c
 * uzerinden HAL_UART_RxCpltCallback, HAL_UART_TxCpltCallback ve
 * HAL_UART_ErrorCallback icinden bu modulun ilgili islemcilerine
 * yonlendirme main.c icinde USER CODE bolumunde yapilir (bkz main.c). */
void RS422_Selector_OnRxByte(UART_HandleTypeDef *huart);
void RS422_Selector_OnTxDone(UART_HandleTypeDef *huart);
void RS422_Selector_OnError(UART_HandleTypeDef *huart);

#ifdef __cplusplus
}
#endif

#endif /* RS422_SELECTOR_H */
