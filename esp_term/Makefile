

ALL:
	gcc -Wall -ggdb -I. -Iat-parser -Ihw-specific http-parser/http_parser.c hw-specific/linux_uart.c at-parser/at_parser.c http_parser_interface.c main.c -o a.app

WIN:
	gcc -Wall -Iat-parser -I. -Ihw-specific http-parser/http_parser.c hw-specific/win_uart.c at-parser/at_parser.c http_parser_interface.c main.c -o a.exe
