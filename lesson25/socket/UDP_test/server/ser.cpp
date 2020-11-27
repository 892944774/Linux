#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main()
{
    //1 创建套接字
    int sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(sockfd < 0)
    {
        perror("socket");
        return -1;
    }
    //2 绑定端口
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(19999);
    addr.sin_addr.s_addr = inet_addr("0.0.0.0");

    int ret = bind(sockfd,(struct sockaddr*)&addr, sizeof(addr));
    if(ret < 0 )
    {
        perror("bind");
        return -1;
    }

    //3 接受
    //4 处理
    //5 回复应答
    while(1)
    {
        char buf[1024] = {0};
        struct sockaddr_in peer_addr;
        socklen_t peer_len = sizeof(peer_addr);
        ssize_t recv_size = recvfrom(sockfd, buf, sizeof(buf) - 1, 0, (struct sockaddr*)&peer_addr, &peer_len);
        if(recv_size < 0)
        {
            perror("recvfromrm");
            return -1;
        }

        printf("cli say: %s\n", buf);

        memset(buf, '\0', sizeof(buf));
        strncpy(buf, "woshiserver", sizeof(buf));
        ssize_t send_size = sendto(sockfd, buf, strlen(buf), 0, (struct sockaddr*)&peer_addr, peer_len);
        if(send_size < 0)
        {
            perror("sendto");
            return -1;
        }
    }
    close(sockfd);
    return 0;
}
