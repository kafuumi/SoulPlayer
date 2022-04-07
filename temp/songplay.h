/*************************************************************************
	> File Name: songplay.h
	> Author: mrhjlong
	> Mail: mrhjlong@163.com 
	> Created Time: 2016年07月24日 星期日 13时08分01秒
 ************************************************************************/

#ifndef __SONGPLAY_H
#define __SONGPLAY_H

#include<stdio.h>
#include"common.h"
#include"songplay.h"
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<dirent.h>
#include<pthread.h>
#include<semaphore.h>

#define NAMESIZE 50

struct song
{
	char name[NAMESIZE];
	struct song *next;
	struct song *prev;
};

struct list_head
{
	struct song *head;
	int song_num;
};

typedef struct song Song;
typedef struct list_head List_head;

//创建链表头
List_head *head_create(void);

//创建结点
Song *node_create(char *name);

//添加结点到链表尾部
void node_insert_tail(List_head *linklist, Song *pSong);

//打印整张链表
void node_print(List_head *linklist);

//查找结点数据，返回结点地址，失败返回NULL
Song *node_search(List_head *linklist, char *name);

//销毁整张链表
void list_destroy(List_head *linklist);

//获得歌曲链表
List_head *get_song_list(char *pathname);

//带空格的歌曲名转换格式，空格前加"\"
void chgform(char *name);

//切换歌曲
void chgsong(char *name);

//处理线程读取的数据
void deal_data(char *data);

//获取按键输入命令-线程
void *getcmd_thread(void *arg);

//读取数据并处理-线程
void *read_thread(void *arg);

//定时发送命令获取数据-线程
void *datacmd_thread(void *arg);

#endif

