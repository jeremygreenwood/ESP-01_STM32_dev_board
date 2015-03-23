#ifndef _USART_PRINT_H
#define _USART_PRINT_H

#include <stdio.h>

#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_usart.h"


#define ERR_UART_RX_BUF_FULL	-1
#define ERR_UART_OVERRUN    	-2

/*--------------------------------------------------------
TODO add other UART releated errors (e.g. framing error)
--------------------------------------------------------*/


void uart_init( uint32_t baud_rate );
uint16_t uart_read( void *buf, uint16_t bytes );
uint16_t uart_write( const void *buf, uint16_t bytes );
void uart_write_byte( uint8_t byte );
void uart_write_msg( char *msg );

#endif
