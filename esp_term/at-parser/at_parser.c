/**************************************************
    Includes
**************************************************/
#include "at_parser.h"
#include "uart.h"
#include <stdio.h>

/**************************************************
    Defines & Memory Constants
**************************************************/
#define cnt_of_array(ary)          (sizeof(ary) / sizeof(ary[0]))

const char AT_TXT_NO_CMD[]  =   "AT_NO_CMD";
const char AT_TXT_RST[]     =   "AT+RST";
const char AT_TXT_AT[]      =   "AT";
const char AT_TXT_GMR[]     =   "AT+GMR";
const char AT_TXT_GSLP[]    =   "AT+GSLP";
const char AT_TXT_ATE[]     =   "ATE";
const char AT_TXT_CWMODE[]  =   "AT+CWMODE";
const char AT_TXT_CWJAP[]   =   "AT+CWJAP";
const char AT_TXT_CWLAP[]   =   "AT+CWLAP";
const char AT_TXT_CWQAP[]   =   "AT+CWQAP";
const char AT_TXT_CWSAP[]   =   "AT+CWSAP";
const char AT_TXT_CWDHCP[]  =   "AT+CWDHCP";
const char AT_TXT_CIPSTATUS[] = "AT+CIPSTATUS";
const char AT_TXT_CIPSTA[]  =   "AT+CIPSTA";
const char AT_TXT_CIPAP[]   =   "AT+CIPAP";
const char AT_TXT_CIPSTART[] =  "AT+CIPSTART";
const char AT_TXT_CIPSEND[] =   "AT+CIPSEND";
const char AT_TXT_CIPCLOSE[] =  "AT+CIPCLOSE";
const char AT_TXT_CIFSR[]   =   "AT+CIFSR";
const char AT_TXT_CIPMUX[]  =   "AT+CIPMUX";
const char AT_TXT_CIPSERVER[] = "AT+CIPSERVER";
const char AT_TXT_IPD[]     =   "+IPD";

const char AT_TXT_no_this_fun[] =   "no this fun";
const char AT_TXT_ok[]          =   "ok";
const char AT_TXT_error[]       =   "error";
const char AT_TXT_no_change[]   =   "no change";

/**************************************************
    Types
**************************************************/
typedef uint8_t at_token_enum;
enum
    {
    AT_TOK_no_this_fun,
    AT_TOK_ok,
    AT_TOK_error,
    AT_TOK_no_change
    /* AT CMDs  */
    AT_TOK_NO_CMD,
    AT_TOK_RST,  
    AT_TOK_AT,  
    AT_TOK_GMR,
    AT_TOK_GSLP,
    AT_TOK_ATE,
    AT_TOK_CWMODE,
    AT_TOK_CWJAP,
    AT_TOK_CWLAP,
    AT_TOK_CWQAP,
    AT_TOK_CWSAP,
    AT_TOK_CWDHCP,
    AT_TOK_CIPSTATUS,
    AT_TOK_CIPSTA,
    AT_TOK_CIPAP,
    AT_TOK_CIPSTART,
    AT_TOK_CIPSEND,
    AT_TOK_CIPCLOSE,
    AT_TOK_CIFSR,
    AT_TOK_CIPMUX,
    AT_TOK_CIPSERVER,
    AT_TOK_IPD,
    };

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
    { AT_CMD_IPD,       AT_TXT_IPD        }
    };
at_cmd_to_text_type *at_cmds = _at_cmds;

at_cmd_to_text_type _at_toks[] =
    {
    { AT_TOK_no_this_fun,   AT_TXT_no_this_fun  },
    { AT_TOK_ok,            AT_TXT_ok           },
    { AT_TOK_error,         AT_TXT_error        },
    { AT_TOK_no_change,     AT_TXT_no_change    },
    { AT_TOK_NO_CMD,        AT_TXT_NO_CMD       },
    { AT_TOK_RST,           AT_TXT_RST          },
    { AT_TOK_AT,            AT_TXT_AT           },
    { AT_TOK_GMR,           AT_TXT_GMR          },
    { AT_TOK_GSLP,          AT_TXT_GSLP         },
    { AT_TOK_ATE,           AT_TXT_ATE          },
    { AT_TOK_CWMODE,        AT_TXT_CWMODE       },
    { AT_TOK_CWJAP,         AT_TXT_CWJAP        },
    { AT_TOK_CWLAP,         AT_TXT_CWLAP        },
    { AT_TOK_CWQAP,         AT_TXT_CWQAP        },
    { AT_TOK_CWSAP,         AT_TXT_CWSAP        },
    { AT_TOK_CWDHCP,        AT_TXT_CWDHCP       },
    { AT_TOK_CIPSTATUS,     AT_TXT_CIPSTATUS    },
    { AT_TOK_CIPSTA,        AT_TXT_CIPSTA       },
    { AT_TOK_CIPAP,         AT_TXT_CIPAP        },
    { AT_TOK_CIPSTART,      AT_TXT_CIPSTART     },
    { AT_TOK_CIPSEND,       AT_TXT_CIPSEND      },
    { AT_TOK_CIPCLOSE,      AT_TXT_CIPCLOSE     },
    { AT_TOK_CIFSR,         AT_TXT_CIFSR        },
    { AT_TOK_CIPMUX,        AT_TXT_CIPMUX       },
    { AT_TOK_CIPSERVER,     AT_TXT_CIPSERVER    },
    { AT_TOK_IPD,           AT_TXT_IPD          }
    };


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
    find_token
**************************************************/
int find_token
    (
    at_cmd_to_text_type    *tok,
    int                     tok_num,
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



/*
If there's x more number of chars,
    compare them with a list of expected strs
    if a token is found
    return the token and idx
*/


}


/**************************************************
    match
**************************************************/
int match
    (
    char                   *tok,
    int                     tok_size,
    char                   *in,
    int                     in_size
    )
{
int             i;
int             found;

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
     && in_size != tok_size )
        {
        found = 0;
        }
    }
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

