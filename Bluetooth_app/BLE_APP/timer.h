/*
 * timer.h
 *
 *  Created on: Nov 20, 2022
 *      Author: sreen
 */

#ifndef TIMER_H_
#define TIMER_H_


#include <stdint.h>
#include <stdbool.h>
#include "cy_pdl.h"
#include "cyhal.h"
#include "cybsp.h"
#include "i2c.h"
#include "stdio.h"

extern volatile bool global_timer_interrupt_flag;
void isr_timer(void *callback_arg, cyhal_timer_event_t event);
void global_timer(void);


#endif /* TIMER_H_ */
