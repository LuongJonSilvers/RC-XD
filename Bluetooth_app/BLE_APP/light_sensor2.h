
#ifndef LIGHT_SENSOR2_H_
#define LIGHT_SENSOR2_H_


#include <stdint.h>
#include <stdbool.h>
#include "cy_pdl.h"
#include "cyhal.h"
#include "cybsp.h"
#include "i2c.h"

#define OPT3001_SUBORDINATE_ADDR          0x44
#define OPT3001_WRITE_PACKET_SIZE            4

#define OPT3001_CHIP_ID_REG			      0x12
#define OPT3001_RESULT_REG			      0x00
#define OPT3001_CONFIG_REG			      0x01

#define  OPT3001_RST                    0xC810
#define  OPT3001_CONT_MODE              0x0600 // Continuous conversions
#define  OPT3001_CONV_TIME              0x0800 // Convert data every 800 ms
#define  OPT3001_CONV_RDY               0x0080 // When 1 conversion ready
#define  OPT3001_ID_REGISTER			0x3001
#define PIN_LIGHTSENSOR_IRQ			     P10_5
#define INT_PRIORITY_CT		                 3

void OPT3001_write_reg(uint16_t reg, uint16_t value);
uint16_t OPT3001_read_reg(uint8_t reg);
void ambient_light_init2(void);
float OPT3001_read_light(void);
//void light_sensor_io_init(void);
//void light_sensor_irq_init(void);
//void light_sensor_handler(void* handler_arg, cyhal_gpio_event_t event);
//void OPT3001_init(void);

static void isr_timer(void* callback_arg, cyhal_timer_event_t event);
cy_rslt_t snippet_cyhal_timer_event_interrupt();

#endif /* LIGHT_SENSOR2_H_ */
