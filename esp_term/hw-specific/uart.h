
/**************************************************
    Prototypes
These are the basic calls for reading and writing
to the ESP-01.  For testing on your sytsem, 
write your own implementations of these calls.
**************************************************/
int uart_open(const char* path, int flags);
int uart_read(int fd, void *buf, int size);
int uart_write(int fd, void *buf, int size);
int uart_close(int fd);

