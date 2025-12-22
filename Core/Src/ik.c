/*
 * pwm_controller_1.c
 *
 *  Created on: Aug 24, 2025
 *      Author: SELCUK
 */
#include "main.h"
#include <stdint.h>
#include <stdio.h>
#include <math.h>



uint16_t  roll, pitch, yaw;
double rotX[3][3];
double rotY[3][3];
double rotZ[3][3];
double rotationMatrix[3][3];
double hipToJoint;
double legLenth;
double hipToJoint=10;
double kneeToGround=25;

double legLength = 45;

double D;

extern float shoulder, hip, knee;
extern float L1;
extern float L2;

// Ayağın hedef konumu (örnek)
float x = 180.0f;
float y = 30.0f;
float z = -100.0f;

extern float q_shoulder;
extern float q_hip;
extern float q_knee;



//void GoTo(float X_target, float Y_target)
//{
//    float dx = X_target - robot_x;
//    float dy = Y_target - robot_y;
//    float distance = sqrt(dx*dx + dy*dy);
//
//    int steps = (int)(distance / STEP_SIZE);
//
//    for (int i = 0; i < steps; i++) {
//        StepForward();        // 1 adım at
//        robot_x += STEP_SIZE; // şase koordinatını güncelle
//    }
//}
//
//
void RotMatrix3D(uint16_t rotation[3],uint8_t isRadian, char order[3])

{

  roll = rotation[0];
  pitch = rotation[1];
  yaw = rotation[2];

  //if parameter is not a radian, convert to radians
  if (isRadian != 1)
  {
    roll = roll*(1/180);
    pitch = pitch*(1/180);
    yaw = yaw*(1/180);

  }

  // rot matrix about each axis
//  updateRot(roll,pitch,yaw);

//  if(order=="xyz")
//  {
//    rotationMatrix = rotZ*rotY*rotX;
//  }
//
//  if(order=="xzy")
//  {
//    rotationMatrix=rotY*rotZ*rotX;
//  }
//
//  if(order=="yxz")
//  {
//    rotationMatrix=rotZ*rotX*rotY;
//  }
//  if(order=="yzx")
//  {
//    rotationMatrix=rotX*rotZ*rotY;
//  }
//  if(order=="zxy")
//  {
//    rotationMatrix=rotY*rotX*rotZ;
//
//  }
//  if(order=="zyx")
//  {
//    rotationMatrix=rotX*rotY*rotZ;
//  }


// return rotationMatrix;

}


double calc_shoulder_angle(double Y, double Z)
{
    uint8_t a = atan(legLength/hipToJoint);
    uint8_t B = atan(Z/Y);


    uint8_t w= a+B;
    D = sqrt((Y*Y+Z*Z-hipToJoint*hipToJoint));

    return w;
}

double calculate_knee_angle(double X)
{

  double G = sqrt((D+X*X-hipToJoint));

  double phi = acos((G*G-hipToJoint*hipToJoint-kneeToGround*kneeToGround)/(-2*kneeToGround*hipToJoint));

  double theta = atan(X/legLenth)+asin(kneeToGround*sin(phi)/G);

  return theta;
}


void updateRot(double roll, double pitch, double yaw)
{
    rotX[0][0] = 1;   rotX[0][1] = 0;           rotX[0][2] = 0;
    rotX[1][0] = 0;   rotX[1][1] = cos(roll);  rotX[1][2] = -sin(roll);
    rotX[2][0] = 0;   rotX[2][1] = sin(roll);  rotX[2][2] = cos(roll);

    rotY[0][0] = cos(pitch); rotY[0][1] = 0; rotY[0][2] = sin(pitch);
    rotY[1][0] = 0; rotY[1][1] = 1; rotY[1][2] = 0;
    rotY[2][0] = -sin(pitch); rotY[2][1] = 0; rotY[2][2] = cos(pitch);

    rotZ[0][0] = cos(yaw); rotZ[0][1] = -sin(yaw); rotZ[0][2] = 0;
    rotZ[1][0] = sin(yaw); rotZ[1][1] = cos(yaw); rotY[1][2] = 0;
    rotZ[2][0] = 0; rotZ[2][1] = 0; rotZ[2][2] = 1;


}

// Ayak pozisyonundan shoulder, hip ve knee açılarını hesaplar
// L1: uyluk uzunluğu, L2: kaval uzunluğu
// x,y,z: ayağın hedef pozisyonu (mm veya cm, fark etmez)
// Sonuç: açı değerleri (radyan)
void legIK(float x, float y, float z, float L1, float L2)
{
    // 1) Shoulder
    float q1 = atan2f(y, x);

    // 2) Ayağı sagittal düzleme döndür
    float xp = cosf(q1) * x + sinf(q1) * y;
    float zp = z;

    // 3) Mesafe
    float d = sqrtf(xp * xp + zp * zp);

    // 4) Diz açısı
    float c3 = (L1*L1 + L2*L2 - d*d) / (2 * L1 * L2);
    if (c3 > 1.0f) c3 = 1.0f;
    if (c3 < -1.0f) c3 = -1.0f;
    float q3 = (float)M_PI - acosf(c3);

    // 5) (hip pitch)
    float ca = (L1*L1 + d*d - L2*L2) / (2 * L1 * d);
    if (ca > 1.0f) ca = 1.0f;
    if (ca < -1.0f) ca = -1.0f;
    float alpha = acosf(ca); //üçgendeki iç açı
    float phi   = atan2f(-zp, xp);
    float q2 = phi - alpha;  // kalça pitch açısı


    q_shoulder = (q1*(180.0/3.1415)*(0.5))+90;
    q_hip = (q2*(180.0/3.1415)*(0.5))+90;
    q_knee = q3*(180.0/3.1415);





}

