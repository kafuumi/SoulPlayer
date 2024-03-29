#include "mplayer.h"
#include "common.h"
#include "lyric.h"
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include <pthread.h>
static pthread_mutex_t mutex;

MPLAYER *initMplayer()
{
    //初始化互斥锁
    pthread_mutex_init(&mutex, NULL);
    //播放列表
    SONGLIST *songList = loadSongList("./assets/music/", "./assets/lyric/");

    MPLAYER *mplayer = malloc(sizeof(MPLAYER));
    mplayer->fifoFd = 0;
    mplayer->running = 0;
    mplayer->playing = 0;
    mplayer->songList = songList;
    return mplayer;
}
//启动mplayer
void startMplayer(MPLAYER *mplayer)
{
    mplayer->running = 1;
    execlp("mplayer", "mplayer", "-ac", "mad", "-slave", "-quiet", "-idle", "-input", "file=./song_fifo", "./assets/music/coffe.mp3", NULL);
    mplayer->running = 0;
}

void sendCommand(int fd, char *cmd, int len)
{
    //同步写入命令
    pthread_mutex_lock(&mutex);
    write(fd, cmd, len);
    pthread_mutex_unlock(&mutex);
}

void *sendPlayer(void *arg) //向mplayer无限的去发
{
    MPLAYER *player = (MPLAYER *)arg;

    while (1)
    {

        if (player->playing)
        {
            // usleep(100 * 1000);
            // sendCommand(player->fifoFd, "get_time_length\n");
            // usleep(100 * 1000);
            sendCommand(player->fifoFd, "get_percent_pos\nget_time_pos\n", 29);
            usleep(100 * 1000);
            // sendCommand(player->fifoFd, "get_meta_album\n");
            // usleep(100 * 1000);
            // sendCommand(player->fifoFd, "get_file_name\n");
            // usleep(100 * 1000);
        }
    }
}

//播放音乐
void playMusic(char *file, MPLAYER *mplayer)
{
    if (mplayer->running)
    {
        int size = strlen(file);
        char *cmd = malloc(sizeof(char) * (size + 11));
        strcpy(cmd, "loadfile ");
        strcat(cmd, file);
        cmd[size + 9] = '\n';
        cmd[size + 10] = '\0';
        sendCommand(mplayer->fifoFd, cmd, size + 10);
        free(cmd);
    }
}
//判断是否是mp3
int isMp3(char *path)
{
    //通过判断后缀名来判断
    int len = strlen(path);
    return path[len - 1] == '3' && path[len - 2] == 'p' && path[len - 3] == 'm' && path[len - 4] == '.';
}
// mp3后缀变为lrc
void mp3ToLrc(char *path, int len)
{
    path[len - 1] = 'c';
    path[len - 2] = 'r';
    path[len - 3] = 'l';
}

//结束mplayer
void quitMplayer(MPLAYER *mplayer)
{
    char *cmd = "q\n";
    sendCommand(mplayer->fifoFd, cmd, 2);
}

//继续播放
void unpausePlayer(MPLAYER *mplayer)
{
    //输入命令和暂停相同
    pausePlayer(mplayer);
}
//暂停正在播放的音乐
void pausePlayer(MPLAYER *mplayer)
{
    if (mplayer->running)
    {
        char *cmd = "pause\n";
        sendCommand(mplayer->fifoFd, cmd, 6);
    }
}

//后退，前进
void backMusic(MPLAYER *mplayer)
{
    if (mplayer->running)
    {
        //后退5秒
        char *cmd = "seek -5\n";
        sendCommand(mplayer->fifoFd, cmd, 8);
    }
}
void aheadMusic(MPLAYER *mplayer)
{
    if (mplayer->running)
    {
        //前进5秒
        char *cmd = "seek 5\n";
        sendCommand(mplayer->fifoFd, cmd, 7);
    }
}

//获取播放列表
SONGLIST *loadSongList(char *base, char *lrcBase)
{
    DIR *dir = opendir(base);
    if (dir == NULL)
    {
        fatal("打开文件失败:", base);
    }
    struct dirent *ptr;
    SONGLIST *songList = malloc(sizeof(SONGLIST));
    songList->now = songList->head = songList->tail = NULL;
    songList->num = 0;
    int dirLen = strlen(base);
    int lrcLen = strlen(lrcBase);
    //读取目录下的所有文件
    while ((ptr = readdir(dir)) != NULL)
    {
        //当前目录或父目录
        if (strcmp(ptr->d_name, ".") == 0 || strcmp(ptr->d_name, "..") == 0)
        {
            continue;
        }
        else if (isMp3(ptr->d_name))
        {
            SONG *song = malloc(sizeof(SONG));
            int size = strlen(ptr->d_name);
            song->title = subString(ptr->d_name, 0, size - 4);
            char *path = malloc(sizeof(char) * (size + dirLen + 1));
            char *lrc = malloc(sizeof(char) * (size + lrcLen + 1));
            //拼接路径
            path[0] = '\0';
            lrc[0] = '\0';
            strcat(path, base);
            strcat(path, ptr->d_name);
            strcat(lrc, lrcBase);
            strcat(lrc, ptr->d_name);

            song->path = path;
            mp3ToLrc(lrc, size + lrcLen);
            song->lrc = parseLrc(lrc);
            song->prev = NULL;
            song->next = NULL;
            //链表为空
            if (songList->head == NULL)
            {
                songList->head = song;
            }
            else
            {
                songList->tail->next = song;
                song->prev = songList->tail;
            }
            songList->tail = song;
            songList->num++;
        }
    }
    songList->now = songList->head;
    //循环链表，实现循环播放
    songList->head->prev = songList->tail;
    songList->tail->next = songList->head;
    return songList;
}

//释放内存
void freeMplayer(MPLAYER *mplayer)
{
    pthread_mutex_destroy(&mutex);
    if (mplayer == NULL)
    {
        return;
    }
    SONGLIST *songList = mplayer->songList;
    if (songList != NULL)
    {
        //循环链表断开
        songList->tail->next = NULL;
        for (SONG *h = songList->head; h != NULL;)
        {
            SONG *temp = h;
            h = h->next;

            lyricFree(temp->lrc);
            free(temp->title);
            free(temp->path);
            free(temp);
        }
        free(songList);
    }
    free(mplayer);
}