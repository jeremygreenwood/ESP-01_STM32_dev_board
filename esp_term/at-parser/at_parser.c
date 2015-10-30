/**************************************************
    Includes
**************************************************/
#include "at_parser.h"
#include "uart.h"
#include <stdio.h>
#include "dbg_log.h"

/**************************************************
    Defines & Memory Constants
**************************************************/
#define cnt_of_array(ary)          (sizeof(ary) / sizeof(ary[0]))

char AT_TXT_NO_CMD[]  =   "AT_NO_CMD";
char AT_TXT_RST[]     =   "AT+RST";
char AT_TXT_AT[]      =   "AT";
char AT_TXT_GMR[]     =   "AT+GMR";
char AT_TXT_GSLP[]    =   "AT+GSLP";
char AT_TXT_ATE[]     =   "ATE";
char AT_TXT_CWMODE[]  =   "AT+CWMODE";
char AT_TXT_CWJAP[]   =   "AT+CWJAP";
char AT_TXT_CWLAP[]   =   "AT+CWLAP";
char AT_TXT_CWQAP[]   =   "AT+CWQAP";
char AT_TXT_CWSAP[]   =   "AT+CWSAP";
char AT_TXT_CWDHCP[]  =   "AT+CWDHCP";
char AT_TXT_CIPSTATUS[] = "AT+CIPSTATUS";
char AT_TXT_CIPSTA[]  =   "AT+CIPSTA";
char AT_TXT_CIPAP[]   =   "AT+CIPAP";
char AT_TXT_CIPSTART[] =  "AT+CIPSTART";
char AT_TXT_CIPSEND[] =   "AT+CIPSEND";
char AT_TXT_CIPCLOSE[] =  "AT+CIPCLOSE";
char AT_TXT_CIFSR[]   =   "AT+CIFSR";
char AT_TXT_CIPMUX[]  =   "AT+CIPMUX";
char AT_TXT_CIPSERVER[] = "AT+CIPSERVER";
char AT_TXT_IPD[]     =   "+IPD";

char AT_TXT_no_this_fun[] =   "no this fun";
char AT_TXT_ok[]          =   "OK";
char AT_TXT_error[]       =   "ERROR";
char AT_TXT_no_change[]   =   "no change";
char AT_TXT_ready[]       =   "ready";
char AT_TXT_send_ok[]     =   "SEND OK";
char AT_TXT_wrong_syntax[] =  "wrong syntax";

/**************************************************
    Types
**************************************************/

/**************************************************
    Variables
**************************************************/
at_cmd_to_text_type _at_cmds[] =
    {
/* BASIC    */
    { AT_CMD_NO_CMD,    AT_TXT_NO_CMD     },
    { AT_CMD_RST,       AT_TXT_RST        },
    { AT_CMD_AT,        AT_TXT_AT         },
    { AT_CMD_GMR,       AT_TXT_GMR        },
    { AT_CMD_GSLP,      AT_TXT_GSLP       },
    { AT_CMD_ATE,       AT_TXT_ATE        },
/* WIFI     */
    { AT_CMD_CWMODE,    AT_TXT_CWMODE     },
    { AT_CMD_CWJAP,     AT_TXT_CWJAP      },
    { AT_CMD_CWLAP,     AT_TXT_CWLAP      },
    { AT_CMD_CWQAP,     AT_TXT_CWQAP      },
    { AT_CMD_CWSAP,     AT_TXT_CWSAP      },
    { AT_CMD_CWDHCP,    AT_TXT_CWDHCP     },
/* TCP/IP   */
    { AT_CMD_CIPSTATUS, AT_TXT_CIPSTATUS  },
    { AT_CMD_CIPSTA,    AT_TXT_CIPSTA     },
    { AT_CMD_CIPAP,     AT_TXT_CIPAP      },
    { AT_CMD_CIPSTART,  AT_TXT_CIPSTART   },
    { AT_CMD_CIPSEND,   AT_TXT_CIPSEND    },
    { AT_CMD_CIPCLOSE,  AT_TXT_CIPCLOSE   },
    { AT_CMD_CIFSR,     AT_TXT_CIFSR      },
    { AT_CMD_CIPMUX,    AT_TXT_CIPMUX     },
    { AT_CMD_CIPSERVER, AT_TXT_CIPSERVER  },
    { AT_CMD_IPD,       AT_TXT_IPD        },
/* AT Responses  */
    { AT_CMD_no_this_fun, AT_TXT_no_this_fun  },
    { AT_CMD_ok,        AT_TXT_ok          },
    { AT_CMD_error,     AT_TXT_error       },
    { AT_CMD_no_change, AT_TXT_no_change   },
    { AT_CMD_ready,     AT_TXT_ready       },
    { AT_CMD_send_ok,   AT_TXT_send_ok     },
    { AT_CMD_wrong_syntax, AT_TXT_wrong_syntax },
    };
at_cmd_to_text_type *at_cmds = _at_cmds;

/**************************************************
    Prototypes
**************************************************/
int call_callback(at_parser_state_type *p, at_return_type *at_ret);
at_cmd_enum any_match(at_cmd_to_text_type *tok, int tok_num, char *in, int in_size);
int match( char *tok, char *in, int in_size);
int find_token( at_cmd_to_text_type *tok, int tok_num, 
                char *in, int in_size, at_cmd_enum *found_tok);

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
int             idx;
int             ret;
at_return_type  at_ret;
int             found;
at_cmd_enum   token; 
int             raw_start;
int             cb_ready;

// dbg_log( "at_process[%d]: --%s--\n", in_size, in );

raw_start = 0;
idx = 0;
ret = 0;
cb_ready = 0;
do
    {
    found = find_token( _at_cmds, cnt_of_array(_at_cmds), &in[idx], in_size - idx, &token );

    if( found >= 0 )
        {
        idx += found;
        switch( token )
            {
            case AT_CMD_no_this_fun:
                at_ret.status = AT_STATUS_ERR;
                at_ret.raw = &in[raw_start];
                idx += strlen( AT_TXT_no_this_fun );
                at_ret.raw_size = idx - raw_start;
                cb_ready = 1;
                break;
            case AT_CMD_send_ok:
                at_ret.status = AT_STATUS_OK;
                at_ret.raw = &in[raw_start];
                idx += strlen( AT_TXT_send_ok );
                at_ret.raw_size = idx - raw_start;
                cb_ready = 1;
                break;
            case AT_CMD_ready:
                at_ret.status = AT_STATUS_OK;
                at_ret.raw = &in[raw_start];
                idx += strlen( AT_TXT_ready );
                at_ret.raw_size = idx - raw_start;
                cb_ready = 1;
                break;
            case AT_CMD_ok:
                at_ret.status = AT_STATUS_OK;
                at_ret.raw = &in[raw_start];
                idx += strlen( AT_TXT_ok );
                at_ret.raw_size = idx - raw_start;
                cb_ready = 1;
                if(at_ret.cmd == AT_CMD_RST)
                    {
                    cb_ready = 0;
                    at_ret.raw_size = 0;
                    }
                if(at_ret.cmd == AT_CMD_CIPSEND)
                    {
                    // idx += atoi(&in[idx + 3]) - 1;
                    // at_ret.raw_size = idx - raw_start;
                    // cb_ready = 1;
                    }
                break;
            case AT_CMD_error:
                at_ret.status = AT_STATUS_ERR;
                at_ret.raw = &in[raw_start];
                idx += strlen( AT_TXT_error );
                at_ret.raw_size = idx - raw_start;
                cb_ready = 1;
                if(at_ret.cmd == AT_CMD_CIPSEND)
                    {
                    // idx += atoi(&in[idx + 3]) - 1;
                    // at_ret.raw_size = idx - raw_start;
                    // cb_ready = 1;
                    }
                break;
            case AT_CMD_wrong_syntax:
                break;
            case AT_CMD_no_change:
                at_ret.status = AT_STATUS_OK;
                at_ret.raw = &in[raw_start];
                idx += strlen( AT_TXT_no_change );
                at_ret.raw_size = idx - raw_start;
                at_ret.status = AT_STATUS_OK;
                cb_ready = 1;
                break;
            case AT_CMD_CIPSEND:
                at_ret.cmd = token;
                raw_start = idx;
                idx += strlen( at_get_cmd_txt( token ) );
                at_ret.status = AT_STATUS_OK;
                at_ret.raw = &in[raw_start];
                // idx += 10;
                // idx += atoi(&in[idx + 3]) - 1;
                at_ret.raw_size = idx - raw_start;
                // cb_ready = 1;
                break;
            case AT_CMD_IPD:
                at_ret.cmd = token;
                raw_start = idx;
                idx += strlen( at_get_cmd_txt( token ) );
                at_ret.status = AT_STATUS_OK;
                at_ret.raw = &in[raw_start];

                // Only do this if it's OK, so wait..
                idx += 10;
                idx += atoi(&in[raw_start + 7]) - 1;

                at_ret.raw_size = idx - raw_start;
                // cb_ready = 1;
                break;
            default:
                if( token > AT_CMD_CMDS_START
                 && token < AT_CMD_CMDS_END )
                    {
                    at_ret.cmd = token;
                    raw_start = idx;
                    idx += strlen( at_get_cmd_txt( token ) );
                    }
                else
                    {
                    dbg_log("Bad enum or cmd received: %u\n", token);
                    /* ERROR  */
                    }
                break;
            }
        }

    if( cb_ready )
        {
        call_callback(p, &at_ret);
        memset( &at_ret, 0, sizeof( at_return_type ) );
        cb_ready = 0;
        ret = idx;
        }

    } while( found >= 0 && idx < in_size );

return(ret);

};

/**************************************************
    find_token
**************************************************/
int find_token
    (
    at_cmd_to_text_type    *tok,
    int                     tok_num,
    char                   *in,
    int                     in_size,
    at_cmd_enum          *found_tok
    )
{
int             i;
int             ret;

ret = -1;

for( i = 0; i < in_size; i++ )
    {
    *found_tok = any_match( tok, tok_num, &in[i], in_size - i);
    if( *found_tok != AT_CMD_INVALID )
        {
        ret = i;
        break;
        } 
    }
return( ret );
}


/**************************************************
    any_match
**************************************************/
at_cmd_enum any_match
    (
    at_cmd_to_text_type    *tok,
    int                     tok_num,
    char                   *in,
    int                     in_size
    )
{
int             i;
at_cmd_enum   found_match;

found_match = AT_CMD_INVALID;

for( i =  0; i < tok_num; i++ )
    {
    if( match( tok[i].txt, in, in_size ) )
        {
        found_match = tok[i].cmd;
        break;
        }
    }
return( found_match );
}


/**************************************************
    match
**************************************************/
int match
    (
    char                   *tok,
    char                   *in,
    int                     in_size
    )
{
int             i;
int             found;
int             tok_size;

tok_size = strlen(tok);
found = 1;

for( i = 0; i < tok_size && i < in_size; i++ )
    {
    if( tok[i] != in[i] )
        {
        found = 0;
        break;
        }    
    }
if( found )
    {
    if( in[tok_size] != '\n'
     && in[tok_size] != '\r'
     && in[tok_size] != '\0'
     && in[tok_size] != '='
     && in[tok_size] != '\"'
     && in[tok_size] != ','
     && in[tok_size] != '0'
     && in[tok_size] != '1'
     && in_size != tok_size )
        {
        found = 0;
        }
    }
return( found );
}


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
}

/**************************************************
    at_send_cmd
**************************************************/
void at_send_cmd
    (
    at_cmd_enum cmd
    )
{
unsigned char   END[] = { 0x0D, 0x0A };
char           *cmd_txt;
int             cmd_len;

cmd_txt = at_get_cmd_txt(cmd);
cmd_len = strlen(cmd_txt);

uart_write( cmd_txt, cmd_len);
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

if( at_ret->cmd == AT_CMD_NO_CMD )
    {
    dbg_log("Problem. Calling callback with bad cmd. cmd: %u\n",
                    at_ret->cmd);
    return( ret );
    }

for( i = 0; i < p->req_num; i++ )
    {
    if( p->requests[i].cmd == at_ret->cmd )
        {
        found = 1;
        if( p->requests[i].cb == NULL)
            {
            dbg_log("Problem. Calling a NULL callback, i: %d, cmd: %u\n",
                    i, at_ret->cmd);
            }
        else
            {
            ret = p->requests[i].cb(at_ret);
            }
        if( p->requests[i].standing == AT_CB_STANDING_TRANSIENT )
            {
            p->requests[i].cmd = AT_CMD_NO_CMD;
            // at_remove_cb( p, &p->requests[i] );
            }
        // break;
        // NB:  calls all matching CB
        }
    }
if( !found )
    {
    dbg_log("Didn't find callback for:  %s\n", at_ret->raw);
    }

return(ret);
};

