#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdio.h>

int main()
{
    int sockfd = socket(AF_INET, SOCK_DGRAM, 17);
    if(sockfd < 0)
    {
        perror("socket");
        return -1;
    }
    
    //地址信息
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(18989);
    //172.16.99.129
    //1 将点分十进制的ip转化为无符号四字节的整数
    //2 将该整数转化为网路字节序
    addr.sin_addr.s_addr = inet_addr("172.16.99.129");
    int ret = bind(sockfd, (struct sockaddr*)&addr, sizeof(addr));
    if(ret < 0)
    {
        perror("bind");
        return -1;
    }

    while(1)
    {
        sleep(1);
    }
    return 0;
}
