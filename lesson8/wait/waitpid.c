#include <stdio.h>
#include <unistd.h>
//调用wait函数用到的头文件
#include <sys/wait.h>
#include <stdlib.h>

int main()
{
    pid_t pid = fork();
    if( pid < 0)
    {
        perror("fork");
        return 0; 
    }
    else if(pid == 0)
    {
        //child
        int count = 10;
        while(1)
        {
           if(count <= 0)
           {
               break;
           }
           printf("begin--->> i am child pid=[%d], ppid=[%d]\n", getpid(), getppid());
           count--;
           sleep(1);
        }
        exit(10);
    }
    else
    {
        //father  pid >0
        printf("------------>i am father pid=[%d], ppid=[%d]\n", getpid(),getppid());
        int status;
        printf("child pid num is %d\n", pid);
        waitpid(pid, &status, WNOHANG);
        //printf("exit_code:%d\n", (status >> 8) & 0xff);
        printf("sig_code : %d\n", status & 0x7f);
        printf("coredump_code:%d\n", (status >> 7) & 0x1);
        while(1)
        {
            printf("i am father pid=[%d], ppid=[%d]\n", getpid(), getppid());
            sleep(1);
        }
    }
    return 0;
}
