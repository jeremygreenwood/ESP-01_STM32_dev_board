#ifndef _USART_PRINT_H
#define _USART_PRINT_H

#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_usart.h"
#include <stdio.h>


void uart_clock_setup( void );
void uart_init( void );
void uart_setup( void );
void uart_test( void );

#endif
