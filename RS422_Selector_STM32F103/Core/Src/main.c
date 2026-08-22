/* main.c
 *
 * RS422 Kaynak Secici Birimi (2 giris / 1 cikis) - STM32F103C8T6
 *
 * Bu dosya STM32CubeIDE/CubeMX "USER CODE" blok kurallarina uyacak
 * sekilde yazilmistir: projeyi CubeMX'te asagida anlatilan periferik
 * ayarlariyla olusturup kod uretimi yaptiktan sonra, buradaki
 * USER CODE icerikleri uretilen main.c'ye kopyalanabilir ve CubeMX
 * tekrar kod uretse bile korunur.
 *
 * CubeMX periferik ayarlari (bkz. RS422_Selector_STM32F103/README.md):
 *   USART1 : Asenkron, Mode = Receive only,  RX = PA10           (KAYNAK 1)
 *   USART2 : Asenkron, Mode = Receive only,  RX = PA3            (KAYNAK 2)
 *   USART3 : Asenkron, Mode = Transmit only, TX = PB10           (CIKIS)
 *   ADC1   : IN0 = PA0, tek kanal, yazilim tetiklemeli tek donusum
 *   GPIO   : PC13 / PB1 cikis (opsiyonel SRC1/SRC2 aktif LED'leri)
 *
 * Butun UART'lar ayni baud rate / 8N1 / akis kontrolsuz olarak
 * ayarlanmalidir (SDDS-SMASH arayuz dokumanina gore varsayilan
 * 115200 bps; gereksinim Md.3 4800..115200 araligini destekler,
 * ihtiyaca gore RS422_UART_BAUDRATE degistirilebilir).
 */

#include "main.h"
#include "rs422_selector.h"

/* ------------------------------------------------------------------ */
#define RS422_UART_BAUDRATE   115200U

UART_HandleTypeDef huart1;   /* Kaynak 1 (RX only) */
UART_HandleTypeDef huart2;   /* Kaynak 2 (RX only) */
UART_HandleTypeDef huart3;   /* Cikis    (TX only) */
ADC_HandleTypeDef  hadc1;    /* Manuel secim anahtari */

static void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_ADC1_Init(void);

int main(void)
{
    HAL_Init();
    HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);
    SystemClock_Config();

    MX_GPIO_Init();
    MX_USART1_UART_Init();
    MX_USART2_UART_Init();
    MX_USART3_UART_Init();
    MX_ADC1_Init();

    /* USER CODE BEGIN 2 */
    RS422_Selector_Init(&huart1, &huart2, &huart3, &hadc1);
    /* USER CODE END 2 */

    while (1) {
        /* USER CODE BEGIN WHILE */
        RS422_Selector_Process();
        /* USER CODE END WHILE */
    }
}

/* ------------------------------------------------------------------ */
/* HAL UART / ADC callback'leri: rs422_selector modulune yonlendirilir. */

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    RS422_Selector_OnRxByte(huart);
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    RS422_Selector_OnTxDone(huart);
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    RS422_Selector_OnError(huart);
}

/* ------------------------------------------------------------------ */

static void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /* 8 MHz HSE (kristal) -> PLL x9 -> 72 MHz SYSCLK.
     * Farkli bir kart/kristal kullaniyorsan bu blogu CubeMX'in ureteceyi
     * degerlerle degistir. */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState       = RCC_HSE_ON;
    RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
    RCC_OscInitStruct.PLL.PLLState   = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource  = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLMUL     = RCC_PLL_MUL9;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        Error_Handler();
    }

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK   | RCC_CLOCKTYPE_SYSCLK
                                 | RCC_CLOCKTYPE_PCLK1  | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider  = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;  /* APB1 max 36 MHz */
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;  /* APB2 max 72 MHz */
    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) {
        Error_Handler();
    }

    /* ADC saati PCLK2(72MHz)/6 = 12 MHz (F1 ADC azami 14 MHz). */
    RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
    PeriphClkInit.AdcClockSelection    = RCC_ADCPCLK2_DIV6;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK) {
        Error_Handler();
    }
}

static void MX_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_AFIO_CLK_ENABLE();

    /* Opsiyonel tanilama LED'leri (SRC1/SRC2 aktif gostergesi). */
    HAL_GPIO_WritePin(SRC1_ACTIVE_LED_GPIO_Port, SRC1_ACTIVE_LED_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(SRC2_ACTIVE_LED_GPIO_Port, SRC2_ACTIVE_LED_Pin, GPIO_PIN_RESET);

    GPIO_InitStruct.Pin   = SRC1_ACTIVE_LED_Pin;
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(SRC1_ACTIVE_LED_GPIO_Port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = SRC2_ACTIVE_LED_Pin;
    HAL_GPIO_Init(SRC2_ACTIVE_LED_GPIO_Port, &GPIO_InitStruct);

    /* PA0 = ADC1_IN0, manuel secim anahtari/kolu. */
    GPIO_InitStruct.Pin  = GPIO_PIN_0;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

static void MX_USART1_UART_Init(void)
{
    /* Kaynak 1 RS422 girisi: sadece RX (PA10). */
    huart1.Instance          = USART1;
    huart1.Init.BaudRate     = RS422_UART_BAUDRATE;
    huart1.Init.WordLength   = UART_WORDLENGTH_8B;
    huart1.Init.StopBits     = UART_STOPBITS_1;
    huart1.Init.Parity       = UART_PARITY_NONE;
    huart1.Init.Mode         = UART_MODE_RX;
    huart1.Init.HwFlowCtl    = UART_HWCONTROL_NONE;
    huart1.Init.OverSampling = UART_OVERSAMPLING_16;
    if (HAL_UART_Init(&huart1) != HAL_OK) {
        Error_Handler();
    }
}

static void MX_USART2_UART_Init(void)
{
    /* Kaynak 2 RS422 girisi: sadece RX (PA3). */
    huart2.Instance          = USART2;
    huart2.Init.BaudRate     = RS422_UART_BAUDRATE;
    huart2.Init.WordLength   = UART_WORDLENGTH_8B;
    huart2.Init.StopBits     = UART_STOPBITS_1;
    huart2.Init.Parity       = UART_PARITY_NONE;
    huart2.Init.Mode         = UART_MODE_RX;
    huart2.Init.HwFlowCtl    = UART_HWCONTROL_NONE;
    huart2.Init.OverSampling = UART_OVERSAMPLING_16;
    if (HAL_UART_Init(&huart2) != HAL_OK) {
        Error_Handler();
    }
}

static void MX_USART3_UART_Init(void)
{
    /* Secilen kaynagin aktarildigi RS422 cikis hatti: sadece TX (PB10). */
    huart3.Instance          = USART3;
    huart3.Init.BaudRate     = RS422_UART_BAUDRATE;
    huart3.Init.WordLength   = UART_WORDLENGTH_8B;
    huart3.Init.StopBits     = UART_STOPBITS_1;
    huart3.Init.Parity       = UART_PARITY_NONE;
    huart3.Init.Mode         = UART_MODE_TX;
    huart3.Init.HwFlowCtl    = UART_HWCONTROL_NONE;
    huart3.Init.OverSampling = UART_OVERSAMPLING_16;
    if (HAL_UART_Init(&huart3) != HAL_OK) {
        Error_Handler();
    }
}

static void MX_ADC1_Init(void)
{
    ADC_ChannelConfTypeDef sConfig = {0};

    hadc1.Instance                   = ADC1;
    hadc1.Init.ScanConvMode          = ADC_SCAN_DISABLE;
    hadc1.Init.ContinuousConvMode    = DISABLE;
    hadc1.Init.DiscontinuousConvMode = DISABLE;
    hadc1.Init.ExternalTrigConv      = ADC_SOFTWARE_START;
    hadc1.Init.DataAlign             = ADC_DATAALIGN_RIGHT;
    hadc1.Init.NbrOfConversion       = 1;
    if (HAL_ADC_Init(&hadc1) != HAL_OK) {
        Error_Handler();
    }

    sConfig.Channel      = ADC_CHANNEL_0;   /* PA0 */
    sConfig.Rank         = ADC_REGULAR_RANK_1;
    sConfig.SamplingTime = ADC_SAMPLETIME_239CYCLES_5;
    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
        Error_Handler();
    }

    /* Secim anahtari yavas degisen bir sinyal oldugundan otokalibrasyon
     * ihmal edilebilir; hassasiyet gerekiyorsa HAL_ADCEx_Calibration_Start
     * baslangicta bir kez cagrilabilir. */
}

void Error_Handler(void)
{
    __disable_irq();
    while (1) {
        /* Kalici hata: guc/gosterge LED'i sonduruluyor gibi dusunulebilir;
         * gercek donanimda ayri bir "HATA" LED'i eklenmesi onerilir. */
    }
}

#ifdef  USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line)
{
    (void)file;
    (void)line;
}
#endif
