/*
 * pwm_controller.h
 *
 *  Created on: Aug 15, 2025
 *      Author: SELCUK
 */

#ifndef INC_PWM_CONTROLLER_H_
#define INC_PWM_CONTROLLER_H_



void Servo_Init(void);
void Servo_SendPacket(uint8_t cmd, uint8_t ch, uint16_t data);
void Servo_SetPosition(uint8_t channel, uint16_t position_us);
void Servo_SetSpeed(uint8_t channel, uint16_t speed);
void Servo_ActionGroup_Stop(void);
uint16_t AngleToPulse(uint8_t angle);
uint8_t PulseToAngle(uint16_t pulse);
void Move_Servos(uint8_t ch, uint8_t angle);
void StepForward(void);
void go_back();
void get_up(void);
void lay_down();
void turnRight();
void turnLeft();
void toNeutral();

#endif /* INC_PWM_CONTROLLER_H_ */
