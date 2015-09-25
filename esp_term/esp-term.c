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
void send_over_air(void *in);
void setup(void *in);
void send_cmd(char *in);

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
{ "Send over air", send_over_air },
{ "setup", setup },
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
char input_2[BUF_SIZE];
char *endptr;

memset( input, 0, BUF_SIZE );
read_num = read( 0, input, BUF_SIZE);
memmove(input_2, input, BUF_SIZE);
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
        cmd_list[i].cmd((void*)input_2);
        }
    else
        {
        help(NULL);
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
int ret, total;
char input[8 * BUF_SIZE]; // 4k chunks for now

total = 0;
do  {
    ret = read( check_port, input, BUF_SIZE - total);
    if(ret > 0)
        {
        total += ret;
        }
    }while( ret > 1 && total < 8 * BUF_SIZE );

input[total] = '\0';
if(total > 0)
    {
    printf("Port_In[%d]: -%s-\n", total, input);
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
   send_over_air
**************************************************/
void send_over_air
    (
    void *in
    )
{
int idx;
char send_buf[BUF_SIZE];
char data_len[32];
int size;
int id;
int i;
char *str;

//printf("IN send_over_air: [%d] %s\n", strlen(in),in);
char resp[] = 
"HTTP/1.1 200 OK\nDate: Fri, 11 Fri 2015 06:25:59 GMT\nContent-Type: text/html\nContent-Length: %u\n\n<html><body><h1>Hello glorious world!</h1>(all the things I needed to say)</body></html>\n\n";


printf("IN send_over_air\n");
id = strtol(strtok( in, " "), &str, 10);
id = strtol(strtok( NULL, " "), &str, 10);
printf("id: %d\n", id);

size = strlen(resp);
sprintf(send_buf, "%s=%d,%d", CIPSEND, id, size);
send_cmd(send_buf);
port_input(NULL);
write( check_port, resp, size);

}


/**************************************************
   setup
**************************************************/
void setup
    (
    void *in
    )
{
#define SND_CMD(cmd)        \
do {                        \
send_cmd(cmd);              \
sleep(1);                   \
port_input(NULL);           \
}while(0)

char send_buf[512];

SND_CMD(AT);
SND_CMD(RST);
SND_CMD(CWMODE "=3");
SND_CMD(CWSAP"=\"SSID\",\"password\",3,1");
SND_CMD(CIPMUX"=1");
SND_CMD(CIPSERVER"=1,5000");
SND_CMD(CIFSR"=?");

//SND_CMD(CIPSTATUS);

}


/**************************************************
   send_cmd
**************************************************/
void send_cmd
    (
    char *in
    )
{
char send_buf[512];
int idx;

memset(send_buf, 0, 512);
idx = 0;
memmove( &send_buf[idx], in, strlen(in));
idx += strlen(in);
memmove( &send_buf[idx], END, strlen(END));
idx += strlen(END);
write( check_port, send_buf, idx);
printf("send_buf[%d]: -%s-\n", idx, send_buf);

}



