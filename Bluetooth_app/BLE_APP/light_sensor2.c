/*
 * light_sensor2.c
 *
 *  Created on: Nov 20, 2022
 *      Author: grant
 */


#include "light_sensor2.h"
#include <stdio.h>
#include <math.h>
volatile uint16_t lux = 0;
volatile bool ALERT_OPT3001_CHANGE = false;

/** Write a register on the OPT3001 (Light Sensor)
 *
 * @param reg The reg address to read
 * @param value The value to be written
 *
 */
void OPT3001_write_reg(uint16_t reg, uint16_t value)
{
	uint16_t write_buffer[2];

	/* Initialize write_buffer[] so that the register address and value are transmitted */
	write_buffer[0] = reg;
	write_buffer[1] = value;

	/* Use cyhal_i2c_master_write to write the required data to the device. */
	cyhal_i2c_master_write(&i2c_master_obj, OPT3001_SUBORDINATE_ADDR, write_buffer, OPT3001_WRITE_PACKET_SIZE, 1, true );
}


/** Read a register on the OPT3001 (Light Sensor)
 *
 * @param reg The reg address to read
 * @return read_buffer[0] The data within the register
 */
uint16_t OPT3001_read_reg(uint8_t reg)
{
	uint8_t write_buffer[1];
	uint16_t read_buffer[1];

	/* Use cyhal_i2c_master_write to write the register address to the device. */
	/* Send the register address, do not generate a stop condition.  This will result in */
	/* a restart condition. */
	write_buffer[0] = reg;
	cyhal_i2c_master_write(&i2c_master_obj, OPT3001_SUBORDINATE_ADDR, write_buffer, OPT3001_WRITE_PACKET_SIZE, 1, false );

	/* Use cyhal_i2c_master_read to read the required data from the device. */
	// The register address has already been set in the write above, so read a single byte of data.
	cyhal_i2c_master_read(&i2c_master_obj, OPT3001_SUBORDINATE_ADDR, read_buffer, OPT3001_WRITE_PACKET_SIZE, 1, true );

	// Return the data
	return read_buffer[0];
}

/** Initialize the Light Sensor
 *
 * @param void
 *
 */
void ambient_light_init2(void)
{

    // Initialize I2C
    i2c_init();

    // Initialize ambient light sensor

    // Reset the device using the CONFIG register
    OPT3001_write_reg(OPT3001_CONFIG_REG, OPT3001_RST);

    // delay
//    for (i = 0; i < 50000; i++)
//    {
//    };

    // Program the CONFIG register to continuous conversions every 800 ms
    OPT3001_write_reg(OPT3001_CONFIG_REG, OPT3001_CONV_TIME | OPT3001_CONT_MODE);

}

/** Examines the data from the Ambient Light Sensor
 *
 * @param void
 * @return light The value stored within the result register
 */
float OPT3001_read_light(void)
{
    uint16_t res;
    uint16_t light;
    uint16_t E;
    uint16_t R;

    // Read the ambient temperature
    res = OPT3001_read_reg(OPT3001_RESULT_REG);
   // printf("****RES BE LIKE: %d****\n\r",res);
    // Convert result register to lux
    E = (res >> 12);
    R = (res << 4);
    R = (R >> 4);

    light = ((0.08) * (pow(2, E)) * R);
    //printf("****LUX BE LIKE: %f****",light);
    return light;
}

///* Light Sensor Handler Struct */
//cyhal_gpio_callback_data_t lightsensor_cb_data =
//{
//    .callback     = light_sensor_handler,
//    .callback_arg = (void*)NULL
//};
//
///*****************************************************
//* Function Name: LED_io_init
//******************************************************
//* Summary:
//* Initializes the IO pin used to control LED
//* change pin
//*
//* Parameters:
//*  void
//*
//* Return:
//*
//*
//*****************************************************/
//void light_sensor_io_init(void)
//{
//    /* to configure LED Pin as an output */
//
//    cyhal_gpio_init(PIN_LIGHTSENSOR_IRQ, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG, false);
//
//}
//
///***********************************************************
//* Initialize Light Sensor Interrupt IO pin for
//* falling edge interrupts
//************************************************************/
//void light_sensor_irq_init(void)
//{
//
//	/* Register the call back function */
//    cyhal_gpio_register_callback(PIN_LIGHTSENSOR_IRQ, &lightsensor_cb_data);
//    /* Enable the event */
//	 cyhal_gpio_enable_event(PIN_LIGHTSENSOR_IRQ, CYHAL_GPIO_IRQ_FALL, 3, true);
//
//}
//
//
///*******************************************************************************
//* Function Name: light_sensor_handler
//********************************************************************************
//* Summary:
//* Handler for the light sensor
//*
//* Parameters:
//*  void
//*
//* Return:
//*  void
//*
//*******************************************************************************/
//void light_sensor_handler(void* handler_arg, cyhal_gpio_event_t event)
//{
//	ALERT_OPT3001_CHANGE = true;
//
//    lux = OPT3001_read_light();
//
//    if (lux < 50) {
//    	// Turn on light
//    	printf ("On!");
//    }
//    else {
//    	// Turn off light
//    	printf("Off!!!!!!!! HAHAHA USA WINS");
//
//    }
//
//}
//
//void OPT3001_init(void)
//{
//	light_sensor_io_init();
//	light_sensor_irq_init();
//}

bool timer_interrupt_flag_light = false;

// Timer object used
cyhal_timer_t time_obj_light;

static void isr_timer(void* callback_arg, cyhal_timer_event_t event)
{
    (void)callback_arg;
    (void)event;

    lux = OPT3001_read_light();

        if (lux < 50) {
        	// Turn on light
        	printf ("On!");
        }
        else {
        	// Turn off light
        	printf("Off!!!!!!!! HAHAHA USA WINS");

        }

    // Set the interrupt flag and process it from the application
    timer_interrupt_flag_light = true;
}


cy_rslt_t snippet_cyhal_timer_event_interrupt()
{
    cy_rslt_t rslt;

    const cyhal_timer_cfg_t timer_cfg =
    {
        .compare_value = 0,                  // Timer compare value, not used
        .period        = 9999,               // Defines the timer period
        .direction     = CYHAL_TIMER_DIR_UP, // Timer counts up
        .is_compare    = false,              // Don't use compare mode
        .is_continuous = true,               // Run the timer indefinitely
        .value         = 0                   // Initial value of counter
    };

    // Initialize the timer object. Does not use pin output ('pin' is NC) and does not use a
    // pre-configured clock source ('clk' is NULL).
    rslt = cyhal_timer_init(&time_obj_light, NC, NULL);

    // Apply timer configuration such as period, count direction, run mode, etc.
    if (CY_RSLT_SUCCESS == rslt)
    {
        rslt = cyhal_timer_configure(&time_obj_light, &timer_cfg);
    }

    // Set the frequency of timer to 10000 Hz
    if (CY_RSLT_SUCCESS == rslt)
    {
        rslt = cyhal_timer_set_frequency(&time_obj_light, 10000);
    }

    if (CY_RSLT_SUCCESS == rslt)
    {
        // Assign the ISR to execute on timer interrupt
        cyhal_timer_register_callback(&time_obj_light, isr_timer, NULL);

        // Set the event on which timer interrupt occurs and enable it
        cyhal_timer_enable_event(&time_obj_light, CYHAL_TIMER_IRQ_TERMINAL_COUNT, 3, true);

        // Start the timer with the configured settings
        rslt = cyhal_timer_start(&time_obj_light);
    }

    return rslt;
}


