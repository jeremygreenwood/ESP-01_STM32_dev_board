/**************************************************
    Includes
**************************************************/
#include "at_parser.h"
#include "uart.h"
#include <stdio.h>

/**************************************************
    Defines
**************************************************/
#define cnt_of_array(ary)          (sizeof(ary) / sizeof(ary[0]))

/**************************************************
    Types
**************************************************/

/**************************************************
    Variables
**************************************************/
at_cmd_to_text_type _at_cmds[] =
    {
/* BASIC    */
    { AT_CMD_NO_CMD,    "AT_NO_CMD"     },
    { AT_CMD_RST,       "AT+RST"        },
    { AT_CMD_AT,        "AT\r"          },
    { AT_CMD_GMR,       "AT+GMR"        },
    { AT_CMD_GSLP,      "AT+GSLP"       },
    { AT_CMD_ATE,       "ATE"           },
/* WIFI     */
    { AT_CMD_CWMODE,    "AT+CWMODE"     },
    { AT_CMD_CWJAP,     "AT+CWJAP"      },
    { AT_CMD_CWLAP,     "AT+CWLAP"      },
    { AT_CMD_CWQAP,     "AT+CWQAP"      },
    { AT_CMD_CWSAP,     "AT+CWSAP"      },
    { AT_CMD_CWDHCP,    "AT+CWDHCP"     },
/* TCP/IP   */
    { AT_CMD_CIPAP,     "AT+CIPAP"      },
    { AT_CMD_CIPSTATUS, "AT+CIPSTATUS"  },
    { AT_CMD_CIPSTART,  "AT+CIPSTART"   },
    { AT_CMD_CIPSEND,   "AT+CIPSEND"    },
    { AT_CMD_CIPCLOSE,  "AT+CIPCLOSE"   },
    { AT_CMD_CIFSR,     "AT+CIFSR"      },
    { AT_CMD_CIPMUX,    "AT+CIPMUX"     },
    { AT_CMD_CIPSERVER, "AT+CIPSERVER"  },
    { AT_CMD_IPD,       "+IPD"          }
    };
at_cmd_to_text_type *at_cmds = _at_cmds;

/**************************************************
    Prototypes
**************************************************/
int call_callback(at_parser_state_type *p, at_return_type *at_ret);

/**************************************************
    Callback Prototypes
**************************************************/

/**************************************************
    at_init_parser
*************************************************/
void at_init_parser
    (
    at_parser_state_type   *p,
    void                   *buf,
    int                     req_num
    )
{
p->requests = buf;
p->req_num = req_num;
memset( p->requests, 0, p->req_num * sizeof(at_cb_request_type) );
};

/**************************************************
    at_process
**************************************************/
int at_process
    (
    at_parser_state_type   *p,
    char                   *in,
    int                     in_size
    )
{
int             i;
int             idx;
int             ret;
char           *ok_ptr;
char           *err_ptr;
at_return_type  at_ret;
int             found;

// printf( "at_process[%d]: --%s--\n", in_size, in );
// HACK
ret = in_size;

idx = 0;
do
{
found = 0;
for( i = 0; i < cnt_of_array(_at_cmds); i++ )
    {
    if( strncmp(&in[idx], at_cmds[i].txt, strlen(at_cmds[i].txt)) == 0 )
        {
        at_ret.status = AT_STATUS_UNKNOWN;
        at_ret.raw_size = in_size;
        err_ptr = strstr( &in[idx], "errror");
        ok_ptr = strstr( &in[idx], "OK");
        if(ok_ptr != NULL && err_ptr != NULL)
            {
            at_ret.status = ok_ptr < err_ptr ? AT_STATUS_OK : AT_STATUS_ERR;
            at_ret.raw_size =  (ok_ptr - &in[idx]) + strlen(ok_ptr < err_ptr ? "OK" : "error");
            }
        else if(ok_ptr != NULL)
            {
            at_ret.status = AT_STATUS_OK;
            at_ret.raw_size =  (ok_ptr - &in[idx]) + strlen("OK");
            }
        else if(err_ptr != NULL)
            {
            at_ret.status = AT_STATUS_ERR;
            at_ret.raw_size =  ((int)ok_ptr - (int)&p[idx]) + (int)strlen("error");
            }
        at_ret.cmd = at_cmds[i].cmd;
        at_ret.raw = &in[idx];
        idx += at_ret.raw_size;
        ret = idx;
        found = 1;
        call_callback(p, &at_ret);
        break;
        }
    }
if( !found )
    {
    idx += 1;
    }
} while( idx < in_size );

return(ret);

};


/**************************************************
    at_submit_cb
**************************************************/
int at_submit_cb
    (
    at_parser_state_type   *p,
    at_cb_request_type     *cb
    )
{
int         i;
int         ret;

ret = AT_STATUS_ERR;
for( i = 0; i < p->req_num; i++ )
    {
    if( p->requests[i].cmd == AT_CMD_NO_CMD )
        {
        memmove(&p->requests[i], cb, sizeof(at_cb_request_type));
        ret = AT_STATUS_OK;
        break;
        }
    }
return( ret );
};

/**************************************************
    at_remove_cb
**************************************************/
int at_remove_cb
    (
    at_parser_state_type   *p,
    at_cb_request_type     *cb
    )
{
int         i;
int         ret;

ret = AT_STATUS_ERR;
for( i = 0; i < p->req_num; i++ )
    {
    if( p->requests[i].cmd == cb->cmd
     && p->requests[i].cb == cb->cb )
        {
        p->requests[i].cmd = AT_CMD_NO_CMD;
        ret = AT_STATUS_OK;
        }
    }
return( ret );
};

/**************************************************
    at_send_cmd
**************************************************/
void at_send_cmd
    (
    char   *in,
    int     in_size
    )
{
unsigned char END[] = { 0x0D, 0x0A };

uart_write( in, in_size);
uart_write( (char*)END, 2);
}

/**************************************************
    at_send_cmd_with_text
**************************************************/
void at_send_cmd_with_text
    (
    at_cmd_enum cmd,
    char   *in,
    int     in_size
    )
{
unsigned char END[] = { 0x0D, 0x0A };
char *cmd_text;
cmd_text = at_get_cmd_txt(cmd);

uart_write(cmd_text, strlen(cmd_text));
uart_write( in, in_size);
uart_write( (char*)END, 2);
}

/**************************************************
    at_get_cmd_txt
**************************************************/
char *at_get_cmd_txt
    (
    at_cmd_enum     cmd
    )
{
int         i;
char       *ret;
ret = at_cmds[0].txt;

for( i = 0; i < cnt_of_array(_at_cmds); i++)
    {
    if(cmd == at_cmds[i].cmd)
        {
        ret = at_cmds[i].txt;
        }
    }
return(ret);

}

/**************************************************
    call_callback
**************************************************/
int call_callback
    (
    at_parser_state_type   *p,
    at_return_type         *at_ret
    )
{
int             i;
int             ret;
int             found;

found = 0;
ret = 0;
for( i = 0; i < p->req_num; i++ )
    {
    if( p->requests[i].cmd == at_ret->cmd )
        {
        found = 1;
        ret = p->requests[i].cb(at_ret);
        if( p->requests[i].standing == AT_CB_STANDING_TRANSIENT )
            {
            at_remove_cb( p, &p->requests[i] );
            }
        // break;
        // NB:  calls all matching CB
        }
    }
if( !found )
    {
    printf("Didn't find callback for:  %s\n", at_ret->raw);
    }

return(ret);
};

