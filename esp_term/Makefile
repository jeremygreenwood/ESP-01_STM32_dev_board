
ALL: WEB
	gcc -Wall -ggdb -I. -Iprj -Iesp-server/http-parser -Iesp-server/at-parser -Iprj/uart esp-server/http-parser/http-parser/http_parser.c prj/uart/linux_uart.c esp-server/at-parser/at_parser.c esp-server/http-parser/http_parser_interface.c main.c -o a.app

WIN:
	gcc -Wall -Iat-parser -I. -Ihw-specific http-parser/http_parser.c hw-specific/win_uart.c at-parser/at_parser.c http_parser_interface.c main.c -o a.exe

WEB:
	@cd prj/template; \
	echo "" > ../templates.h; \
	for file in * ; do xxd -i $$file >> ../templates.h; done


