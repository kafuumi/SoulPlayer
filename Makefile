NAME = SoulPlayer
CC = gcc
obj = main.o lyric.o common.o mplayer.o
cflags = -Wall -O2 `pkg-config "gtk+-3.0" --cflags --libs`

all: ${obj}
	@${CC} -o ${NAME} ${obj} ${cflags}

%.o: %.c
	@${CC} -c $< -o $@ ${cflags} 

# 将文件从虚拟机复制到主机中
.PHONY: copy
copy:
	cp ./* /mnt/hgfs/SoulPlayer/ -r
	cp .vscode/ /mnt/hgfs/SoulPlayer -r
	cp .gitignore /mnt/hgfs/SoulPlayer

.PHONY: clean
clean:
	-rm -f ${NAME} *.o