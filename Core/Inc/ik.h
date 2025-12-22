/*
 * pwm_controller_1.h
 *
 *  Created on: Aug 24, 2025
 *      Author: SELCUK
 */

#ifndef INC_IK_H_
#define INC_IK_H_

void GoTo(float X_target, float Y_target);


void RotMatrix3D(uint16_t rotation[3],uint8_t isRadian, char order[3]);

double calc_shoulder_angle(double Y, double Z);
double calculate_knee_angle(double X);
void updateRot(double roll, double pitch, double yaw);
void legIK(float x, float y, float z, float L1, float L2);



#endif /* INC_IK_H_ */
