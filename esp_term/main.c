#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include "at_parser.h"
#include "uart.h"

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
Setup up at-parser
---------------------------------- */
at_init_parser(&at_state, at_memory, CB_REQ_NUM);

/* Setup uart */
if( uart_open("/dev/ttyUSB0", 0) < 0 )
    {
    printf("Failed to open device\n");
    return(-1);
    }

/*---------------------------------- 
Setup callback & send first cmd
---------------------------------- */
at_cb_req.cmd = AT_CMD_CWMODE;
at_cb_req.cb = cb_setup;
at_cb_req.standing = AT_CB_STANDING_TRANSIENT;
ret = at_submit_cb(&at_state, &at_cb_req);
at_send_cmd_with_text(AT_CMD_CWMODE, "=3", sizeof("=3")); 
// at_send_cmd(at_get_cmd_txt(at_cb_req.cmd), strlen(at_get_cmd_txt(at_cb_req.cmd)));

/*---------------------------------- 
Do main forever
---------------------------------- */
buf_curr = 0;
for(;;)
    {
    sleep(3);
    read_ret = uart_read(&main_buf[buf_curr], MAIN_BUF_SIZE - buf_curr);
    if(read_ret > 0 )
        {
        // printf("uart_read[%d]: --%s--\n", read_ret, main_buf);
        }
    buf_curr += read_ret;
    if( read_ret > 0 )
        {
        at_processed = at_process(&at_state, main_buf, buf_curr);
        buf_curr -= at_processed;
        memmove(main_buf, &main_buf[at_processed], buf_curr );
        }
    }

return(ret);
}

/**************************************************
    cb_at
**************************************************/
int cb_at
    ( 
    at_return_type   *ret
    )
{
/*---------------------------------- 
Local Variables
---------------------------------- */
at_cb_request_type      at_cb_req;
char                   *cmd;
int                     send;

/*---------------------------------- 
Init
---------------------------------- */
send = 0;

/*---------------------------------- 
Print
---------------------------------- */
printf("Got cb_at()\ncmd: %d, status: %d, raw[%d]: --%s--\n",
        ret->cmd, ret->status, ret->raw_size, ret->raw); 

/*---------------------------------- 
Switch on incoming cmd
---------------------------------- */
switch( ret->cmd )
    {
    case AT_CMD_AT:
        at_cb_req.cmd = AT_CMD_GMR;
        at_cb_req.cb = cb_at;
        at_cb_req.standing = AT_CB_STANDING_TRANSIENT;
        cmd = at_get_cmd_txt(at_cb_req.cmd);
        send = 1;
        break;

    }

/*---------------------------------- 
Setup callback, and write cmd out
---------------------------------- */
if(send)
    {
    at_submit_cb(&at_state, &at_cb_req);
    at_send_cmd(cmd, strlen(cmd));
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

/*
at_cmd_to_text_type cmd_sequence[] = 
{
    { AT_CMD_RST,       ""                                  },
    { AT_CMD_CWMODE,    "=3"                                },
    { AT_CMD_CWSAP,     "\"=esp_ssid\",\"password\",5,3"    },
    { AT_CMD_CIPMUX,    "=1"                                },
    { AT_CMD_CIPSERVER, "=1,5000"                           },
    { AT_CMD_CIPAP,     "=\"192.168.5.1\""                  },
}; 
*/

/*---------------------------------- 
Init
---------------------------------- */

/*---------------------------------- 
Print
---------------------------------- */
printf("Got cb_at()\ncmd: %d, status: %d, raw[%d]: --%s--\n",
        ret->cmd, ret->status, ret->raw_size, ret->raw); 

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
                                "\"=esp_ssid\",\"password\",5,3", 
                                sizeof("\"=esp_ssid\",\"password\",5,3")); 
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
        at_cb_req.cmd = AT_CMD_CIPAP;
        at_cb_req.cb = cb_setup;
        at_cb_req.standing = AT_CB_STANDING_TRANSIENT;
        at_submit_cb(&at_state, &at_cb_req);
        at_send_cmd_with_text(  AT_CMD_CIPAP, "=\"192.168.5.1\"", sizeof("=\"192.168.5.1\""));
        break;
    case AT_CMD_CIPAP:
        printf("All done\n");
    }
    
return(0);
}

