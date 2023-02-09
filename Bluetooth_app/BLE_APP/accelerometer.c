/*
 * accelerometer.c
 *
 *  Created on: Nov 9, 2022
 *      Author: grant
 */

#include "accelerometer.h"
#include "stdio.h"



/** Write a register on the IIS328DQTR (Accelerometer) <><><> WHO AM I <><><>
 *
 * @param reg The reg address to read
 * @param value The value to be written
 *
 */
void IIS328DQTR_write_reg(uint8_t reg, uint8_t value)
{
	cy_rslt_t result;
	uint8_t write_buffer[2];

	/* Initialize write_buffer[] so that the register address and value are transmitted */
	write_buffer[0] = reg;
	write_buffer[1] = value;

	/* Use cyhal_i2c_master_write to write the required data to the device. */
	result = cyhal_i2c_master_write(&i2c_master_obj, IIS328DQTR_SUBORDINATE_ADDR, write_buffer, IIS328DQTR_WRITE_PACKET_SIZE, 1, true);
	if (result != CY_RSLT_SUCCESS)
	{
		printf("I2C write :: Accelerometer :: failed");
		CY_ASSERT(0);
	}
}

/** Read a register on the IIS328DQTR (Accelerometer)
 *
 * @param reg The reg address to read
 *
 */
uint8_t IIS328DQTR_read_reg(uint8_t reg)
{
	cy_rslt_t result;
	uint8_t write_buffer[1];
	uint8_t read_buffer[1];

	/* Use cyhal_i2c_master_write to write the register address to the device. */
	/* Send the register address, do not generate a stop condition.  This will result in */
	/* a restart condition. */
	write_buffer[0] = reg;
	result = cyhal_i2c_master_write(&i2c_master_obj, IIS328DQTR_SUBORDINATE_ADDR, write_buffer, IIS328DQTR_WRITE_PACKET_SIZE / 2, 1, false);
	if (result != CY_RSLT_SUCCESS)
	{
		printf("I2C write in write :: Accelerometer :: failed");
		CY_ASSERT(0);
	}

	/* Use cyhal_i2c_master_read to read the required data from the device. */
	// The register address has already been set in the write above, so read a single byte of data.
	cyhal_i2c_master_read(&i2c_master_obj, IIS328DQTR_SUBORDINATE_ADDR, read_buffer, IIS328DQTR_WRITE_PACKET_SIZE / 2, 1, true);
	if (result != CY_RSLT_SUCCESS)
	{
		printf("I2C read in write  :: Accelerometer :: failed");
		CY_ASSERT(0);
	}

	// Return the data
	return read_buffer[0];
}

/** Read Chip ID Register
 *
 *
 *
 */
uint8_t IIS328DQTR_read_chip_id(void)
{
	return IIS328DQTR_read_reg(IIS328DQTR_CHIP_ID_REG);
}

/** Read CTRL_REG1
 *
 *
 *
 */
uint8_t IIS328DQTR_read_CTRL_REG1(void)
{
	return IIS328DQTR_read_reg(IIS328DQTR_CTRL_REG1);
}

/** Read CTRL_REG2
 *
 *
 *
 */
uint8_t IIS328DQTR_read_CTRL_REG2(void)
{
	return IIS328DQTR_read_reg(IIS328DQTR_CTRL_REG2);
}

/** Read CTRL_REG3
 *
 *
 *
 */
uint8_t IIS328DQTR_read_CTRL_REG3(void)
{
	return IIS328DQTR_read_reg(IIS328DQTR_CTRL_REG3);
}

/** Read CTRL_REG3
 *
 *
 *
 */
uint8_t IIS328DQTR_read_CTRL_REG4(void)
{
	return IIS328DQTR_read_reg(IIS328DQTR_CTRL_REG4);
}

/**
 * initialize the acceleromter peripheral
 */
void accelerometer_IIS328DQTR_init(void)
{
	printf("running .. ");
	// acelerometer is powered down by default. So set it to
	// PM2 PM1 PM0 DR1 DR0 Zen Yen Xen
	//  0   0	1 	0	0	1	1	1 //0x27
	IIS328DQTR_write_reg(IIS328DQTR_CTRL_REG1, 0x27);

	// BOOT HPM1 HPM0 FDS HPen2 HPen1 HPCF1 HPCF0
	//    0	   0    0   1     1     1     1     1 //0x1F
	IIS328DQTR_write_reg(IIS328DQTR_CTRL_REG2, 0x1F);

	// BDU BLE FS1 FS0 STsign 0 ST SIM
	//	1	0	0	0	   0 0  0   0   //0x80
	IIS328DQTR_write_reg(IIS328DQTR_CTRL_REG4, 0x80);
	return;
}

int16_t accelerometer_x_axis(void)
{
	return (int16_t)((IIS328DQTR_read_reg(IIS328DQTR_OUT_X_H) << 8) | IIS328DQTR_read_reg(IIS328DQTR_OUT_X_L));
}

int16_t accelerometer_y_axis(void)
{
	return (int16_t)((IIS328DQTR_read_reg(IIS328DQTR_OUT_Y_H) << 8) | IIS328DQTR_read_reg(IIS328DQTR_OUT_Y_L));
}

int16_t accelerometer_z_axis(void)
{
	return (int16_t)((IIS328DQTR_read_reg(IIS328DQTR_OUT_Z_H) << 8) | IIS328DQTR_read_reg(IIS328DQTR_OUT_Z_L));
}

bool crash_occured(void)
{

	return ((accelerometer_x_axis() * accelerometer_x_axis() + accelerometer_y_axis() * accelerometer_y_axis()) > CRASH_THRESHOLD);
}
