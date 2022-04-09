//
#include "common.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//输出错误信息，然后结束程序
void fatal(const char *prefix, const char *msg)
{
    if (msg != NULL)
    {
        printf("%s: %s\n", prefix, msg);
        exit(1);
    }
}

//输出信息
void info(const char *prefix, const char *msg)
{
    if (msg != NULL)
    {
        printf("%s: %s\n", prefix, msg);
    }
}

//截取字符串字串
char *subString(const char *src, int start, int end)
{
    int size = 0;
    if (start < 0)
    {
        start = 0;
    }
    if (end < 0)
    {
        end = strlen(src);
    }
    char *dst = malloc(sizeof(char) * (end - start + 1));
    for (int i = start; i < end; i++)
    {
        //回车或结束符,退出
        if (src[i] == '\0' || src[i] == '\r' || src[i] == '\n')
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