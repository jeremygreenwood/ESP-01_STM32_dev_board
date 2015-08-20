#include <stdio.h>
#include "diag/Trace.h"

#include "Timer.h"
#include "led.h"
#include "uart_print.h"


#define LED_ON_PERCENT	10			/* LED blink percentage 		*/
#define BLINK_ON_TICKS  ( TIMER_FREQUENCY_HZ * LED_ON_PERCENT / 100 )
#define BLINK_OFF_TICKS ( TIMER_FREQUENCY_HZ - BLINK_ON_TICKS )

#pragma GCC diagnostic ignored "-Wunused-parameter"


int main( int argc, char* argv[] )
{
    /* initialization */
    timer_start();
    led_init();
    uart_init();
    uart_test();

    /* main loop */
	while( 1 )
    {
		blink_led_on();
        timer_sleep( BLINK_ON_TICKS );

		blink_led_off();
        timer_sleep( BLINK_OFF_TICKS );
    }
}


