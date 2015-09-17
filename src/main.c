/*----------------------------------------------------------------------
                            INCLUDES
----------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>

#include "timer.h"
#include "led.h"
#include "uart_print.h"

/*----------------------------------------------------------------------
                            CONSTANTS
----------------------------------------------------------------------*/

#define LED_ON_PERCENT	50			/* LED blink percentage 		*/
#define BLINK_ON_TICKS  ( TIMER_FREQUENCY_HZ * LED_ON_PERCENT / 100 )
#define BLINK_OFF_TICKS ( TIMER_FREQUENCY_HZ - BLINK_ON_TICKS )
#define UART1_BAUD_RATE 115200      /* baud rate for UART 1 data    */


#pragma GCC diagnostic ignored "-Wunused-parameter"

/*----------------------------------------------------------------------
                            PROCEDURES
----------------------------------------------------------------------*/


int main( int argc, char* argv[] )
{
    /*--------------------------------------------------------
    Local variables
    --------------------------------------------------------*/
    uint16_t            count;      /* seconds counter              */
    char                cnt_str[ 20 ];
                                    /* temporary count string       */

    /*--------------------------------------------------------
    Initialization
    --------------------------------------------------------*/
    count = 0;
    timer_start();
    led_init();
    uart_init( UART1_BAUD_RATE );
    uart_test();

    uart_write_msg( "Press any key to continue..." );
    uart_wait_rx_ready();

    /*--------------------------------------------------------
    Forever loop
    --------------------------------------------------------*/
	while( 1 )
    {
		blink_led_on();
        timer_sleep( BLINK_ON_TICKS );

		blink_led_off();
        timer_sleep( BLINK_OFF_TICKS );

        sprintf( cnt_str, "%d", count );
        uart_write_msg( cnt_str );

        count++;
    }
}


