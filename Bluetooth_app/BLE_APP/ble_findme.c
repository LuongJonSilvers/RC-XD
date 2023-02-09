/******************************************************************************
 * File Name: ble_findme.c
 *
 * Description: This file contains BLE related functions.
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
 * Include header files
 *****************************************************************************/
#include "ble_findme.h"
#include "cyhal.h"
#include "cy_retarget_io.h"
#include "cybsp.h"
#include "cycfg_ble.h"
#include "main.h"
#include "motor_controls.h"

/*******************************************************************************
 * Macros
 ********************************************************************************/
#define BLESS_INTR_PRIORITY (1u)
#define WAKEUP_INTR_PRIORITY (7u)
#define WAKEUP_TIMER_DELAY_MS (250)
/* Timer value to get 0.25 sec with wakeup timer input clock of 32768 Hz,
 * where 32768Hz is LFCLK in PSoC 6 MCU */
#define WAKEUP_TIMER_MATCH_VALUE (WAKEUP_TIMER_DELAY_MS * 32768 / 1000)

/*******************************************************************************
 * Global Variables
 ********************************************************************************/
cyhal_lptimer_t wakeup_timer;
bool wakeup_intr_flag = false;
bool gpio_intr_flag = false;
uint8 alert_level = CY_BLE_NO_ALERT;
cy_stc_ble_conn_handle_t app_conn_handle;
uint8_t BTN_COUNT = 0;

/* ECE453 EDIT */
cy_stc_ble_gatt_write_param_t *write_req_param;
cy_stc_ble_gatts_char_val_read_req_t *read_req_param;

/*******************************************************************************
 * Function Prototypes
 ********************************************************************************/
static void ble_init(void);
static void bless_interrupt_handler(void);
static void stack_event_handler(uint32 event, void *eventParam);
static void ble_start_advertisement(void);
static void wakeup_timer_init(void);
static void wakeup_timer_interrupt_handler(void *handler_arg, cyhal_lptimer_event_t event);
static void ble_ias_callback(uint32 event, void *eventParam);
static void enter_low_power_mode(void);

/*******************************************************************************
 * Function Name: ble_findme_init
 ********************************************************************************
 * Summary:
 * This function initializes the BLE and a deep sleep wakeup timer.
 *
 *******************************************************************************/
void ble_findme_init(void)
{
    /* Configure BLE */
    ble_init();

    /* Configure deep sleep wakeup timer */
    wakeup_timer_init();

    /* Enable global interrupts */
    __enable_irq();
}

/*******************************************************************************
 * Function Name: ble_findme_process
 ********************************************************************************
 * Summary:
 *  This function processes the BLE events and configures the device to enter
 *  low power mode as required.
 *
 *******************************************************************************/
void ble_findme_process(void)
{
    /* Enter low power mode. The call to enter_low_power_mode also causes the
     * device to enter hibernate mode if the BLE stack is shutdown.
     */
    enter_low_power_mode();

    /* Cy_BLE_ProcessEvents() allows the BLE stack to process pending events */
    Cy_BLE_ProcessEvents();

    if (wakeup_intr_flag)
    {
        wakeup_intr_flag = false;

        /* Update ECE453_USR_LED to indicate current BLE status */
        if (CY_BLE_ADV_STATE_ADVERTISING == Cy_BLE_GetAdvertisementState())
        {
            cyhal_gpio_toggle((cyhal_gpio_t)ECE453_USR_LED);
        }
        else if (CY_BLE_CONN_STATE_CONNECTED == Cy_BLE_GetConnectionState(app_conn_handle))
        {
            cyhal_gpio_write((cyhal_gpio_t)ECE453_USR_LED, CYBSP_LED_STATE_ON);
        }
        else
        {
            cyhal_gpio_write((cyhal_gpio_t)ECE453_USR_LED, CYBSP_LED_STATE_OFF);
        }

        /* Update CYBSP_USER_LED2 to indicate current alert level */
        switch (alert_level)
        {
        case CY_BLE_NO_ALERT:
        {
            // cyhal_gpio_write((cyhal_gpio_t)CYBSP_USER_LED2, CYBSP_LED_STATE_OFF);
            break;
        }
        case CY_BLE_MILD_ALERT:
        {
            // cyhal_gpio_toggle((cyhal_gpio_t)CYBSP_USER_LED2);
            break;
        }
        case CY_BLE_HIGH_ALERT:
        {
            // cyhal_gpio_write((cyhal_gpio_t)CYBSP_USER_LED2, CYBSP_LED_STATE_ON);
            break;
        }
        default:
        {
            break;
        }
        }
    }
}

/*******************************************************************************
 * Function Name: ble_init
 ********************************************************************************
 * Summary:
 *  This function initializes the BLE and registers IAS callback function.
 *
 *******************************************************************************/
static void ble_init(void)
{
    static const cy_stc_sysint_t bless_isr_config =
        {
            /* The BLESS interrupt */
            .intrSrc = bless_interrupt_IRQn,

            /* The interrupt priority number */
            .intrPriority = BLESS_INTR_PRIORITY};

    /* Hook interrupt service routines for BLESS */
    (void)Cy_SysInt_Init(&bless_isr_config, bless_interrupt_handler);

    /* Store the pointer to blessIsrCfg in the BLE configuration structure */
    cy_ble_config.hw->blessIsrConfig = &bless_isr_config;

    /* Registers the generic callback functions  */
    Cy_BLE_RegisterEventCallback(stack_event_handler);

    /* Initializes the BLE host */
    Cy_BLE_Init(&cy_ble_config);

    /* Enables BLE */
    Cy_BLE_Enable();

    /* Enables BLE Low-power mode (LPM)*/
    Cy_BLE_EnableLowPowerMode();

    /* Register IAS event handler */
    Cy_BLE_IAS_RegisterAttrCallback(ble_ias_callback);
}

/******************************************************************************
 * Function Name: bless_interrupt_handler
 *******************************************************************************
 * Summary:
 *  Wrapper function for handling interrupts from BLESS.
 *
 ******************************************************************************/
static void bless_interrupt_handler(void)
{
    Cy_BLE_BlessIsrHandler();
}

/*******************************************************************************
 * Function Name: stack_event_handler
 ********************************************************************************
 *
 * Summary:
 *   This is an event callback function to receive events from the BLE Component.
 *
 * Parameters:
 *  uint32 event:      event from the BLE component
 *  void* eventParam:  parameters related to the event
 *
 *******************************************************************************/
static void stack_event_handler(uint32_t event, void *eventParam)
{
    switch (event)
    {
    /**********************************************************************
     * General events
     *********************************************************************/

    /* This event is received when the BLE stack is started */
    case CY_BLE_EVT_STACK_ON:
    {
        printf("[INFO] : BLE stack started \r\n");
        ble_start_advertisement();
        break;
    }

    /* This event is received when there is a timeout */
    case CY_BLE_EVT_TIMEOUT:
    {
        /* Reason for Timeout */
        cy_en_ble_to_reason_code_t reason_code =
            ((cy_stc_ble_timeout_param_t *)eventParam)->reasonCode;

        switch (reason_code)
        {
        case CY_BLE_GAP_ADV_TO:
        {
            printf("[INFO] : Advertisement timeout event \r\n");
            break;
        }
        case CY_BLE_GATT_RSP_TO:
        {
            printf("[INFO] : GATT response timeout\r\n");
            break;
        }
        default:
        {
            printf("[INFO] : BLE timeout event\r\n");
            break;
        }
        }
        break;
    }

    /* This event indicates completion of Set LE event mask */
    case CY_BLE_EVT_LE_SET_EVENT_MASK_COMPLETE:
    {
        printf("[INFO] : Set LE mask event mask command completed\r\n");
        break;
    }

    /* This event indicates set device address command completed */
    case CY_BLE_EVT_SET_DEVICE_ADDR_COMPLETE:
    {
        printf("[INFO] : Set device address command has completed \r\n");
        break;
    }

    /* This event indicates set Tx Power command completed */
    case CY_BLE_EVT_SET_TX_PWR_COMPLETE:
    {
        printf("[INFO] : Set Tx power command completed\r\n");
        break;
    }

    /* This event indicates BLE Stack Shutdown is completed */
    case CY_BLE_EVT_STACK_SHUTDOWN_COMPLETE:
    {
        printf("[INFO] : BLE shutdown complete\r\n");
        break;
    }

    /**********************************************************************
     * GAP events
     *********************************************************************/

    /* This event is generated at the GAP Peripheral end after connection
     * is completed with peer Central device
     */
    case CY_BLE_EVT_GAP_DEVICE_CONNECTED:
    {
        printf("[INFO] : GAP device connected \r\n");
        break;
    }
    /* This event is triggered instead of 'CY_BLE_EVT_GAP_DEVICE_CONNECTED',
     * if Link Layer Privacy is enabled in component customizer
     */
    case CY_BLE_EVT_GAP_ENHANCE_CONN_COMPLETE:
    {
        printf("[INFO] : GAP enhanced connection complete \r\n");
        break;
    }

    /* This event is generated when disconnected from remote device or
     * failed to establish connection
     */
    case CY_BLE_EVT_GAP_DEVICE_DISCONNECTED:
    {
        if (CY_BLE_CONN_STATE_DISCONNECTED ==
            Cy_BLE_GetConnectionState(app_conn_handle))
        {
            printf("[INFO] : GAP device disconnected\r\n");
            alert_level = CY_BLE_NO_ALERT;
            ble_start_advertisement();
        }
        break;
    }

    /* This event indicates that the GAP Peripheral device has
     * started/stopped advertising
     */
    case CY_BLE_EVT_GAPP_ADVERTISEMENT_START_STOP:
    {
        if (CY_BLE_ADV_STATE_ADVERTISING == Cy_BLE_GetAdvertisementState())
        {
            printf("[INFO] : BLE advertisement started\r\n");
        }
        else
        {
            printf("[INFO] : BLE advertisement stopped\r\n");

            Cy_BLE_Disable();
        }
        break;
    }

    /**********************************************************************
     * GATT events
     *********************************************************************/

    /* This event is generated at the GAP Peripheral end after connection
     * is completed with peer Central device
     */
    case CY_BLE_EVT_GATT_CONNECT_IND:
    {
        app_conn_handle = *(cy_stc_ble_conn_handle_t *)eventParam;
        printf("[INFO] : GATT device connected\r\n");
        break;
    }

    /* This event is generated at the GAP Peripheral end after disconnection */
    case CY_BLE_EVT_GATT_DISCONNECT_IND:
    {
        printf("[INFO] : GATT device disconnected\r\n");
        break;
    }

    /* This event indicates that the 'GATT MTU Exchange Request' is received */
    case CY_BLE_EVT_GATTS_XCNHG_MTU_REQ:
    {
        printf("[INFO] : GATT MTU Exchange Request received \r\n");
        break;
    }

    /* This event received when GATT read characteristic request received */
    /* ECE453 Read Characteristic START*/
    case CY_BLE_EVT_GATTS_READ_CHAR_VAL_ACCESS_REQ:
    {
        // int gps_val = 69;
        printf("[INFO] : GATT read characteristic request received \r\n");
        read_req_param = (cy_stc_ble_gatts_char_val_read_req_t *)eventParam;

        if (CY_BLE_BUTTONS_USR_BTN_CHAR_HANDLE == read_req_param->attrHandle)
        {

            uint8_t little_endian[sizeof(int32_t)] = {0};
            //              012345678
            // gps_latitude = "1234.5678";
            char *gps_latitude_0 = malloc(sizeof(char));
            char *gps_latitude_1 = malloc(sizeof(char));
            char *gps_latitude_2 = malloc(sizeof(char));
            char *gps_latitude_3 = malloc(sizeof(char));
            // 4 is period
            char *gps_latitude_5 = malloc(sizeof(char));
            char *gps_latitude_6 = malloc(sizeof(char));
            char *gps_latitude_7 = malloc(sizeof(char));
            char *gps_latitude_8 = malloc(sizeof(char));
            *gps_latitude_0 = gps_latitude[0];
            *gps_latitude_1 = gps_latitude[1];
            *gps_latitude_2 = gps_latitude[2];
            *gps_latitude_3 = gps_latitude[3];
            *gps_latitude_5 = gps_latitude[5];
            *gps_latitude_6 = gps_latitude[6];
            *gps_latitude_7 = gps_latitude[7];
            *gps_latitude_8 = gps_latitude[8];
            little_endian[0] = atoi(gps_latitude_0) * 10 + atoi(gps_latitude_1);
            little_endian[1] = atoi(gps_latitude_2) * 10 + atoi(gps_latitude_3);
            little_endian[2] = atoi(gps_latitude_5) * 10 + atoi(gps_latitude_6);
            little_endian[3] = atoi(gps_latitude_7) * 10 + atoi(gps_latitude_8);

            CY_BLE_GATT_DB_ATTR_SET_GEN_VALUE(CY_BLE_BUTTONS_USR_BTN_CHAR_HANDLE, little_endian, sizeof(int32_t));
            free(gps_latitude_0);
            free(gps_latitude_1);
            free(gps_latitude_2);
            free(gps_latitude_3);
            free(gps_latitude_5);
            free(gps_latitude_6);
            free(gps_latitude_7);
            free(gps_latitude_8);

            // printf("[INFO] : BTN_COUNT %d\r\n", dummy);
            printf("[INFO] GPS : LATITUTDE READ:\r\n");
            printf("-----------------------------------------------------\r\n");
        }
        else if (CY_BLE_LONGI_USR_LONGI_CHAR_HANDLE == read_req_param->attrHandle)
        {

            uint8_t little_endian[sizeof(int32_t)] = {0};
            //              012345678
            gps_latitude = "1234.5678";
            char *gps_longitude_0 = malloc(sizeof(char));
            char *gps_longitude_1 = malloc(sizeof(char));
            char *gps_longitude_2 = malloc(sizeof(char));
            char *gps_longitude_3 = malloc(sizeof(char));
            char *gps_longitude_5 = malloc(sizeof(char));
            char *gps_longitude_6 = malloc(sizeof(char));
            char *gps_longitude_7 = malloc(sizeof(char));
            char *gps_longitude_8 = malloc(sizeof(char));
            *gps_longitude_0 = gps_longitude[0];
            *gps_longitude_1 = gps_longitude[1];
            *gps_longitude_2 = gps_longitude[2];
            *gps_longitude_3 = gps_longitude[3];
            *gps_longitude_5 = gps_longitude[5];
            *gps_longitude_6 = gps_longitude[6];
            *gps_longitude_7 = gps_longitude[7];
            *gps_longitude_8 = gps_longitude[8];
            little_endian[0] = atoi(gps_longitude_0) * 10 + atoi(gps_longitude_1);
            little_endian[1] = atoi(gps_longitude_2) * 10 + atoi(gps_longitude_3);
            little_endian[2] = atoi(gps_longitude_5) * 10 + atoi(gps_longitude_6);
            little_endian[3] = atoi(gps_longitude_7) * 10 + atoi(gps_longitude_8);

            CY_BLE_GATT_DB_ATTR_SET_GEN_VALUE(CY_BLE_LONGI_USR_LONGI_CHAR_HANDLE, little_endian, sizeof(int32_t));
            free(gps_longitude_0);
            free(gps_longitude_1);
            free(gps_longitude_2);
            free(gps_longitude_3);
            free(gps_longitude_5);
            free(gps_longitude_6);
            free(gps_longitude_7);
            free(gps_longitude_8);

            // printf("[INFO] : BTN_COUNT %d\r\n", dummy);
            printf("[INFO] GPS : LONGITUDE READ:\r\n");
            printf("-----------------------------------------------------\r\n");
        }
        else if (CY_BLE_NS_USR_NS_CHAR_HANDLE == read_req_param->attrHandle)
        {

            CY_BLE_GATT_DB_ATTR_SET_GEN_VALUE(CY_BLE_NS_USR_NS_CHAR_HANDLE, gps_N_1_S_0, 1);
            printf("[INFO] GPS : N/S READ:\r\n");
            printf("-----------------------------------------------------\r\n");
        }
        else if (CY_BLE_EW_USR_EW_CHAR_HANDLE == read_req_param->attrHandle)
        {

            CY_BLE_GATT_DB_ATTR_SET_GEN_VALUE(CY_BLE_EW_USR_EW_CHAR_HANDLE, gps_E_1_W_0, 1);
            printf("[INFO] GPS : E/W READ:\r\n");
            printf("-----------------------------------------------------\r\n");
        }
        else if (CY_BLE_ACCLC_USR_ACCL_CHAR_HANDLE == read_req_param->attrHandle)
        {
            // send_crash_ble = 1;
            // uint8_t little_endian[sizeof(int8_t)] = {0};
            // little_endian[0] = 1;
            CY_BLE_GATT_DB_ATTR_SET_GEN_VALUE(CY_BLE_ACCLC_USR_ACCL_CHAR_HANDLE, send_crash_ble, 1);
            printf("[INFO] BLE ACCLEROMETER READ : CRASH EVENT READ FROM PSOC send_crash_ble VALUE: %d\r\n", send_crash_ble);
            printf("-----------------------------------------------------\r\n");
        }

        break;
    }
    /* ECE453 Read Characteristic END*/

    /* ECE453 Write Characteristic START*/
    case CY_BLE_EVT_GATTS_WRITE_CMD_REQ:
    {
        write_req_param = (cy_stc_ble_gatt_write_param_t *)eventParam;

        if (CY_BLE_DRIVEF_USR_DRIVE_CHAR_HANDLE == write_req_param->handleValPair.attrHandle)
        {
            ble_duty_cycle_drive_train = write_req_param->handleValPair.value.val[0];
            motor_pwm_set(&pwm_obj_drive_train, PWM_FREQUENCY, ble_duty_cycle_drive_train, true, true, true);
            printf("[INFO] F : GATT write FORWARD with value(decimel): %d\r\n", ble_duty_cycle_drive_train);
            printf("[INFO] F : GATT write FORWARD with value(Hex    ): %x\r\n", ble_duty_cycle_drive_train);
            printf("-----------------------------------------------------\r\n");
        }
        else if (CY_BLE_DRIVEB_USR_DRIVE_CHAR_HANDLE == write_req_param->handleValPair.attrHandle)
        {
            ble_duty_cycle_drive_train = write_req_param->handleValPair.value.val[0];
            motor_pwm_set(&pwm_obj_drive_train, PWM_FREQUENCY, ble_duty_cycle_drive_train, true, true, false);
            printf("[INFO] B : GATT write BACKWARD with value(decimel): %d\r\n", ble_duty_cycle_drive_train);
            printf("[INFO] B : GATT write BACKWARD with value(Hex    ): %x\r\n", ble_duty_cycle_drive_train);
            printf("-----------------------------------------------------\r\n");
        }
        else if (CY_BLE_SERVOR_USR_SERVO_CHAR_HANDLE == write_req_param->handleValPair.attrHandle)
        {
            ble_duty_cycle_servo = write_req_param->handleValPair.value.val[0];
            servo_pwm_set(&pwm_obj_servo, PWM_FREQUENCY, ble_duty_cycle_servo, true, true);
            printf("[INFO] R : GATT write RIGHT with value (deicmel): %d\r\n", ble_duty_cycle_servo);
            printf("[INFO] R : GATT write RIGHT with value (hex    ): %x\r\n", ble_duty_cycle_servo);
            printf("-----------------------------------------------------\r\n");
        }
        else if (CY_BLE_SERVOL_USR_SERVO_CHAR_HANDLE == write_req_param->handleValPair.attrHandle)
        {
            ble_duty_cycle_servo = write_req_param->handleValPair.value.val[0];
            servo_pwm_set(&pwm_obj_servo, PWM_FREQUENCY, ble_duty_cycle_servo, true, false);
            printf("[INFO] L : GATT write LEFT  with value (deicmel): %d\r\n", ble_duty_cycle_servo);
            printf("[INFO] L : GATT write LEFT  with value (hex    ): %x\r\n", ble_duty_cycle_servo);
            printf("-----------------------------------------------------\r\n");
        }
        else if (CY_BLE_SERVOO_USR_SERVO_CHAR_HANDLE == write_req_param->handleValPair.attrHandle)
        {
            ble_duty_cycle_servo = write_req_param->handleValPair.value.val[0];
            servo_pwm_set(&pwm_obj_servo, PWM_FREQUENCY, 0, false, false);
            printf("[INFO] O : GATT write nothing with value (deicmel): %d\r\n", 0);
            printf("[INFO] O : GATT write nothing with value (hex    ): %x\r\n", 0);
            printf("-----------------------------------------------------\r\n");
        }
        else if (CY_BLE_ACCLA_USR_ACCL_CHAR_HANDLE == write_req_param->handleValPair.attrHandle)
        {
            // send_crash_ble = 0;
            printf("[INFO] BLE ACCLEROMETER WRITE : CRASH EVENT ACKNOWLEDGED FROM PHONE send_crash_ble set to 0. send_crash_ble: %d\r\n", send_crash_ble);
            printf("-----------------------------------------------------\r\n");
        }
        else if (CY_BLE_TURRET_USR_TURRET_CHAR_HANDLE == write_req_param->handleValPair.attrHandle)
        {
            turret_fire = write_req_param->handleValPair.value.val[0];
            ;
            printf("[INFO] TURRET : VALUE sent to P5_3 :: %d\r\n", turret_fire);
            printf("-----------------------------------------------------\r\n");
        }
        break;
    }
    /* ECE453 Write Characteristic END*/
    default:
    {
        printf("[INFO] : BLE Event 0x%lX\r\n", (unsigned long)event);
    }
    }
}

/*******************************************************************************
 * Function Name: ble_ias_callback
 ********************************************************************************
 * Summary:
 *  This is an event callback function to receive events from the BLE, which are
 *  specific to Immediate Alert Service.
 *
 * Parameters:
 *  uint32 event:      event from the BLE component
 *  void* eventParams: parameters related to the event
 *
 *******************************************************************************/
void ble_ias_callback(uint32 event, void *eventParam)
{
    /* Alert Level Characteristic write event */
    if (event == CY_BLE_EVT_IASS_WRITE_CHAR_CMD)
    {
        /* Read the updated Alert Level value from the GATT database */
        Cy_BLE_IASS_GetCharacteristicValue(CY_BLE_IAS_ALERT_LEVEL,
                                           sizeof(alert_level), &alert_level);
    }

    /* Remove warning for unused parameter */
    (void)eventParam;
}

/******************************************************************************
 * Function Name: ble_start_advertisement
 *******************************************************************************
 * Summary:
 *  This function starts the advertisement.
 *
 ******************************************************************************/
static void ble_start_advertisement(void)
{
    cy_en_ble_api_result_t ble_api_result;

    if ((CY_BLE_ADV_STATE_ADVERTISING != Cy_BLE_GetAdvertisementState()) &&
        (Cy_BLE_GetNumOfActiveConn() < CY_BLE_CONN_COUNT))
    {
        ble_api_result = Cy_BLE_GAPP_StartAdvertisement(
            CY_BLE_ADVERTISING_FAST,
            CY_BLE_PERIPHERAL_CONFIGURATION_0_INDEX);

        if (CY_BLE_SUCCESS != ble_api_result)
        {
            printf("[ERROR] : Failed to start advertisement \r\n");
        }
    }
}

/*******************************************************************************
 * Function Name: wakeup_timer_interrupt_handler
 ********************************************************************************
 * Summary:
 *  wakeup_timer interrupt handler.
 *
 * Parameters:
 *  void *handler_arg (unused)
 *  cyhal_lptimer_irq_event_t event (unused)
 *
 *******************************************************************************/
void wakeup_timer_interrupt_handler(void *handler_arg, cyhal_lptimer_event_t event)
{
    /* Set the interrupt flag */
    wakeup_intr_flag = true;

    /* Reload the timer to get periodic interrupt */
    cyhal_lptimer_reload(&wakeup_timer);
    cyhal_lptimer_set_match(&wakeup_timer, WAKEUP_TIMER_MATCH_VALUE);
}

/*******************************************************************************
 * Function Name: wakeup_timer_init
 ********************************************************************************
 * Summary:
 *  Initialize deep sleep wakeup timer for generating interrupt.
 *
 *******************************************************************************/
static void wakeup_timer_init(void)
{
    cyhal_lptimer_init(&wakeup_timer);
    cyhal_lptimer_set_match(&wakeup_timer, WAKEUP_TIMER_MATCH_VALUE);
    cyhal_lptimer_reload(&wakeup_timer);
    cyhal_lptimer_register_callback(&wakeup_timer, wakeup_timer_interrupt_handler, NULL);
    cyhal_lptimer_enable_event(&wakeup_timer, CYHAL_LPTIMER_COMPARE_MATCH,
                               WAKEUP_INTR_PRIORITY, true);
}

/*******************************************************************************
 * Function Name: enter_low_power_mode
 ********************************************************************************
 * Summary:
 *  Configures the device to enter low power mode.
 *
 *  The function configures the device to enter deep sleep - whenever the
 *  BLE is idle and the UART transmission/reception is not happening.
 *
 *  In case if BLE is  turned off, the function configures the device to
 *  enter hibernate mode.
 *
 *******************************************************************************/
static void enter_low_power_mode(void)
{
    /* Enter hibernate mode if BLE is turned off  */
    if (CY_BLE_STATE_STOPPED == Cy_BLE_GetState())
    {
        printf("[INFO] : Entering hibernate mode\r\n");

        /* Turn of user LEDs */
        cyhal_gpio_write((cyhal_gpio_t)ECE453_USR_LED, CYBSP_LED_STATE_OFF);
        // cyhal_gpio_write((cyhal_gpio_t)CYBSP_USER_LED2, CYBSP_LED_STATE_OFF);

        /* Wait until UART transfer complete  */
        while (1UL == cyhal_uart_is_tx_active(&cy_retarget_io_uart_obj))
            ;
        cyhal_syspm_hibernate(CYHAL_SYSPM_HIBERNATE_PINB_LOW);
    }
    else
    {
        cyhal_syspm_deepsleep();
    }
}

/* [] END OF FILE */

// #include <stdint.h>
// #include <stdbool.h>
// #include "cy_ble.h"

// // Handle of the attribute to write
// cy_ble_gatt_db_attr_handle_t attrHandle;

// // 32-bit integer value to write
// int32_t intValue = 12345678;

// // Convert the integer value to a byte array
// uint8_t byteArray[4];
// byteArray[0] = (intValue >> 24) & 0xFF;
// byteArray[1] = (intValue >> 16) & 0xFF;
// byteArray[2] = (intValue >> 8) & 0xFF;
// byteArray[3] = intValue & 0xFF;

// // Write the byte array to the attribute
// cy_stc_ble_gatt_handle_value_param_t writeParam =
// {
//     .attrHandle = attrHandle,
//     .offset = 0,
//     .value = byteArray,
//     .valueLen = 4
// };
// cy_en_ble_api_result_t writeResult = Cy_BLE_GATTS_WriteAttributeValueLocal(&writeParam);

// if (writeResult != CY_BLE_SUCCESS)
// {
//     // Handle error
// }
