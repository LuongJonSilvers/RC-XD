/*
 * light_sensor.c
 *
 *  Created on: Nov 9, 2022
 *      Author: sreen
 */

#include "light_sensor.h"

/** Write a register on the TSL25911FN (Light Sensor)
 *
 * @param reg The reg address to read
 * @param value The value to be written
 *
 */
void TSL25911FN_write_reg(uint8_t reg, uint8_t value)
{
	cy_rslt_t result;
	uint8_t write_buffer[2];

	/* Initialize write_buffer[] so that the register address and value are transmitted */
	write_buffer[0] = reg;
	write_buffer[1] = value;

	/* Use cyhal_i2c_master_write to write the required data to the device. */
	result = cyhal_i2c_master_write(&i2c_master_obj, TSL25911FN_SUBORDINATE_ADDR, write_buffer, TSL25911FN_WRITE_PACKET_SIZE, 1, true );
	if (result != CY_RSLT_SUCCESS)
	{
		printf("I2C write :: Accelerometer :: failed");
		CY_ASSERT(0);
	}
}


/** Read a register on the TSL25911FN (Light Sensor)
 *
 * @param reg The reg address to read
 *
 */
uint8_t TSL25911FN_read_reg(uint8_t reg)
{
	cy_rslt_t result;
	uint8_t write_buffer[1];
	uint8_t read_buffer[1];

	/* Use cyhal_i2c_master_write to write the register address to the device. */
	/* Send the register address, do not generate a stop condition.  This will result in */
	/* a restart condition. */
	write_buffer[0] = reg;
	result = cyhal_i2c_master_write(&i2c_master_obj, TSL25911FN_SUBORDINATE_ADDR, write_buffer, TSL25911FN_WRITE_PACKET_SIZE/2, 1, false );
	if (result != CY_RSLT_SUCCESS)
	{
		printf("I2C write in write :: Accelerometer :: failed");
		CY_ASSERT(0);
	}

	/* Use cyhal_i2c_master_read to read the required data from the device. */
	// The register address has already been set in the write above, so read a single byte of data.
	cyhal_i2c_master_read(&i2c_master_obj, TSL25911FN_SUBORDINATE_ADDR, read_buffer, TSL25911FN_WRITE_PACKET_SIZE/2, 1, true );
	if (result != CY_RSLT_SUCCESS)
	{
		printf("I2C read in write  :: Accelerometer :: failed");
		CY_ASSERT(0);
	}

	// Return the data
	return read_buffer[0];
}

