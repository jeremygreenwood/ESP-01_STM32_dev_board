#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include "at_parser.h"

/**************************************************
    Defines
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
    Callback Prototypes
**************************************************/
int cb_at(at_return_type *ret);

/**************************************************
    Globals etc
**************************************************/

/**************************************************
    main
**************************************************/

int main
    ( 
    void   *argv, 
    int     argn 
    )
{
static char             main_buf[MAIN_BUF_SIZE];
static char             at_memory[CB_REQ_NUM * sizeof(at_cb_request_type)];
at_parser_state_type    at_state;
int                     fd;
int                     read_ret;
int                     at_processed;
int                     process_again;
int                     buf_curr;
int                     ret;
at_cb_request_type      at_cb_req;


/* Setup up at-parser */
at_init_parser(&at_state, at_memory, CB_REQ_NUM);

/* Setup uart */
fd = uart_open("/dev/ttyUSB0", 0);
if( fd < 0 )
    {
    printf("Failed to open device\n");
    return(-1);
    }

/* Setup callbacks */
at_cb_req.cmd = AT_CMD_AT;
at_cb_req.cb = cb_at;
at_cb_req.standing = AT_CB_STANDING_TRANSIENT;
ret = at_submit_cb(&at_state, &at_cb_req);


buf_curr = 0;
at_send_cmd(at_get_cmd_txt(AT_CMD_AT), strlen(at_get_cmd_txt(AT_CMD_AT)));
do
    {
    read_ret = uart_read(fd, &main_buf[buf_curr], MAIN_BUF_SIZE - buf_curr);
    printf("uart_read[%d]: %s\n", read_ret, main_buf);
    buf_curr += read_ret;
    if( read_ret > 0 )
        {
        at_processed = at_process(&at_state, main_buf, buf_curr);
        buf_curr -= at_processed;
        memmove(main_buf, &main_buf[at_processed], buf_curr );
        }
    } while( 1 ); 
}

/**************************************************
    main
**************************************************/

int cb_at
    ( 
    at_return_type   *ret
    )
{
printf("Got cb_at()\ncmd: %d, status: %d, raw[%d]: --%s--\n",
        ret->cmd, ret->status, ret->raw_size, ret->raw); 

}





