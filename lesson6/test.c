#include <stdio.h>
#include <unistd.h>

int main()
{
    printf("hello\n");
    int g_val = 100;
    pid_t ret = fork();
    if(ret < 0)
    {
        perror("fork");
        return 0;
    }
    else if(ret == 0)
    {
        //child
        // while(1)
        {
             printf("i am child pid = [%d], ppid = [%d]\n", getpid(), getppid());
             g_val -= 50;//50
             printf("child, g_val:%d\n", g_val);
             sleep(20);
        }        
    }
    else
    {
        //ret > 0:father
        //while(1)
        {
             printf("i am father pid = [%d], ppid = [%d]\n", getpid(), getppid());
             g_val += 50;//150
             printf("father, g_val:%d\n", g_val);
             sleep(5);
        }
    }
    return 0;
}
