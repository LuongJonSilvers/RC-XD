/*
 * motor_controls.h
 *
 *  Created on: Nov 6, 2022
 *      Author: sreen
 */

#ifndef MOTOR_CONTROLS_H_
#define MOTOR_CONTROLS_H_

/***** DRIVE TRAIN ******/ /// both motor
#define PWM_drive_train P10_4 //pwm pwm function parameter 1 red
#define DRIVE_TRAIN_IN1 P10_5//out 1 gpio orange
#define DRIVE_TRAIN_IN2 P9_5 //out 2 gpio yellow


/***** SERVO ******/
#define PWM_servo P10_6 //blue
#define SERVO_IN1 P10_3 //purple
#define SERVO_IN2 P12_6 //green


// //front motor
// #define PWM_drive_train2 P9_4//pwm pwm function parameter 2 red
// #define DRIVE_TRAIN_IN3 P10_4 //out 3 gpio yellow
// #define DRIVE_TRAIN_IN4 P10_6//out 4 gpio orange


#include "cy_pdl.h"
#include "cy_retarget_io.h"
#include "cybsp.h"
#include "cyhal.h"
#include "ble_findme.h"
#include "main.h"
#include "usr_btn.h"

/***** DRIVE TRAIN ******/
void motor_pwm_set(cyhal_pwm_t *pwm_obj_drive_train, int pwm_frequency, float pwm_duty_cycle, bool motor_1_on, bool motor_2_on, bool forward);
void motor_pwm_init(cyhal_pwm_t *pwm_obj_drive_train);
/***** SERVO ******/
void servo_pwm_init(cyhal_pwm_t *pwm_obj_servo);
void servo_pwm_set(cyhal_pwm_t *pwm_obj_servo, int pwm_frequency, float pwm_duty_cycle, bool servo_on, bool right);

#endif /* MOTOR_CONTROLS_H_ */
