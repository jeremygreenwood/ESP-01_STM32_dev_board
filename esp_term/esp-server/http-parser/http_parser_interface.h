#include "http-parser/http_parser.h"

void http_init_parser( void );
int http_parse( char *buffer, int buffer_size );
void http_test_parser( void );


/*

url, keys_type, web_content_template

main
    setup uart
    setup cb for at cmds
    setup cb for http parser
    setup cb for html keys

at_cb.c
http_cb.c
html_cb.c


at-parser
    callbacks for AT CMDS
    IPD -> http-parser
http-parser
    callbacks for http requests
    GET cmd -> html-eng -> uart send over the wire
html-eng
    callbacks for processing templates with keys
*/






