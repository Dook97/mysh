.PHONY: all clean
.INTERMEDIATE: lex.yy.c pars.tab.c pars.tab.h

all: mysh

mysh: src/*.c lex.yy.c pars.tab.c
	cc -o $@ -I./include -lreadline -lfl $^

lex.yy.c: src/lex.l pars.tab.h
	flex src/lex.l

pars.tab.c pars.tab.h: src/pars.y
	bison -d $^

clean:
	rm -f *.o mysh lex.yy.c pars.tab.c pars.tab.h
