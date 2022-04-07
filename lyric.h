//
// Created by Hami Lemon on 2022/4/6.
//

#ifndef SOULPLAYER_LYRIC_H
#define SOULPLAYER_LYRIC_H

#define LINE_NUM 100
#define TRUE 1
#define FALSE 0
typedef int BOOL;
//歌词信息,代表一句具体的歌词
typedef struct LYRIC_NODE {
    //歌词开始时间点，单位毫秒
    int time;
    //歌词内容
    char *lyric;
} LYRIC_NODE;

//歌词链表
typedef struct LIST_NODE {
    LYRIC_NODE *data;
    struct LIST_NODE *prev;
    struct LIST_NODE *next;
} LIST_NODE;

//所有的歌词信息
typedef struct LYRIC {
    //歌手
    char *artist;
    //歌曲名
    char *title;
    //歌词链表的头节点和尾节点
    LIST_NODE *head;
    LIST_NODE *tail;
} LYRIC;

//解析lrc文件
LYRIC *parseLrc(const char *file);
//添加歌词
void addLyric(LYRIC *lyric, LYRIC_NODE *node);

//根据计算分，秒，毫秒计算其对应的毫秒值
int timeToMs(int m, int s, int ms);

#endif //SOULPLAYER_LYRIC_H
