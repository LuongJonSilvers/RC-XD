#include <stdio.h> 

#define servo_Pin_PWM P10_3
#define servo_Pin_IN2 P10_2
#define servo_Pin_IN1 P10_1
cy_rslt_t rslt;
cyhal_pwm_t pwm_obj;


void drive_servo(char frequency, char duty_Cycle, bool right){
    rslt = cyhal_gpio_init(servo_Pin_IN1, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG, false);
    rslt = cyhal_gpio_init(servo_Pin_IN2, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG, false);

//drive outputs for IN1 & IN2 of Hbridge module

    if(right){//right turn hbridge pins IN1=1,IN2=1
        cyhal_gpio_write(servo_Pin_IN1,true);
        cyhal_gpio_write(servo_Pin_IN2,false);
    }else{ //left turn hbridge pins IN1=0, IN2=1
        cyhal_gpio_write(servo_Pin_IN1,false);
        cyhal_gpio_write(servo_Pin_IN2,true);

    }
//pwm activity 
        /* Initialize PWM on the supplied pin and assign a new clock */
        rslt = cyhal_pwm_init(&pwm_obj, servo_Pin_PWM, NULL);
        CY_ASSERT(CY_RSLT_SUCCESS == rslt);
    /* Set a duty cycle of 50% and frequency of 1Hz */
        rslt = cyhal_pwm_set_duty_cycle(&pwm_obj, duty_Cycle, frequency);

        /* Start the PWM output */
        rslt = cyhal_pwm_start(&pwm_obj);


}
