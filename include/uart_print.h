#ifndef _USART_PRINT_H
#define _USART_PRINT_H

#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_usart.h"
#include <stdio.h>


void uart_clock_setup( void );
void uart_init( uint32_t baud_rate );
void uart_setup( uint32_t baud_rate );
void uart_test( void );
void uart_write_buf( char *buf, uint16_t count );
void uart_write_byte( uint8_t byte );
void uart_write_msg( char *msg );
void uart_write_str( char *str );

/*--------------------------------------------------------
TODO
  * add functionality to read data from UART 1
  * consider creating DMA or interrupt driven UART functionality
--------------------------------------------------------*/

#endif
