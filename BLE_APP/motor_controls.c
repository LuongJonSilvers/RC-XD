/******************************************************************************
* Header files
******************************************************************************/
// #define ENA 
// #define IN1 
// #define IN2
// #define IN3
// #define IN4
// #define ENB
#include "cy_pdl.h"
#include "cy_retarget_io.h"
#include "cybsp.h"
#include "cyhal.h"
#include "ble_findme.h"
#include "main.h"
#include "usr_btn.h"
//#include <stdlib.h>


void motor_pwm_set(int pwm_frequency, float pwm_duty_cycle, bool motor_1_on, bool motor_2_on, bool forward){
    cy_rslt_t result;
    bool write_val = false; //write_val used for IN1 and IN2 for direction output of Hbridge
    
    if (motor_1_on == false && motor_2_on == false){ //coasting
        ////////////////// NOTE:: P10.2 --> input_1 & NOTE:: P10.3 --> input_2 ///////////////////////////
        // P10.2
        /* Initialize pin P0_0 GPIO as an output with strong drive mode and initial value = false (low) */
        result = cyhal_gpio_init(P10_2, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG, false);
        /* Write the value to the output pin */
        cyhal_gpio_write(P10_2, write_val);

        // P10.3
        /* Initialize pin P0_0 GPIO as an output with strong drive mode and initial value = false (low) */
        result = cyhal_gpio_init(P10_3, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG, false);
        /* Write the value to the output pin */
        cyhal_gpio_write(P10_3, write_val);
        return;
    }
    else //forward or backward state
    {
        if(forward){
           write_val = true; //forward
        }
        else{
            write_val = false; //reverse
        }
        /* Initialize pin P0_0 GPIO as an output with strong drive mode and initial value = false (low) */
        result = cyhal_gpio_init(P10_2, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG, false);
        /* Write the value to the output pin */
        cyhal_gpio_write(P10_2, write_val);

        // P10.3
        /* Initialize pin P0_0 GPIO as an output with strong drive mode and initial value = false (low) */
        result = cyhal_gpio_init(P10_3, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG, false);
        /* Write the value to the output pin */
        cyhal_gpio_write(P10_3, !write_val); 
    }
    

    
	cyhal_pwm_t pwm_obj_motor1;
    cyhal_pwm_t pwm_obj_motor2;
    /* Initialize PWM on the supplied pin and assign a new clock */
    // For motor 1
    result = cyhal_pwm_init(&pwm_obj_motor1, P10_1, NULL);
    /* PWM Initialization failed. Stop program execution */
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }
    /* Initialize PWM on the supplied pin and assign a new clock */
    // For motor 2
    result = cyhal_pwm_init(&pwm_obj_motor2, P10_0, NULL);
    /* PWM Initialization failed. Stop program execution */
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    result = cyhal_pwm_set_duty_cycle(&pwm_obj_motor1, pwm_duty_cycle, pwm_frequency);
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }
    result = cyhal_pwm_set_duty_cycle(&pwm_obj_motor2, pwm_duty_cycle, pwm_frequency);
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    /* Start the PWM output */
    result = cyhal_pwm_start(&pwm_obj_motor1);
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }
    /* Start the PWM output */
    result = cyhal_pwm_start(&pwm_obj_motor2);
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }
    return;
}
