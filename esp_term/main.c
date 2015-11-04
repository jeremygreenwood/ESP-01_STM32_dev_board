#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include "at_parser.h"
#include "http_parser_interface.h"
#include "uart.h"
#include "dbg_log.h"
#include "cb.h"

/**************************************************
    Defines & Memory Consts
**************************************************/
#define cnt_of_array(ary)          (sizeof(ary) / sizeof(ary[0]))
#define MAIN_BUF_SIZE 1024
#define CB_REQ_NUM 30

/**************************************************
    Types
**************************************************/

/**************************************************
    Prototypes
**************************************************/

/**************************************************
    Globals etc
**************************************************/
at_parser_state_type    at_state;

/**************************************************
    main
**************************************************/
int main
    ( 
    int     argn, 
    char  **argv
    )
{
/*---------------------------------- 
Local variables
---------------------------------- */
static char             main_buf[MAIN_BUF_SIZE];
static char             at_memory[CB_REQ_NUM * sizeof(at_cb_request_type)];
int                     read_ret;
int                     at_processed;
int                     buf_curr;
int                     ret;

/*---------------------------------- 
Setup up at-parser & http-parser
---------------------------------- */
at_init_parser(&at_state, at_memory, CB_REQ_NUM);
http_init_parser();

/* Setup uart */
if( uart_open(HW_SPECIFIC_PATH, 0) < 0 )
    {
    dbg_log("Failed to open device\n");
    return(-1);
    }

/*---------------------------------- 
Reset CB, does setup
---------------------------------- */
at_quick_submit( &at_state, AT_CMD_RST, cb_setup, AT_CB_STANDING_TRANSIENT );

/*---------------------------------- 
CB for HTTP requests
---------------------------------- */
at_quick_submit( &at_state, AT_CMD_IPD, cb_ipd, AT_CB_STANDING_PERSISTENT );

/*---------------------------------- 
Start setup sequence
---------------------------------- */
at_send_cmd(AT_CMD_RST);

/*---------------------------------- 
Do main forever
---------------------------------- */
buf_curr = 0;
for(;;)
    {
    sleep(1);
    read_ret = uart_read(&main_buf[buf_curr], MAIN_BUF_SIZE - buf_curr);
    if(read_ret > 0 )
        {
        // dbg_log("uart_read[%d]: --%s--\n", read_ret, main_buf);
        buf_curr += read_ret;
        }
    if( buf_curr > 0 )
        {
        at_processed = at_process(&at_state, main_buf, buf_curr);
        buf_curr -= at_processed;
        memmove(main_buf, &main_buf[at_processed], buf_curr );
        }
    }

return(ret);
}
















