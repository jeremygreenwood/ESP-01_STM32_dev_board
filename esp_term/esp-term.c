#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <errno.h>

#include "esp-at.h"

/**************************************************
    Defines
**************************************************/
#define cnt_of_array(ary)          (sizeof(ary) / sizeof(ary[0]))
#define BUF_SIZE 512

/**************************************************
    Types
**************************************************/
typedef struct 
    {
    char *info;
    void (*cmd)(void*);
    }cmd_list_type;

/**************************************************
    Prototypes
**************************************************/
void at(void *in);
void done_cmd(void *in);
void help(void *in);
void user_input(void *in);
void open_port(void *in);
void close_port(void *in);
void port_input(void *in);
void reset(void *in);
void mode_inquiry(void *in);
void mode_set(void *in);
void mux_set(void *in);
void server_set(void *in);
void send_hi(void *in);
void get_data(void *in);
void ap_settings(void *in);
void get_ip(void *in);
void get_ip_status(void *in);

/**************************************************
    Globals etc
**************************************************/
fd_set fds;
fd_set fds_new;
int    maxfd;
int    check_port;
volatile int done;
int cmd_mode;
cmd_list_type cmd_list[] =
{
{ "Print Help", help },
{ "Open port <port>", open_port },
{ "Close port", close_port },
{ "Read port", port_input },
{ "Send AT cmds", at },
{ "Send reset cmd", reset },
{ "Mode Inquiry", mode_inquiry },
{ "Mode Set 3", mode_set },
{ "CIPMUX Set 1", mux_set },
{ "Server Set 1:5000", server_set },
{ "Send HI!!", send_hi },
{ "Get Data", get_data },
{ "AP Settings", ap_settings },
{ "Get IP Address", get_ip },
{ "Get IP Status", get_ip_status },
{ "Exit", done_cmd }
};


// alt j m 
// 'AT/x0D/x0A'
// followed by control-m and control-j, yes both carriage return and 
// use stty -F /dev/ttyUSB0 115200, to set up the baud rate

/**************************************************
    main
        This does all the main things.
**************************************************/

int main
    ( 
    void   *argv, 
    int     argn 
    )
{
int in;
char line[512];
size_t ln_size;
struct timeval tv;

done = 0;
cmd_mode = 0;
check_port = -1;
tv.tv_usec = 0;
tv.tv_sec = 1;

/* Setup stdin */
FD_ZERO(&fds);
FD_ZERO(&fds_new);
FD_SET(STDIN_FILENO, &fds);
maxfd = STDIN_FILENO + 1;  /* maximum bit entry (fd) to test */
          
help(NULL);
while( !done )
    {
    fds_new = fds;
    /* Use the select as delay in the loop */
    tv.tv_usec = 0;
    tv.tv_sec = 1;
    select(maxfd, &fds_new, NULL, NULL, &tv);
    if(FD_ISSET(STDIN_FILENO, &fds_new))
        {
        user_input(NULL);
        }
    if( check_port != -1 )
        {
        port_input(NULL);
        }
    }
}

/**************************************************
    user_input
**************************************************/
void user_input
    (
    void *in
    )
{
int i, read_num;
char input[BUF_SIZE];
char *endptr;

memset( input, 0, BUF_SIZE );
read_num = read( 0, input, BUF_SIZE);
// printf("Cmd_mode: %d,  Input[%d]: %s\n", cmd_mode, strlen(input), input);
if(cmd_mode == 1)
    {
    at(input);
    }
else
    {
    i = strtol(strtok( input, " "), &endptr, 10);
    // printf("Input Num: %d\n", i);
    if( endptr != input
     && i >= 0 
     && i < cnt_of_array(cmd_list) )
        {
        //printf("CMD(%s): ", cmd_list[i].info);
        cmd_list[i].cmd((void*)input);
        }
    }
}

/**************************************************
    at
**************************************************/
void at
    (
    void *in
    )
{
int i; 
char *str;
char send_buf[512];
int idx;

memset( send_buf, 0, sizeof(send_buf));

if( cmd_mode == 0 )
    {
    cmd_mode = 1;
    return;
    }

// printf("In[%d]: %s\n", strlen(in), in);
i = strtol(strtok( in, " "), &str, 10);
if(i == 0)
    {
    printf("--- AT cmds, add params to choice ---\n");
    for( i = 0; i < cnt_of_array(at_list); i++ )
        {
        printf("%d - %s\n", i, at_list[i]);
        }
    printf("%d - EXIT\n", i);
    }
else if( i == cnt_of_array(at_list))
    {
    cmd_mode = 0;
    help(NULL);
    }
else
    {
    idx = 0;
    memmove( &send_buf[idx], at_list[i], strlen(at_list[i]));
    idx += strlen(at_list[i]);
    while( str = strtok( NULL, " \n\r"))
        {
        //printf( "Adding: -%s-\n", str);
        memmove( &send_buf[idx], str, strlen(str));
        idx += strlen(str);
        }
    memmove( &send_buf[idx], END, strlen(END));
    idx += strlen(END);
    write( check_port, send_buf, idx);
    printf("send_buf[%d]: -%s-\n", idx, send_buf);
    }
}

/**************************************************
    open_port
**************************************************/
void open_port
    (
    void *in
    )
{
char *port_in;
char p[] = "/dev/ttyUSB0";
struct termios options;

port_in = p;
errno = 0;
check_port = open(port_in, O_RDWR | O_NOCTTY | O_NONBLOCK);
printf("Opened Port: %s, num: %d, errno: %s\n", port_in, check_port, strerror(errno));
if( check_port < 0 )
    {
    printf("Problem opening port\n");
    return;
    }

//setting baud rates and stuff
tcgetattr(check_port, &options); 
cfsetispeed(&options, B115200);
cfsetospeed(&options, B115200);
options.c_cflag |= (CLOCAL | CREAD);
options.c_cflag &= ~PARENB;//next 4 lines setting 8N1
options.c_cflag &= ~CSTOPB;
options.c_cflag &= ~CSIZE;
options.c_cflag |= CS8;

options.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP
                           | INLCR | IGNCR | ICRNL | IXON);
options.c_oflag &= ~OPOST;
options.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
options.c_cflag &= ~(CSIZE | PARENB);
options.c_cflag |= CS8;

cfmakeraw(&options);
tcsetattr(check_port, TCSANOW, &options);
//tcsetattr(check_port, TCSAFLUSH, &options);

//options.c_cflag &= ~CNEW_RTSCTS;
//options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); //raw input
//options.c_iflag &= ~(IXON | IXOFF | IXANY); //disable software flow control

sleep(2); //required to make flush work, for some reason
tcflush(check_port,TCIOFLUSH);
FD_SET(check_port, &fds);
maxfd = check_port;
printf("Exit open_port\n");

}

/**************************************************
    close_port
**************************************************/
void close_port
    (
    void *in
    )
{
if( check_port != -1 )
    {
    printf("Closing port\n");
    close(check_port);
    check_port = -1;
    maxfd = STDIN_FILENO + 1;
    }
else
    {
    printf("Port is not open\n");
    }
}

/**************************************************
    port_input
**************************************************/
void port_input
    (
    void *in
    )
{
int read_num;
char input[BUF_SIZE];
int cnt;

cnt = 1;
memset( input, 0, BUF_SIZE );
errno = 0;
//    printf("About to read from port...\n");
read_num = read( check_port, input, BUF_SIZE);
//    printf("Done read from port...\n");
//if(read_num < 0)
//    {
//    printf("Error: %d  %s\n", errno, strerror(errno) ); 
//    }
while( read_num > 1 && cnt > 0 )
    {
    //printf("reading, cnt: %d...", cnt );
    printf("Port_In[%d]: -%s-\n", read_num, input);
    read_num = read( check_port, input, BUF_SIZE);
    cnt--;
    }
}

/**************************************************
    exit
**************************************************/
void done_cmd
    (
    void *in
    )
{
printf("Sending exit\n");
close_port(NULL);
done = 1;
}

/**************************************************
    help
**************************************************/
void help
    (
    void *in
    )
{
int i;
printf("--- Help Info ---\n");
for( i = 0; i < cnt_of_array(cmd_list); i++ )
    {
    printf("%d - %s\n", i, cmd_list[i].info);
    }
}


/**************************************************
   reset
**************************************************/
void reset
    (
    void *in
    )
{
int i;
char buf[BUF_SIZE];

i = 0;
memcpy( &buf[i], RST, sizeof(RST));
i += sizeof(RST);
memcpy( &buf[i], END, sizeof(END));
i += sizeof(END);

printf("reset\n");
//write( check_port, buf, i);
// write( check_port, cmd, sizeof(cmd));

}


/**************************************************
   mode_inquiry
**************************************************/
void mode_inquiry
    (
    void *in
    )
{
int i;
char buf[BUF_SIZE];

printf("mode_inquiry\n");
write( check_port, MODE, sizeof(MODE)-1);
write( check_port, Q, sizeof(Q));
write( check_port, END, sizeof(END));

}


/**************************************************
   mode_set
**************************************************/
void mode_set
    (
    void *in
    )
{
int i;
char buf[BUF_SIZE];

write( check_port, MODE, sizeof(MODE)-1);
write( check_port, EQ, sizeof(EQ));
write( check_port, "2", 1);
write( check_port, END, sizeof(END));
}

/**************************************************
   ap_settings
**************************************************/
void ap_settings
    (
    void *in
    )
{
int i;
char buf[BUF_SIZE];

write( check_port, CWSAP, sizeof(CWSAP)-1);
write( check_port, Q, sizeof(Q)-1);
//write( check_port, EQ, sizeof(EQ));
//write( check_port, "PK,pk,5,2", sizeof("PK,pk,5,2")-1);
write( check_port, END, sizeof(END));
}

/**************************************************
   mux_set
**************************************************/
void mux_set
    (
    void *in
    )
{
int i;
char buf[BUF_SIZE];

write( check_port, CIPMUX, sizeof(CIPMUX)-1);
write( check_port, EQ, sizeof(EQ));
write( check_port, "1", 1);
write( check_port, END, sizeof(END));
}

/**************************************************
   server_set
**************************************************/
void server_set
    (
    void *in
    )
{
int i;
char buf[BUF_SIZE];

write( check_port, CIPSERVER, sizeof(CIPSERVER)-1);
write( check_port, EQ, sizeof(EQ));
write( check_port, "1,5000", 6);
write( check_port, END, sizeof(END));
}

/**************************************************
   send_hi
**************************************************/
void send_hi
    (
    void *in
    )
{
int i;
char buf[BUF_SIZE];

write( check_port, CIPSEND, sizeof(CIPSEND)-1);
write( check_port, EQ, sizeof(EQ));
write( check_port, "4", 1);
write( check_port, END, sizeof(END));
write( check_port, "HI!!", sizeof("HI!!"));
}

/**************************************************
   get_data
**************************************************/
void get_data
    (
    void *in
    )
{
int i;
char buf[BUF_SIZE];

write( check_port, IPD, sizeof(IPD)-1);
// write( check_port, END, sizeof(END));
}

/**************************************************
   get_ip
**************************************************/
void get_ip
    (
    void *in
    )
{
int i;
char buf[BUF_SIZE];

char s[] = "AT+CIPAP?";

write( check_port, s, sizeof(s)-1);
write( check_port, END, sizeof(END));
}

/**************************************************
   get_ip_status
**************************************************/
void get_ip_status
    (
    void *in
    )
{
char *str;

//printf("In[%u]: %s\n", strlen(in), in);

while( str = strtok( NULL, " "))
    {
    printf("In[%u]: %s\n", strlen(str), str);
    }
write( check_port, CIPSTATUS, sizeof(CIPSTATUS)-1);
write( check_port, Q, sizeof(Q)-1);
write( check_port, END, sizeof(END));
}

