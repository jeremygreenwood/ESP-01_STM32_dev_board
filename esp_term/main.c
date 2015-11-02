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

#include "templates.h"

/**************************************************
    Defines & Memory Consts
**************************************************/
#define cnt_of_array(ary)          (sizeof(ary) / sizeof(ary[0]))
#define MAIN_BUF_SIZE 1024
#define CB_REQ_NUM 30

char resp_http_ok[] = 
"HTTP/1.1 200 OK\n"                                 \
"Date: Fri, 11 Fri 2015 06:25:59 GMT\n"             \
"Content-Type: text/html\nContent-Length: %u\n\n%s";

char resp_http_body[] = 
"<html><body>"                                      \
"<h1>Hello glorious world!</h1>"                    \
"<h2>You're id: %d</h2>"                            \
"<h3>Here's all the raw data:\n</h3>"               \
"<h5>%s\n</h5>"             \
"</body></html>\n\n";

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
Setup callback for setup
---------------------------------- */
at_quick_submit( &at_state, AT_CMD_RST, cb_setup, AT_CB_STANDING_TRANSIENT );

/*---------------------------------- 
Setup callback for HTTP requests
---------------------------------- */
at_quick_submit( &at_state, AT_CMD_IPD, cb_ipd, AT_CB_STANDING_PERSISTENT );

/*---------------------------------- 
Send reset cmd to start setup sequence
---------------------------------- */
at_send_cmd(AT_CMD_RST);
// at_send_cmd_with_text(AT_CMD_ATE, "1", sizeof("0")); 

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
char                    cmd_buf[512];
char                    hdr_buf[1024];
char                    body_buf[1024];
static int              id;
int                     cont_len;
int                     i;

/*---------------------------------- 
Print
---------------------------------- */

dbg_log("Got cb_ipd():: cmd: %d, status: %d, raw_size: %d\n",
        ret->cmd, ret->status, ret->raw_size); 
i = 0;
do 
    {
    dbg_log("%s", &ret->raw[i]);
    i += strlen(&ret->raw[i]) + 1;
    }while( i <= ret->raw_size );
dbg_log("--\n");

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
        dbg_log( "Sending HTML to %d\n", id );

        sprintf(body_buf, resp_http_body, id, ret->raw);
        sprintf(hdr_buf, resp_http_ok, strlen(body_buf), body_buf);
        cont_len = strlen(hdr_buf);
        sprintf(cmd_buf, "=%d,%d", id, cont_len);
        
        // dbg_log( "cmd_buf[%u]: %s\n\n", strlen(cmd_buf), cmd_buf );
        // dbg_log( "hdr_buf[%u]: %s\n\n", cont_len, hdr_buf );
       
        at_send_cmd_with_text(AT_CMD_CIPSEND, cmd_buf, strlen(cmd_buf));
        sleep(1);
        uart_write(hdr_buf, strlen(hdr_buf));
        
        /*
This uart write needs to be moved into the next cb.
How to determine if the cmd for CIPSEND means we just 
sent what was in the uart and is OK, or we got the send 
cmd and are ready for you to write it to the uart.

            */


        break;
    case AT_CMD_CIPSEND:
        if( ret->status == AT_STATUS_OK )
            {
            at_cb_req.cmd = AT_CMD_CIPCLOSE;
            at_cb_req.cb = cb_ipd;
            at_cb_req.standing = AT_CB_STANDING_TRANSIENT;
            at_submit_cb(&at_state, &at_cb_req);
            dbg_log( "Sending close cmd to %d\n", id );
            sprintf(cmd_buf, "=%d", id);
            at_send_cmd_with_text(AT_CMD_CIPCLOSE, cmd_buf, 2);
            }
        else
            {
            dbg_log( "Send request returned error, client ID: %d\n", id );
            }
        break;
    case AT_CMD_CIPCLOSE:
        dbg_log("Done with Send, Closed it.\n");
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
// int                     i;

/*---------------------------------- 
Print
---------------------------------- */
dbg_log("Got cb_at():: cmd: %d, status: %d, raw_size: %d:\n--",
        ret->cmd, ret->status, ret->raw_size); 
//i = 0;
//do 
//    {
//    dbg_log("%s", &ret->raw[i]);
//    i += strlen(&ret->raw[i]) + 1;
//    }while( i <= ret->raw_size );
//dbg_log("--\n");

/*---------------------------------- 
Error
---------------------------------- */
if( ret->status != AT_STATUS_OK )
    {
    dbg_log( "Failed on cmd: %s.  Bailing.\n",
        at_get_cmd_txt(ret->cmd));
    return(0);
    }

/*---------------------------------- 
Switch on incoming cmd
---------------------------------- */
switch( ret->cmd )
    {
    case AT_CMD_ATE:
        at_cb_req.cmd = AT_CMD_RST;
        at_cb_req.cb = cb_setup;
        at_cb_req.standing = AT_CB_STANDING_TRANSIENT;
        at_submit_cb(&at_state, &at_cb_req);
        at_send_cmd(AT_CMD_RST);
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
        dbg_log("Setup Complete\n");
        //at_cb_req.cmd = AT_CMD_CIPAP;
        //at_cb_req.cb = cb_setup;
        //at_cb_req.standing = AT_CB_STANDING_TRANSIENT;
        //at_submit_cb(&at_state, &at_cb_req);
        //at_send_cmd_with_text(  AT_CMD_CIPAP, "=\"192.168.5.1\"", sizeof("=\"192.168.5.1\""));
        break;
    case AT_CMD_CIPAP:
        dbg_log("All done, CIPAP\n");
        break;
    }
    
return(0);
}

