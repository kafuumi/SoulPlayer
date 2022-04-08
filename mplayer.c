#include "mplayer.h"
#include "common.h"
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <stdio.h>
//启动mplayer
void startMplayer(MPLAYER *mplayer)
{
    mplayer->running = 1;
    execlp("mplayer", "mplayer", "-ac", "mad", "-slave", "-quiet", "-idle", "-input", "file=./song_fifo", NULL);
    mplayer->running = 0;
}
//获取当前播放的时间点
void getTimePos(MPLAYER *mplayer)
{
    if (mplayer->running)
    {
        char *cmd = "get_time_pos\n";
        write(mplayer->fifoFd, cmd, strlen(cmd));
    }
}
//获取总时长
void getTimeLength(MPLAYER *mplayer)
{
    if (mplayer->running)
    {
        char *cmd = "get_time_length\n";
        write(mplayer->fifoFd, cmd, strlen(cmd));
    }
}
//获取当前播放进度的百分比
void getPercentPos(MPLAYER *mplayer)
{
    if (mplayer->running)
    {
        char *cmd = "get_percent_pos\n";
        write(mplayer->fifoFd, cmd, strlen(cmd));
    }
}
//获取播放歌曲的专辑名
void getMetaAlbum(MPLAYER *mplayer)
{
    if (mplayer->running)
    {
        char *cmd = "get_meta_album\n";
        write(mplayer->fifoFd, cmd, strlen(cmd));
    }
}
//获取播放歌曲的文件名
void getFileName(MPLAYER *mplayer)
{
    if (mplayer->running)
    {
        char *cmd = "get_file_name\n";
        write(mplayer->fifoFd, cmd, strlen(cmd));
    }
}

//播放音乐
void playMusic(char *file, MPLAYER *mplayer)
{
    if (mplayer->running)
    {
        int size = strlen(file);
        char *cmd = malloc(sizeof(char) * (size + 10));
        cmd[0] = '\0';
        strcat(cmd, "loadfile ");
        strcat(cmd, file);
        strcat(cmd, "\n");
        write(mplayer->fifoFd, cmd, strlen(cmd));
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
    write(mplayer->fifoFd, cmd, strlen(cmd));
}


//继续播放
void unpausePlayer(MPLAYER *mplayer){
    //输入命令和暂停相同
    pausePlayer(mplayer);
}
//暂停正在播放的音乐
void pausePlayer(MPLAYER *mplayer){
    if(mplayer->running){
        char *cmd = "pause\n";
        write(mplayer->fifoFd, cmd, strlen(cmd));
    }
}

//后退，前进
void backMusic(MPLAYER *mplayer){
    if(mplayer->running){
        //后退5秒
        char *cmd = "seek -5\n";
        write(mplayer->fifoFd, cmd, strlen(cmd));
    }
}
void aheadMusic(MPLAYER *mplayer){
    if(mplayer->running){
        //前进5秒
        char *cmd = "seek 5\n";
        write(mplayer->fifoFd, cmd, strlen(cmd));
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
            song->lrc = lrc;
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
    return songList;
}