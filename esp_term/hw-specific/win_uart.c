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








#ifdef SOME_RAND_CODE

// +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=
E_boolean OpenCom1(void)
{
   COMMTIMEOUTS timeouts;

   comPorthandle = CreateFile("COM1",  // Specify port device: default "COM1"
   GENERIC_READ | GENERIC_WRITE,       // Specify mode that open device.
   0,                                  // the device isn't shared.
   NULL,                               // the object gets a default security.
   OPEN_EXISTING,                      // Specify which action to take on file.
   0,                                  // default (not overlapped i/o).
   NULL);                              // default (hTemplate must be NULL for COM devices).

   if (comPorthandle == INVALID_HANDLE_VALUE)
      return False;

   deviceControlBlock.DCBlength = sizeof(deviceControlBlock);

    if((GetCommState(comPorthandle, &deviceControlBlock) == 0))
    {
      // CodeMe: do what?
      return False;
    }

    deviceControlBlock.BaudRate = CBR_115200;
    deviceControlBlock.StopBits = ONESTOPBIT;
    deviceControlBlock.Parity   = NOPARITY;
    deviceControlBlock.ByteSize = DATABITS_8;
    deviceControlBlock.fRtsControl = 0;

    if (!SetCommState(comPorthandle, &deviceControlBlock))
    {
      // CodeMe: do what?
      return False;
    }

#if 0
// alternative to GetCommState() and SetCommState()
// both versions succeed
   if (!BuildCommDCB("115200,n,8,1", &deviceControlBlock))
    {
      // CodeMe: do what?
      return False;
    }
#endif 

    // set short timeouts on the comm port.
    timeouts.ReadIntervalTimeout = 0;
    timeouts.ReadTotalTimeoutMultiplier = 1;
    timeouts.ReadTotalTimeoutConstant = 1;
    timeouts.WriteTotalTimeoutMultiplier = 1;
    timeouts.WriteTotalTimeoutConstant = 1;
    if (!SetCommTimeouts(comPorthandle, &timeouts))
    {
      // CodeMe: do what?
      return False;
    }

   return True;
}//OpenCom1()

// +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=
void      ReadCharacterFromCom1(INPUT char *theCharacter)
{
   DWORD numBytesRead;

   numBytesRead = 0;

   while (numBytesRead == 0)
   {
      ReadFile(comPorthandle,           // handle of file to read
               theCharacter,            // store read data here
               sizeof(theCharacter),    // number of bytes to read
               &numBytesRead,           // pointer to number of bytes actually read
               NULL);
   }
   return;
}//ReadCharacterFromCom1()


#endif










