/*
 * usr_btn.c
 *
 *  Created on: Sep 9, 2022
 *      Author: Joe Krachey
 */

#include "usr_btn.h"

#define TIMER_USR_BTN_FREQ	10000
#define TIMER_USR_BTN_PERIOD 100


bool timer_interrupt_flag = false;

// Timer object used
cyhal_timer_t timer_obj;

static void isr_usr_btn_timer(void* callback_arg, cyhal_timer_event_t event)
{
	static uint8_t btn_state = 0xff;

    (void)callback_arg;
    (void)event;

    btn_state = btn_state << 1;

    // The buttons are active low, so a return value of
    // true means the user is not pressing the button.
    if( cyhal_gpio_read(ECE453_USR_BTN) == true)
    {
    	btn_state |= 1;
    }

    // Check to see if the button has been pressed.
    // This is only true when the btn_state is equal to
    // 0x80
    if(btn_state == 0x80)
    {
    	BTN_COUNT++;
    }

}


cy_rslt_t usr_btn_init(void)
{
	cy_rslt_t rslt;

	const cyhal_timer_cfg_t timer_cfg =
	{
		.compare_value = 0,                  // Timer compare value, not used
		.period        = TIMER_USR_BTN_PERIOD ,            // Defines the timer period
		.direction     = CYHAL_TIMER_DIR_UP, // Timer counts up
		.is_compare    = false,              // Don't use compare mode
		.is_continuous = true,               // Run the timer indefinitely
		.value         = 0                   // Initial value of counter
	};

	/* Configure USER_BTN */
	rslt |= cyhal_gpio_init(ECE453_USR_BTN, CYHAL_GPIO_DIR_INPUT,
		                              CYHAL_GPIO_DRIVE_PULLUP, CYBSP_BTN_OFF);

	// Initialize the timer object. Does not use pin output ('pin' is NC) and does not use a
	// pre-configured clock source ('clk' is NULL).
	rslt = cyhal_timer_init(&timer_obj, NC, NULL);
	// Apply timer configuration such as period, count direction, run mode, etc.
	if (CY_RSLT_SUCCESS == rslt)
	{
		rslt = cyhal_timer_configure(&timer_obj, &timer_cfg);
	}
	// Set the frequency of timer to 100 Hz
	if (CY_RSLT_SUCCESS == rslt)
	{
		rslt = cyhal_timer_set_frequency(&timer_obj, TIMER_USR_BTN_FREQ);
	}
	if (CY_RSLT_SUCCESS == rslt)
	{
		// Assign the ISR to execute on timer interrupt
		cyhal_timer_register_callback(&timer_obj, isr_usr_btn_timer, NULL);
		// Set the event on which timer interrupt occurs and enable it
		cyhal_timer_enable_event(&timer_obj, CYHAL_TIMER_IRQ_TERMINAL_COUNT, 3, true);
		// Start the timer with the configured settings
		rslt = cyhal_timer_start(&timer_obj);
	}
	return rslt;
}

