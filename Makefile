NAME = SoulPlayer
CC = gcc
obj = main.o lyric.o common.o mplayer.o

all: ${obj}
	${CC} -o ${NAME} ${obj}
debug: ${obj}
	${CC} -g -o ${NAME} ${obj}
main.o: lyric.h main.c
lyric.o: lyric.c lyric.h common.h
common.o: common.h common.c
mplayer.o: mplayer.c mplayer.h

# 将文件从虚拟机复制到主机中
.PHONY: copy
copy:
	cp ./* /mnt/hgfs/SoulPlayer/ -r
	cp .vscode/ /mnt/hgfs/SoulPlayer -r
	cp .gitignore /mnt/hgfs/SoulPlayer

.PHONY: clean
clean:
	-rm -f ${NAME} *.o