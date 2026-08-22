/* main.h
 *
 * RS422 Kaynak Secici Birimi - STM32F103C8T6 (ornek: "Blue Pill" karti).
 * Bu dosya, STM32CubeMX'in normalde uretecegi main.h iskeletinin elle
 * hazirlanmis halidir. Projeyi CubeMX/CubeIDE'de asagidaki periferik
 * ayarlariyla olusturduktan sonra bu dosyayi (ve main.c, rs422_selector.*
 * dosyalarini) projene ekleyebilirsin; CubeMX yeniden kod uretse bile
 * USER CODE blok isimleri korunmustur.
 *
 * Varsayilan pin haritasi (ihtiyaca gore CubeMX'te degistirilebilir,
 * uygulama mantigi (rs422_selector.c) pin'e degil handle'a bagli calisir):
 *
 *   USART1 (Kaynak 1 girisi, RS422 alici)   : PA10 = RX  (Mode: Receive only)
 *   USART2 (Kaynak 2 girisi, RS422 alici)   : PA3  = RX  (Mode: Receive only)
 *   USART3 (Cikis, RS422 surucu)            : PB10 = TX  (Mode: Transmit only)
 *   ADC1_IN0 (Manuel secim anahtari/kolu)   : PA0
 *   PC13                                     : SRC1_ACTIVE_LED (opsiyonel)
 *   PB1                                      : SRC2_ACTIVE_LED (opsiyonel)
 *
 * Guc gostergesi (Md.10) icin ayrica devre uzerinde, MCU'dan bagimsiz,
 * dogrudan besleme hattina bagli bir LED kullanilmasi onerilir (bkz.
 * README). Yukaridaki SRC1/SRC2 LED'leri ise hangi kaynagin aktif
 * oldugunu gostermek icin tanilama amaclidir.
 */

#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f1xx_hal.h"

void Error_Handler(void);

/* Opsiyonel tanilama LED'leri. Kullanmiyorsan CubeMX'te bu pinleri
 * atama, asagidaki define'lari da main.c'den kaldir. */
#define SRC1_ACTIVE_LED_Pin        GPIO_PIN_13
#define SRC1_ACTIVE_LED_GPIO_Port  GPIOC
#define SRC2_ACTIVE_LED_Pin        GPIO_PIN_1
#define SRC2_ACTIVE_LED_GPIO_Port  GPIOB

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
