#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "html-eng.h"

#define BUF_SZ          400

char s_in_buf[ BUF_SZ ];
char s_out_buf[ BUF_SZ ];

int callback_key1
    (
    char               *out_buf,
    int                 buf_sz
    );

int callback_key2
    (
    char               *out_buf,
    int                 buf_sz
    );

int callback_key3
    (
    char               *out_buf,
    int                 buf_sz
    );

bool hlpr_read_file
    (
    const char         *filename,   /* file to load input from      */
    template_buf_type  *input       /* input buffer to fill         */
    );

void test_str_copy
    (
    char               *in_buf,
    char               *out_buf
    );

void test_read_from_file
    (
    char               *filename,
    char               *in_buf,
    char               *out_buf
    );

static const key_call_type key_calls[] =
    {
    { "key_1", callback_key1 },
    { "key_2", callback_key2 },
    { "asdlkfjaslkdf", callback_key3 }
    };

/*********************************************************************
*
*   PROCEDURE NAME:
*       main
*
*   DESCRIPTION:
*       Entry point into this program
*
*********************************************************************/

int main
    (
    int                 argc,       /* number of arguments          */
    char              **argv        /* argument array               */
    )
{
/*----------------------------------------------------------
Local Variables
----------------------------------------------------------*/

test_str_copy( s_in_buf, s_out_buf );
printf( "\n" );

test_read_from_file( "test_1.html", s_in_buf, s_out_buf );
test_read_from_file( "test_2.html", s_in_buf, s_out_buf );
test_read_from_file( "test_3.html", s_in_buf, s_out_buf );

return( 0 );

}   /* main() */


/*********************************************************************
*
*   PROCEDURE NAME:
*       callback_key1
*
*   DESCRIPTION:
*       Callback function for "key_1"
*
*********************************************************************/

int callback_key1
    (
    char               *out_buf,
    int                 buf_sz
    )
{
/*----------------------------------------------------------
Local Variables
----------------------------------------------------------*/

return( snprintf( out_buf, buf_sz, "42" ) );

}   /* callback_key1() */


/*********************************************************************
*
*   PROCEDURE NAME:
*       callback_key2
*
*   DESCRIPTION:
*       Callback function for "key_2"
*
*********************************************************************/

int callback_key2
    (
    char               *out_buf,
    int                 buf_sz
    )
{
/*----------------------------------------------------------
Local Variables
----------------------------------------------------------*/

return( snprintf( out_buf, buf_sz, "Claire de Lune" ) );

}   /* callback_key2() */


/*********************************************************************
*
*   PROCEDURE NAME:
*       callback_key3
*
*   DESCRIPTION:
*       Callback function for "key_3"
*
*********************************************************************/

int callback_key3
    (
    char               *out_buf,
    int                 buf_sz
    )
{
/*----------------------------------------------------------
Local Variables
----------------------------------------------------------*/

return( snprintf( out_buf, buf_sz, "--------------------------------------------------" ) );

}   /* callback_key3() */


/*********************************************************************
*
*   PROCEDURE NAME:
*       hlpr_read_file
*
*   DESCRIPTION:
*       Helper function to read input from file. Returns true if
*       successfully read file and false otherwise.
*
*********************************************************************/

bool hlpr_read_file
    (
    const char         *filename,   /* file to load input from      */
    template_buf_type  *input       /* input buffer to fill         */
    )
{
/*----------------------------------------------------------
Local Variables
----------------------------------------------------------*/
FILE                   *fil_ptr;
size_t                  read_cnt;

/*----------------------------------------------------------
Open file
----------------------------------------------------------*/
fil_ptr = fopen( filename, "r" );
if( !fil_ptr )
    {
    fprintf( stderr, "%s: Failed to open test file\n", __func__ );
    return( false );
    }

/*----------------------------------------------------------
Read as much of the test file into the input buffer as
possible.
----------------------------------------------------------*/
read_cnt = fread( input->buf, sizeof( char ), input->size_buf, fil_ptr );
input->size_buf = read_cnt;

return( ferror( fil_ptr ) == 0 );

}   /* hlpr_read_file() */


/*********************************************************************
*
*   PROCEDURE NAME:
*       test_str_copy
*
*   DESCRIPTION:
*       tests if the html engine can do a basic string copy
*
*********************************************************************/

void test_str_copy
    (
    char               *in_buf,
    char               *out_buf
    )
{
/*----------------------------------------------------------
Local Variables
----------------------------------------------------------*/
template_buf_type       input;
template_buf_type       output;

input.buf = in_buf;
memset( input.buf, 0, BUF_SZ );
input.size_buf = snprintf( input.buf, BUF_SZ, "This is a test string" ) + 1;

output.buf = out_buf;
memset( output.buf, 0, BUF_SZ );
output.size_buf = BUF_SZ;

process_template( &input, &output, NULL );

printf( "%s:\n\t%s\n", __func__, output.buf );

}   /* test_str_copy() */


/*********************************************************************
*
*   PROCEDURE NAME:
*       test_read_from_file
*
*   DESCRIPTION:
*       More general test case using an input file
*
*********************************************************************/

void test_read_from_file
    (
    char               *filename,
    char               *in_buf,
    char               *out_buf
    )
{
/*----------------------------------------------------------
Local Variables
----------------------------------------------------------*/
template_buf_type       input;
template_buf_type       output;
keys_type               keys;

/*----------------------------------------------------------
Setup input and output buffers
----------------------------------------------------------*/
input.buf = in_buf;
memset( input.buf, 0, BUF_SZ );
input.size_buf = BUF_SZ;
output.buf = out_buf;
memset( output.buf, 0, BUF_SZ );
output.size_buf = BUF_SZ;

keys.key_call = key_calls;
keys.num_keys = cnt_of_array( key_calls );

/*----------------------------------------------------------
Read file and if no error, then process the template
----------------------------------------------------------*/
if( hlpr_read_file( filename, &input ) )
    {
    process_template( &input, &output, &keys );

    printf( "%s:\n%s\n", __func__, output.buf );
    }

}   /* test_read_from_file() */


