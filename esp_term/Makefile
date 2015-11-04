# Sorry windows, need to update to make for you.

CC=gcc
CFLAGS=-Wall -ggdb
LDFLAGS=
INCDIRS=-I. -Iprj -Iprj/callbacks -Iesp-server/http-parser -Iesp-server/at-parser -Iprj/uart
SOURCES=esp-server/http-parser/http-parser/http_parser.c        \
        esp-server/at-parser/at_parser.c                        \
        esp-server/http-parser/http_parser_interface.c          \
        prj/callbacks/at_cb.c									\
        prj/callbacks/key_cb.c									\
        main.c
        
LINUX_SRC=prj/uart/linux_uart.c
WIN_SRC=hw-specific/win_uart.c

OBJECTS=$(SOURCES:.c=.o) $(LINUX_SRC:.c=.o)
WIN_OBJECTS=$(WIN_SRC:.c=.o)
LINUX_OBJECTS=$(LINUX_SRC:.c=.o)
APP=a.app

all: $(APP)
win: WEB $(SOURCES) $(WIN_SRC) $(APP)
    
$(APP): WEB $(OBJECTS) 
	@$(CC) $(INCDIRS) $(LDFLAGS) $(OBJECTS) -o $@

%.o: %.c
	@$(CC) $(CFLAGS) $(INCDIRS) -c $< -o $@ 

WEB:
	@cd prj/template; \
	echo "#ifndef TEMPLATES_H" > ../templates.h; \
	echo "#define TEMPLATES_H" >> ../templates.h; \
	for file in * ; do xxd -i $$file >> ../templates.h; done; \
	echo "#endif" >> ../templates.h;        \
	echo " " >> ../templates.h

clean:
	@rm -rf $(OBJECTS)

