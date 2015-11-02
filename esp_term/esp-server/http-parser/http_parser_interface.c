#include <string.h>
#include <stdio.h>

#include "http_parser_interface.h"

struct http_parser parser;

int body_cb( http_parser *p, const char *buf, size_t len )
{
  printf( "Body.\r\n" );
  return 0;
}


int chunk_complete_cb( http_parser *p )
{
  printf( "Chunk complete.\r\n" );
  return 0;
}


int chunk_header_cb( http_parser *p )
{
  printf( "Chunk header.\r\n" );
  return 0;
}


int headers_complete_cb( http_parser *p )
{
  printf( "Header complete.\r\n" );
  return 0;
}


int header_field_cb( http_parser *p, const char *buf, size_t len )
{
  printf( "Header field.\r\n" );
  return 0;
}


int header_value_cb( http_parser *p, const char *buf, size_t len )
{
  printf( "Header value.\r\n" );
  return 0;
}


int message_begin_cb( http_parser *p )
{
  printf( "Message begin./r/n" );
  return 0;
}


int message_complete_cb( http_parser *p )
{
  printf( "Message complete./r/n" );
  return 0;
}


int response_status_cb( http_parser *p, const char *buf, size_t len )
{
  printf( "Response status./r/n" );
  return 0;
}


int request_url_cb( http_parser *p, const char *buf, size_t len )
{
  printf( "Request url./r/n" );
  return 0;
}


const struct http_parser_settings settings =
								  {
								   .on_message_begin    = message_begin_cb
								  ,.on_header_field     = header_field_cb
								  ,.on_header_value     = header_value_cb
								  ,.on_url              = request_url_cb
								  ,.on_status           = response_status_cb
								  ,.on_body             = body_cb
								  ,.on_headers_complete = headers_complete_cb
								  ,.on_message_complete = message_complete_cb
								  ,.on_chunk_header     = chunk_header_cb
								  ,.on_chunk_complete   = chunk_complete_cb
								  };


void http_init_parser( void )
{
http_parser_init( &parser, HTTP_REQUEST );
}


int http_parse( char *buffer, int buffer_size )
{
return( http_parser_execute( &parser, &settings, buffer, buffer_size ) );
}


void http_test_parser( void )
{
	char message_buffer[] = "GET / HTTP/1.1\r\n"
				            "Host: 192.168.4.1:5000\r\n"
							"User-Agent: Mozilla/5.0 (X11; Linux armv7l; rv:31.0) Gecko/20100101 Firefox/31.0 Iceweasel/31.6.0\r\n"
					        "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n"
                            "Accept-Language: en-US,en;q=0.5\r\n"
                            "Accept-Encoding: gzip, deflate\r\n"
                            "Connection: keep-alive\r\n"
							"\r\n";

	printf( "Parsing header.\r\n" );
	(void)http_parse( (char *)&message_buffer, sizeof( message_buffer ) );
}
