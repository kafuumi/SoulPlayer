//
// Created by Hami Lemon on 2022/4/6.
//

#include "lyric.h"
#include <stdlib.h>
#include <stdio.h>
#include <regex.h>
#include <string.h>

int subString(const char *src, int start, int end, char *dst) {
    int size = 0;
    if (start == -1) {
        start = 0;
    }
    for (int i = start; src[i] != '\0'; i++) {
        if (end != -1 && i >= end) {
            break;
        }
        dst[size] = src[i];
        size++;
    }
    dst[size] = '\0';
    return size;
}

BOOL parseLrc(const char *file, LYRIC *lyric) {
    if (file == NULL) {
        return FALSE;
    }
    FILE *fp = fopen(file, "rt");
    if (fp == NULL) {
        return FALSE;
    }
    //歌词信息正则，\[(两个小写字母):任意个空格(任意字符)]任意个空字符
    regex_t infoReg;
    int ret = regcomp(&infoReg, "^\\[([a-zA-Z]{2}):[[:blank:]]*(.+)][[:space:]]*$", REG_EXTENDED);
    if (ret != 0) {
        return FALSE;
    }

    lyric = malloc(sizeof(LYRIC));

    char line[LINE_NUM] = "\0";
    while (fgets(line, LINE_NUM, fp) != NULL) {
        size_t nMatch = 3;
        regmatch_t pMatch[nMatch];
        int ok = regexec(&infoReg, line, nMatch, pMatch, 0);
        if (ok != 0) {
            break;
        }
        char dst[LINE_NUM];
        int len = subString(line, pMatch[1].rm_so, pMatch[1].rm_eo, dst);
        if (len == 0) {
            continue;
        }
        if (strcmp(dst, "ar") == 0) {
            lyric->artist = dst;
        } else if (strcmp(dst, "ti") == 0) {
            lyric->title = dst;
        }
    }
    regfree(&infoReg);

    regex_t lyricReg;
    ret = regcomp(&lyricReg, "^\\[([0-9]{2}):([0-9]{2}).([0-9]{2,3})][[:blank:]]*(.+)[[:space:]]*$", REG_EXTENDED);
    while (fgets(line, LINE_NUM, fp) != NULL) {
        size_t nMatch = 5;
        regmatch_t pMatch[nMatch];
        int ok = regexec(&lyricReg, line, nMatch, pMatch, 0);

    }

    regfree(&lyricReg);
    fclose(fp);
    return TRUE;
}
