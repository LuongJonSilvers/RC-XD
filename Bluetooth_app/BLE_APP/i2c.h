/*
 * i2c.h
 *
 *  Created on: Jan 21, 2022
 *      Author: Joe Krachey
 */

#ifndef I2C_H_
#define I2C_H	_

#include "cy_pdl.h"
#include "cyhal.h"
#include "cybsp.h"

/* Macros */
#define PIN_MCU_SCL			P6_4
#define PIN_MCU_SDA			P6_5
#define I2C_MASTER_FREQUENCY 100000u

/* Public Global Variables */
extern cyhal_i2c_t i2c_master_obj;

/* Public API */
void i2c_init(void);

#endif /* I2C_H_ */
