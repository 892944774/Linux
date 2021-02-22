#pragma once 
#include <string.h>
#include <pthread.h>
#include <iostream>
#include <string>
#include <unordered_map>

#include "ConnectionInfo.hpp"

/*
 * key-value 结构体来保存用户的信息
 *   Key : 用户id
 *   value : 用来代表单个用户的信息
 *   eg：1{nickname，school, passwd}
 *       2{nickname, school, passwd}
 *
 *  不好的地方：
 *      每次重启服务进程之后，之前注册的所有用户的信息都没有了（之前的注册的用户没有持久化）
 *  解决方案1：
 *      将之前注册的数据放到数据库当中保存
 *      1.启动服务的时候，需要去数据库当中获取之前注册的用户信息，加载到内存进程当中，在判断登录的时候，不需要查询数据库，用户访问就比较快捷。
 *      2.当注册完毕之后，还需要将数据及时写到数据当中（持久化）
 * 解决方案2：
 *      将注册的数据持久化到文件中
 * */

class UserInfo
{
    public:
        UserInfo(const std::string& nick_name, const std::string& school,const std::string& passwd, uint32_t user_id)
        {
            nick_name_ = nick_name;
            school_ = school;
            passwd_ = passwd;
            user_id_ = user_id;
        }

        ~UserInfo()
        {

        }

        std::string& GetPasswd()
        {
            return passwd_;
        }

    private:
        std::string nick_name_;
        std::string school_;
        std::string passwd_;

        //用户id
        uint32_t user_id_;
};

class UserManager
{
    public:
        UserManager()
        {
            user_map_.clear();
            pthread_mutex_init(&map_lock_, NULL);
            prepare_id_ = 0;
        }

        ~UserManager()
        {
            pthread_mutex_destroy(&map_lock_);
        }

        //处理注册请求
        //user_id是出参，返回给调用者，出参—主调函数调用，入参—被调函数调用
        int DealRegister(const std::string& nick_name, const std::string& school, const std::string& passwd, uint32_t* user_id)
        {
            //1.判断 字段是否为空
            if(nick_name.size() == 0 || school.size() == 0 || passwd.size() == 0)
            {
                return -1;
            }
            //2.创建单个用户  UserInfo这个类的对象
            pthread_mutex_lock(&map_lock_);
            //3.分配用户id
            UserInfo ui(nick_name, school, passwd, prepare_id_);
            *user_id = prepare_id_;
            //TODO 需要更改当前用户的状态 
             
            //4.将用户的数据插入到map当中
            user_map_.insert(std::make_pair(prepare_id_, ui));
            //5.更新预分配的用户id
            prepare_id_++;
            pthread_mutex_unlock(&map_lock_);
            return 0;
        }
        
        //用户提交的内容，在用户管理模块进行用户信息管理
        int DealLogin(uint32_t id, const std::string& passwd)
        {
            //1.判断password是否为空
            //2.判断id,在unordered_map, 当中查找是否有该id对应的值
            //  2.1没找到该id的值，返回登录失败
            //  2.2找到了对象id的值
            //       对比保存的密码的值和该次提交上来的密码的值是否一致
            //         a.如果不一致，则登录失败
            //         b.如果一致，则登录成功
            
            if(passwd.size() == 0)
            {
                return -1;
            }

            std::unordered_map<uint32_t, UserInfo>::iterator iter;

            pthread_mutex_lock(&map_lock_);
            iter = user_map_.find(id);
            if(iter == user_map_.end())
            {

                return -2;
            }

            //查找到了，比较密码
            std::string reg_passwd = iter->second.GetPasswd();
            if(reg_passwd != passwd)
            {
                pthread_mutex_unlock(&map_lock_);
                return -3;
            }

            //密码对比成功
            pthread_mutex_unlock(&map_lock_);
            return 0;
        }
    private:
        /*
         * std::string --> id
         * UserInfo --> 保存的是具体用户的信息
         * */
        std::unordered_map<uint32_t, UserInfo> user_map_;
        pthread_mutex_t map_lock_;

        //预分配的用户id,当用户管理模块接收到的注册请求之后，将prepare_id分配给注册的用户，分配完毕之后，需要对prepare_id进行更新
        uint32_t prepare_id_;
};
