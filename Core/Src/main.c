/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "pwm_controller.h"
#include "command.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include "PCA9685.h"
#include "ik.h"


/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */



#define MOTOR_SPEED   0x01
#define MOTOR_POSITION  0x02
#define START_CODE_BYTE   0xFF

#define SERVO_CHANNEL_0   0
#define SERVO_CHANNEL_1   1
#define SERVO_CHANNEL_2   2
#define SERVO_CHANNEL_3   3
#define SERVO_CHANNEL_4   4
#define SERVO_CHANNEL_5   5


#define RX_BUFF_SIZE  16

#define command_length  2

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
CAN_HandleTypeDef hcan1;

I2C_HandleTypeDef hi2c1;

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;

/* USER CODE BEGIN PV */






volatile uint8_t motor_angle[4];

char msg[192];

uint8_t commands[command_length];



uint8_t rx_buffer[RX_BUFF_SIZE];

volatile uint16_t rx_head = 0;
volatile uint16_t rx_tail = 0;
volatile uint8_t rx_index=0;
volatile int cmd_ready = 0;
int cmd;
int sayac=0;
int prev_sayac=0;

uint8_t command_ready = 1;
uint8_t last_cmd;


float shoulder, hip, knee;
float L1 = 120.0f; // uyluk uzunluğu (mm)
float L2 = 120.0f; // kaval uzunluğu (mm)

float q_shoulder;
float q_hip;
float q_knee;


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM1_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_TIM2_Init(void);
static void MX_CAN1_Init(void);
static void MX_TIM3_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_USART3_UART_Init(void);
/* USER CODE BEGIN PFP */

#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
void parse_angles(char *data);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

//int __io_putchar(int ch)
//{
//    HAL_UART_Transmit(&huart1, (uint8_t*)&ch, 1, HAL_MAX_DELAY);
//    return ch;
//}




/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM1_Init();
  MX_I2C1_Init();
  MX_USART1_UART_Init();
  MX_TIM2_Init();
  MX_CAN1_Init();
  MX_TIM3_Init();
  MX_USART2_UART_Init();
  MX_USART3_UART_Init();
  /* USER CODE BEGIN 2 */

//  HAL_TIM_PWM_Init(&htim1);
//  PCA9685_Init(50); // 50Hz for servo
//  HAL_Delay(50);
//  HAL_TIM_Base_Start_IT(&htim2);
//
  USART1->CR1 |= USART_CR1_RXNEIE; // RXNE interrupt enable
  NVIC_EnableIRQ(USART1_IRQn);     // NVIC'te USART1 enable


//  PCA9685_SetServoAngle(5, 0);
//  PCA9685_SetServoAngle(10, 0);
//  PCA9685_SetServoAngle(13, 0);
//  PCA9685_SetServoAngle(2, 0);
//  HAL_Delay(1500);

//  HAL_UART_Receive_IT(&huart1, &rx_buffer[rx_index], 1);

//  process();
//    PCA9685_SetServoAngle(0, 60); // 0.83 ms ccr=42
//    PCA9685_SetServoAngle(1, 120); //1.66 ms ccr = 83
//    HAL_Delay(2000);
//    PCA9685_SetServoAngle(0, 90);
//    PCA9685_SetServoAngle(1, 90); // 1.25 ms ccr = 63
//    HAL_Delay(2000);



  //  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
  //  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
  //  HAL_Delay(20);
  //  __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_1,42);
  //  __HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_1,83);
  //  HAL_Delay(2000);
   // __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_1,22);
   // __HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_1,63);





//  uint8_t eraseData[4];
//  eraseData[0] = 0x55;
//  eraseData[1] = 0x55;
//  eraseData[2] = 0x02;   // Length
//  eraseData[3] = 0x05;   // CMD: Chip Erase
//  HAL_UART_Transmit(&huart1, eraseData, 4, 100);


// out of action mode
//  Servo_SendPacket(0x0B, 0x00, 1);  // Stop
//  HAL_Delay(50);
//  Servo_SendPacket(0x0B, 0x00, 0);  // Resume
//  HAL_Delay(50);





  Servo_Init();  // açılışta her şeyi nötrle

  HAL_Delay(100);
  sprintf(msg,"DRIVE THE ROBOT => press 1 for forward , press 2 to turn right , press 3 to turn left, press 4 for laydown, press 5 for get up.\r\n ");
  HAL_UART_Transmit(&huart1, (uint8_t *)msg, strlen(msg), HAL_MAX_DELAY);




   //Move_Servos(SERVO_CHANNEL_0, 5);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
   // StepForward();


    if((cmd_ready!=0) && (command_ready == 1)){


      if((last_cmd ==4) && (cmd_ready !=4))
      {
        get_up();

      }


      ExecuteCommand(cmd);

    }






}








}

  /* USER CODE END 3 */


/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 192;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief CAN1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_CAN1_Init(void)
{

  /* USER CODE BEGIN CAN1_Init 0 */

  /* USER CODE END CAN1_Init 0 */

  /* USER CODE BEGIN CAN1_Init 1 */

  /* USER CODE END CAN1_Init 1 */
  hcan1.Instance = CAN1;
  hcan1.Init.Prescaler = 16;
  hcan1.Init.Mode = CAN_MODE_NORMAL;
  hcan1.Init.SyncJumpWidth = CAN_SJW_1TQ;
  hcan1.Init.TimeSeg1 = CAN_BS1_1TQ;
  hcan1.Init.TimeSeg2 = CAN_BS2_1TQ;
  hcan1.Init.TimeTriggeredMode = DISABLE;
  hcan1.Init.AutoBusOff = DISABLE;
  hcan1.Init.AutoWakeUp = DISABLE;
  hcan1.Init.AutoRetransmission = DISABLE;
  hcan1.Init.ReceiveFifoLocked = DISABLE;
  hcan1.Init.TransmitFifoPriority = DISABLE;
  if (HAL_CAN_Init(&hcan1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CAN1_Init 2 */

  /* USER CODE END CAN1_Init 2 */

}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 960-1;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 1000-1;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_PWM_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTimeConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */
  HAL_TIM_MspPostInit(&htim1);

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 48000-1;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 2-1;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 960-1;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 1000-1;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */
  HAL_TIM_MspPostInit(&htim3);

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 9600;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 9600;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */





void USART1_IRQHandler(void)
{
    if (USART1->SR & USART_SR_RXNE)
    {
        char data = USART1->DR;  // gelen byte

        if (isdigit((unsigned char)data) || (data == '-') || (data == ','))
        {
            uint16_t next_head = (rx_head + 1) % RX_BUFF_SIZE;
            if (next_head != rx_tail) {
                rx_buffer[rx_head] = data;
                rx_head = next_head;
            }
        }
        else if (data == '\r' || data == '\n')
        {
            // string sonlandır
            rx_buffer[rx_head] = '\0';
            parse_angles((char *)rx_buffer);
            rx_head = 0;
        }
        else{

          sprintf(msg,"\r\ninvalid key.\r\n");
          HAL_UART_Transmit(&huart1, (uint8_t *)msg, strlen(msg), HAL_MAX_DELAY);
        }


    }
}

void parse_angles(char *data)
{


    char *token = strtok(data, ",");
    int i = 0;

    while (token != NULL && i < (sizeof(commands)/sizeof(commands[0])))
    {
        commands[i] = atof(token);
        token = strtok(NULL, ",");
        i++;
    }

    cmd = atoi(data);  // string'i int'e çevir
    cmd_ready = cmd;       // sadece flag ata





}










//void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
//{
//
//  for(int i=0 ; i < 3; i++){
//
//    if(i==0){
//
//      motor_angle[i] += 1;
//    }
//
//    if(i==1){
//
//          motor_angle[i] += 2;
//        }
//
//    if(i==2){
//
//          motor_angle[i] += 0.5;
//        }
//
//    if(motor_angle[2]>160)
//    {
//      if(i==2){
//
//            motor_angle[i] -= 0.5;
//       }
//
//    }
//
//    if((motor_angle[2]>=0) && (motor_angle[2]<160))
//        {
//          if(i==2){
//
//                motor_angle[i] += 0.5;
//
//           }
//        }
//
//  }
//
//  for(int i=0;i<3;i++){
//
//    if(motor_angle[i] <= 0){
//
//      motor_angle[i]=0;
//    }
//
//    if((0 < motor_angle[i]) && (motor_angle[i] < 180)){
//
//      motor_dir[i]=0;
//    }
//
//    if(motor_angle[i] >= 180){
//
//      motor_angle[i]=180;
//
//    }
//
//  }
//
//
//
//
//  for(int i=0;i<3;i++){
//    if(i==0){
//      motor_angle[i]-=10;
//      PCA9685_SetServoAngle(i, motor_angle[i]);
//
//    }
//    if(i==1){
//      motor_angle[i]-=20;
//      PCA9685_SetServoAngle(i, motor_angle[i]);
//
//        }
//
//    if(i==2){
//      motor_angle[i]-=25;
//      PCA9685_SetServoAngle(i, motor_angle[i]);
//
//    }
//
//  }
//}

//void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
//{
//
//  if(huart->Instance == USART1)
//     {
//         if(rx_buffer[rx_index] == '\r') // Satır sonu gelirse
//         {
//             rx_buffer[rx_index] = '\0'; // String sonu koy
//             parse_angles((char*)rx_buffer);
//             rx_index = 0;
//         }
//         else
//         {
//             rx_index++;
//         }
//
//         // Tekrar interrupt başlat
//         HAL_UART_Receive_IT(&huart1, &rx_buffer[rx_index], 1);
//     }
//
//
//
//}


//void USART1_IRQHandler(void)
//{
//
//
//    if (USART1->SR & USART_SR_RXNE)
//    {
//        char data = USART1->DR;
//
//
//        uint16_t next_head = (rx_head + 1) % RX_BUF_SIZE;
//
//
//
//        if (next_head != rx_tail)
//        {
//
//            if (isdigit((unsigned char)data) || data == '.' || data == '-' || data == ',' || data == '\n' || data == '\r')
//            {
//                  rx_buffer[rx_head] = data;
//                  rx_head = next_head;
//            }
//            if((rx_buffer[rx_head-1]=='\n') || (rx_buffer[rx_head-1]=='\r'))
//            {
//                  rx_buffer[rx_head-1]='\0';
//                  parse_angles((char *)rx_buffer);
//                  rx_head = 0;
//
//
//
//            }
//
//
//        }
//    }
//}






// Bir satır veri oku (Enter gelene kadar)
//int uart_read_line(char *dest, int max_len)
//{
//    int count = 0;
//
//    while (rx_tail != rx_head && count < max_len - 1)
//    {
//        char c = rx_buffer[rx_tail];
//        rx_tail = (rx_tail + 1) % RX_BUF_SIZE;
//
//        if (c == '\n' || c == '\r')
//        {
//
//            if (count > 0) // boş satır değilse
//            {
//                dest[count] = '\0';
//
//                return count;
//            }
//            else
//            {
//                return 0;
//            }
//        }
//        dest[count++] = c;
//    }
//    return 0;
//}



//void parse_angles(char *data)
//{
//   sayac++;
//
//
//   char *token = strtok(data, ",");
//   int i = 0;
//
//   if (prev_sayac != sayac){
//
//     for (int i = 0; i < sizeof(motor_angle); i++){
//
//         motor_angle[i]=0;
//     }
//
//
//   }
//
//   while (token != NULL && i < 4)
//   {
//        motor_angle[i] = atof(token);
//        token = strtok(NULL, ",");
//        i++;
//
//   }
//
//   prev_sayac = sayac;
//
//   for (int i = 0; i < sizeof(rx_buffer); i++)
//   {
//
//         rx_buffer[i]=0;
//   }
//
//}



/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
