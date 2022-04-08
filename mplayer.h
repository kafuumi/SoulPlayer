#ifndef SOULPLAYER_MPLAYER_H
#define SOULPLAYER_MPLAYER_H

//歌曲
typedef struct SONG
{
    char *path;
    //对应的歌词文件路径
    char *lrc;
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
    int pepeFd[2];
    //有名管道
    int fifoFd;
    //是否正在运行
    int running;
    SONGLIST *songList;
} MPLAYER;
//启动mplayer
void startMplayer(MPLAYER *mplayer);

//获取当前播放的时间点
void getTimePos(MPLAYER *mplayer);

//获取总时长
void getTimeLength(MPLAYER *mplayer);

//获取当前播放进度的百分比
void getPercentPos(MPLAYER *mplayer);

//获取播放歌曲的专辑名
void getMetaAlbum(MPLAYER *mplayer);

//获取播放歌曲的文件名
void getFileName(MPLAYER *mplayer);

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
//dir歌曲文件所在的目录
//lrcBase歌词文件所在的目录
SONGLIST *loadSongList(char *dir, char *lrcBase);
#endif