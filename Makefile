
NAME = SoulPlayer
CC = gcc
obj = main.o list.o lyric.o

all: ${obj}
	@echo build ${obj}
	@${CC} -o ${NAME}  ${obj}

${obj}:

.PHONY: clean
clean:
	@echo clean
	-rm -f ${NAME} *.o