.PHONY: all clean

CC = cc
CFLAGS = -I./include -Wall -Wextra
LDFLAGS = -lreadline -lfl

all: mysh

mysh: src/*.c include/*.h lex.yy.c pars.tab.c
	$(CC) $(CFLAGS) $(LDFLAGS) $(CUFLAGS) -o $@ $$(find -type f -name '*.c')

lex.yy.c: src/lex.l pars.tab.h
	flex src/lex.l

pars.tab.c pars.tab.h: src/pars.y
	bison -d $^

clean:
	rm -f *.o mysh lex.yy.c pars.tab.c pars.tab.h
