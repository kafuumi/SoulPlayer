NAME = SoulPlayer
CC = gcc
obj = main.o lyric.o common.o

all: ${obj}
	${CC} -o ${NAME} ${obj}
debug: ${obj}
	${CC} -g -o ${NAME} ${obj}
main.o: lyric.h main.c
lyric.o: lyric.c lyric.h common.h
common.o: common.h common.c

.PHONY: clean
clean:
	-rm -f ${NAME} *.o