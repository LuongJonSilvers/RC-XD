
#include "uart.h"
/* Variable Declarations */
    cy_rslt_t rslt;
    cyhal_uart_t uart_obj;
    uint32_t actualbaud;
    uint8_t tx_buf[TX_BUF_SIZE] = {0xAB,0xCD,0xAD};
    uint8_t rx_buf[RX_BUF_SIZE];
    size_t tx_length = TX_BUF_SIZE;
    size_t rx_length = RX_BUF_SIZE;
    uint32_t value = 'A';
/*    char init_string[5]={b,e,g,i,n};
    char rx_string[5];*/



void uart_init(){
	   /* Initialize the UART configuration structure */
	        const cyhal_uart_cfg_t uart_config =
	        {
	            .data_bits = DATA_BITS_8,
	            .stop_bits = STOP_BITS_1,
	            .parity = CYHAL_UART_PARITY_NONE,
	            .rx_buffer = rx_buf,
	            .rx_buffer_size = RX_BUF_SIZE
	        };


    /* Initialize the UART Block */
    rslt = cyhal_uart_init(&uart_obj, UART_TX, UART_RX,NC,NC,NULL,&uart_config);
    /* Set the baud rate */
    rslt = cyhal_uart_set_baud(&uart_obj, BAUD_RATE, &actualbaud);
//    /* Begin Tx Transfer */
//    cyhal_uart_write(&uart_obj, (void*)tx_buf, &tx_length);
//    cyhal_system_delay_ms(UART_DELAY);
//    /* Send a Character */
//    cyhal_uart_putc(&uart_obj, init_string);
//    cyhal_system_delay_ms(UART_DELAY);
//    /* Begin Rx Transfer */
//    cyhal_uart_read(&uart_obj, (void*)rx_string, &rx_length);
//    cyhal_system_delay_ms(UART_DELAY);
    }

void uart_send_data(){
//uint32_t value = 0xdead;
	/* Begin Tx Transfer */
	    cyhal_uart_write(&uart_obj, (void*)tx_buf, &tx_length);
	    //cyhal_system_delay_ms(UART_DELAY);
	    /* Send a Character */
	    //cyhal_uart_putc(&uart_obj, value);
	    //cyhal_system_delay_ms(UART_DELAY);


}

void uart_receive_data(){

	/* Begin Rx Transfer */
	    cyhal_uart_read(&uart_obj, (void*)rx_buf, &rx_length);
	    //cyhal_system_delay_ms(UART_DELAY);


}



char uart_get_char(void){
char c;
uint32_t value;
	//rslt = cyhal_uart_getc(&uart_obj, character,100);
	cyhal_uart_getc(&uart_obj, (uint8_t*) &value, 1);
	c = (char)value;
	return c;
}



//
//#include "uart.h"
//extern cyhal_uart_t cy_retarget_gps_uart_obj;
//
///*******************************************************************************
//* Function Name: ece453_irq_init
//********************************************************************************
//* Summary:
//* Enables and Registers all interrupt sources for the ECE453 Base project
//*
//* Parameters:
//*  void
//*
//* Return:
//*
//*
//*******************************************************************************/
//
///*******************************************************************************
//* Set up the console retarget IO
//*******************************************************************************/
//static void gps_uart_init_retarget(void)
//{
//
//    /* Initialize retarget-io to use the debug UART port, 8N1 */
//	cy_retarget_io_init(
//			UART_TX,
//			UART_RX,
//			115200);	// Baud Rate
//
//}
//
///*******************************************************************************
//* Function Name: console_init
//********************************************************************************
//* Summary:
//* 	Enables the SCB used for console interface.  Rx interrupts are turned on
//*
//* Parameters:
//*  void
//*
//* Return:
//*
//*
//*******************************************************************************/
//void gps_init(void)
//{
//	gps_uart_init_retarget();
//
//}
//
//
//
//
//
//
//
