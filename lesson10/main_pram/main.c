#include <stdio.h>

//命令行参数
//在启动这个程序的时候，可以给这个程序从命令行当中传递一些参数
int main(int argc, char* argv[], char* env[])
{
    //argc:命令行参数个数
    //argv:每一个命令行参数的值
    //env:环境变量
    int i =0;
    for(i = 0; i < argc; i++)
    {
        printf("argv[%d]: %s\n", i, argv[i]);
    }
    return 0;
}
