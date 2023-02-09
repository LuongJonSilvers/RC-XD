/*
 * light_sensor.h
 *
 *  Created on: Nov 9, 2022
 *      Author: sreen
 */

#ifndef LIGHT_SENSOR_H_
#define LIGHT_SENSOR_H_

#include <stdint.h>
#include <stdbool.h>
#include "cy_pdl.h"
#include "cyhal.h"
#include "cybsp.h"
#include "i2c.h"
#include "stdio.h"

#define TSL25911FN_SUBORDINATE_ADDR       0x29
#define TSL25911FN_WRITE_PACKET_SIZE         2
#define TSL25911FN_CHIP_ID_REG            0x12
#define TSL25911FN_STATUS                 0x13
#define TSL25911FN_ENABLE                 0x00

void TSL25911FN_write_reg(uint8_t reg, uint8_t value);
uint8_t TSL25911FN_read_reg(uint8_t reg);


#endif /* LIGHT_SENSOR_H_ */
