#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <errno.h>

#include "http-parser/http_parser.h"
#include "at-parser/esp-at.h"

/**************************************************
    Defines
**************************************************/
#define cnt_of_array(ary)          (sizeof(ary) / sizeof(ary[0]))

/**************************************************
    Types
**************************************************/

/**************************************************
    Prototypes
**************************************************/

/**************************************************
    Callback Prototypes
**************************************************/

/**************************************************
    Globals etc
**************************************************/





/**************************************************
    main
        This does all the main things.
**************************************************/

int main
    ( 
    void   *argv, 
    int     argn 
    )
{
#define MAIN_BUF_SIZE 1024
char main_buf[MAIN_BUF_SIZE];
#define CB_REQ_NUM 30
char at_memory[CB_REQ_NUM * sizeof(at_cb_request_type)];
at_parser_state_type at_state;
int fd;
int read_ret;
int at_processed;
int process_again;
int buf_curr;

/* Setup up at-parser */
at_init_parser(&at_state, at_memory, CB_REQ_NUM);

/* Setup uart */
fd = uart_open("/dev/ttyUSB1", 0);
if( fd < 0 )
    {
    // Log & exit
    }


/* Setup callbacks */
// int at_submit_cb(at_parser_state_type *p, at_cb_request_type *cb);

buf_curr = 0;
while( 1 )
    {
    read_ret = uart_read(fd, &main_buf[buf_curr], MAIN_BUF_SZ - buf_curr);
    buf_curr += read_ret;
    if( read_ret > 0 )
        {
        at_processed = at_process(at_parser_state_type *p, main_buf, buf_curr);
        process_again = in_size - at_processed;
        memmove(&main_buf[], &main_buf[], 
        
        }
    }
}

/**************************************************
    user_input
**************************************************/


