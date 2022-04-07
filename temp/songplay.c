/*************************************************************************
	> File Name: songplay.c
	> Author: mrhjlong
	> Mail: mrhjlong@163.com 
	> Created Time: 2016年07月24日 星期日 13时08分09秒
 ************************************************************************/

#include "songplay.h"

int flag = 0;	//显示数据标志	1：显示  0：不显示
Song *pnow = NULL;
Song *plast = NULL;
int clsthread = 0;	//关闭线程
int flagpause = -1;	//暂停标志
int fdpp[2];	//无名管道
sem_t sem;		//信号量


//创建链表头
List_head *head_create(void)
{
	List_head *linklist;
	linklist = (List_head *)malloc(sizeof(List_head));
	if(linklist == NULL)
		err_sys("head create error");
	
	linklist->head = NULL;
	linklist->song_num = 0;
	
	return linklist;
}

//创建结点
Song *node_create(char *name)
{
	Song *pSong = (Song *)malloc(sizeof(Song));
	if(pSong == NULL)
		err_sys("create node error");

	pSong->next = NULL;
	pSong->prev = NULL;
	bzero(pSong->name, NAMESIZE);
	strcpy(pSong->name, name);

	return pSong;
}

//添加结点到链表尾部
void node_insert_tail(List_head *linklist, Song *pSong)
{
	if(linklist->head == NULL)
	{
		linklist->head = pSong;
		linklist->song_num = 1;
	}
	else
	{
		Song *p = linklist->head;
		while(p->next != NULL)
		{
			p = p->next;
		}
		p->next = pSong;
		pSong->prev = p;
		linklist->song_num++;
	}
}

//打印整张链表
void node_print(List_head *linklist)
{
	printf("\n******************************************************\n");
	Song *p = linklist->head;

	printf("song_num: %d\n", linklist->song_num);
	printf("song list:\n");
	int cnt = 0;
	while(p != NULL)
	{
		printf("%s  ", p->name);
		cnt++;
		if(cnt == 5)
		{
			printf("\n");
			cnt = 0;
		}
		p = p->next;
	}
	printf("\n******************************************************\n");
}

//查找结点数据，返回结点地址，失败返回NULL
Song *node_search(List_head *linklist, char *name)
{
	Song *p = linklist->head;

	while(p != NULL)
	{
		if(strcmp(p->name, name) == 0)
			return p;
		else
			p = p->next;
	}
	
	return NULL;
}

//销毁整张链表
void list_destroy(List_head *linklist)
{
	Song *p = linklist->head;
	Song *next = NULL;

	while(p != NULL)
	{
		next = p->next;
		p->next = NULL;
		free(p);
		p = next;
	}

	linklist->head = NULL;
	free(linklist);
}

//获得歌曲双向链表
List_head *get_song_list(char *pathname)
{
	List_head *linklist = head_create();
	Song *pSong = NULL;

	DIR *dirp = opendir(pathname);
	if(dirp == NULL)
		err_sys("No music files in the directory!");

	struct dirent *pfile;
	pfile = readdir(dirp);
	while(pfile != NULL)
	{
		if(strstr(pfile->d_name, ".mp3") != NULL || strstr(pfile->d_name, ".flac") != NULL || 
		strstr(pfile->d_name, ".m4a") != NULL)
		{
			pSong = node_create(pfile->d_name);
			node_insert_tail(linklist, pSong);
		}
		pfile = readdir(dirp);
	}
	closedir(dirp);
	plast = pSong;

	return linklist;
}

void chgform(char *name)	//歌名有空格的在空格前加/  （其实加上双引号“”就行）
{
	char buf[NAMESIZE] = {0};
	char *p = strtok(name, " ");
	if(p != NULL)
		strcat(buf, p);
	p = strtok(NULL, " ");
	while(p != NULL)
	{
		strcat(buf, "\\ ");
		strcat(buf, p);
		p = strtok(NULL, " ");
	}
	strcpy(name, buf);
}

void chgsong(char *name)	//歌名有空格的在空格前加/，其实加上双引号“”就行
{
	printf("playing %s...\n", strstr(name, "./") + 2);
	chgform(name);
	char pathname[NAMESIZE] = {0};
	strcat(pathname, "loadfile ");
	strcat(pathname, name);
	strcat(pathname, "\n");
	//printf("pathname:%s", pathname);
	
	int fd = open("cmdfifo", O_WRONLY | O_NONBLOCK);
	if(fd < 0)
		err_sys("open file error");
	write(fd, pathname, strlen(pathname));

	close(fd);
}

//解析获取的歌曲信息，并显示
void deal_data(char *arg)
{
	char data[1024] = {0};
	char *p = NULL;
	char *ps = NULL;
	strcpy(data, arg);
	
	p = strtok(data, "\n");
	while(p != NULL)
	{
		ps = strstr(p, "=");
		if(strstr(p, "ANS_FILENAME") != NULL)
		{
			printf("NOW PLAYING %s\n", ps+1);
		}
		else if(strstr(p, "ANS_LENGTH") != NULL)
		{
			printf("LENGTH:%s(s)\n", ps+1);
		}
		else if(strstr(p, "ANS_TIME_POSITION") != NULL)
		{
			printf("TIME:%s(s)\n", ps+1);
		}
		else if(strstr(p, "ANS_META_ARTIST") != NULL)
		{
			printf("SINGER:%s\n", ps+1);
		}
		else if(strstr(p, "ANS_PERCENT_POSITION") != NULL)
		{
			printf("PERCENT:%s%%\n", ps+1);
		}
		else
			printf("no deal data: %s\n", p);
		
		p = strtok(NULL, "\n");
	}
}

//输入命令线程
void *getcmd_thread(void *arg)
{	
	List_head *linklist = (List_head *)arg;
	//Song *pnow = linklist->head;
	
	char incmd[20] = {0};
	char name[NAMESIZE] = {0};

	printf("playing %s...\n", pnow->name);
	while(1)
	{
		printf("\n******************************************************\n");
		printf("COMMAND: l-last | n-next | p-pause | s-show | q-quit...");
		printf("\n******************************************************\n");
		bzero(incmd, 20);
		fflush(stdout);
		fgets(incmd, 20, stdin);
		
		if(flagpause == 1)
			flagpause = -1;
		if(strcmp(incmd, "n\n") == 0) //下一首
		{
			if(pnow->next == NULL)
				pnow = linklist->head;
			else
				pnow = pnow->next;
			
			bzero(name, NAMESIZE);
			strcat(name, "./");
			strcat(name, pnow->name);
			chgsong(name);
		}
		else if(strcmp(incmd, "l\n") == 0) //上一首
		{
			if(pnow->prev == NULL)
				pnow = plast;
			else
				pnow = pnow->prev;
		
			bzero(name, NAMESIZE);
			strcat(name, "./");
			strcat(name, pnow->name);
			chgsong(name);
		}
		else if(strcmp(incmd, "s\n") == 0)	//显示信息
		{
			node_print(linklist);
			flag = 1;	//显示标志打开
		}
		else
		{
			if(strcmp(incmd, "p\n") == 0 || strcmp(incmd, "pause\n") == 0)
				flagpause = -flagpause;
			
			int fd = open("cmdfifo", O_WRONLY | O_NONBLOCK);
			if(fd < 0)
				err_sys("open file error");
			write(fd, incmd, strlen(incmd));
			close(fd);
			
			if(strcmp(incmd, "q\n") == 0)	//退出
			{
				clsthread = 1;
				break;
			}	
		}
		
		sleep(1);
		while(flag)		//等待显示完成
		{usleep(100);}
	}
	
	return NULL;
}

//读取mplayer输出到管道的数据
void *read_thread(void *arg)
{
	List_head *linklist = (List_head *)arg;
	
	char info[1024] = {0};
	char *p = NULL;
	char name[NAMESIZE] = {0};
	int ret = 0;
	while(1)
	{
		sem_wait(&sem);
		if(flag == 1)	//头开始读取
			flag = 2;
		bzero(info, 1024);
		ret = read(fdpp[0], info, 1024);
		while(ret > 0)
		{
			if(flag > 1)	
			{
				flag = 3;	
				deal_data(info);
			}
			
			if(strstr(info, "ANS_PERCENT_POSITION") != NULL) //自动下一首
			{
				p = strstr(info, "=");
				if(p[1] == '9' && p[2] == '9')	//播放到99%，等待几秒后自动播放下一首
				{
					sleep(4);
					printf("automatically next song\n");
					if(pnow->next == NULL)
						pnow = linklist->head;
					else
						pnow = pnow->next;
			
					bzero(name, NAMESIZE);
					strcat(name, "./");
					strcat(name, pnow->name);
					chgsong(name);	
					sleep(1);
					printf("\n******************************************************\n");
					printf("COMMAND: l-last | n-next | p-pause | s-show | q-quit...");
					printf("\n******************************************************\n");
				}
			}
			
			bzero(info, 1024);
			ret = read(fdpp[0], info, 1024);
		}
		
		if(flag == 3)	//显示完成，关闭
		{
			flag = 0;
			printf("******************************************************\n");
		}
		sem_post(&sem);
		
		while(flagpause == 1)		//暂停
		{sleep(1);}
		
		if(clsthread)	//退出
			break;
	}
	
	return NULL;
}

//发送命令获取歌曲信息
void *datacmd_thread(void *arg)
{
	int fd = 0;
	char info[1024] = {0};
	int ret = 0;
	char str[] = "get_file_name\nget_time_length\nget_time_pos\nget_meta_artist\nget_percent_pos\n";
	while(1)
	{
		sem_wait(&sem);
		ret = read(fdpp[0], info, 1024);	//清除管道信息
		while(ret > 0)
		{
			ret = read(fdpp[0], info, 1024);
		}
		fd = open("cmdfifo", O_WRONLY | O_NONBLOCK);
		if(fd < 0)
			err_sys("open file error");
		
		write(fd, str, strlen(str));
		close(fd);
		sem_post(&sem);
		
		sleep(1);
		while(flagpause == 1)		//暂停
		{sleep(1);}
		
		if(clsthread)			//关闭线程
			break;
	}
	
	return NULL;
}