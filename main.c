#include "lyric.h"
#include "mplayer.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pthread.h>
#include <sys/wait.h>
int main()
{
    /*LYRIC *lyric = parseLrc("./assets/lyric/spring.lrc");
    for (LYRIC_NODE *h = lyric->head; h != NULL; h = h->next)
    {
        printf("%d = %s\n", h->time, h->lyric);
    }
    SONGLIST *songList = loadSongList("./temp/", "./assets/lrc/");
    for(SONG *song = songList->head; song!=NULL; song=song->next){
        printf("%s %s\n", song->path, song->lrc);
    }*/

    MPLAYER *mplayer = malloc(sizeof(MPLAYER));
    mplayer->fifoFd = 0;
    mplayer->running = 0;
    mkfifo("./song_fifo", 0666);
    pid_t pid;
    pid = fork();
    if (pid == 0)
    {
        startMplayer(mplayer);
    }else{
        //等待子进程结束
        int status;
        wait(&status);
        printf("main");
    }
    return 0;
}
