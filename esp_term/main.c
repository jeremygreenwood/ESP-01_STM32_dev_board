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

/**************************************************
    Defines & Memory Consts
**************************************************/
#define cnt_of_array(ary)          (sizeof(ary) / sizeof(ary[0]))
#define MAIN_BUF_SIZE 1024
#define CB_REQ_NUM 30

char resp[] = 
"HTTP/1.1 200 OK\nDate: Fri, 11 Fri 2015 06:25:59 GMT\nContent-Type: text/html\nContent-Length: %u\n\n<html><body><h1>Hello glorious world!</h1>(all the things I needed to say)</body></html>\n\n";

/**************************************************
    Types
**************************************************/

/**************************************************
    Prototypes
**************************************************/

/**************************************************
    Callback Prototypes
**************************************************/
int cb_ipd(at_return_type *ret);
int cb_setup(at_return_type *ret);

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
at_cb_request_type      at_cb_req;

/*---------------------------------- 
Setup up at-parser & http-parser
---------------------------------- */
at_init_parser(&at_state, at_memory, CB_REQ_NUM);
http_init_parser();
//http_test_parser();

/* Setup uart */
if( uart_open(HW_SPECIFIC_PATH, 0) < 0 )
    {
    printf("Failed to open device\n");
    return(-1);
    }

/*---------------------------------- 
Setup callback for setup
---------------------------------- */
at_cb_req.cmd = AT_CMD_RST;
at_cb_req.cb = cb_setup;
at_cb_req.standing = AT_CB_STANDING_TRANSIENT;
ret = at_submit_cb(&at_state, &at_cb_req);

/*---------------------------------- 
Setup callback for HTTP requests
---------------------------------- */
at_cb_req.cmd = AT_CMD_IPD;
at_cb_req.cb = cb_ipd;
at_cb_req.standing = AT_CB_STANDING_PERSISTENT;
ret = at_submit_cb(&at_state, &at_cb_req);

/*---------------------------------- 
Send reset cmd to start setup sequence
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
        // printf("uart_read[%d]: --%s--\n", read_ret, main_buf);
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

/**************************************************
    cb_ipd
**************************************************/
int cb_ipd
    ( 
    at_return_type   *ret
    )
{
/*---------------------------------- 
Local Variables
---------------------------------- */
at_cb_request_type      at_cb_req;
char                    send_buf[512];
static int              id;
int                     i;

/*---------------------------------- 
Print
---------------------------------- */

printf("Got cb_ipd():: cmd: %d, status: %d, raw[%d]:\n--",
        ret->cmd, ret->status, ret->raw_size); 
i = 0;
do 
    {
    printf("%s", &ret->raw[i]);
    i += strlen(&ret->raw[i]) + 1;
    }while( i <= ret->raw_size );
printf("--\n");

/*---------------------------------- 
Setup callback, and write cmd out
---------------------------------- */
switch( ret->cmd )
    {
    case AT_CMD_IPD:
        at_cb_req.cmd = AT_CMD_CIPSEND;
        at_cb_req.cb = cb_ipd;
        at_cb_req.standing = AT_CB_STANDING_TRANSIENT;
        at_submit_cb(&at_state, &at_cb_req);
        id = strtol( &ret->raw[5], NULL, 10);
        printf( "Sending HTML to %d\n", id );
        sprintf(send_buf, "=%d,%d", id, strlen(resp));
        at_send_cmd_with_text(AT_CMD_CIPSEND, send_buf, strlen(send_buf));
        sleep(1);
        uart_write(resp, strlen(resp));
        break;
    case AT_CMD_CIPSEND:
        at_cb_req.cmd = AT_CMD_CIPCLOSE;
        at_cb_req.cb = cb_ipd;
        at_cb_req.standing = AT_CB_STANDING_TRANSIENT;
        at_submit_cb(&at_state, &at_cb_req);
        printf( "Sending close cmd to %d\n", id );
        sprintf(send_buf, "=%d", id);
        at_send_cmd_with_text(AT_CMD_CIPCLOSE, send_buf, 2);
        break;
    case AT_CMD_CIPCLOSE:
        printf("Done with Send, Closed it.\n");
        break;
    }

return(0);
}

/**************************************************
    cb_setup
**************************************************/
int cb_setup
    ( 
    at_return_type   *ret
    )
{
/*---------------------------------- 
Local Variables
---------------------------------- */
at_cb_request_type      at_cb_req;
int                     i;

/*---------------------------------- 
Print
---------------------------------- */
printf("Got cb_at():: cmd: %d, status: %d, raw[%d]:\n--",
        ret->cmd, ret->status, ret->raw_size); 
i = 0;
do 
    {
    printf("%s", &ret->raw[i]);
    i += strlen(&ret->raw[i]) + 1;
    }while( i <= ret->raw_size );
printf("--\n");

/*---------------------------------- 
Error
---------------------------------- */
if( ret->status != AT_STATUS_OK )
    {
    printf( "Failed on cmd: %s.  Bailing.\n",
        at_get_cmd_txt(ret->cmd));
    return(0);
    }

/*---------------------------------- 
Switch on incoming cmd
---------------------------------- */
switch( ret->cmd )
    {
    case AT_CMD_RST:
        at_cb_req.cmd = AT_CMD_CWMODE;
        at_cb_req.cb = cb_setup;
        at_cb_req.standing = AT_CB_STANDING_TRANSIENT;
        at_submit_cb(&at_state, &at_cb_req);
        at_send_cmd_with_text(AT_CMD_CWMODE, "=3", sizeof("=3")); 
        break;
    case AT_CMD_CWMODE:
        at_cb_req.cmd = AT_CMD_CWSAP;
        at_cb_req.cb = cb_setup;
        at_cb_req.standing = AT_CB_STANDING_TRANSIENT;
        at_submit_cb(&at_state, &at_cb_req);
        at_send_cmd_with_text(  AT_CMD_CWSAP,
                                "=\"esp_ssid\",\"password\",5,3", 
                                sizeof("=\"esp_ssid\",\"password\",5,3")); 
        break;
    case AT_CMD_CWSAP:
        at_cb_req.cmd = AT_CMD_CIPMUX;
        at_cb_req.cb = cb_setup;
        at_cb_req.standing = AT_CB_STANDING_TRANSIENT;
        at_submit_cb(&at_state, &at_cb_req);
        at_send_cmd_with_text(  AT_CMD_CIPMUX, "=1", sizeof("=1"));
        break;
    case AT_CMD_CIPMUX:
        at_cb_req.cmd = AT_CMD_CIPSERVER;
        at_cb_req.cb = cb_setup;
        at_cb_req.standing = AT_CB_STANDING_TRANSIENT;
        at_submit_cb(&at_state, &at_cb_req);
        at_send_cmd_with_text(  AT_CMD_CIPSERVER, "=1,5000", sizeof("=1,5000"));
        break;
    case AT_CMD_CIPSERVER:
        printf("All done\n");
        //at_cb_req.cmd = AT_CMD_CIPAP;
        //at_cb_req.cb = cb_setup;
        //at_cb_req.standing = AT_CB_STANDING_TRANSIENT;
        //at_submit_cb(&at_state, &at_cb_req);
        //at_send_cmd_with_text(  AT_CMD_CIPAP, "=\"192.168.5.1\"", sizeof("=\"192.168.5.1\""));
        break;
    case AT_CMD_CIPAP:
        printf("All done\n");
    }
    
return(0);
}

