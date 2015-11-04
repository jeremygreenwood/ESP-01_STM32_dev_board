#ifndef AT_PARSER_H
#define AT_PARSER_H

/**************************************************
    Includes
**************************************************/
#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>

/**************************************************
    Enums
**************************************************/
typedef uint8_t at_cmd_enum;
enum
    {
    AT_CMD_NO_CMD,
    AT_CMD_CMDS_START,
    AT_CMD_RST,
    AT_CMD_AT,
    AT_CMD_GMR,
    AT_CMD_GSLP,
    AT_CMD_ATE,
    AT_CMD_CWMODE,
    AT_CMD_CWJAP,
    AT_CMD_CWLAP,
    AT_CMD_CWQAP,
    AT_CMD_CWSAP,
    AT_CMD_CWDHCP,
    AT_CMD_CIPAP,
    AT_CMD_CIPSTATUS,
    AT_CMD_CIPSTART,
    AT_CMD_CIPSTA,
    AT_CMD_CIPCLOSE,
    AT_CMD_CIFSR,
    AT_CMD_CIPMUX,
    AT_CMD_CIPSERVER,
    /* AT Normal cmds end  */
    AT_CMD_CMDS_END,
    /* AT special cmds  */
    AT_CMD_CIPSEND,
    AT_CMD_IPD,
    /* AT Responses  */
    AT_CMD_no_this_fun,
    AT_CMD_ok,
    AT_CMD_error,
    AT_CMD_no_change,
    AT_CMD_ready,
    AT_CMD_send_ok,
    AT_CMD_wrong_syntax,
    AT_CMD_INVALID,
    };

typedef uint8_t at_status_enum;
enum
    {
    AT_STATUS_OK,
    AT_STATUS_ERR,
    AT_STATUS_UNKNOWN
    };

typedef uint8_t at_cb_standing_enum;
enum
    {
    AT_CB_STANDING_PERSISTENT,
    AT_CB_STANDING_TRANSIENT
    };

/**************************************************
    Defines
**************************************************/

/**************************************************
    Types
**************************************************/
typedef struct 
    {
    at_cmd_enum         cmd;
    char               *txt;
    }at_cmd_to_text_type;

typedef struct 
    {
    at_cmd_enum         cmd;
    at_status_enum      status;
    char               *raw;
    int                 raw_size;
    }at_return_type;

typedef int (*at_callback_type) (at_return_type *ret);

typedef struct 
    {
    at_cmd_enum         cmd;
    at_callback_type    cb;
    at_cb_standing_enum standing; 
    }at_cb_request_type;

typedef struct 
    {
    at_cb_request_type *requests;
    int                 req_num;
    }at_parser_state_type;


/**************************************************
    Prototypes
**************************************************/
void at_init_parser(at_parser_state_type *p, void *buf, int req_num);
int at_process(at_parser_state_type *p, char *in, int in_size);
int at_submit_cb(at_parser_state_type *p, at_cb_request_type *cb);
int at_remove_cb(at_parser_state_type *p, at_cb_request_type *cb);

void at_send_cmd(at_cmd_enum cmd);
void at_send_cmd_with_text(at_cmd_enum cmd, char *in, int in_size);
char *at_get_cmd_txt(at_cmd_enum cmd);

#define at_quick_submit( at, cmd, cb, standing )        \
    do                                                  \
    {                                                   \
    at_cb_request_type req = { cmd, cb, standing };     \
    at_submit_cb(at, &req);                             \
    }while(0)                                           \


/**************************************************
    Variables
**************************************************/
extern at_cmd_to_text_type *at_cmds;

#endif 
