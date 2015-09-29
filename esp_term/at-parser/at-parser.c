/**************************************************
    Includes
**************************************************/
at-parser.h

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
at_cmd_to_text_type at_cmds[] =
    {
/* BASIC    */
    { AT_CMD_NO_CMD,    "AT_NO_CMD"     },
    { AT_CMD_RST,       "AT+RST"        },
    { AT_CMD_AT,        "AT"            },
    { AT_CMD_GMR,       "AT+GMR"        },
    { AT_CMD_GSLP,      "AT+GSLP"       },
    { AT_CMD_ATE,       "ATE"           }
/* WIFI     */
    { AT_CMD_CWMODE,    "AT+CWMODE"     },
    { AT_CMD_CWJAP,     "AT+CWJAP"      },
    { AT_CMD_CWLAP,     "AT+CWLAP"      },
    { AT_CMD_CWQAP,     "AT+CWQAP"      },
    { AT_CMD_CWSAP,     "AT+CWSAP"      },
    { AT_CMD_CWDHCP,    "AT+CWDHCP"     },
/* TCP/IP   */
    { AT_CMD_CIPSTATUS, "AT+CIPSTATUS"  },
    { AT_CMD_CIPSTART,  "AT+CIPSTART"   },
    { AT_CMD_CIPSEND,   "AT+CIPSEND"    },
    { AT_CMD_CIPCLOSE,  "AT+CIPCLOSE"   },
    { AT_CMD_CIFSR,     "AT+CIFSR"      },
    { AT_CMD_CIPMUX,    "AT+CIPMUX"     },
    { AT_CMD_CIPSERVER, "AT+CIPSERVER"  },
    { AT_CMD_IPD,       "+IPD"          }
    };

/**************************************************
    Prototypes
**************************************************/

/**************************************************
    Callback Prototypes
**************************************************/

/**************************************************
    at_init_parser
**************************************************/
int at_init_parser
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
    void                   *in,
    int                     in_size
    )
{
int             i;
int             idx;
char           *ok_ptr;
char           *err_ptr;
at_return_type  at_ret;
int             found;


typedef struct 
    {
    at_cmd_enum         cmd;
    at_status_enum      status;
    char               *raw;
    int                 raw_size;
    }at_return_type;

idx = 0;
do
{
found = 0;
for( i = 0; i < cnt_of_array(at_cmds); i++ )
    {
    if( strncmp(&p[idx], at_cmds[i].txt, strlen(at_cmds[i].txt)) == 0 )
        {
        at_ret.status = AT_STATUS_UNKNOWN;
        err_ptr = strstr( &p[idx], "errror");
        ok_ptr = strstr( &p[idx], "OK");
        if(ok_ptr != NULL && err_ptr != NULL)
            {
            at_ret.status = ok_ptr < err_ptr ? AT_STATUS_OK : AT_STATUS_ERR;
            at_ret.raw_size =  (ok_prt - &p[idx]) + strlen(ok_ptr < err_ptr ? "OK" : "error");
            }
        else if(ok_ptr != NULL)
            {
            at_ret.status = ok_ptr < err_ptr ? AT_STATUS_OK : AT_STATUS_ERR;
            at_ret.raw_size =  (ok_prt - &p[idx]) + strlen("OK");
            }
        else if(err_ptr != NULL)
            {
            at_ret.status = ok_ptr < err_ptr ? AT_STATUS_OK : AT_STATUS_ERR;
            at_ret.raw_size =  (ok_prt - &p[idx]) + strlen("error");
            }
        at_ret.cmd = at_cmds.cmd;
        at_ret.raw = &p[idx];
        idx += at_ret.raw_size;
        found = 1;
        at_call_callback(p, at_ret);
        break;
        }
    if( !found )
        {
        idx += 1;
        }
    }
while( idx < in_size );

};

/**************************************************
    at_call_callback
**************************************************/
int at_call_callback
    (
    at_parser_state_type   *p,
    at_return_type         *at_ret
    )
{
int             i;

for( i = 0; i < p->req_num; i++ )
    {
    if( p->requests[i].cmd == at_ret->cmd )
        {
        p->requests[i].cb(at_ret);
        }
    }

};

typedef struct 
    {
    at_cb_request_type *requests;
    int                 req_num;
    }at_parser_state_type;

typedef struct 
    {
    at_cmd_enum         cmd;
    at_callback_type    cb;
    at_cb_standing_enum standing; 
    }at_cb_request_type;

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

int         ret = AT_STATUS_ERR;
for( i = 0; i < p->req_num; i++ )
    {
    if( p->requests[i].cmd == AT_CMD_NO_CMD )
        {
        memmove(&p->requests[i], cb, sizeof(at_cb_request_type));
        ret = AT_STATUS_OK;
        }
    }
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

int         ret = AT_STATUS_ERR;
for( i = 0; i < p->req_num; i++ )
    {
    if( p->requests[i].cmd == cb->cmd
     && p->requests[i].cb == cb->cb )
        {
        p->requests[i].cmd = AT_CMD_NO_CMD;
        ret = AT_STATUS_OK;
        }
    }
};




