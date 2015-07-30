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

/**************************************************
    Globals etc
**************************************************/
fd_set fds;
int    maxfd;
int    check_port;
volatile done;
cmd_list_type cmd_list[] =
{
{ "Print Help", help },
{ "Send AT cmd", AT },
{ "Open port <port>", open_port },
{ "CLose port", close_port },
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
check_port = 0;
    
/* Setup stdin */
FD_ZERO(&fds);
FD_SET(STDIN_FILENO, &fds);
maxfd = STDIN_FILENO + 1;  /* maximum bit entry (fd) to test */
          
help(NULL);
while( !done )
    {
    /* block until input becomes available */
    select(maxfd, &fds, NULL, NULL, NULL);
    if(FD_ISSET(STDIN_FILENO, &fds))
        {
        user_input(NULL);
        }
    if(check_port && FD_ISSET(check_port, &fds))
        {
        printf("Port is set\n");
        //port_input(NULL);
        }
    }
}

#ifdef NEWPROG
      #include <sys/time.h>
      #include <sys/types.h>
      #include <unistd.h>
        
      main()
      {
        int    fd1, fd2;  /* input sources 1 and 2 */
        fd_set readfs;    /* file descriptor set */
        int    maxfd;     /* maximum file desciptor used */
        int    loop=1;    /* loop while TRUE */ 
        
        /* open_input_source opens a device, sets the port correctly, and
           returns a file descriptor */
        fd1 = open_input_source("/dev/ttyS1");   /* COM2 */
        if (fd1<0) exit(0);
        fd2 = open_input_source("/dev/ttyS2");   /* COM3 */
        if (fd2<0) exit(0);
        maxfd = MAX (fd1, fd2)+1;  /* maximum bit entry (fd) to test */
        
        /* loop for input */
        while (loop) {
          FD_SET(fd1, &readfs);  /* set testing for source 1 */
          FD_SET(fd2, &readfs);  /* set testing for source 2 */
          /* block until input becomes available */
          select(maxfd, &readfs, NULL, NULL, NULL);
          if (FD_ISSET(fd1))         /* input from source 1 available */
            handle_input_from_source1();
          if (FD_ISSET(fd2))         /* input from source 2 available */
            handle_input_from_source2();
        }
      }   
#endif


/**************************************************
    user_input
**************************************************/
void user_input
    (
    void *in
    )
{
int i;
char *input = readline(NULL);

i = atoi(strtok( input, " " ));
//i = atoi(input);
if( i >= 0 && i < cnt_of_array(cmd_list) )
    {
    cmd_list[i].cmd((void*)input);
    }
else
    {
    help(NULL);
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

// port_in = strtok( NULL, " ");
port_in = p;

printf("Opening Port: %s\n", port_in);
check_port = open(port_in, O_RDONLY ); // WR | O_NOCTTY ); // | O_NONBLOCK);
}

/**************************************************
    close_port
**************************************************/
void close_port
    (
    void *in
    )
{
if(check_port)
    {
    close(check_port);
    check_port = 0;
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
    printf("%d - %s\n", i, cmd_list[i]);
    }
}



