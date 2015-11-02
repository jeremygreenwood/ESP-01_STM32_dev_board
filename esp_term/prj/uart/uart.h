
#ifdef OS_WINDOWS
#define HW_SPECIFIC_PATH "\\\\.\\COM4"
#else
#define HW_SPECIFIC_PATH "/dev/ttyUSB0"
#endif

/**************************************************
    Prototypes
These are the basic calls for reading and writing
to the ESP-01.  For testing on your sytsem, 
write your own implementations of these calls.
**************************************************/
int uart_open(const char* path, int flags);
int uart_read(char *buf, int size);
int uart_write(char *buf, int size);
int uart_close();
