
/* Line End */
//unsigned char   END = "\r\n"
unsigned char END[] = { 0x0D, 0x0A };
unsigned char Q[] = { 0x3F };
unsigned char EQ[] = { 0x3D };



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
MODE,
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



char resp_hdr[] = 
"HTTP/1.1 200 OK\nDate: Fri, 11 Fri 2015 06:25:59 GMT\nContent-Type: text/html\nContent-Length: %u\n\n";

char resp_body[] = "<html><body><h1>Hello glorious world!</h1>(all the things I needed to say)</body></html>\n\n";


//Link

//+IPD,0,303:GET / HTTP/1.1
//Host: 192.168.4.1:5000
//User-Agent: Mozilla/5.0 (X11; Linux armv7l; rv:31.0) Gecko/20100101 Firefox/31.0 Iceweasel/31.6.0
//Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8
//Accept-Language: en-US,en;q=0.5
//Accept-Encoding: gzip, deflate
//Connection: keep-alive


//OK





