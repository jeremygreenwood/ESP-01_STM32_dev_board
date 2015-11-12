#include "at_parser.h"
#include "dbg_log.h"
#include "uart.h"
#include "cb.h"
#include "templates.h"


/**************************************************

All this needs to get pushed into either prj callbacks,
or esp at parser.  I lean torwards at parser, but need 
to figure it out.

The main problem is that the mem for at_state is 
declared in main.  Maybe just pull that out and 
push it all into esp_server. 

Leaving here for now, because compiling is nice. 
**************************************************/
char resp_http_ok_long[] = 
"HTTP/1.1 200 OK\r\n"                                 \
"Date: Fri, 11 Fri 2015 06:25:59 GMT\r\n"             \
"Content-Type: text/html\r\n"                         \
"Content-Length: %u\r\n\r\n%s";

char resp_http_ok[] = 
"HTTP/1.1 200 OK\r\n"                               \
"Content-Length: %d\r\n\r\n";

char resp_http_body[] = 
"<html><body>"                                      \
"<h1>Hello glorious world!</h1>"                    \
"<h2>You're talking to me.</h2>"                            \
"<h3>that's all \n</h3>"               \
"</body></html>\n\n";

void send_html(int id, char *data, int data_len );


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
//char                    hdr_buf[1024];
//char                    body_buf[1024];
static int              id;
//int                     cont_len;
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
        at_quick_submit( &at_state, AT_CMD_CIPSEND, 
                          cb_ipd, AT_CB_STANDING_TRANSIENT );
        
        id = strtol( &ret->raw[5], NULL, 10);
        dbg_log( "Sending HTML to %d\n", id );
            
        send_html( id, resp_http_body, strlen(resp_http_body) );
/*
        if( strncmp( &ret->raw[11], "GET / HTTP", 6 ) == 0 )
            {
            send_html( id, index_html, index_html_len );
            }
        else if( strncmp( &ret->raw[11], "GET /bootstrap.css", sizeof("GET /bootstrap.css") ) == 0 )
            {
            send_html( id, bootstrap_css, bootstrap_css_len );
            }
        else if( strncmp( &ret->raw[11], "GET /starter-template.css", sizeof("GET /starter-template.css") ) == 0 )
            {
            send_html( id, starter_template_css, starter_template_css_len );
            }
*/
        //sprintf(body_buf, resp_http_body, id, ret->raw);
        //sprintf(hdr_buf, resp_http_ok, strlen(body_buf), body_buf);
        //cont_len = strlen(hdr_buf);
        //sprintf(cmd_buf, "=%d,%d", id, cont_len);
        
        // dbg_log( "cmd_buf[%u]: %s\n\n", strlen(cmd_buf), cmd_buf );
        // dbg_log( "hdr_buf[%u]: %s\n\n", cont_len, hdr_buf );
       
        //at_send_cmd_with_text(AT_CMD_CIPSEND, cmd_buf, strlen(cmd_buf));
        //sleep(1);
        //uart_write(hdr_buf, strlen(hdr_buf));
        
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



/**************************************************
    send_html
**************************************************/
void send_html
    ( 
    int                 id,
    char               *data,
    int                 data_len
    )
{
/*---------------------------------- 
local variables
---------------------------------- */
char cmd_buf[64];
char hdr_buf[128];
int  full_len;

/*---------------------------------- 
local variables
---------------------------------- */
sprintf(hdr_buf, resp_http_ok, data_len);
full_len = strlen(hdr_buf) + data_len;
sprintf(cmd_buf, "=%d,%d", id, full_len);
        
dbg_log( "cmd_buf[%u]: %s\n\n", strlen(cmd_buf), cmd_buf );
// dbg_log( "hdr_buf[%u]: %s\n\n", cont_len, hdr_buf );
       
at_send_cmd_with_text(AT_CMD_CIPSEND, cmd_buf, strlen(cmd_buf));
sleep(1);
uart_write(hdr_buf, strlen(hdr_buf));
uart_write((char*)data, data_len);

}

