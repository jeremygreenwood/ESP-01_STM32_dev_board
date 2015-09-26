/**************************************************
    Includes
**************************************************/

/**************************************************
    Enums
**************************************************/
typedef uint8 at_cmd_enum;
enum
    {
    AT_CMD_NO_CMD,
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
    AT_CMD_CIPSTATUS,
    AT_CMD_CIPSTART,
    AT_CMD_CIPSEND,
    AT_CMD_CIPCLOSE,
    AT_CMD_CIFSR,
    AT_CMD_CIPMUX,
    AT_CMD_CIPSERVER,
    AT_CMD_IPD
    };

typedef uint8 at_status_enum;
enum
    {
    AT_STATUS_OK,
    AT_STATUS_ERR
    };

typedef uint8 at_cb_standing_enum;
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
    char               *txt
    }at_cmd_to_text_type;

typedef struct 
    {
    at_cmd_enum         cmd;
    at_status_enum      status;
    char               *raw;
    int                 raw_size;
    }at_return_type;

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

typedef int (*at_callback_type) (at_return_type ret);

/**************************************************
    Prototypes
**************************************************/
int at_init_parser(at_parser_state_type *p, void *buf, int req_num);
int at_process(at_parser_state_type *p, void *in, int in_size);
int at_submit_cb(at_parser_state_type *p, at_cb_request_type *cb);
int at_remove_cb(at_parser_state_type *p, at_cb_request_type *cb);

/**************************************************
    Variables
**************************************************/



extern at_cmd_to_text_type *at_cmds;

at_cmd_to_text_type at_cmds[] =
    {
    { AT_CMD_NO_CMD,
    { AT_CMD_RST,
    { AT_CMD_AT,
    { AT_CMD_GMR,
    { AT_CMD_GSLP,
    { AT_CMD_ATE,
    { AT_CMD_CWMODE,
    { AT_CMD_CWJAP,
    { AT_CMD_CWLAP,
    { AT_CMD_CWQAP,
    { AT_CMD_CWSAP,
    { AT_CMD_CWDHCP,
    { AT_CMD_CIPSTATUS,
    { AT_CMD_CIPSTART,
    { AT_CMD_CIPSEND,
    { AT_CMD_CIPCLOSE,
    { AT_CMD_CIFSR,
    { AT_CMD_CIPMUX,
    { AT_CMD_CIPSERVER,
    { AT_CMD_IPD
    };



/* Basic    */
#define RST     "AT+RST"
#define AT      "AT"
#define GMR     "AT+GMR"
#define GSLP    "AT+GSLP"
#define ATE     "ATE"



/* WIFI     */
#define CWMODE  "AT+CWMODE" /*  DES:    wifi mode
                                SET:    AT+CWMODE=<mode>
                                INQ:    AT+CWMODE?
                                TST:    AT+CWMODE=?
                                PAR:    1= Sta, 2= AP, 3=both
                                EXP:    -                           */

#define CWJAP   "AT+CWJAP"  /*  DES:    join the AP
                                SET:    AT+ CWJAP =<ssid>,< pwd >
                                INQ:    AT+ CWJAP?
                                TST:    
                                PAR:    ssid=ssid, pwd=password
                                EXP:    -                           */

#define CWLAP   "AT+CWLAP"  /*  DES:    list the AP
                                SET:    AT+CWLAP                    */

#define CWQAP   "AT+CWQAP"  /*  DES:    quit the AP
                                SET:    AT+CWQAP
                                INQ:    -
                                TST:    AT+CWQAP=?                  */

#define CWSAP   "AT+CWSAP"  /*  DES:    set the AP
                                SET:    AT+ CWSAP= <ssid>,<pwd>,<chl>, <ecn>
                                INQ:    AT+ CWSAP?
                                TST:    -
                                PAR:    ssid, pwd, chl = channel, ecn = encryption
                                EXP:    AT+CWJAP="YOURSSID","helloworld"    */



#define CWDHCP   "AT+CWDHCP" 






/* TCP/IP   */
#define CIPSTATUS  "AT+CIPSTATUS"   /*  DES:    get the connection status
                                        SET:    AT+ CIPSTATUS               */

#define CIPSTART   "AT+CIPSTART"    /*  DES:    set up TCP or UDP connection
                                        SET:    1)single connection (+CIPMUX=0):    AT+CIPSTART= <type>,<addr>,<port>
                                                2) multiple connection (+CIPMUX=1): AT+CIPSTART= <id><type>,<addr>, <port>
                                            INQ:    -
                                            TST:    AT+CIPSTART=?
                                            PAR:    id = 0-4, type = TCP/UDP, addr = IP address, port= port
                                            EXP:    Connect to another TCP server, set multiple connection first:
                                                    AT+CIPMUX=1; connect: AT+CIPSTART=4,"TCP","X1.X2.X3.X4",9999    */

#define CIPSEND    "AT+CIPSEND"    /*  DES:    send data
                                       SET:    1)single connection(+CIPMUX=0) AT+CIPSEND=<length>
                                               2) multiple connection (+CIPMUX=1) AT+CIPSEND= <id>,<length>
                                       INQ:    -
                                       TST:    AT+CIPSEND=?
                                       PAR:    -
                                       EXP:    send data: AT+CIPSEND=4,15 and then enter the data  */

#define CIPCLOSE   "AT+CIPCLOSE"   /*  DES:    close TCP or UDP connection
                                       SET:    AT+CIPCLOSE=<id> or AT+CIPCLOSE
                                       INQ:    -
                                       TST:    AT+CIPCLOSE=?     */

#define CIFSR      "AT+CIFSR"      /*  DES:    Get IP address
                                       SET:    AT+CIFSR
                                       TST:    AT+ CIFSR=?    */

#define CIPMUX     "AT+CIPMUX"     /*  DES:    set mutiple connection
                                       SET:    AT+ CIPMUX=<mode>
                                       INQ:    AT+ CIPMUX?
                                       PAR:    0 for single connection 1 for mutiple connection   */

#define CIPSERVER  "AT+CIPSERVER"  /*  DES:    set as server
                                       SET:    AT+CIPSERVER= <mode>[,<port> ]
                                       PAR:    mode 0 to close server mode, mode 1 to open; port = port
                                       EXP:    turn on as a TCP server: AT+CIPSERVER=1,8888
                                               check the self server IP address: AT+CIFSR=?    */

#define IPD        "+IPD"          /*  DES:    received data   */



char *at_list[] =
{
"Print Cmds",
RST,
AT,
GMR,
GSLP,
ATE,
CWMODE,
CWJAP,
CWLAP,
CWQAP,
CWSAP,
CWDHCP,
CIPSTATUS,
CIPSTART,
CIPSEND,
CIPCLOSE,
CIFSR,
CIPMUX,
CIPSERVER,
IPD
};




