#include "motor_controls.h"

void servo_pwm_init(cyhal_pwm_t *pwm_obj_servo){
    cy_rslt_t result;
    // Direction control
    result = cyhal_gpio_init(SERVO_IN1, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG, false);
    CY_ASSERT(CY_RSLT_SUCCESS == result);
    result = cyhal_gpio_init(SERVO_IN2, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG, false);
    CY_ASSERT(CY_RSLT_SUCCESS == result);
    
    // speed control
    result = cyhal_pwm_init(pwm_obj_servo, PWM_servo, NULL);
    CY_ASSERT(CY_RSLT_SUCCESS == result);
    /* Start the PWM output */
    result = cyhal_pwm_start(pwm_obj_servo);
    CY_ASSERT(CY_RSLT_SUCCESS == result);

    return;
}

void servo_pwm_set(cyhal_pwm_t *pwm_obj_servo, int pwm_frequency, float pwm_duty_cycle, bool servo_on, bool right){
    cy_rslt_t result;
    bool write_val = false; //write_val used for IN1 and IN2 for direction output of Hbridge
    
    if (!servo_on){  // SERVO OFF
        cyhal_gpio_write(SERVO_IN1, write_val);
        cyhal_gpio_write(SERVO_IN2, write_val);
        return;
    }
    else //forward or backward state
    {
        cyhal_gpio_write(SERVO_IN1, right);
        cyhal_gpio_write(SERVO_IN2, !right); 
    }
    
    result = cyhal_pwm_set_duty_cycle(pwm_obj_servo, pwm_duty_cycle, pwm_frequency);
    CY_ASSERT(CY_RSLT_SUCCESS == result);


    return;
}

void motor_pwm_init(cyhal_pwm_t *pwm_obj_drive_train){
    cy_rslt_t result;
    // Direction control
    result = cyhal_gpio_init(DRIVE_TRAIN_IN1, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG, false);
    CY_ASSERT(CY_RSLT_SUCCESS == result);
    result = cyhal_gpio_init(DRIVE_TRAIN_IN2, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG, false);
    CY_ASSERT(CY_RSLT_SUCCESS == result);
    
    // speed control
    result = cyhal_pwm_init(pwm_obj_drive_train, PWM_drive_train, NULL);
    CY_ASSERT(CY_RSLT_SUCCESS == result);
    /* Start the PWM output */
    result = cyhal_pwm_start(pwm_obj_drive_train);
    CY_ASSERT(CY_RSLT_SUCCESS == result);

    return;
}

void motor_pwm_set(cyhal_pwm_t *pwm_obj_drive_train, int pwm_frequency, float pwm_duty_cycle, bool motor_1_on, bool motor_2_on, bool forward){
    cy_rslt_t result;
    bool write_val = false; //write_val used for IN1 and IN2 for direction output of Hbridge
    
    if (motor_1_on == false && motor_2_on == false){  // MOTORS OFF
        cyhal_gpio_write(DRIVE_TRAIN_IN1, write_val);
        cyhal_gpio_write(DRIVE_TRAIN_IN2, write_val);
        return;
    }
    else //forward or backward state
    {
        cyhal_gpio_write(DRIVE_TRAIN_IN1, forward);
        cyhal_gpio_write(DRIVE_TRAIN_IN2, !forward); 
    }
    
    result = cyhal_pwm_set_duty_cycle(pwm_obj_drive_train, pwm_duty_cycle, pwm_frequency);
    CY_ASSERT(CY_RSLT_SUCCESS == result);


    return;
}



