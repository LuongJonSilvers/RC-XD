/*
 * main.h
 *
 *  Created on: Aug 30, 2022
 *      Author: TEAM RCXD ©
 */

#ifndef MAIN_H_
#define MAIN_H_


#define ECE453_USR_BTN P5_6
#define ECE453_USR_LED P5_5


#define TURRET_PIN P5_3

#define ECE453_DEBUG_TX	P5_1
#define ECE453_DEBUG_RX P5_0

/*******************************************************************************
* Macros
*******************************************************************************/
#define PWM_FREQUENCY (100)
#define PWM_DUTY_CYCLE (50.0f)

////////////////////// Global Variable for storing BLE PWM ////////////////
extern volatile int ble_duty_cycle_drive_train;
extern volatile int ble_duty_cycle_servo;
extern cyhal_pwm_t pwm_obj_drive_train;
extern cyhal_pwm_t pwm_obj_servo;
extern volatile int send_crash_ble;
extern volatile int turret_fire;
extern volatile uint8_t GPS_BUFFER[200];


extern volatile char* gps_latitude;
extern volatile char *gps_longitude;
extern volatile int gps_N_1_S_0;
extern volatile int gps_E_1_W_0;

///////////////////////////////////////////////////////////////////////////

#include "leds.h"
#include "push_button.h"
#include "console.h"
#include "i2c.h"
#include "at42qt2120.h"

#include <stdio.h>
#include "stdlib.h"

#endif /* MAIN_H_ */
