#pragma once
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <pthread.h>
#include <string>

#include "ConnectionInfo.hpp"
#include "UserManager.hpp"
#include "tool.hpp"

#define MAX_ROUND_COUNT 10

class TcpConnect
{
    public:
        TcpConnect()
        {
            new_sock_ = -1;
            server_ = NULL;
        }

        ~TcpConnect()
        {

        }

        void SetSockFd(int fd)
        {
            new_sock_ = fd;
        }

        void SetServer(void* server)
        {
            server_ = server;
        }

        int GetSockFd()
        {
            return new_sock_;
        }

        void* Getserver()
        {
            return server_;
        }
    private:
        int new_sock_;

        //保存ChatServer这个类的this指针，确保在tcp的线程入口函数当中可以获取到用户管理模块的实例化指针
        void* server_;
};

class ChatServer
{
    public:
        ChatServer()
        {
            //登录注册
            tcp_sock_ = -1;
            tcp_port_ = TCP_PORT;
        }
        
        ~ChatServer()
        {}

        /*
         * 初始化变量的接口，被调用者调用的接口
         * 用户管理模块的实例化对象，消息池的实例化对象
         * */
        int InitSvr(uint16_t tcp_port = TCP_PORT)
        {
            //1.创建tcp_socket,绑定地址信息，监听
            //注册登录模块
            tcp_sock_ = socket(AF_INET, SOCK_STREAM, 0);
            if(tcp_sock_ < 0)
            {
                return -1;
            }

            struct sockaddr_in addr;
            addr.sin_family = AF_INET;
            addr.sin_port = htons(tcp_port);
            addr.sin_addr.s_addr = inet_addr("0.0.0.0");
            /*
             * inet_addr:
             * 1.将点分十进制的IP地址转换为uint32_t
             * 2.将unit32_t从主机字节序转换为网络字节序
             *
             * "0.0.0.0"
             * */
            int ret = bind(tcp_sock_,(struct sockaddr*)&addr, sizeof(addr));
            if(ret < 0)
            {
                return -2;
            }

            //监听：
            ret = listen(tcp_sock_, 5);
            if(ret < 0)
            {
                return -3;
            }


            Log(INFO, __FILE__, __LINE__, "listen port is ") << tcp_port << std::endl;

            //暂时没有考虑udp通信和登录注册模块，消息池的初始化
            return 0;
        }

        /*
         * 启动线程
         * */
        int Start()
        {
            /*
             * udp应该有两类线程
             * 1 生产线程，负责接收udp数据，将udp数据放到消息池当中
             * 2 消费线程，负责将消息池中的获取的消息，发送到在线用户的客户端
             *
             * tcp
             * 针对每一个注册的登录请求，（TCP连接），针对每一个tcp连接，都创建一个线程，专门为该客户端处理注册和登录请求，注册登录完毕之后，服务端关闭连接，销毁线程
             *
             * tcp是否创建线程，取决于accept函数是否调用成功
             * */

            //TODO
            //udp 线程的创建
            struct sockaddr_in peer_addr;
            socklen_t peer_addrlen = sizeof(peer_addr);
            while(1)
            {
                int new_sock = accept(tcp_sock_, (struct sockaddr*)&peer_addr, &peer_addrlen);
                if(new_sock < 0)
                {
                    continue;
                }

                //正常接收
                //创建线程，为客户端的登录和注册请求负责
                TcpConnect* tc = new TcpConnect();
                tc->SetSockFd(new_sock);
                tc->SetServer((void*)this);

                pthread_t tid;
                int ret = pthread_create(&tid, NULL, LoginRegisterStart, (void*)tc);
                if(ret < 0)
                {
                    close(new_sock);
                    delete tc;
                    continue;
                }
            }   
        }

private:
        static void* LoginRegisterStart(void* arg)
        {
            /*
             * 1.分离自己，当线程退出之后，线程所占用的资源就被操作系统回收
             * 2.接受一个自己的数据，判断请求的类型，根据不同的请求类型，调用不同的函数进行处理（注册和登录）
             * 1.注册
             * 2.登录
             **/
            pthread_detach(pthread_self());
            TcpConnect* tc = (TcpConnect*)arg;
            ChatServer* cs = (ChatServer*)tc->Getserver();
            char ques_type = -1;

            ssize_t recv_size = recv(tc->GetSockFd(), &ques_type, 1, 0);
            if(recv_size < 0)
            {
                close(tc->GetSockFd());
                return NULL;
            }
            
            else if (recv_size == 0)
            {
                //recv_size为0表示对端连接关闭了
                close(tc->GetSockFd());
                return NULL;
            }
            
            //接受回来了一个字节的数据 
            int resp_status = -1;
            uint32_t user_id;
            switch(ques_type)
            {
                case REGISTER_RESQ:
                    {
                        //TODO
                        //处理注册请求,DealRegister
                        resp_status = cs->DealRegister(tc->GetSockFd(), &user_id);
                        break;
                    }
                case LOGIN_RESQ:
                    {
                        //处理登录请求,DealLogin
                        resp_status = cs->DealLogin(tc->GetSockFd(), &user_id);
                        break;
                    }
            }    

            //响应的构造
            //struct ReplyInfo
            //status
            //userid
            //send进行发送
            
            struct ReplyInfo ri;
            ri.resp_status_ = resp_status;
            ri.id_ = user_id;

            LOG(INFO, "resp_status is") << ri.resp_status_ << std::endl;
            LOG(INFO, "id is ") << ri.id_ << std::endl;

            int max_round_count = MAX_ROUND_COUNT;
            while(max_round_count > 0)
            {
                
                ssize_t send_size = send(tc->GetSockFd(), &ri, sizeof(ri), 0);
                if(send_size >= 0)
                {  
                    LOG(INFO, "Send reply uccess") << std::endl;
                    break;
                }   
                /*
                *发送失败之后，是否需要重复发送？需不需要将待发送的数据缓存起来
                **/
                LOG(WARNING, "Send reply failed") << std::endl;
                max_round_count--;
            }

            //关闭当前的套接字
            close(tc->GetSockFd());
            delete tc;
            tc = NULL;
            return NULL;
        }

private:
        //不管登录成功还是注册失败，都要给客户端返回一个应答
        int DealRegister(int new_sock, uint32_t* user_id)
        {
            //继续从tcp连接当中接受注册数据，方法就是直接使用RegisterInfo函数接收
            struct RegisterInfo ri;
            ssize_t recv_size = recv(new_sock, &ri, sizeof(ri), 0);
            if(recv_size < 0)
            {
                return -1;
            }
            else if(recv_size == 0)
            {
                close(new_sock);
                return -2;
            }

            //正常接收到了
            //需要将数据递交给用户管理模块，进行注册，并且将用户数据进行留存
            //需要和用户管理模块进行交互了
            //TODO
            int ret = user_manager_->DealRegister(ri.nick_name_, ri.school_, ri.passwd_, user_id);
            if(ret < 0)
            {
                //注册失败
                return REGISTER_FAILED;
            }

            //注册成功
            return REGISTER_SUCCESS;
        }

        int DealLogin(int new_sock, uint32_t* user_id)
        {
            //继续从tcp连接中接收登录数据，策略就是直接使用LoginInfo
            struct LoginInfo li;

            ssize_t recv_size = recv(new_sock, &li, sizeof(li), 0);
            if(recv_size < 0)
            {
                return -1;
            }
            else if(recv_size == 0)
            {
                close(new_sock);
                return -2;
            }

            *user_id = li.id_;
            //正常逻辑
            //TODO 调用管理模块，判断登录请求id和密码是否正确
            int ret = user_manager_->DealLogin(li.id_, li.passwd_);
            if(ret < 0)
            {
                return LOGIN_FAILED;
            }
            return LOGIN_SUCCESS;
        }

private:
    int tcp_sock_;
    uint16_t tcp_port_;
    int udp_sock_;

    UserManager* user_manager_;
};
