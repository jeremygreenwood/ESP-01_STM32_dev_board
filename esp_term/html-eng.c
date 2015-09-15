#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "html-eng.h"

/*--------------------------------------------------------------------
    DEFINES
--------------------------------------------------------------------*/

#define BUF_SIZE 512
#define MAX_KEY_LEN 20

/*--------------------------------------------------------------------
    PROTOTYPES
--------------------------------------------------------------------*/

char s_key_buf[ MAX_KEY_LEN ];

/*--------------------------------------------------------------------
    PROTOTYPES
--------------------------------------------------------------------*/

/*********************************************************************
*
*   PROCEDURE NAME:
*       process_template
*
*   DESCRIPTION:
*       Receives an input html template and inserts the keys in the
*       template with data from the callbacks.
*
*********************************************************************/

int process_template
    (
    template_buf_type  *in,         /* input template buffer        */
    template_buf_type  *out,        /* output template buffer       */
    keys_type          *keys        /* list of keys and callbacks   */
    )
{
/*----------------------------------------------------------
Local Variables
----------------------------------------------------------*/
char                   *in_buf;
char                   *out_buf;
int                     i;
char                    c;
char                   *end_brack;
char                   *key_cpy;
char                   *key_ptr;
int                     out_buf_sz;

in_buf = in->buf;
out_buf = out->buf;
out_buf_sz = out->size_buf;

while( *in_buf )
    {
    c = *in_buf;
    in_buf++;

    if( c == '{' )
        {
        end_brack = strchr( in_buf, '}' );

        if( end_brack )
            {
            *end_brack = '\0';
            key_cpy = in_buf;
            key_ptr = s_key_buf;
            memset( s_key_buf, 0, sizeof( s_key_buf ) );

            while( *key_cpy )
                {
                if( !isspace( *key_cpy ) )
                    {
                    *key_ptr = *key_cpy;
                    key_ptr++;
                    }
                key_cpy++;
                }

            for( i = 0; i < keys->num_keys; i++ )
                {
                if( strcmp( s_key_buf, keys->key_call[ i ].key ) == 0 )
                    {
                    out_buf += keys->key_call[ i ].cb( out_buf, out_buf_sz );
                    break;
                    }
                }

            *end_brack = '}';
            in_buf = end_brack + 1;
            }
        else
            {
            *out_buf = c;
            out_buf++;
            out_buf_sz--;
            }
        }
    else
        {
        *out_buf = c;
        out_buf++;
        out_buf_sz--;
        }
    }

return( 0 );

}   /* process_template() */


