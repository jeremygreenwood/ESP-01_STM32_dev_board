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


int fd;


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
// Hard coding for now
fd = open(path, O_RDWR | O_NOCTTY | O_NONBLOCK);
printf("Opened Port: %s, num: %d, errno: %s\n", path, fd, strerror(errno));
if( fd < 0 )
    {
    printf("Problem opening port\n");
    return;
    }

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

}

/**************************************************
    uart_close
**************************************************/
int uart_close
    (
    void
    )
{
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
    printf("uart_read[%d]: ---%s---\n", total, buf);
    }
return( total );
}

