//
// Created by Hami Lemon on 2022/4/6.
//

#include "lyric.h"
#include <stdio.h>

BOOL parseLrc(const char *file, LYRIC *lyric) {
    if (file == NULL) {
        return FALSE;
    }
    FILE *fp = fopen(file, "rt");
    if (fp == NULL) {
        return FALSE;
    }

    char line[LINE_NUM] = "\0";
    while (fgets(line, LINE_NUM, fp) != NULL) {

    }
    fclose(fp);
    return TRUE;
}
