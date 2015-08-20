

/* Line End */
//unsigned char   END = "\r\n"
unsigned char   END[] = { 0x0D, 0x0A };

/* Basic    */
#define RST "AT+RST"        /*  DES:    restart the module          */

/* WIFI     */
#define MODE  "AT+CWMODE"       /*  DES:    wifi mode
                                SET:    AT+CWMODE=<mode>
                                INQ:    AT+CWMODE?
                                TST:    AT+CWMODE=?
                                PAR:    1= Sta, 2= AP, 3=both
                                EXP:    -                           */
#ifdef PK_
#define   "AT+CWJAP"        /*  DES:    join the AP
                                SET:    AT+ CWJAP =<ssid>,< pwd >
                                INQ:    AT+ CWJAP?
                                TST:    
                                PAR:    ssid=ssid, pwd=password
                                EXP:    -                           */
#define   "AT+CWLAP"        /*  DES:    list the AP
                                SET:    AT+CWLAP                    */ 
#define   "AT+CWQAP"        /*  DES:    quit the AP
                                SET:    AT+CWQAP
                                INQ:    -
                                TST:    AT+CWQAP=?                  */
#define   "AT+ CWSAP"       /*  DES:    set the AP
                                SET:    AT+ CWSAP= <ssid>,<pwd>,<chl>, <ecn>
                                INQ:    AT+ CWSAP?
                                TST:    -
                                PAR:    ssid, pwd, chl = channel, ecn = encryption
                                EXP:    AT+CWJAP="YOURSSID","helloworld"    */

/* TCP/IP   */
#define   "AT+ CIPSTATUS"   /*  DES:    get the connection status
                                SET:    AT+ CIPSTATUS               */ 
#define   "AT+CIPSTART"     /*  DES:    set up TCP or UDP connection    
                                SET:    1)single connection (+CIPMUX=0):    AT+CIPSTART= <type>,<addr>,<port>  
                                        2) multiple connection (+CIPMUX=1): AT+CIPSTART= <id><type>,<addr>, <port>
                                INQ:    -
                                TST:    AT+CIPSTART=?
                                PAR:    id = 0-4, type = TCP/UDP, addr = IP address, port= port
                                EXP:    Connect to another TCP server, set multiple connection first:
                                            AT+CIPMUX=1; connect: AT+CIPSTART=4,"TCP","X1.X2.X3.X4",9999    */
#define   "AT+CIPSEND"      /*  DES:    send data
                                SET:    1)single connection(+CIPMUX=0) AT+CIPSEND=<length>
                                        2) multiple connection (+CIPMUX=1) AT+CIPSEND= <id>,<length>
                                INQ:    -
                                TST:    AT+CIPSEND=?
                                PAR:    -
                                EXP:    send data: AT+CIPSEND=4,15 and then enter the data  */
#define   "AT+CIPCLOSE"     /*  DES:    close TCP or UDP connection
                                SET:    AT+CIPCLOSE=<id> or AT+CIPCLOSE
                                INQ:    -
                                TST:    AT+CIPCLOSE=?     */ 
#define   "AT+CIFSR"        /*  DES:    Get IP address
                                SET:    AT+CIFSR
                                TST:    AT+ CIFSR=?    */
#define   "AT+ CIPMUX"      /*  DES:    set mutiple connection
                                SET:    AT+ CIPMUX=<mode>
                                INQ:    AT+ CIPMUX?
                                PAR:    0 for single connection 1 for mutiple connection   */
#define   "AT+ CIPSERVER"   /*  DES:    set as server
                                SET:    AT+ CIPSERVER= <mode>[,<port> ]
                                PAR:    mode 0 to close server mode, mode 1 to open; port = port
                                EXP:    turn on as a TCP server: AT+CIPSERVER=1,8888
                                        check the self server IP address: AT+CIFSR=?    */
#define    "+IPD"           /*  DES:    received data   */


#endif


