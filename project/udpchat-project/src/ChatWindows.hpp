#pragma once
#include <pthread.h>
#include <iostream>
#include <vector>
using namespace std;

#include <ncurses.h>
#include "tool.hpp"
#include "ChatClient.hpp"

class ChatWindow;

class Pram
{
    public:
        Pram(int thread_num, ChatWindow* cw, UdpClient* uc)
        {
            thread_num_ = thread_num;
            cw_ = cw;
            uc_ = uc;
        }

    public:
        int thread_num_;
        ChatWindow* cw_;

        UdpClient* uc_;
};

class ChatWindow
{
    public:
        ChatWindow()
        {
            header_ = NULL;
            output_ = NULL;
            user_list_ = NULL;
            intput_ = NULL;
            vec_.clear();

            pthread_mutex_init(&lock_init, NULL);
        }

        ~ChatWindow()
        {
            if(header_)
            {
                delwin(header_);
            }

            if(output_)
            {
                delwin(output_);
            }

            if(user_list_)
            {
                delwin(user_list_);
            }

            if(input_)
            {
                delwin(input_);
            }

            pthread_mutex_destroy(&lock_win_);

            endwin();
        }

        //启动线程
        int start(UdpClient* uc)
        {
            //初始化ncurses
            initsrc();
            for(int i = 0; i < 4; i++)
            {
                Pram* pram = new Pram(i, this, uc);
                if(!pram)
                {
                    LOG(ERROR, "create pram failed") << endl;
                    exit(1);
                }

                pthread_t tid;
                int ret = pthread_create(&tid, NULL, RunWIndowStart, (void*)pram);
                if(ret < 0)
                {
                    LOG(ERROR, "create window thread failed") << endl;
                    exit(1);
                }
                vec_.push_back(tid);
            }

            for(size_t i = 0; i<vec_.size();i++)
            {
                pthread_join(vec_[i], NULL);
            }
        }

        static void* RunWIndowStart(void* arg)
        {
            //1.需要区分不同的线程，让不同的线程去绘制不同的窗口
            // 使用线程创建时候的变量i进行区分
            //2.传递进来窗口的句柄
            pram* pram = (Pram*)arg;


        }


    private:
        WINDOW* header_;
        WINDOW* output_;
        WINDOW* user_list_;
        WINDOW* input_;

        pthread_mutex_t lock_win_;

        std::vector<pthread_t> vec_;
};


