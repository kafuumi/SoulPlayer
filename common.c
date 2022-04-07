//
#include "common.h"
#include <stdio.h>
#include <stdlib.h>
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