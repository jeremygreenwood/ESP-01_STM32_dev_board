/**************************************************
    Includes
**************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <windows.h>
#include "uart.h"


/* NB:  This file is mostly just a stub.  Never been used.  */

/**************************************************
    Global
**************************************************/
HANDLE fd;

/**************************************************
    uart_open
**************************************************/
int uart_open
    (
    const char* path,
    int flags
    )
{
fd = CreateFile( path,  
                 GENERIC_READ | GENERIC_WRITE, 
                 0, 
                 0, 
                 OPEN_EXISTING,
                 FILE_FLAG_OVERLAPPED,
                 0);
if (fd == INVALID_HANDLE_VALUE)
    {
    printf("Problem opening port\n");
    return;
    }
}

/**************************************************
    uart_close
**************************************************/
int uart_close
    (
    void
    )
{
printf("uart_close\n");
return( 0 );
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
printf("uart_write\n");
return( 0 );
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
DWORD read_num;
int total;

ReadFile(fd, buf, size, &read_num, NULL);
      // if (GetLastError() != ERROR_IO_PENDING)
total = (int)read_num;

if(total > 0)
    {
    printf("uart_read[%d]: ---%s---\n", total, buf);
    }

return( (int)read_num );
}

