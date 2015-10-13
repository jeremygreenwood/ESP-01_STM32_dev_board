#include "http-parser/http_parser.h"

void http_init_parser( void );
int http_parse( char *buffer, int buffer_size );
void http_test_parser( void );
