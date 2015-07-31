#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>


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
{ "Send AT cmd", AT },
{ "Open port <port>", open_port },
{ "Close port", close_port },
{ "Read port", port_input },
{ "Exit", done_cmd }
};

//alt j m 
// 'AT/x0D/x0A'


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
done = 0;
check_port = -1;

/* Setup stdin */
FD_ZERO(&fds);
FD_ZERO(&fds_new);
FD_SET(STDIN_FILENO, &fds);
maxfd = STDIN_FILENO + 1;  /* maximum bit entry (fd) to test */
          
help(NULL);
while( !done )
    {

    fds_new = fds;
    /* block until input becomes available */
    select(maxfd, &fds_new, NULL, NULL, NULL);
    if(FD_ISSET(STDIN_FILENO, &fds_new))
        {
        user_input(NULL);
        }
    if( check_port != -1 
     && FD_ISSET(check_port, &fds_new))
        {
        printf("Port is set\n");
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
    cmd_list[i].cmd((void*)input);
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
printf("Sending AT\n");
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
char p[] = "/dev/random";

port_in = p;
check_port = open(port_in, O_RDWR | O_NOCTTY | O_NONBLOCK);
printf("Opened Port: %s, num: %d\n", port_in, check_port);

FD_SET(check_port, &fds);
maxfd = check_port;
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
int i, read_num;
char input[BUF_SIZE];

memset( input, 0, BUF_SIZE );
read_num = read( check_port, input, BUF_SIZE);
printf("Input[%d]: %s (0X%02x 0X%02x)\n", read_num, input, input[0], input[1]);

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
    printf("%d - %s\n", i, cmd_list[i]);
    }
}

