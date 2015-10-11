/*----------------------------------------------------------------------
                            INCLUDES
----------------------------------------------------------------------*/

#include <string.h>
#include <stdbool.h>

#include "uart_print.h"

/*----------------------------------------------------------------------
                            CONSTANTS
----------------------------------------------------------------------*/

#define UART_RX_BUF_SZ  1024        /* UART read buffer size        */

/*--------------------------------------------------------
TESTING
--------------------------------------------------------*/
volatile char StringLoop[] = "The quick brown fox jumps over the lazy dog\r\n";

/*----------------------------------------------------------------------
                            TYPES
----------------------------------------------------------------------*/

/*--------------------------------------------------------
TODO convert this to a ring buffer to eliminate possible
overruns (hardware has only a single byte for RX/TX)
--------------------------------------------------------*/
typedef struct                      /* UART interrupt buffer data   */
{
    uint8_t            *buf_ptr_start;
                                    /* pointer to beginning of buffer*/
    uint8_t            *buf_ptr_cur;/* pointer to current spot in buf*/
    uint16_t            buf_sz;     /* size of the buffer            */
    uint16_t            num_bytes;  /* number of bytes in the buffer */
    bool                error_rx_full;
                                    /* UART RX buffer full  		 */
    bool                error_overrun;
                                    /* out of memory error occurred  */
} uart_irq_buf_type;

/*----------------------------------------------------------------------
                            VARIABLES
----------------------------------------------------------------------*/

static uint8_t          s_uart_rx_buf[ UART_RX_BUF_SZ ];
                                    /* UART read buffer             */
static uart_irq_buf_type
                        s_uart_rx_buf_data;
                                    /* UART RX buffer data          */

/*----------------------------------------------------------------------
                            PROCEDURES
----------------------------------------------------------------------*/

/*--------------------------------------------------------
Local functions
--------------------------------------------------------*/
static void uart_irq_buf_reset( uart_irq_buf_type *irq_buf );


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
    Enable UART 1 RX interrupt
    NOTE: use the following line to enable TX interrupt:
    USART_ITConfig( USART1, USART_IT_TXE, ENABLE );
    --------------------------------------------------------*/
    USART_ITConfig( USART1, USART_IT_RXNE, ENABLE );

    /* Configure the NVIC Preemption Priority Bits */
    NVIC_PriorityGroupConfig( NVIC_PriorityGroup_0 );

    /* Enable the USART 1 Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init( &NVIC_InitStructure );

    /*--------------------------------------------------------
    Setup UART RX state data
    --------------------------------------------------------*/
    s_uart_rx_buf_data.buf_sz        = UART_RX_BUF_SZ;
    s_uart_rx_buf_data.num_bytes     = 0;
    s_uart_rx_buf_data.buf_ptr_start = s_uart_rx_buf;
    s_uart_rx_buf_data.buf_ptr_cur   = s_uart_rx_buf;
    s_uart_rx_buf_data.error_rx_full = false;
    s_uart_rx_buf_data.error_overrun = false;
}


/*--------------------------------------------------------
Get UART 1 RX data which has been read via interrupt.
This has a signature similar to read() of unistd.h.
NOTE: this functionality is not protected from interrupts
and should be updated to use a ring buffer.
--------------------------------------------------------*/
uint16_t uart_read( void *buf, uint16_t bytes_req )
{
    /*--------------------------------------------------------
    Local variables
    --------------------------------------------------------*/
    uint16_t            bytes_ret;  /* number of bytes to copy      */
    uint16_t            bytes_rem;  /* number of bytes remaining    */

    /*--------------------------------------------------------
    Check for UART errors
    --------------------------------------------------------*/
    if( s_uart_rx_buf_data.error_rx_full == true )
    {
        /*--------------------------------------------------------
        Clear UART RX buffer data
        --------------------------------------------------------*/
        uart_irq_buf_reset( &s_uart_rx_buf_data );

        /*--------------------------------------------------------
        Return error status
        --------------------------------------------------------*/
        return ERR_UART_RX_BUF_FULL;
    }

    /*--------------------------------------------------------
    Disable UART interrupts while processing.
    NOTE: consider removing when implementing ring buffer.
    --------------------------------------------------------*/
    NVIC_DisableIRQ( USART1_IRQn );

    /*--------------------------------------------------------
    Check if requested number of bytes can be returned
    --------------------------------------------------------*/
    if( bytes_req <= s_uart_rx_buf_data.num_bytes )
    {
        /*--------------------------------------------------------
        Get the requested number of bytes from the UART RX buffer
        --------------------------------------------------------*/
        bytes_ret = bytes_req;
        bytes_rem = s_uart_rx_buf_data.num_bytes - bytes_req;
    }
    else
    {
        /*--------------------------------------------------------
        Get all of the bytes from the UART RX buffer
        --------------------------------------------------------*/
        bytes_ret = s_uart_rx_buf_data.num_bytes;
        bytes_rem = 0;
    }

    /*--------------------------------------------------------
    Copy received data to return buffer
    --------------------------------------------------------*/
    memcpy( buf, s_uart_rx_buf_data.buf_ptr_start, bytes_ret );

    /*--------------------------------------------------------
    Move any remaining buffer data which needs to be copied
    back to the beginning of the buffer. This does nothing if
    bytes remaining is zero.
    NOTE: this will not be necessary once a ring buffer is
    implemented.
    --------------------------------------------------------*/
    memmove( s_uart_rx_buf_data.buf_ptr_start, s_uart_rx_buf_data.buf_ptr_cur, bytes_rem );

    /*--------------------------------------------------------
    Update buffer data
    --------------------------------------------------------*/
    s_uart_rx_buf_data.buf_ptr_cur = s_uart_rx_buf_data.buf_ptr_start;
    s_uart_rx_buf_data.num_bytes   = bytes_rem;

    /*--------------------------------------------------------
    Re-enable UART interrupts.
    --------------------------------------------------------*/
    NVIC_EnableIRQ( USART1_IRQn );

    /*--------------------------------------------------------
    Return number of bytes copied to buffer
    --------------------------------------------------------*/
    return bytes_ret;
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
NOTE: this will block indefinitely
--------------------------------------------------------*/
void uart_wait_rx_ready( void )
{
    /*--------------------------------------------------------
    This blocks indefinitely waiting for the RX data register
    to have data
    --------------------------------------------------------*/
    while( USART_GetFlagStatus( USART1, USART_FLAG_RXNE ) == RESET );
}


/*--------------------------------------------------------
Wait for UART 1 transmit to become ready
NOTE: this will block indefinitely
--------------------------------------------------------*/
void uart_wait_tx_ready( void )
{
    /*--------------------------------------------------------
    This blocks indefinitely waiting for the TX data register
    to become empty
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
UART 1 ISR. This handles both RX and TX interrupts.
--------------------------------------------------------*/
void USART1_IRQHandler( void )
{
    /*--------------------------------------------------------
    Check if UART 1 has an RX data byte
    --------------------------------------------------------*/
    if( USART_GetITStatus( USART1, USART_IT_RXNE ) != RESET )
    {
        /*--------------------------------------------------------
        Check if UART RX buffer is full
        --------------------------------------------------------*/
        if( s_uart_rx_buf_data.num_bytes >= s_uart_rx_buf_data.buf_sz )
        {
            /*--------------------------------------------------------
            Set overrun error
            --------------------------------------------------------*/
            s_uart_rx_buf_data.error_rx_full = true;
        }
        else
        {
            /*--------------------------------------------------------
            Receive data byte
            --------------------------------------------------------*/
            *s_uart_rx_buf_data.buf_ptr_cur = USART_ReceiveData( USART1 );

            /*--------------------------------------------------------
            Advance the current buffer pointer
            --------------------------------------------------------*/
            s_uart_rx_buf_data.buf_ptr_cur++;

            /*--------------------------------------------------------
            Increment the number of bytes in the buffer
            --------------------------------------------------------*/
            s_uart_rx_buf_data.num_bytes++;
        }
    }
}


/*--------------------------------------------------------
Local functions
--------------------------------------------------------*/

/*--------------------------------------------------------
Reset IRQ buffer data
--------------------------------------------------------*/
static void uart_irq_buf_reset( uart_irq_buf_type *irq_buf )
{
    irq_buf->num_bytes     = 0;
    irq_buf->buf_ptr_cur   = irq_buf->buf_ptr_start;
    irq_buf->error_rx_full = false;
    irq_buf->error_overrun = false;
}


