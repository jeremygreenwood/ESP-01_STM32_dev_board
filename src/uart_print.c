/*----------------------------------------------------------------------
                            INCLUDES
----------------------------------------------------------------------*/

#include <string.h>
#include "uart_print.h"


/*--------------------------------------------------------
TESTING
--------------------------------------------------------*/
volatile char StringLoop[] = "The quick brown fox jumps over the lazy dog\r\n";


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
void uart_init( uint32_t baud_rate )
{
    uart_clock_setup();
    uart_irq_setup();
    uart_setup( baud_rate );
}


/*--------------------------------------------------------
Setup UART 1 interrupts
--------------------------------------------------------*/
void uart_irq_setup( void )
{
    /*--------------------------------------------------------
    Local variables
    --------------------------------------------------------*/
    NVIC_InitTypeDef    NVIC_InitStructure;

    /*--------------------------------------------------------
    Enable UART 1 interrupts
    --------------------------------------------------------*/
    USART_ITConfig( USART1, USART_IT_TXE, ENABLE );
    USART_ITConfig( USART1, USART_IT_RXNE, ENABLE );

    /* Configure the NVIC Preemption Priority Bits */
    NVIC_PriorityGroupConfig( NVIC_PriorityGroup_0 );

    /* Enable the USART 1 Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init( &NVIC_InitStructure );
}


/*--------------------------------------------------------
Configure UART 1 baud rate, GPIO, etc
--------------------------------------------------------*/
void uart_setup( uint32_t baud_rate )
{
    /*--------------------------------------------------------
    Local variables
    --------------------------------------------------------*/
    GPIO_InitTypeDef    GPIO_InitStructure;
    USART_InitTypeDef   USART_InitStructure;

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
    USART_InitStructure.USART_BaudRate = baud_rate;
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
Wait for UART 1 receive to have data ready
--------------------------------------------------------*/
void uart_wait_rx_ready( void )
{
    /*--------------------------------------------------------
    This blocks indefinitely waiting for the RX data register
    to have data
    TODO consider adding a timeout to this while loop
    --------------------------------------------------------*/
    while( USART_GetFlagStatus( USART1, USART_FLAG_RXNE ) == RESET );
}


/*--------------------------------------------------------
Wait for UART 1 transmit to become ready
--------------------------------------------------------*/
void uart_wait_tx_ready( void )
{
    /*--------------------------------------------------------
    This blocks indefinitely waiting for the TX data register
    to become empty
    TODO consider adding a timeout to this while loop
    --------------------------------------------------------*/
    while( USART_GetFlagStatus( USART1, USART_FLAG_TXE ) == RESET );
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
    uart_wait_tx_ready();

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


/*--------------------------------------------------------
UART 1 interrupt handler
--------------------------------------------------------*/
void USART1_IRQHandler( void )
{
    /*--------------------------------------------------------
    TESTING
    --------------------------------------------------------*/

    /*--------------------------------------------------------
    Local static variables
    --------------------------------------------------------*/
    static uint16_t     tx_index = 0;
    static uint16_t     rx_index = 0;

    if( USART_GetITStatus( USART1, USART_IT_TXE ) != RESET ) // Transmit the string in a loop
    {
        USART_SendData( USART1, StringLoop[ tx_index++ ] );

        if( tx_index >= ( sizeof(StringLoop) - 1 ) )
        {
            tx_index = 0;
        }
    }

    if( USART_GetITStatus( USART1, USART_IT_RXNE ) != RESET ) // Received characters modify string
    {
        StringLoop[ rx_index++ ] = USART_ReceiveData( USART1 );

        if( rx_index >= ( sizeof(StringLoop) - 1 ) )
        {
            rx_index = 0;
        }
    }
}
