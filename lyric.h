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
    //歌词开始时间点，单位秒
    float time;
    //歌词内容
    char *lyric;
    struct LYRIC_NODE *prev;
    struct LYRIC_NODE *next;
} LYRIC_NODE;

//解析lrc文件
LYRIC_NODE *parseLrc(const char *file);

//释放内存
void lyricFree(LYRIC_NODE *lyric);
#endif // SOULPLAYER_LYRIC_H
