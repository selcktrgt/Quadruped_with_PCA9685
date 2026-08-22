/* stm32f1xx_it.c
 *
 * Kesme (IRQ) isleyicileri. Her biri ilgili HAL_UART_IRQHandler()'i
 * cagirir; asil is mantigi main.c icindeki HAL_UART_Rx/TxCplt/Error
 * callback'lerinde ve dolayisiyla rs422_selector.c icinde yapilir.
 */

#include "main.h"

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart3;

void NMI_Handler(void)
{
    while (1) { }
}

void HardFault_Handler(void)
{
    while (1) { }
}

void MemManage_Handler(void)
{
    while (1) { }
}

void BusFault_Handler(void)
{
    while (1) { }
}

void UsageFault_Handler(void)
{
    while (1) { }
}

void SVC_Handler(void)
{
}

void DebugMon_Handler(void)
{
}

void PendSV_Handler(void)
{
}

void SysTick_Handler(void)
{
    HAL_IncTick();
}

/* USART1: KAYNAK 1 girisi */
void USART1_IRQHandler(void)
{
    HAL_UART_IRQHandler(&huart1);
}

/* USART2: KAYNAK 2 girisi */
void USART2_IRQHandler(void)
{
    HAL_UART_IRQHandler(&huart2);
}

/* USART3: CIKIS hatti */
void USART3_IRQHandler(void)
{
    HAL_UART_IRQHandler(&huart3);
}
