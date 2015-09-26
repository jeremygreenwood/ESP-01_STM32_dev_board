/**************************************************
    Includes
**************************************************/
at-parser.h

/**************************************************
    Defines
**************************************************/

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




