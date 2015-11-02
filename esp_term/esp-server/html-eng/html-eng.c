#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include <string.h>

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
    char *key;
    void (*call)(void*);
    }key_call_type;

typedef struct 
    {
    key_call_type *key_call;
    int num_keys;
    char *delim;
    }keys_type;

typedef struct
    {
    char *buf;
    int size_buf;
    }template_buf_type;

/**************************************************
    Prototypes
**************************************************/
int process_template
    ( 
    template_buf_type *in, 
    template_buf_type *out, 
    keys_type *keys
    );

/**************************************************
    process_template
**************************************************/
int process_template
    ( 
    template_buf_type *in, 
    template_buf_type *out, 
    keys_type *keys
    )
{
char    *ret;
char    *in_buf;
char    *out_buf;

in_buf = in->buf;
out_buf = out_buf;

ret = strstr( in->buf, keys->delim);




}






