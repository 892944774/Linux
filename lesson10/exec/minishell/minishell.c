#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <sys/wait.h>

char g_command[1024];

int Getcommand()
{
    printf("[zs@localhost minishell]");
    fflush(stdout);
    memset(g_command, '\0', sizeof(g_command));//初始化
    if(fgets(g_command, sizeof(g_command) -1, stdin) == NULL)
    {
        perror("fgets perror");
        return -1;
    }
    printf("%s\n", g_command);
    return 0;
}

int ExecCommand(char* argv[])
{
    //创建子进程
    //让子进程进行程序替换
    if(argv[0] == NULL)
    {
        printf("ExecCommand failed\n");
        return -1;
     }
   
    pid_t pid = fork();
    if(pid < 0)
    {
        perror("fork");
        return -1;
    }
    else if(pid == 0)
    {
        //child -->进程程序替换
        execvp(argv[0], argv);
        //如果调用了execvp,还能走到下面的逻辑，则表示替换失败了
        //将没有替换成功的子进程杀掉
        exit(0);
    }
    else
    {
        //father -->进程等待，防止子进程变成僵尸进程
        waitpid(pid, NULL, 0);
    }
    return 0;
}

int Dealcommand(char* command)
{
    //ls -l ==> ls 可执行程序 -l 传递给ls的命令行参数
    //1. 切分读进来的字符串，区分一下什么是可执行程序
    //  1.1切分出来的第一个字符串就是可执行程序
    //  1.2后面的都是命令行参数进行切割
    //  1.3用空格作为分隔符来进行切割
    int argc = 0;
    char* argv[1024] = {0};
    while(*command)
    {
        //判断当前的字符是否是空格 或者 \0,如不是就保存在char*
        if(!isspace(*command) && *command != '\0')
        {
            argv[argc] = command;  
            argc++;

            while(!isspace(*command)&& *command != '\0')
            {
                command++;
            }
            *command = '\0';//将非字符的全部用\0代替
        }
        command++;//向后寻找
    }

    argv[argc] = NULL;

    int i = 0;
    for(i = 0; i < argc; i++)
    {
        printf("%d:%s\n", i , argv[i]);
    }
}

int main()
{
    while(1)
    {
        //1.获取用户数日的命令
        if(Getcommand() == -1)
        {
            continue;
        }
        //2.创建子进程
        //3.让子进程进行程序替换
        //4.进程等待
        Dealcommand(g_command);
    }

    return 0;
}
