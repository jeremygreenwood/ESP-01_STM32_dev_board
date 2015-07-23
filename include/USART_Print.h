#ifndef _USART_PRINT_H
#define _USART_PRINT_H

#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_usart.h"
#include <stdio.h>


/***************************************************************************//**
 * Declare function prototypes
 ******************************************************************************/
void USART_Print(void);
void SetupClock(void);
void SetupUSART(void);

#endif
