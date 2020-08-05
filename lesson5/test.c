#include <stdio.h>
#include <unistd.h>

int main()
{
    printf("-----begin----\n");
    pid_t ret = fork();
    if(ret < 0)
    {
        printf("fork");
        return 0;
    }
    
    else if(ret > 0)
    {
        //father
       while(1)
       {
             printf("i am father 22222222\n");
             sleep(1);
       }
    }

    else 
    {
        //child
        while(1)
        {
             printf("i am child 2222222\n");
             sleep(1);
        }
    }
   //printf("linux84 666\n"); 
    return 0;
}
