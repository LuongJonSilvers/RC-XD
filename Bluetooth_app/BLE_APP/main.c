/******************************************************************************
 * File Name: main.c
 *
 * Description: This is source code for the PSoC 6 MCU with BLE Find Me code
 *              example.
 *
 * Related Document: README.md
 *
 *******************************************************************************
 * Copyright 2019-2021, Cypress Semiconductor Corporation (an Infineon company) or
 * an affiliate of Cypress Semiconductor Corporation.  All rights reserved.
 *
 * This software, including source code, documentation and related
 * materials ("Software") is owned by Cypress Semiconductor Corporation
 * or one of its affiliates ("Cypress") and is protected by and subject to
 * worldwide patent protection (United States and foreign),
 * United States copyright laws and international treaty provisions.
 * Therefore, you may use this Software only as provided in the license
 * agreement accompanying the software package from which you
 * obtained this Software ("EULA").
 * If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
 * non-transferable license to copy, modify, and compile the Software
 * source code solely for use in connection with Cypress's
 * integrated circuit products.  Any reproduction, modification, translation,
 * compilation, or representation of this Software except as specified
 * above is prohibited without the express written permission of Cypress.
 *
 * Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
 * reserves the right to make changes to the Software without notice. Cypress
 * does not assume any liability arising out of the application or use of the
 * Software or any product or circuit described in the Software. Cypress does
 * not authorize its products for use in any products where a malfunction or
 * failure of the Cypress product may reasonably be expected to result in
 * significant property damage, injury or death ("High Risk Product"). By
 * including Cypress's product in a High Risk Product, the manufacturer
 * of such system or application assumes all risk of such use and in doing
 * so agrees to indemnify Cypress against all liability.
 *******************************************************************************/

/******************************************************************************
 * Header files
 ******************************************************************************/
#include "cy_pdl.h"
#include "cy_retarget_io.h"
#include "cybsp.h"
#include "cyhal.h"
#include "ble_findme.h"
#include "main.h"
#include "usr_btn.h"
#include "stdlib.h"
#include "motor_controls.h"
#include "accelerometer.h"
#include "light_sensor2.h"
#include "timer.h"
#include "console.h"
#include "uart.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

#define PERIPHERAL_LOG_ON true

volatile int send_crash_ble = 0;

/* drive train & servo*/
volatile int ble_duty_cycle_drive_train;
volatile int ble_duty_cycle_servo;
cyhal_pwm_t pwm_obj_drive_train;
cyhal_pwm_t pwm_obj_servo;
/*GPS unit*/
extern uint8_t rx_buf[RX_BUF_SIZE];
volatile uint8_t GPS_BUFFER[200];
extern cyhal_uart_t uart_obj;

volatile int turret_fire = 0;
volatile char *gps_latitude;
volatile char *gps_longitude;
volatile int gps_N_1_S_0;
volatile int gps_E_1_W_0;

void turret_fire_ble(void)
{
    turret_fire ? cyhal_gpio_write(TURRET_PIN, true) : cyhal_gpio_write(TURRET_PIN, false);
    return;
}

int gps_split(char *str, char c, char ***arr)
{
    int count = 1;
    int token_len = 1;
    int i = 0;
    char *p;
    char *t;

    p = str;
    while (*p != '\0')
    {
        if (*p == c)
            count++;
        p++;
    }

    *arr = (char **)malloc(sizeof(char *) * count);
    if (*arr == NULL)
        exit(1);

    p = str;
    while (*p != '\0')
    {
        if (*p == c)
        {
            (*arr)[i] = (char *)malloc(sizeof(char) * token_len);
            if ((*arr)[i] == NULL)
                exit(1);

            token_len = 0;
            i++;
        }
        p++;
        token_len++;
    }
    (*arr)[i] = (char *)malloc(sizeof(char) * token_len);
    if ((*arr)[i] == NULL)
        exit(1);

    i = 0;
    p = str;
    t = ((*arr)[i]);
    while (*p != '\0')
    {
        if (*p != c && *p != '\0')
        {
            *t = *p;
            t++;
        }
        else
        {
            *t = '\0';
            i++;
            t = ((*arr)[i]);
        }
        p++;
    }

    return count;
}

void global_timer_peripherals(void)
{
    float lumen;
    if (global_timer_interrupt_flag)
    {
        lumen = OPT3001_read_light();

        (lumen < 7500) ? cyhal_gpio_write(P12_7, true) : cyhal_gpio_write(P12_7, false);

        if (crash_occured())
        {
            send_crash_ble = 1;
        }
        global_timer_interrupt_flag = false;

        if (PERIPHERAL_LOG_ON)
        {
            printf("********LUMEN : %f********** \r\n\n", lumen);
            printf("CRASH_VAL :: %f\r\n", (float)(accelerometer_x_axis() * accelerometer_x_axis() + accelerometer_y_axis() * accelerometer_y_axis() + accelerometer_z_axis() * accelerometer_z_axis()));
            printf("X_AXIS :: %d, Y_AXIS :: %d, Z_AXIS :: %d\r\n", accelerometer_x_axis(), accelerometer_y_axis(), accelerometer_z_axis());
        }
    }
}

int main(void)
{

    /* Enable global interrupts */
    __enable_irq();
    uint32_t value = 0;
    char start_char;
    char G_char;
    char P_char;
    char G2_char;
    char G3_char;
    char A_char;
    float lumen;
    uint8_t button_status;

    cy_rslt_t result;

    /* Initialize the device and board peripherals */
    result = cybsp_init();

    /* Board init failed. Stop program execution */
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    /* Initialize retarget-io to use the debug UART port */
    result = cy_retarget_io_init(ECE453_DEBUG_TX, ECE453_DEBUG_RX,
                                 CY_RETARGET_IO_BAUDRATE);
    /* retarget-io init failed. Stop program execution */
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    /* Initialize the User LEDs */
    result = cyhal_gpio_init(ECE453_USR_LED, CYHAL_GPIO_DIR_OUTPUT,
                             CYHAL_GPIO_DRIVE_STRONG, CYBSP_LED_STATE_OFF);

    result = usr_btn_init();
    printf("* -- Initializing I2C Bus\n\r");
    i2c_init();

    // send_crash_ble = 1; /// remove meee

    /* GPIO init failed. Stop program execution */
    if (result != CY_RSLT_SUCCESS)
    {
        printf("Error initializing the timer\r\n");
        CY_ASSERT(0);
    }

    // TURRET INIT
    result = cyhal_gpio_init(TURRET_PIN, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG, false);
    CY_ASSERT(CY_RSLT_SUCCESS == result);

    accelerometer_IIS328DQTR_init();
    global_timer();

    /* \x1b[2J\x1b[;H - ANSI ESC sequence for clear screen */
    printf("\x1b[2J\x1b[;H");
    printf("PSoC 6 MCU With BLE Connectivity Find Me\r\n\n");

    motor_pwm_init(&pwm_obj_drive_train);
    servo_pwm_init(&pwm_obj_servo);
    ble_findme_init();

    cyhal_gpio_init(P12_7, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG, false);

    uart_init();
    for (;;)
    {
        turret_fire_ble();
        ble_findme_process();
        global_timer_peripherals();
        start_char = uart_get_char();
        G_char = uart_get_char();
        P_char = uart_get_char();
        G2_char = uart_get_char();
        G3_char = uart_get_char();
        A_char = uart_get_char();
        if (((int)start_char == 36) && ((int)G_char == 71) && ((int)P_char == 80) && ((int)G2_char == 71) && ((int)G3_char == 71) && ((int)A_char == 65))
        {
            if (PERIPHERAL_LOG_ON)
            {
                printf("%c%c%c%c%c%c", start_char, G_char, P_char, G2_char, G3_char, A_char);
            }
            char gps_style[6] = {start_char, G_char, P_char, G2_char, G3_char, A_char};
            char gps_style_chosen[6] = "$GPGGA";

            for (int i = 0; i < 197; i++)
            {
                char gps_data;
                gps_data = uart_get_char();
                if (PERIPHERAL_LOG_ON)
                {
                    // printf("check\r\n");
                    printf("%c", gps_data);
                }

                GPS_BUFFER[i] = gps_data;
            }
            if (strcmp(gps_style_chosen, gps_style) == 0)
            {
                // n1, e1
                char **gps_list = NULL;
                // this is the GPS style chosen
                int gps_len = gps_split(GPS_BUFFER, ',', &gps_list);
                if (gps_list[1] != NULL && isdigit(gps_list[1][0]) &&// latitude
                    gps_list[2] != NULL && isalpha(gps_list[2][0]) &&// N/s
                    gps_list[3] != NULL && isdigit(gps_list[3][0]) &&// longitude
                    gps_list[4] != NULL && isalpha(gps_list[4][0])  // E/W
                )
                {
                    for (int j = 0; j < gps_len; j++){
                        printf("%s", gps_list[j]);
                    }
                    // printf("\r\n");
                    // printf("----------%s-----------\r\n", gps_list[1]);
                    // printf("----------%s-----------\r\n", gps_list[2]);
                    // printf("----------%s-----------\r\n", gps_list[3]);
                    // printf("----------%s-----------\r\n", gps_list[4]);
                    gps_latitude = gps_list[1];
                    gps_N_1_S_0 = atoi(gps_list[2]);
                    gps_longitude = gps_list[3];
                    gps_E_1_W_0 = atoi(gps_list[4]);
                }
                free(gps_list);
            }
        }
    }
}

/* END OF FILE */
