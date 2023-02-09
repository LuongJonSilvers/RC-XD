
#ifndef uart_H_
#define uart_H_
#include "cy_pdl.h"
#include "cyhal.h"
#include "cybsp.h"
#include "cy_retarget_io.h"

#define DATA_BITS_8     8
#define STOP_BITS_1     1
#define BAUD_RATE       9600
#define UART_DELAY      10u
#define RX_BUF_SIZE     50 //
#define TX_BUF_SIZE     6
#define UART_RX 		P9_0
#define UART_TX 		P9_1


/* Public Function API */
void gps_init(void);


void uart_init(void);
void gps_uart_send_data(void);
void uart_receive_data(void);
char uart_get_char(void);
#endif
