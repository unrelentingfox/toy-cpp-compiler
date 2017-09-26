YACC=bison
LEX=flex
CC=cc

all: 120

.c.o:
	$(CC) -c $<

120: 120bison.o 120flex.o
	cc -o 120 120bison.o 120flex.o

120bison.c 120bison.h: 120bison.y
	$(YACC) -dt --verbose 120bison.y
	mv -f 120bison.tab.c 120bison.c
	mv -f 120bison.tab.h 120bison.h

120flex.c: 120flex.l
	$(LEX) -t 120flex.l >120flex.c

120flex.o: 120bison.h

clean:
	rm -f 120 *.o
	rm -f 120flex.c 120bison.c 120bison.h