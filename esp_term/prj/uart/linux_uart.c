/**************************************************
    Includes
**************************************************/
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

#include "uart.h"

/**************************************************
    globals
**************************************************/
static int fd;

#define DBG_UART 1
#ifdef DBG_UART
int dbg_fd;
char dbg_file[] = "dbg_uart.log";
#endif

/**************************************************
    uart_open
**************************************************/
int uart_open
    (
    const char* path,
    int flags
    )
{
struct termios options;

errno = 0;
fd = open(path, O_RDWR | O_NOCTTY | O_NONBLOCK);
printf("Opened Port: %s, num: %d, errno: %s\n", path, fd, strerror(errno));
if( fd < 0 )
    {
    printf("Problem opening port\n");
    return -1;
    }


#ifdef DBG_UART
errno = 0;
dbg_fd = open(dbg_file, O_WRONLY | O_CREAT);
printf("Opening file: %s, num: %d, errno: %s\n", dbg_file, dbg_fd, strerror(errno));
if( dbg_fd < 0 )
    {
    printf("Problem opening dbg file\n");
    return -1;
    }
#endif


//setting baud rates and stuff
tcgetattr(fd, &options); 
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
tcsetattr(fd, TCSANOW, &options);

sleep(2); //required to make flush work, for some reason
tcflush(fd,TCIOFLUSH);
printf("opened port\n");
return(0);
}

/**************************************************
    uart_close
**************************************************/
int uart_close
    (
    void
    )
{

#ifdef DBG_UART
close(dbg_fd);
#endif
return( close(fd) );
}

/**************************************************
    uart_write
**************************************************/
int uart_write
    (
    char *buf,
    int size
    )
{

#ifdef DBG_UART
write(dbg_fd, "\n<OUT>", sizeof("\n<OUT>"));
write(dbg_fd, buf, size);
write(dbg_fd, "<OUT\\>", sizeof("<OUT\\>"));
#endif

return( write( fd, buf, size) );
}

/**************************************************
    uart_read
**************************************************/
int uart_read
    (
    char *buf,
    int size
    )
{
int ret, total;

total = 0;
do  {
    ret = read( fd, &buf[total], size - total);
    if(ret > 0)
        {
        total += ret;
        }
    }while( ret > 1 && total < size );

buf[total] = '\0';
if(total > 0)
    {
    // printf("uart_read[%d]: ---%s---\n", total, buf);
    #ifdef DBG_UART
    write(dbg_fd, "\n<IN>", sizeof("\n<IN>"));
    write(dbg_fd, buf, total);
    write(dbg_fd, "<IN\\>", sizeof("<IN\\>"));
    #endif
    }
return( total );
}

