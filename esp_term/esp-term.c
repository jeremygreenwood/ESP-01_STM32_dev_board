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
void AT(void *in);
void done_cmd(void *in);
void help(void *in);
void user_input(void *in);
void open_port(void *in);
void close_port(void *in);
void port_input(void *in);
void reset(void *in);
void mode_inquiry(void *in);

/**************************************************
    Globals etc
**************************************************/
fd_set fds;
fd_set fds_new;
int    maxfd;
int    check_port;
volatile int done;
cmd_list_type cmd_list[] =
{
{ "Print Help", help },
{ "Open port <port>", open_port },
{ "Close port", close_port },
{ "Read port", port_input },
{ "Send AT cmd", AT },
{ "Send reset cmd", reset },
{ "Mode Inquiry", mode_inquiry },
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
    /* Use the select as delay in reading the port */
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
i = strtol(strtok( input, " "), &endptr, 10);
// printf("Input Num: %d\n", i);
if( endptr != input
 && i >= 0 
 && i < cnt_of_array(cmd_list) )
    {
    printf("CMD(%s): ", cmd_list[i].info);
    cmd_list[i].cmd((void*)input);
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
    printf("Port_In[%d]: %s\n", read_num, input);
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
    AT
**************************************************/
void AT
    (
    void *in
    )
{
int i;
char buf[BUF_SIZE];
i = 0;
memcpy( &buf[i], "AT", sizeof("AT"));
i += sizeof("AT");
//memcpy( &buf[i], END, sizeof(END));
// i += sizeof(END);
// printf("Sending AT\n");
write( check_port, buf, i);
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

// "AT+RST" + 0x0D 0x0A
unsigned char cmd[] = { 0x41, 0x54, 0x2B, 0x52, 0x53, 0x54, 0x0D, 0x0A };

i = 0;
memcpy( &buf[i], RST, sizeof(RST));
i += sizeof(RST);
memcpy( &buf[i], END, sizeof(END));
i += sizeof(END);

printf("reset\n");
//write( check_port, buf, i);
write( check_port, cmd, sizeof(cmd));

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

// "AT+CWMODE?" + 0x0D 0x0A
unsigned char cmd[] = { 0x41, 0x54, 0x2B, 0x43, 0x57, 0x4D, 0x4F, 0x44, 0x45, 0x3F, 0x0D, 0x0A }; 

printf("mode_inquiry\n");
write( check_port, cmd, sizeof(cmd));

}



