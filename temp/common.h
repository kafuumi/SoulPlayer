#ifndef __COMMON_H
#define __COMMON_H

/*
	File		:common.h
	Description	:自定义的头文件，包含常用的系统头文件，常用的宏，自定义的工具函数
*/

/*头文件统一包含*/

/*标准头文件*/
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

/*linux系统调用头文件*/
#include <unistd.h>
#include <sys/types.h>

#define MAXLINE 4096  /*一行的最大长度*/


/*自定义函数原型*/
/*以err_开头的是错误处理函数，用于出错时打印提示信息并进行相关操作*/

/*系统调用相关的非致命错误，打印用户提示信息和系统提示信息，然后返回*/
void err_ret(const char *msg, ...);

/*系统调用相关的致命错误，打印用户提示信息和系统提示信息，然后直接调用exit()终止程序*/
void err_sys(const char *msg);

/*系统调用相关的致命错误，打印用户提示信息和系统提示信息并产生核心转储文件，然后调用exit()终止程序*/
void err_dump(const char *fmt, ...);

/*系统调用无关的非致命错误，打印用户提示信息，然后返回*/
void err_msg(const char *msg, ...);

/*系统调用无关的非致命错误，打印用户提示信息和error指定的系统提示信息，然后返回*/
void err_cont(int error, const char *msg, ...);

/*系统调用无关的致命错误，打印用户提示信息，然后调用exit()终止程序*/
void err_quit(const char *msg, ...);

/*系统调用无关的致命错误， 打印用户提示信息和error指定的系统提示信息，然后调用exit()终止程序*/
void err_exit(int error, const char *msg, ...);

#endif




















