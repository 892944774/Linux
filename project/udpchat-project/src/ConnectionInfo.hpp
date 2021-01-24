#pragma once
#include <stdio.h>
#include <string.h>
#include <iostream>

//注册请求的数据格式
//1.昵称 2.学校 3.用户密码
//clinet to server 严格按照RegisterInfo传输数据
struct RegisterInfo
{
    RegisterInfo()
    {
        memset(nick_name_, '\0', sizeof(nick_name_));
        memset(school_, '\0', sizeof(school_));
        memset(passwd_, '\0', sizeof(passwd_));
    }
    char nick_name_[20];
    char school_[20];
    char passwd_[20];
};

//登录请求的数据格式
//1. 用户ID   2. 密码
struct LoginInfo
{
    LoginInfo()
    {
        memset(passwd_, '\0', sizeof(passwd_));
    }
    uint32_t id_;
    char passwd_[20];
};


//服务端给客户端回复应答的数据格式
//1. 当前的状态（注册状态，登录状态）
//2. 返回用户ID，类似于注册完毕之后的QQ号
struct ReplyInfo
{
    int resp_status;
    uint32_t id_;
};

//枚举状态
enum Status
{
    REGISTER_FAILED = 0,
    REGISTER_SUCCESS,
    LOGIN_FAILED,
    LOGIN_SUCCESS 
};

//如何标识当前的请求是注册请求还是登录请求
/*
 * 1.对于每一种请求，在tcp层面都会发送两个tcp包
 * 第一个tcp包，发送一个字节，表示请求是注册还是登录
 * 第二个tcp包，发送具体的注册或者登录请求数据
 * */
enum ResqType
{
    REGISTER_RESQ = 0,
    LOGIN_RESQ
};
