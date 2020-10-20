#include <stdio.h>
#include <unistd.h>

int main()
{
#if 0
    //execl 进程程序替换
   printf("begin....................\n");
   //看到ped的结果就是替换成功了，看到execl failed就是替换失败
   execl("/usr/bin/pwd", "pwd", NULL);
   printf("execl failed\n");
#endif
    char* argv[10];
    argv[0] = "ls";
    argv[1] = "-l";
    argv[2] = "..";
    argv[3] = "NULL";
    int ret = execv("/usr/bin/ls", argv);
    printf("execl failed, ret %d\n", ret);
    return 0;
}
