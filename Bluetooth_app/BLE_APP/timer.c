/*
 * timer.c
 *
 *  Created on: Nov 20, 2022
 *      Author: sreen
 */

#include "timer.h"


volatile bool global_timer_interrupt_flag = false;

/* Timer object used */
cyhal_timer_t global_timer_obj;

void isr_timer(void *callback_arg, cyhal_timer_event_t event)
{
    (void) callback_arg;
    (void) event;
    /* Set the interrupt flag and process it from the application */
    global_timer_interrupt_flag = true;
}


void global_timer(void)
{
    cy_rslt_t rslt;
    const cyhal_timer_cfg_t timer_cfg =
    {
        .compare_value = 0,                 /* Timer compare value, not used */
        .period = 100,                      /* Defines the timer period */
        .direction = CYHAL_TIMER_DIR_UP,    /* Timer counts up */
        .is_compare = false,                /* Don't use compare mode */
        .is_continuous = true,              /* Run the timer indefinitely */
        .value = 0                          /* Initial value of counter */
    };
    /* Initialize the timer object. Does not use pin output ('pin' is NC) and
     * does not use a pre-configured clock source ('clk' is NULL). */
    rslt = cyhal_timer_init(&global_timer_obj, NC, NULL);
    CY_ASSERT(CY_RSLT_SUCCESS == rslt);
    /* Apply timer configuration such as period, count direction, run mode, etc. */
    rslt = cyhal_timer_configure(&global_timer_obj, &timer_cfg);
    CY_ASSERT(CY_RSLT_SUCCESS == rslt);
    /* Set the frequency of timer to 10000 Hz */
    rslt = cyhal_timer_set_frequency(&global_timer_obj, 10000);
    CY_ASSERT(CY_RSLT_SUCCESS == rslt);
    /* Assign the ISR to execute on timer interrupt */
    cyhal_timer_register_callback(&global_timer_obj, isr_timer, NULL);
    /* Set the event on which timer interrupt occurs and enable it */
    cyhal_timer_enable_event(&global_timer_obj, CYHAL_TIMER_IRQ_TERMINAL_COUNT, 3, true);
    /* Start the timer with the configured settings */
    rslt = cyhal_timer_start(&global_timer_obj);
    CY_ASSERT(CY_RSLT_SUCCESS == rslt);
}