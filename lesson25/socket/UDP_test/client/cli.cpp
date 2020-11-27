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

    //发送数据
    //接受应答
    //打印应答
    while(1)
    {
        char buf[1024] = {0};
        memset(buf, '\0', sizeof(buf));
        strncpy(buf, "woshiclient", sizeof(buf));
        struct sockaddr_in svr_addr;
        svr_addr.sin_family = AF_INET;
        svr_addr.sin_port = htons(19999);
        svr_addr.sin_addr.s_addr = inet_addr("172.16.99.129");
        socklen_t svr_len = sizeof(svr_addr);
        ssize_t send_size = sendto(sockfd, buf, strlen(buf), 0, NULL, sizeof(svr_len));
        if(send_size < 0)
        {
            perror("sendto");
            return -1;
        }
        struct sockaddr_in peer_addr;
        socklen_t peer_len = sizeof(peer_addr);
        ssize_t recv_size = recvfrom(sockfd, buf, sizeof(buf) - 1, 0, (struct sockaddr*)&peer_addr, &peer_len);
        if(recv_size < 0)
        {
            perror("recvfromrm");
            return -1;
        }

        printf("svr say: %s\n", buf);
    }
    close(sockfd);
    return 0;
}
