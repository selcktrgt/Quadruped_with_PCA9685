/*
 * pwm_controller.c
 *
 *  Created on: Aug 15, 2025
 *      Author: SELCUK
 */

#include "main.h"
#include "pwm_controller.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>

uint8_t data_buffer[5];
extern char msg[192];

extern uint8_t command_ready;


extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart3;

//PINS OF LEGS
#define BACK_RIGHT_HIP   0
#define BACK_RIGHT_KNEE  1
#define BACK_LEFT_HIP  2
#define BACK_LEFT_KNEE 3
#define FRONT_RIGHT_HIP    4
#define FRONT_RIGHT_KNEE   5
#define FRONT_LEFT_HIP   6
#define FRONT_LEFT_KNEE  7
#define BACK_LEFT_SHOULDER  8
#define BACK_RIGHT_SHOULDER  9
#define FRONT_LEFT_SHOULDER  10
#define FRONT_RIGHT_SHOULDER  11



#define FRONT_LEFT_KNEE_LIFT  105
#define BACK_RIGHT_KNEE_LIFT  75
#define FRONT_LEFT_HIP_FORWARD 95
#define BACK_RIGHT_HIP_FORWARD 70
#define FRONT_LEFT_KNEE_PULL  140
#define BACK_RIGHT_KNEE_PULL 40
#define FRONT_LEFT_HIP_NEUTRAL 105
#define BACK_RIGHT_HIP_NEUTRAL 46
#define FRONT_LEFT_KNEE_NEUTRAL  120
#define BACK_RIGHT_KNEE_NEUTRAL  60


#define FRONT_RIGHT_KNEE_LIFT 75
#define BACK_LEFT_KNEE_LIFT 105
#define FRONT_RIGHT_HIP_FORWARD 80
#define BACK_LEFT_HIP_FORWARD 110
#define FRONT_RIGHT_KNEE_PULL 40
#define BACK_LEFT_KNEE_PULL 140

#define FRONT_RIGHT_HIP_NEUTRAL 70
#define BACK_LEFT_HIP_NEUTRAL 130
#define FRONT_RIGHT_KNEE_NEUTRAL 60
#define BACK_LEFT_KNEE_NEUTRAL 120

#define FRONT_RIGHT_HIP_BACK  60
#define FRONT_RIGHT_KNEE_BACK_LIFT  45
#define FRONT_RIGHT_KNEE_BACK_PULL  80
#define BACK_LEFT_HIP_BACK  130
#define BACK_LEFT_KNEE_BACK 135

#define BACK_LEFT_SHOULDER_NEUTRAL  83
#define FRONT_LEFT_SHOULDER_NEUTRAL 85
#define FRONT_RIGHT_SHOULDER_NEUTRAL  97
#define BACK_RIGHT_SHOULDER_NEUTRAL 90


#define SERVO_SPEED_5   5
#define SERVO_SPEED_6   6









// -----DİZLER-------
//back left knee 0 derece ileride 180 derece geride
// front right knee 0 derece geride, 180 ileride
//back right knee, 0 derece geride, 180 derece ileride
//front left knee 0 derece ileride, 180 derece geride

// ------KALÇALAR-------
//back left hip 0 derece ileride, 180 derece geride
//front right hip 0 derece geride, 180 derece ileride
//back right hip 0 derece geride, 180 derece ileride
//front left hip 0 derece ileride, 180 derece geride


// -----OMUZLAR-----
// front right shoulder 0 derece aşağı, 180 derece yukarı doğru
//front left shoulder 0 derece sola doğru, 180 derece içe doğru
//BACK LEFT SHOULDER 0 derece sağa doğru(içe), 180 derece sola doğru (dışa)



// all servos are configured to 90 degree first. Then step_forward func. is running.

void Servo_Init(void)
{
    // 1) Emergency Stop
    Servo_ActionGroup_Stop();
    HAL_Delay(100);

    sprintf(msg,"-----CONFIGURATION IS RUNNING------\r\n");
    HAL_UART_Transmit(&huart1, (uint8_t *)msg, strlen(msg), HAL_MAX_DELAY);


    // 3) Tüm kanalları nötr (1500us) konuma getir
    for (uint8_t ch = 0; ch < 16; ch++)
    {

        Servo_SendPacket(0x02, ch, 1500); // Position control: 1500us = 90°
        HAL_Delay(20);
    }

    HAL_Delay(2000);
    Servo_SetSpeed(BACK_LEFT_SHOULDER,SERVO_SPEED_5);
    Servo_SetSpeed(FRONT_LEFT_SHOULDER,SERVO_SPEED_5);
    Servo_SetSpeed(FRONT_LEFT_HIP,SERVO_SPEED_5);
    Servo_SetPosition(BACK_LEFT_SHOULDER, AngleToPulse(BACK_LEFT_SHOULDER_NEUTRAL));
    Servo_SetPosition(FRONT_LEFT_SHOULDER, AngleToPulse(FRONT_LEFT_SHOULDER_NEUTRAL));
    Servo_SetPosition(FRONT_RIGHT_SHOULDER, AngleToPulse(FRONT_RIGHT_SHOULDER_NEUTRAL));

    sprintf(msg,"BACK LEFT SHOULDER ANGLE : %d \r\n",BACK_LEFT_SHOULDER_NEUTRAL);
    HAL_UART_Transmit(&huart1, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);

    sprintf(msg,"FRONT LEFT SHOULDER ANGLE : %d \r\n",FRONT_LEFT_SHOULDER_NEUTRAL);
    HAL_UART_Transmit(&huart1, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);

    sprintf(msg,"FRONT RIGHT SHOULDER ANGLE : %d \r\n",FRONT_RIGHT_SHOULDER_NEUTRAL);
    HAL_UART_Transmit(&huart1, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
    HAL_Delay(2000);



    Servo_SetSpeed(BACK_LEFT_KNEE,SERVO_SPEED_5);
    Servo_SetPosition(BACK_LEFT_KNEE, AngleToPulse(BACK_LEFT_KNEE_NEUTRAL));
    Servo_SetSpeed(BACK_LEFT_HIP,SERVO_SPEED_5);
    Servo_SetPosition(BACK_LEFT_HIP, AngleToPulse(BACK_LEFT_HIP_NEUTRAL));
    sprintf(msg,"BACK LEFT HIP ANGLE : %d \r\n",BACK_LEFT_HIP_NEUTRAL);
    HAL_UART_Transmit(&huart1, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);




    Servo_SetSpeed(BACK_RIGHT_KNEE,SERVO_SPEED_5);
    Servo_SetPosition(BACK_RIGHT_KNEE, AngleToPulse(BACK_RIGHT_KNEE_NEUTRAL));
    Servo_SetSpeed(BACK_RIGHT_HIP,SERVO_SPEED_5);
    Servo_SetPosition(BACK_RIGHT_HIP, AngleToPulse(BACK_RIGHT_HIP_NEUTRAL));
    sprintf(msg,"BACK RIGHT HIP angle : %d \r\n", BACK_RIGHT_HIP_NEUTRAL);
    HAL_UART_Transmit(&huart1, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);




    Servo_SetSpeed(FRONT_LEFT_KNEE,SERVO_SPEED_5);
    Servo_SetPosition(FRONT_LEFT_KNEE, AngleToPulse(FRONT_LEFT_KNEE_NEUTRAL));
    Servo_SetSpeed(FRONT_LEFT_HIP,SERVO_SPEED_5);
    Servo_SetPosition(FRONT_LEFT_HIP, AngleToPulse(FRONT_LEFT_HIP_NEUTRAL));
    sprintf(msg,"FRONT LEFT HIP angle : %d \r\n", FRONT_LEFT_HIP_NEUTRAL);
    HAL_UART_Transmit(&huart1, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);




    Servo_SetSpeed(FRONT_RIGHT_KNEE,SERVO_SPEED_5);
    Servo_SetPosition(FRONT_RIGHT_KNEE, AngleToPulse(FRONT_RIGHT_KNEE_NEUTRAL));
    Servo_SetSpeed(FRONT_RIGHT_HIP,SERVO_SPEED_5);
    Servo_SetPosition(FRONT_RIGHT_HIP, AngleToPulse(FRONT_RIGHT_HIP_NEUTRAL));
    sprintf(msg,"FRONT RIGHT HIP angle : %d \r\n", FRONT_RIGHT_HIP_NEUTRAL);
    HAL_UART_Transmit(&huart1, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);

    HAL_Delay(2000);

}



void Servo_SendPacket(uint8_t cmd, uint8_t ch, uint16_t data)
{
    uint8_t tx[5];
    tx[0] = 0xFF;         // start byte
    tx[1] = cmd;          // komut
    tx[2] = ch;           // kanal
    tx[3] = data & 0xFF;  // low byte
    tx[4] = data >> 8;    // high byte

    HAL_UART_Transmit(&huart3, tx, 5, HAL_MAX_DELAY);


}


void Servo_SetPosition(uint8_t channel, uint16_t position_us)
{
    uint8_t txData[5];
    txData[0] = 0xFF;          // Start byte
    txData[1] = 0x02;          // CMD: Position control
    txData[2] = channel;       // Servo channel (0-15)
    txData[3] = position_us & 0xFF;     // Low byte
    txData[4] = (position_us >> 8) & 0xFF; // High byte

    HAL_UART_Transmit(&huart3, txData, 5, HAL_MAX_DELAY);

}


void Servo_SetSpeed(uint8_t channel, uint16_t speed)
{
    uint8_t txData[5];
    txData[0] = 0xFF;          // Start byte
    txData[1] = 0x01;          // speed control
    txData[2] = channel;       // Servo channel (0-15)
    txData[3] = speed & 0xFF;     // Low byte
    txData[4] = (speed >> 8) & 0xFF; // High byte

    HAL_UART_Transmit(&huart3, txData, 5, HAL_MAX_DELAY);

}



void Servo_ActionGroup_Stop(void)
{
    uint8_t tx[5];
    tx[0] = 0xFF;
    tx[1] = 0x0B;   // CMD: Emergency stop / recovery
    tx[2] = 0x00;   // CH = 0 (tümünü etkiler)
    tx[3] = 0x01;   // DataL = 1 (stop)
    tx[4] = 0x00;   // DataH = 0

    HAL_UART_Transmit(&huart3, tx, 5, HAL_MAX_DELAY);

}






uint16_t AngleToPulse(uint8_t angle)
{
    // 0 derece => 500us, 180 derece => 2500us
    if (angle < 5) angle = 5;
    if (angle > 175) angle = 175;
    return 500 + ((uint32_t)angle * 2000) / 180;
}


uint8_t PulseToAngle(uint16_t pulse)
{
    if (pulse < 500) pulse = 500;
    if (pulse > 2500) pulse = 2500;
    return (uint8_t)(((uint32_t)(pulse - 500) * 180) / 2000);
}

void Move_Servos(uint8_t ch, uint8_t angle)
{
      uint16_t position_us = AngleToPulse(angle);


      Servo_SetSpeed(ch, SERVO_SPEED_6);
      Servo_SetPosition(ch, position_us);

}






void StepForward(void)
{
    // 1) dizi yukarı çek, kalçayı ileri al, dizi aşağı çek, kalçayı gerş çekileri götür, dizi aşağı çek
 // printf("---------GOING FORWARD..------------\r\n");

  command_ready=0;

  sprintf(msg,"-----GOING FORWARD-----\r\n");
  HAL_UART_Transmit(&huart1, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);

  Servo_SetSpeed(FRONT_LEFT_KNEE, SERVO_SPEED_6);
  Servo_SetSpeed(FRONT_LEFT_HIP, SERVO_SPEED_6);
  Servo_SetSpeed(BACK_RIGHT_KNEE, SERVO_SPEED_6);
  Servo_SetSpeed(BACK_RIGHT_HIP, SERVO_SPEED_6);
  Servo_SetPosition(FRONT_LEFT_KNEE, AngleToPulse(FRONT_LEFT_KNEE_LIFT));
  Servo_SetPosition(BACK_RIGHT_KNEE, AngleToPulse(BACK_RIGHT_KNEE_LIFT));

  HAL_Delay(200);
  Servo_SetPosition(FRONT_LEFT_HIP, AngleToPulse(FRONT_LEFT_HIP_FORWARD));
  Servo_SetPosition(BACK_RIGHT_HIP, AngleToPulse(BACK_RIGHT_HIP_FORWARD));

  sprintf(msg,"FL angle - speed : %d - %d angle/s \r\n",FRONT_LEFT_HIP_FORWARD,6);
  HAL_UART_Transmit(&huart1, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
  sprintf(msg,"BR angle - speed : %d - %d angle/s \r\n",BACK_RIGHT_HIP_FORWARD,6);
  HAL_UART_Transmit(&huart1, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);



  HAL_Delay(200);

  Servo_SetPosition(FRONT_LEFT_KNEE, AngleToPulse(FRONT_LEFT_KNEE_PULL));
  Servo_SetPosition(BACK_RIGHT_KNEE, AngleToPulse(BACK_RIGHT_KNEE_PULL));
  HAL_Delay(200);
  Servo_SetPosition(FRONT_LEFT_HIP, AngleToPulse(FRONT_LEFT_HIP_NEUTRAL));
  Servo_SetPosition(BACK_RIGHT_HIP, AngleToPulse(BACK_RIGHT_HIP_NEUTRAL));
  Servo_SetPosition(FRONT_LEFT_KNEE, AngleToPulse(FRONT_LEFT_KNEE_NEUTRAL));
  Servo_SetPosition(BACK_RIGHT_KNEE, AngleToPulse(BACK_RIGHT_KNEE_NEUTRAL));

  sprintf(msg,"FL angle - speed : %d - %d angle/s \r\n",FRONT_LEFT_HIP_NEUTRAL,SERVO_SPEED_6);
  HAL_UART_Transmit(&huart1, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
  sprintf(msg,"BR angle - speed : %d - %d angle/s \r\n",BACK_RIGHT_HIP_NEUTRAL,SERVO_SPEED_6);
  HAL_UART_Transmit(&huart1, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);



  HAL_Delay(200);


  Servo_SetSpeed(FRONT_RIGHT_KNEE, SERVO_SPEED_6);
  Servo_SetSpeed(FRONT_RIGHT_HIP, SERVO_SPEED_6);
  Servo_SetSpeed(BACK_LEFT_KNEE, SERVO_SPEED_6);
  Servo_SetSpeed(BACK_LEFT_HIP, SERVO_SPEED_6);
  Servo_SetPosition(FRONT_RIGHT_KNEE, AngleToPulse(FRONT_RIGHT_KNEE_LIFT));
  Servo_SetPosition(BACK_LEFT_KNEE, AngleToPulse(BACK_LEFT_KNEE_LIFT));
  HAL_Delay(200);
  Servo_SetPosition(FRONT_RIGHT_HIP, AngleToPulse(FRONT_RIGHT_HIP_FORWARD));
  Servo_SetPosition(BACK_LEFT_HIP, AngleToPulse(BACK_LEFT_HIP_FORWARD));
  sprintf(msg,"FR angle - speed : %d - %d angle/s \r\n",FRONT_RIGHT_HIP_FORWARD,SERVO_SPEED_6);
  HAL_UART_Transmit(&huart1, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
  sprintf(msg,"BL angle - speed : %d - %d angle/s \r\n",BACK_LEFT_HIP_FORWARD,SERVO_SPEED_6);
  HAL_UART_Transmit(&huart1, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);



  HAL_Delay(200);

  Servo_SetPosition(FRONT_RIGHT_KNEE, AngleToPulse(FRONT_RIGHT_KNEE_PULL));
  Servo_SetPosition(BACK_LEFT_KNEE, AngleToPulse(BACK_LEFT_KNEE_PULL));
  HAL_Delay(200);
  Servo_SetPosition(FRONT_RIGHT_HIP, AngleToPulse(FRONT_RIGHT_HIP_NEUTRAL));
  Servo_SetPosition(BACK_LEFT_HIP, AngleToPulse(BACK_LEFT_HIP_NEUTRAL));
  Servo_SetPosition(FRONT_RIGHT_KNEE, AngleToPulse(FRONT_RIGHT_KNEE_NEUTRAL));
  Servo_SetPosition(BACK_LEFT_KNEE, AngleToPulse(BACK_LEFT_KNEE_NEUTRAL));

  sprintf(msg,"FR angle - speed : %d - %d angle/s \r\n",FRONT_RIGHT_HIP_NEUTRAL,SERVO_SPEED_6);
  HAL_UART_Transmit(&huart1, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
  sprintf(msg,"BL angle - speed : %d - %d angle/s \r\n",BACK_LEFT_HIP_NEUTRAL,SERVO_SPEED_6);
  HAL_UART_Transmit(&huart1, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);

  command_ready = 1;
  HAL_Delay(200);



}

void go_back()
{
  //dizi az ileri al, kalçayı geri çek, dizi aşağı indir, bacağı nötr konuma al ve dizi nötr konuma al.

  sprintf(msg,"----GOING BACK----");
  HAL_UART_Transmit(&huart1, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);

//  Servo_SetSpeed(FRONT_RIGHT_HIP, SERVO_SPEED_6);
//  Servo_SetSpeed(FRONT_RIGHT_KNEE, SERVO_SPEED_6);
//  Servo_SetPosition(FRONT_RIGHT_HIP, AngleToPulse(FRONT_RIGHT_HIP_BACK));
//  Servo_SetPosition(FRONT_RIGHT_KNEE, AngleToPulse(FRONT_RIGHT_KNEE_BACK_LIFT));
//  HAL_Delay(100);
//  Servo_SetPosition(FRONT_RIGHT_KNEE, AngleToPulse(FRONT_RIGHT_KNEE_BACK_PULL));
//  Servo_SetPosition(FRONT_RIGHT_HIP, AngleToPulse(FRONT_RIGHT_HIP_NEUTRAL));
//  HAL_Delay(100);
//  Servo_SetPosition(FRONT_RIGHT_KNEE, AngleToPulse(FRONT_RIGHT_KNEE_NEUTRAL));
//  HAL_Delay(100);
//
//  Servo_SetSpeed(BACK_LEFT_HIP, SERVO_SPEED_6);
//  Servo_SetSpeed(BACK_LEFT_KNEE, SERVO_SPEED_6);

//  Servo_SetPosition(BACK_LEFT_KNEE, BACK_LEFT_KNEE_LIFT);
//  Servo_SetPosition(BACK_LEFT_HIP, BACK_LEFT_HIP_BACK);
//  HAL_Delay(100);
//  Servo_SetPosition(BACK_LEFT_KNEE, BACK_LEFT_KNEE_BACK_LIFT);
//  Servo_SetPosition(BACK_LEFT_HIP, BACK_LEFT_HIP_NEUTRAL);
//  HAL_Delay(100);
//  Servo_SetPosition(BACK_LEFT_KNEE, BACK_LEFT_KNEE_BACK_PULLL);
//  HAL_Delay(100);
//  Servo_SetPosition(BACK_LEFT_KNEE, BACK_LEFT_KNEE_NEUTRAL);
//
//


}



void get_up()
{
  command_ready=0;

  sprintf(msg,"----GETTING UP----");
  HAL_UART_Transmit(&huart1, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);

  Servo_SetSpeed(BACK_LEFT_KNEE,SERVO_SPEED_5);
  Servo_SetPosition(BACK_LEFT_KNEE, AngleToPulse(BACK_LEFT_KNEE_NEUTRAL));
  sprintf(msg,"BL Knee angle - speed : %d - %d angle/s \r\n",BACK_LEFT_KNEE_NEUTRAL,SERVO_SPEED_6);
  HAL_UART_Transmit(&huart1, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);


  Servo_SetSpeed(BACK_RIGHT_KNEE,SERVO_SPEED_5);
  Servo_SetPosition(BACK_RIGHT_KNEE, AngleToPulse(BACK_RIGHT_HIP_NEUTRAL));
  sprintf(msg,"BR Knee angle - speed : %d - %d angle/s \r\n",BACK_RIGHT_KNEE_NEUTRAL,SERVO_SPEED_6);
  HAL_UART_Transmit(&huart1, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);


  Servo_SetSpeed(FRONT_LEFT_KNEE,SERVO_SPEED_5);
  Servo_SetPosition(FRONT_LEFT_KNEE, AngleToPulse(FRONT_LEFT_KNEE_NEUTRAL));
  sprintf(msg,"FL Knee angle - speed : %d - %d angle/s \r\n",FRONT_LEFT_KNEE_NEUTRAL,SERVO_SPEED_6);
  HAL_UART_Transmit(&huart1, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);


  Servo_SetSpeed(FRONT_RIGHT_KNEE,SERVO_SPEED_5);
  Servo_SetPosition(FRONT_RIGHT_KNEE, AngleToPulse(FRONT_RIGHT_KNEE_NEUTRAL));
  sprintf(msg,"FR Knee angle - speed : %d - %d angle/s \r\n",FRONT_RIGHT_KNEE_NEUTRAL,SERVO_SPEED_6);
  HAL_UART_Transmit(&huart1, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);

  command_ready = 1;
  HAL_Delay(2000);




}

void lay_down()

{
  command_ready = 0;

  sprintf(msg,"----LAYING DOWN----");
  HAL_UART_Transmit(&huart1, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);

  Servo_SetSpeed(BACK_LEFT_KNEE, SERVO_SPEED_6);
  Servo_SetPosition(BACK_LEFT_KNEE, AngleToPulse(50));
  sprintf(msg,"BL Knee angle - speed : %d - %d angle/s \r\n",50,SERVO_SPEED_6);
  HAL_UART_Transmit(&huart1, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);


  Servo_SetSpeed(BACK_RIGHT_KNEE, SERVO_SPEED_6);
  Servo_SetPosition(BACK_RIGHT_KNEE, AngleToPulse(130));
  sprintf(msg,"BR Knee angle - speed : %d - %d angle/s \r\n",130,SERVO_SPEED_6);
  HAL_UART_Transmit(&huart1, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);


  Servo_SetSpeed(FRONT_LEFT_KNEE, SERVO_SPEED_6);
  Servo_SetPosition(FRONT_LEFT_KNEE, AngleToPulse(50));
  sprintf(msg,"FL Knee angle - speed : %d - %d angle/s \r\n",50,SERVO_SPEED_6);
  HAL_UART_Transmit(&huart1, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);


  Servo_SetSpeed(FRONT_RIGHT_KNEE, SERVO_SPEED_6);
  Servo_SetPosition(FRONT_RIGHT_KNEE, AngleToPulse(130));
  sprintf(msg,"FR Knee angle - speed : %d - %d angle/s \r\n",130,SERVO_SPEED_6);
  HAL_UART_Transmit(&huart1, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);

  command_ready = 1;
  HAL_Delay(1500);
}

void turnRight()
{
  command_ready=0;

  sprintf(msg,"----TURNING RIGHT----");
  HAL_UART_Transmit(&huart1, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);

  Servo_SetSpeed(FRONT_RIGHT_SHOULDER, SERVO_SPEED_6);
  Servo_SetSpeed(BACK_RIGHT_SHOULDER, SERVO_SPEED_6);
  Servo_SetPosition(FRONT_RIGHT_SHOULDER, AngleToPulse(117));
  Servo_SetPosition(BACK_RIGHT_SHOULDER, AngleToPulse(70));
  sprintf(msg,"FR Shoulder angle - speed : %d - %d angle/s \r\n",117,SERVO_SPEED_6);
  HAL_UART_Transmit(&huart1, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
  sprintf(msg,"BR Shoulder angle - speed : %d - %d angle/s \r\n",70,SERVO_SPEED_6);
  HAL_UART_Transmit(&huart1, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
  HAL_Delay(200);
  Servo_SetPosition(FRONT_RIGHT_SHOULDER, AngleToPulse(FRONT_RIGHT_SHOULDER_NEUTRAL));
  Servo_SetPosition(BACK_RIGHT_SHOULDER, AngleToPulse(BACK_RIGHT_SHOULDER_NEUTRAL));
  sprintf(msg,"FR Shoulder angle - speed : %d - %d angle/s \r\n",FRONT_RIGHT_SHOULDER_NEUTRAL,SERVO_SPEED_6);
  HAL_UART_Transmit(&huart1, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
  sprintf(msg,"BR Shoulder angle - speed : %d - %d angle/s \r\n",BACK_RIGHT_SHOULDER_NEUTRAL,SERVO_SPEED_6);
  HAL_UART_Transmit(&huart1, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);

  command_ready = 1;
  HAL_Delay(1000);


}

void turnLeft()
{
  command_ready = 0;

  sprintf(msg,"----TURNING LEFT----");
  HAL_UART_Transmit(&huart1, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);

  Servo_SetSpeed(FRONT_LEFT_SHOULDER, SERVO_SPEED_6);
  Servo_SetSpeed(BACK_LEFT_SHOULDER, SERVO_SPEED_6);
  Servo_SetPosition(FRONT_LEFT_SHOULDER, AngleToPulse(65));
  Servo_SetPosition(BACK_LEFT_SHOULDER, AngleToPulse(105));
  sprintf(msg,"FL Shoulder angle - speed : %d - %d angle/s \r\n",65,SERVO_SPEED_6);
  HAL_UART_Transmit(&huart1, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
  sprintf(msg,"BL Shoulder angle - speed : %d - %d angle/s \r\n",105,SERVO_SPEED_6);
  HAL_UART_Transmit(&huart1, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
  HAL_Delay(200);
  Servo_SetPosition(FRONT_LEFT_SHOULDER, AngleToPulse(FRONT_LEFT_SHOULDER_NEUTRAL));
  Servo_SetPosition(BACK_LEFT_SHOULDER, AngleToPulse(BACK_LEFT_SHOULDER_NEUTRAL));
  sprintf(msg,"FL Shoulder angle - speed : %d - %d angle/s \r\n",FRONT_LEFT_SHOULDER_NEUTRAL,SERVO_SPEED_6);
  HAL_UART_Transmit(&huart1, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
  sprintf(msg,"BL Shoulder angle - speed : %d - %d angle/s \r\n",BACK_LEFT_SHOULDER_NEUTRAL,SERVO_SPEED_6);
  HAL_UART_Transmit(&huart1, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);

  command_ready = 1;

  HAL_Delay(1000);
}

void toNeutral()
{
     command_ready = 0;

     sprintf(msg,"----GETTING BACK TO NEUTRAL----\r\n");
     HAL_UART_Transmit(&huart1, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);


     Servo_SetSpeed(BACK_LEFT_SHOULDER,SERVO_SPEED_5);
     Servo_SetSpeed(FRONT_LEFT_SHOULDER,SERVO_SPEED_5);
     Servo_SetSpeed(FRONT_LEFT_HIP,SERVO_SPEED_5);
     Servo_SetPosition(BACK_LEFT_SHOULDER, AngleToPulse(BACK_LEFT_SHOULDER_NEUTRAL));
     Servo_SetPosition(FRONT_LEFT_SHOULDER, AngleToPulse(FRONT_LEFT_SHOULDER_NEUTRAL));
     Servo_SetPosition(FRONT_RIGHT_SHOULDER, AngleToPulse(FRONT_RIGHT_SHOULDER_NEUTRAL));

     sprintf(msg,"BACK LEFT SHOULDER ANGLE : %d \r\n",BACK_LEFT_SHOULDER_NEUTRAL);
     HAL_UART_Transmit(&huart1, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);

     sprintf(msg,"FRONT LEFT SHOULDER ANGLE : %d \r\n",FRONT_LEFT_SHOULDER_NEUTRAL);
     HAL_UART_Transmit(&huart1, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);

     sprintf(msg,"FRONT RIGHT SHOULDER ANGLE : %d \r\n",FRONT_RIGHT_SHOULDER_NEUTRAL);
     HAL_UART_Transmit(&huart1, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
     HAL_Delay(2000);



     Servo_SetSpeed(BACK_LEFT_KNEE,SERVO_SPEED_5);
     Servo_SetPosition(BACK_LEFT_KNEE, AngleToPulse(BACK_LEFT_KNEE_NEUTRAL));
     Servo_SetSpeed(BACK_LEFT_HIP,SERVO_SPEED_5);
     Servo_SetPosition(BACK_LEFT_HIP, AngleToPulse(BACK_LEFT_HIP_NEUTRAL));
     sprintf(msg,"BACK LEFT HIP ANGLE : %d \r\n",BACK_LEFT_HIP_NEUTRAL);
     HAL_UART_Transmit(&huart1, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);




     Servo_SetSpeed(BACK_RIGHT_KNEE,SERVO_SPEED_5);
     Servo_SetPosition(BACK_RIGHT_KNEE, AngleToPulse(BACK_RIGHT_KNEE_NEUTRAL));
     Servo_SetSpeed(BACK_RIGHT_HIP,SERVO_SPEED_5);
     Servo_SetPosition(BACK_RIGHT_HIP, AngleToPulse(BACK_RIGHT_HIP_NEUTRAL));
     sprintf(msg,"BACK RIGHT HIP angle : %d \r\n", BACK_RIGHT_HIP_NEUTRAL);
     HAL_UART_Transmit(&huart1, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);




     Servo_SetSpeed(FRONT_LEFT_KNEE,SERVO_SPEED_5);
     Servo_SetPosition(FRONT_LEFT_KNEE, AngleToPulse(FRONT_LEFT_KNEE_NEUTRAL));
     Servo_SetSpeed(FRONT_LEFT_HIP,SERVO_SPEED_5);
     Servo_SetPosition(FRONT_LEFT_HIP, AngleToPulse(FRONT_LEFT_HIP_NEUTRAL));
     sprintf(msg,"FRONT LEFT HIP angle : %d \r\n", FRONT_LEFT_HIP_NEUTRAL);
     HAL_UART_Transmit(&huart1, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);




     Servo_SetSpeed(FRONT_RIGHT_KNEE,SERVO_SPEED_5);
     Servo_SetPosition(FRONT_RIGHT_KNEE, AngleToPulse(FRONT_RIGHT_KNEE_NEUTRAL));
     Servo_SetSpeed(FRONT_RIGHT_HIP,SERVO_SPEED_5);
     Servo_SetPosition(FRONT_RIGHT_HIP, AngleToPulse(FRONT_RIGHT_HIP_NEUTRAL));
     sprintf(msg,"FRONT RIGHT HIP angle : %d \r\n", FRONT_RIGHT_HIP_NEUTRAL);
     HAL_UART_Transmit(&huart1, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);

     command_ready = 1;
     HAL_Delay(1500);




}





