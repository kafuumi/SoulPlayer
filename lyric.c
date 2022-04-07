//
// Created by Hami Lemon on 2022/4/6.
// 解析lrc文件，生成歌词流

#include "lyric.h"
#include <stdlib.h>
#include <stdio.h>
#include <regex.h>
#include <string.h>
#include "common.h"

//截取字符串字串
char *subString(const char *src, int start, int end)
{
    int size = 0;
    if (start == -1)
    {
        start = 0;
    }
    if (end == -1)
    {
        end = strlen(src);
    }
    char *dst = malloc(sizeof(char) * (end - start + 1));
    for (int i = start; i < end; i++)
    {
        //回车或结束符,退出
        if (src[i] == '\0' || src[i] == '\n')
        {
            break;
        }
        dst[size] = src[i];
        size++;
    }
    dst[size] = '\0';
    if (size == 0)
    {
        return NULL;
    }
    return dst;
}

//解析lrc文件
// file: 文件名路径
// lyric: 返回的歌词流
LYRIC *parseLrc(const char *file)
{
    if (file == NULL)
    {
        return NULL;
    }
    FILE *fp = fopen(file, "rt");
    if (fp == NULL)
    {
        return NULL;
    }
    //歌词信息正则，\[(两个小写字母):任意个空格(任意字符)]任意个空字符
    regex_t infoReg;
    int ret = regcomp(&infoReg, "^\\[([a-zA-Z]{2}):[[:blank:]]*(.+)][[:space:]]*$", REG_EXTENDED);
    if (ret != 0)
    {
        char buffer[256];
        regerror(ret, &infoReg, buffer, 256);
        fatal("正则表达式错误:", buffer);
        return NULL;
    }
    // 歌词正则
    regex_t lyricReg;
    ret = regcomp(&lyricReg, "^\\[([0-9]{2}):([0-9]{2}).([0-9]{2,3})][[:blank:]]*(.+)[[:space:]]*$", REG_EXTENDED);
    if (ret != 0)
    {
        char buffer[256];
        regerror(ret, &lyricReg, buffer, 256);
        fatal("正则表达式错误:", buffer);
        return NULL;
    }

    LYRIC *lyric = malloc(sizeof(LYRIC));
    lyric->head = NULL;
    lyric->artist = "";
    lyric->title = "";

    char line[LINE_NUM] = "\0";
    while (fgets(line, LINE_NUM, fp) != NULL)
    {
        size_t nMatch = 5;
        regmatch_t pMatch[nMatch];
        int ok = regexec(&infoReg, line, nMatch, pMatch, 0);
        //歌词元数据
        if (ok == 0)
        {
            char *dst = subString(line, pMatch[1].rm_so, pMatch[1].rm_eo);
            if (dst == NULL)
            {
                continue;
            }
            //歌手名
            if (strcmp(dst, "ar") == 0)
            {
                char *artist = subString(line, pMatch[2].rm_so, pMatch[2].rm_eo);
                lyric->artist = artist;
            }
            else if (strcmp(dst, "ti") == 0)
            {
                //歌曲名
                char *title = subString(line, pMatch[2].rm_so, pMatch[2].rm_eo);
                lyric->title = title;
            }
            free(dst);
        }
        else
        {
            //歌词数据
            ok = regexec(&lyricReg, line, nMatch, pMatch, 0);
            char *mStr = subString(line, pMatch[1].rm_so, pMatch[1].rm_eo);
            char *sStr = subString(line, pMatch[2].rm_so, pMatch[2].rm_eo);
            char *msStr = subString(line, pMatch[3].rm_so, pMatch[3].rm_eo);
            char *text = subString(line, pMatch[4].rm_so, pMatch[4].rm_eo);

            int m = atoi(mStr);
            int s = atoi(sStr);
            int ms = atoi(msStr) * 10;
            //转换成毫秒为单位的时间
            int time = m * 60 * 1000 + s * 1000 + ms;
            //释放内存
            free(mStr);
            free(sStr);
            free(msStr);
            if(text == NULL){
                continue;
            }

            //歌词节点
            LYRIC_NODE *node = malloc(sizeof(LYRIC_NODE));
            node->time = time;
            node->lyric = text;
            node->prev = NULL;
            node->next = NULL;
            //歌词链表
            if (lyric->head == NULL)
            {
                lyric->head = node;
            }
            else
            {
                lyric->tail->next = node;
                node->prev = lyric->tail;
            }
            lyric->tail = node;
        }
    }
    regfree(&infoReg);
    regfree(&lyricReg);
    fclose(fp);
    return lyric;
}

//根据计算分，秒，毫秒计算其对应的毫秒值
int timeToMs(int m, int s, int ms);

//释放内存
void lyricFree(LYRIC *lyric)
{
    if (lyric == NULL)
    {
        return;
    }
    for (LYRIC_NODE *h = lyric->head; h != NULL;)
    {
        LYRIC_NODE *temp = h;
        h = h->next;
        free(temp->lyric);
        free(temp);
    }
    free(lyric->artist);
    free(lyric->title);
    free(lyric);
}