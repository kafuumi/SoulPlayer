//
// Created by Hami Lemon on 2022/4/6.
//
//解析lrc歌词文件
#ifndef SOULPLAYER_LYRIC_H
#define SOULPLAYER_LYRIC_H
#define LINE_NUM 256

//歌词信息,代表一句具体的歌词
typedef struct LYRIC_NODE
{
    //歌词开始时间点，单位毫秒
    int time;
    //歌词内容
    char *lyric;
    struct LYRIC_NODE *prev;
    struct LYRIC_NODE *next;
} LYRIC_NODE;

//所有的歌词信息
typedef struct LYRIC
{
    //歌手
    char *artist;
    //歌曲名
    char *title;
    //歌词链表的头节点和尾节点
    LYRIC_NODE *head;
    LYRIC_NODE *tail;
} LYRIC;

//解析lrc文件
LYRIC *parseLrc(const char *file);

//释放内存
void lyricFree(LYRIC *lyric);
#endif // SOULPLAYER_LYRIC_H
