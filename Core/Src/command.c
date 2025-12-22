/*
 * command.c
 *
 *  Created on: Aug 21, 2025
 *      Author: SELCUK
 */

#include "main.h"
#include "command.h"
#include "pwm_controller.h"
#include <stdio.h>
#include <string.h>

extern uint8_t cmd_ready;
extern uint8_t last_cmd;

extern UART_HandleTypeDef huart1;
extern char msg[];


void ExecuteCommand(int cmd)
{
    switch (cmd) {
        case 1:

            last_cmd = 1;
            while(1){
              if(cmd_ready==1)
              {

                StepForward();
              }
              else
              {
                break;
              }

            }

            break;
//        case -1:
//            printf("Back\r\n");
//
//
//            go_back();
//
//
//            break;
        case 2:
            last_cmd = 2;
            printf("Right\r\n");

            while(cmd_ready==2)
            {
              turnRight();
            }




            break;
        case 3:
            last_cmd = 3;
            printf("Left\r\n");

            while(cmd_ready==3)
            {
              turnLeft();
            }

            break;
        case 4:
            last_cmd = 4;
            printf("Lay down\r\n");



            lay_down();

            last_cmd = 4;




            break;
        case 5:
            last_cmd = 5;
            printf("Get up\r\n");


            get_up();





            break;
        case 6:
            last_cmd = 6;
            printf("Getting reset\r\n");


            toNeutral();




            break;


        default:

            break;
    }
}



