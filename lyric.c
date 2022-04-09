//
// Created by Hami Lemon on 2022/4/6.
// 解析lrc文件，生成歌词流

#include "lyric.h"
#include <stdlib.h>
#include <stdio.h>
#include <regex.h>
#include <string.h>
#include "common.h"

//解析lrc文件
// file: 文件名路径
// lyric: 返回的歌词流
LYRIC_NODE *parseLrc(const char *file)
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
    // 歌词正则 [00:00.00] xxxx
    regex_t lyricReg;
    int ret = regcomp(&lyricReg, "^\\[([0-9]{2}):([0-9]{2}).([0-9]{2,3})][[:blank:]]*(.+)[[:space:]]*$", REG_EXTENDED);
    if (ret != 0)
    {
        char buffer[256];
        regerror(ret, &lyricReg, buffer, 256);
        fatal("正则表达式错误:", buffer);
        return NULL;
    }
    char line[LINE_NUM] = "\0";
    LYRIC_NODE *head = NULL;
    LYRIC_NODE *prev = NULL;
    while (fgets(line, LINE_NUM, fp) != NULL)
    {
        size_t nMatch = 5;
        regmatch_t pMatch[nMatch];
        int ok = regexec(&lyricReg, line, nMatch, pMatch, 0);
        if (ok == 0)
        {
            //歌词数据
            char *mStr = subString(line, pMatch[1].rm_so, pMatch[1].rm_eo);
            char *sStr = subString(line, pMatch[2].rm_so, pMatch[2].rm_eo);
            char *msStr = subString(line, pMatch[3].rm_so, pMatch[3].rm_eo);
            char *text = subString(line, pMatch[4].rm_so, pMatch[4].rm_eo);
            int m = atoi(mStr);
            int s = atoi(sStr);
            int ms = atoi(msStr) * 10;
            //转换成毫秒为单位的时间
            float time = m * 60 + s + ms / 1000.0;
            //释放内存
            free(mStr);
            free(sStr);
            free(msStr);
            if (text == NULL)
            {
                continue;
            }
            //歌词节点
            LYRIC_NODE *node = malloc(sizeof(LYRIC_NODE));
            node->time = time;
            node->lyric = text;
            node->prev = NULL;
            node->next = NULL;
            //歌词链表
            if (head == NULL)
            {
                head = node;
            }
            else
            {
                prev->next = node;
                node->prev = prev;
            }
            prev = node;
        }
    }
    regfree(&lyricReg);
    fclose(fp);
    return head;
}

//释放内存
void lyricFree(LYRIC_NODE *lyric)
{
    if (lyric == NULL)
    {
        return;
    }
    for (LYRIC_NODE *h = lyric; h != NULL;)
    {
        LYRIC_NODE *temp = h;
        h = h->next;
        free(temp->lyric);
        free(temp);
    }
}