#include <stdio.h>
#include <stdlib.h>

/*系统调用相关的致命错误，打印用户提示信息和系统提示信息，然后直接调用exit()终止程序*/
void err_sys(const char *msg) {
    printf("%s\n", msg);
    exit(1);
}

