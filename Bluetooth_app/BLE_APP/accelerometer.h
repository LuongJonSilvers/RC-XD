/*
 * accelerometer.h
 *
 *  Created on: Nov 9, 2022
 *      Author: grant
 */

#ifndef ACCELEROMETER_H_
#define ACCELEROMETER_H_

#include <stdint.h>
#include <stdbool.h>
#include "cy_pdl.h"
#include "cyhal.h"
#include "cybsp.h"
#include "i2c.h"

#define CRASH_THRESHOLD 2476213760.000000

#define IIS328DQTR_SUBORDINATE_ADDR       0x18
#define IIS328DQTR_WRITE_PACKET_SIZE         2

#define IIS328DQTR_CHIP_ID_REG            0x0F
// //////////// removeme 
// #define light_EN 0x00
// //////////// remove me
#define IIS328DQTR_CTRL_REG1              0x20
#define IIS328DQTR_CTRL_REG2              0x21
#define IIS328DQTR_CTRL_REG3              0x22
#define IIS328DQTR_CTRL_REG4              0x23
#define IIS328DQTR_CTRL_REG5              0x24
#define IIS328DQTR_REFERENCE              0x26
#define IIS328DQTR_STATUS_REG             0x27
#define IIS328DQTR_OUT_X_L                0x28
#define IIS328DQTR_OUT_X_H                0x29
#define IIS328DQTR_OUT_Y_L                0x2A
#define IIS328DQTR_OUT_Y_H                0x2B
#define IIS328DQTR_OUT_Z_L                0x2C
#define IIS328DQTR_OUT_Z_H                0x2D
#define IIS328DQTR_INT1_CFG               0x30
#define IIS328DQTR_INT1_SRC               0x31
#define IIS328DQTR_INT1_THS               0x32
#define IIS328DQTR_INT1_DURATION          0x33
#define IIS328DQTR_INT2_CFG               0x34
#define IIS328DQTR_INT2_SRC               0x35
#define IIS328DQTR_INT2_THS               0x36
#define IIS328DQTR_INT2_DURATION          0x37

void IIS328DQTR_write_reg(uint8_t reg, uint8_t value);
uint8_t IIS328DQTR_read_reg(uint8_t reg);

void accelerometer_IIS328DQTR_init(void);



uint8_t IIS328DQTR_read_chip_id(void);


uint8_t IIS328DQTR_read_CTRL_REG1(void);
uint8_t IIS328DQTR_read_CTRL_REG2(void);
uint8_t IIS328DQTR_read_CTRL_REG3(void);
uint8_t IIS328DQTR_read_CTRL_REG4(void);


///////////////////////////////////////////////////////////
uint8_t IIS328DQTR_read_reg_value(uint8_t reg);
void IIS328DQTR_write_reg_value(uint8_t reg, uint8_t value);
///////////////////////////////////////////////////////////
bool crash_occured(void);

int16_t accelerometer_x_axis (void);
int16_t accelerometer_y_axis (void);
int16_t accelerometer_z_axis (void);




#endif /* ACCELEROMETER_H_ */
