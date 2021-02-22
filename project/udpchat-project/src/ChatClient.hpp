#pragma once
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <string>

#include "ConnectionInfo.hpp"
#include "tool.hpp"

struct MySelf
{
    std::string nick_name_;
    std::string school_;
    std::string passwd_;
    uint32_t user_id_;
};

class UdpClient
{
    public:
        UdpClient()
        {
            tcp_sock_ = -1;
        }

        ~UdpClient()
        {
             
        }

        int CreateSock()
        {
            tcp_sock_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
            if(tcp_sock_ < 0)
            {
                LOG(ERROR, "create socket failed") << std::endl;
                 return -1;
            }
            /*
             * 为了在客户端可以在同一台机器上多开，我们不主动绑定端口
             * 让操作系统进行绑定
             * */
            return 0;
        }

        int ConnectoSvr(const std::string& ip)
        {
            //ip是服务端的ip地址
            //服务端的ip地址已经约定好了，在ConnectInfo这个文件当中
            struct sockaddr_in dest_addr;
            dest_addr.sin_family = AF_INET;
            dest_addr.sin_port = htons(TCP_PORT);
            dest_addr.sin_addr.s_addr = inet_addr(ip.c_str());

            int ret = connect(tcp_sock_,(struct sockaddr*)&dest_addr, sizeof(dest_addr));
            if(ret < 0)
            {
                LOG(ERROR, "Connect server failed, addr is") << ip << ":" << TCP_PORT << std::endl;
                return -1;
            }
            return 0;
        }  

        int RegistertoSvr(const std::string& ip)
        {
            //1.创建套接字
            int ret = CreateSock();
            if(ret < 0)
            {
                return -1;
            }
            //2.连接服务器
            ret = ConnectoSvr(ip);
            if(ret < 0)
            {
                return -1;
            }
            //3.准备注册包
            char type = REGISTER_RESQ;
            ssize_t send_size = send(tcp_sock_, &type, 1, 0);
            if(send_size < 0)
            {
                return -1;
            }

            struct RegisterInfo ri;
            std::cout << "please enter nick-name: ";
            fflush(stdout);
            std::cin >> ri.nick_name_;
            std::cout << "please enter school: ";
            fflush(stdout);
            std::cin >> ri.school_;
            /*
             * 对于密码字段而言，我们需要进行双重校验，防止用户在输入密码的时候，第一次输入错误
             * */
            while(1)
            {
                std::string first_passwd;
                std::string second_passwd;
                std::cout << "please enter your passwd: ";
                fflush(stdout);
                std::cin >> first_passwd;
                std::cout << "please entry your passwd: ";
                fflush(stdout);
                std::cin >> second_passwd;
                if(first_passwd == second_passwd)
                {
                    strncpy(ri.passwd_, first_passwd.c_str(), sizeof(ri.passwd_));
                    break;
                }
            }
            //4.发送注册包
            send_size = send(tcp_sock_, &ri, sizeof(ri), 0);
            if(send_size < 0)
            {
                LOG(ERROR, "Send register failed") << std::endl;
                return -1;
            }
            //5.接受应答
            struct ReplyInfo reply_info;
            ssize_t recv_size = recv(tcp_sock_, &reply_info, sizeof(reply_info), 0);
            if(recv_size < 0)
            {
                LOG(ERROR, "recv register response failed") << std::endl;
                return -1;
            }
            else if(recv_size == 0)
            {
                LOG(ERROR, "udpchat srver shutdown connect") << std::endl;
            }

            //6.判断应答结果
            if(reply_info.resp_status_ == REGISTER_FAILED)
            {
                LOG(ERROR, "server response register failed") << std::endl;
                return -1;
            }
            //7.返回给上层调用者注册的结果
            LOG(INFO, "register success") << std::endl;
            return 0;
        }
    private: 
        int tcp_sock_; 
};