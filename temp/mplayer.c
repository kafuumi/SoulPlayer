/*************************************************************************
	> File Name: mplayer.c
	> Author: mrhjlong
	> Mail: mrhjlong@163.com 
	> Created Time: 2016年07月23日 星期六 15时27分19秒
 ************************************************************************/

#include "songplay.h"
#include <pthread.h>
#include <string.h>

extern int flag;    //显示数据标志	1：显示  0：不显示
extern Song *pnow;
extern Song *plast;
extern int clsthread;    //关闭线程
extern int flagpause;
extern int fdpp[2];    //无名管道
extern sem_t sem;        //信号量

int main(void) {
    List_head *linklist = get_song_list("./");
    node_print(linklist);
    pnow = linklist->head;
    char name[NAMESIZE] = {0};
    strcat(name, "./");
    strcat(name, pnow->name);

    int ret = sem_init(&sem, 0, 1);
    if (ret == -1)
        err_sys("sem_init error");

    if (access("./cmdfifo", F_OK) == 0) {
        //	printf("cmdfifo exist\n");
        unlink("./cmdfifo");
        mkfifo("./cmdfifo", 0777);
    } else {
        mkfifo("./cmdfifo", 0777);
    }

    if (pipe(fdpp) < 0)
        err_sys("pipe error");

    fcntl(fdpp[0], F_SETFL, O_NONBLOCK); //无名管道设为非阻塞

    pid_t pid = fork();
    if (pid < 0)
        err_sys("fork error");
    else if (pid == 0) {
        close(fdpp[0]);
        dup2(fdpp[1], 1);
        execlp("mplayer", "mplayer", "-slave", "-quiet", "-idle", "-input", "file=./cmdfifo", name,
               NULL);    //命名管道cmdfifo有命令数据，mplayer会自动读取
    } else {
        close(fdpp[1]);
        sleep(1);

        pthread_t get_tid1, get_tid2, get_tid3;
        ret = 0;

        ret = pthread_create(&get_tid1, NULL, getcmd_thread, (List_head *) linklist);
        if (ret != 0)
            err_sys("pthread_create error");

        ret = pthread_create(&get_tid2, NULL, datacmd_thread, NULL);
        if (ret != 0)
            err_sys("pthread_create error");

        //sleep(1);
        //char data[1024] = {0};
        //read(fd[0], data, 1024); //清除冗余信息
        ret = pthread_create(&get_tid3, NULL, read_thread, (List_head *) linklist);
        if (ret != 0)
            err_sys("pthread_create error");

        pthread_join(get_tid1, NULL);
        pthread_join(get_tid2, NULL);
        pthread_join(get_tid3, NULL);
        sem_destroy(&sem);
        list_destroy(linklist);
    }

    //sleep(1);
    return 0;
}
