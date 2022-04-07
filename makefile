NAME = SoulPlayer
CC = gcc
obj = main.o lyric.o

all: ${obj}
	${CC} -o ${NAME} ${obj}

main.o: lyric.h main.c
lyric.o: lyric.c lyric.h

.PHONY: clean
clean:
	-rm -f ${NAME} *.o