#ifndef SOULPLAYER_COMMON_H
#define SOULPLAYER_COMMON_H
void fatal(const char *prefix, const char *msg);
void info(const char *prefix, const char *msg);
//截取字符串字串
char *subString(const char *src, int start, int end);
#endif