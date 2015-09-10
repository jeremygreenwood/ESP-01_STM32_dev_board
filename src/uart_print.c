/*----------------------------------------------------------------------
                            INCLUDES
----------------------------------------------------------------------*/

#include <string.h>
#include "uart_print.h"


/*--------------------------------------------------------
Setup processor clocks to use UART 1
--------------------------------------------------------*/
void uart_clock_setup( void )
{
    RCC_DeInit(); /* RCC system reset(for debug purpose)*/
    RCC_HSEConfig( RCC_HSE_ON ); /* Enable HSE                         */

    /* Wait till HSE is ready                                               */
    while( RCC_GetFlagStatus( RCC_FLAG_HSERDY ) == RESET );

    RCC_PCLK2Config( RCC_HCLK_Div1 ); /* PCLK2  = HCLK                  */

    /* PLLCLK = 8MHz * 3 = 24 MHz                                           */
    RCC_PLLConfig( 0x00010000, RCC_PLLMul_3 );

    RCC_PLLCmd( ENABLE ); /* Enable PLL                     */

    /* Wait till PLL is ready                                               */
    while( RCC_GetFlagStatus( RCC_FLAG_PLLRDY ) == RESET );

    /* Select PLL as system clock source                                    */
    RCC_SYSCLKConfig( RCC_SYSCLKSource_PLLCLK );

    /* Wait till PLL is used as system clock source                         */
    while( RCC_GetSYSCLKSource() != 0x08 );

    /* Enable USART1 and GPIOA clock                                        */
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE );
}


/*--------------------------------------------------------
Initialize UART 1
--------------------------------------------------------*/
void uart_init( void )
{
    uart_clock_setup();
    uart_setup();
}


/*--------------------------------------------------------
Configure buad rate, etc and setup GPIO for UART 1

TODO update to take baud rate parameter
--------------------------------------------------------*/
void uart_setup( void )
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;

    /* Enable GPIOA clock                                                   */
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA, ENABLE );

    /* Configure USART1 Rx (PA10) as input floating                         */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init( GPIOA, &GPIO_InitStructure );

    /* Configure USART1 Tx (PA9) as alternate function push-pull            */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init( GPIOA, &GPIO_InitStructure );

    /* USART1 configured as follow:
     - BaudRate = 115200 baud
     - Word Length = 8 Bits
     - One Stop Bit
     - No parity
     - Hardware flow control disabled (RTS and CTS signals)
     - Receive and transmit enabled
     - USART Clock disabled
     - USART CPOL: Clock is active low
     - USART CPHA: Data is captured on the middle
     - USART LastBit: The clock pulse of the last data bit is not output to
     the SCLK pin
     */
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init( USART1, &USART_InitStructure );
    USART_Cmd( USART1, ENABLE );
}


/*--------------------------------------------------------
Test UART 1 by writing out a hello world message
--------------------------------------------------------*/
void uart_test( void )
{
    /*--------------------------------------------------------
    Local variables
    --------------------------------------------------------*/
    char                hello_str[] = "Hello, World!";

    /*--------------------------------------------------------
    Write a test message
    --------------------------------------------------------*/
    uart_write_msg( hello_str );
}


/*--------------------------------------------------------
Write buffer data out UART 1
--------------------------------------------------------*/
void uart_write_buf( char *buf, uint16_t num_bytes )
{
    /*--------------------------------------------------------
    Local variables
    --------------------------------------------------------*/
    uint16_t            i;          /* loop counter                 */

    /*--------------------------------------------------------
    Loop over all bytes in the buffer
    --------------------------------------------------------*/
    for( i = 0; i < num_bytes; i++ )
    {
        /*--------------------------------------------------------
        Write the byte out the UART
        --------------------------------------------------------*/
        uart_write_byte( buf[ i ] );
    }
}


/*--------------------------------------------------------
Write a single byte out UART 1
--------------------------------------------------------*/
void uart_write_byte( uint8_t byte )
{
    /*--------------------------------------------------------
    Wait for UART transmit to become ready
    --------------------------------------------------------*/
    while( USART_GetFlagStatus( USART1, USART_FLAG_TXE ) == RESET );

    /*--------------------------------------------------------
    Send byte out UART
    --------------------------------------------------------*/
    USART_SendData( USART1, byte );
}


/*--------------------------------------------------------
Write string message out UART 1 with windows line endings.
The string must be null terminated.
--------------------------------------------------------*/
void uart_write_msg( char *msg )
{
    uart_write_str( msg );
    uart_write_byte( '\n' );
    uart_write_byte( '\r' );
}


/*--------------------------------------------------------
Write string out UART 1. The string must be null
terminated.
--------------------------------------------------------*/
void uart_write_str( char *str )
{
    uart_write_buf( str, strlen( str ) );
}

