.PHONY: all clean

CC = cc
CFLAGS = -I./include -Wall -Wextra -g
LDFLAGS = -lreadline

all: mysh

mysh: src/*.c include/*.h src/lex.yy.c src/pars.tab.c include/pars.tab.h
	$(CC) $(CFLAGS) $(LDFLAGS) $(CUFLAGS) -o $@ src/*.c

src/lex.yy.c: src/lex.l include/pars.tab.h
	flex -o $@ $<

src/pars.tab.c include/pars.tab.h: src/pars.y
	bison -d $^
	mv pars.tab.c src/
	mv pars.tab.h include/

clean:
	rm -f mysh src/lex.yy.c src/pars.tab.c include/pars.tab.h
