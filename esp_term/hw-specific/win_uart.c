/**************************************************
    Includes
**************************************************/
#include <stdio.h>
#include <windows.h>
#include "uart.h"

/**************************************************
    Global
**************************************************/
static HANDLE fd;

/**************************************************
    uart_open
**************************************************/
int uart_open
    (
    const char* path,
    int flags
    )
{
DCB parameters;

fd = CreateFile( path, // example "\\\\.\\COM4"
                 GENERIC_READ | GENERIC_WRITE, 
                 0, 
                 NULL,
                 OPEN_EXISTING,
				 FILE_ATTRIBUTE_NORMAL,
                 NULL );

if( fd == INVALID_HANDLE_VALUE )
    {
    printf( "Problem opening port.\n" );
    return( 1 );
    }

parameters.DCBlength = sizeof( parameters );

if( GetCommState( fd, &parameters ) == 0 )
{
    printf( "Problem getting parameters.\n");
    CloseHandle( fd );
    return( 1 );
}

parameters.BaudRate = CBR_115200;
parameters.ByteSize = 8;
parameters.StopBits = ONESTOPBIT;
parameters.Parity   = NOPARITY;

if(SetCommState(fd, &parameters) == 0)
{
    printf( "Problem setting parameters.\n" );
    CloseHandle( fd );
    return( 1 );
}

return( 0 );
}

/**************************************************
    uart_close
**************************************************/
int uart_close
    (
    void
    )
{
return( CloseHandle( fd ) );
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
DWORD bytes_written;

if( !WriteFile( fd, buf, size, &bytes_written, NULL ) )
{
	printf( "Problem with UART write.\n" );
	CloseHandle( fd );
	return( 1 );
}

return( bytes_written );
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

if( !ReadFile( fd, buf, size, &read_num, NULL ) )
{
	printf( "Problem with UART read.\n" );
	CloseHandle( fd );
	return( 1 );
}

return( (int)read_num );
}

