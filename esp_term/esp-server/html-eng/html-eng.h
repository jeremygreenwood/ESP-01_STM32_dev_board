#ifndef HTML_ENG_H_
#define HTML_ENG_H_

/*--------------------------------------------------------------------
    DEFINES
--------------------------------------------------------------------*/

#ifndef cnt_of_array
#define cnt_of_array(ary)          (sizeof(ary) / sizeof(ary[0]))
#endif

/*--------------------------------------------------------------------
    TYPES
--------------------------------------------------------------------*/

typedef int (*tmplt_key_cb_type)( char *out_buf, int buf_sz );

typedef struct
    {
    char               *key;        /* null terminated key string   */
    tmplt_key_cb_type   cb;         /* callback to handle key       */
    } key_call_type;

typedef struct
    {
    const key_call_type
                       *key_call;   /* list of keys and callbacks   */
    int                 num_keys;   /* number of keys               */
    } keys_type;

typedef struct
    {
    char               *buf;        /* template buffer              */
    int                 size_buf;   /* size of buffer               */
    } template_buf_type;

/*--------------------------------------------------------------------
    PROTOTYPES
--------------------------------------------------------------------*/

int process_template
    (
    template_buf_type  *in,         /* input template buffer        */
    template_buf_type  *out,        /* output template buffer       */
    keys_type          *keys        /* list of keys and callbacks   */
    );

#endif /* HTML_ENG_H_ */
