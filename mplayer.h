#include "lyric.h"
#ifndef SOULPLAYER_MPLAYER_H
#define SOULPLAYER_MPLAYER_H

//歌曲
typedef struct SONG
{
    //歌曲名
    char *title;
    char *path;
    //对应的歌词
    LYRIC_NODE *lrc;
    struct SONG *prev;
    struct SONG *next;
} SONG;
//播放列表
typedef struct SONGLIST
{
    //歌曲数目
    int num;
    //当前播放的歌曲
    SONG *now;
    //链表头
    SONG *head;
    //链表尾
    SONG *tail;
} SONGLIST;

typedef struct MPLAYER
{
    //无名管道
    int pipeFd[2];
    //有名管道
    int fifoFd;
    //是否正在运行
    int running;
    //是否正在播放
    int playing;
    SONGLIST *songList;
} MPLAYER;
//启动mplayer
void startMplayer(MPLAYER *mplayer);

MPLAYER *initMplayer();
//发送指令
void *sendPlayer(void *arg);

//播放音乐
void playMusic(char *file, MPLAYER *mplayer);

//继续播放
void unpausePlayer(MPLAYER *mplayer);
//暂停正在播放的音乐
void pausePlayer(MPLAYER *mplayer);

//下一首，上一首
void nextMusic(MPLAYER *mplayer);
void prevMusic(MPLAYER *mplayer);

//后退，前进
void backMusic(MPLAYER *mplayer);
void aheadMusic(MPLAYER *mplayer);

//结束mplayer
void quitMplayer(MPLAYER *mplayer);
//获取播放列表
// dir歌曲文件所在的目录
// lrcBase歌词文件所在的目录
SONGLIST *loadSongList(char *dir, char *lrcBase);

//释放内存
void freeMplayer(MPLAYER *mplayer);
#endif