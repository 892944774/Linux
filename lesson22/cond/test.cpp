#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

//0 没有面
//1 有面

#define THREAD_COUNT 2

int g_bowl = 0;

pthread_mutex_t g_tex;
pthread_cond_t cond;//消费者条件变量
pthread_cond_t product_cond;//生产者条件变量

void* EatNoodle(void* arg)
{
    pthread_detach(pthread_self());
    (void)arg;
    while(1)
    {
        pthread_detach(pthread_self());
        if(g_bowl <= 0)
        {
            //1 将执行流放入到PCB等待对列中
            //2 解锁
            //3 等待被唤醒，竞争互斥锁
            pthread_cond_wait(&cond, &g_tex);
        }

        g_bowl--;
        printf("Eat--g_bowl:%d\n", g_bowl);
        pthread_mutex_unlock(&g_tex);

        //通知做面的人
        pthread_cond_signal(&product_cond);

    }
    return NULL;
}

void* MakeNoodle(void* arg)
{
    pthread_detach(pthread_self());
    (void)arg;
    while(1)
    {
        pthread_mutex_lock(&g_tex);
        if(g_bowl > 0)
        {
            //等待
            pthread_cond_wait(&product_cond, &g_tex);
        }

        g_bowl++;
        printf("Make-----g_bowl：%d\n", g_bowl);
        pthread_mutex_unlock(&g_tex);
        //通知吃面的人
        pthread_cond_signal(&cond); 
    }
    return NULL;
}

int main()
{
    pthread_mutex_init(&g_tex, NULL);
    pthread_cond_init(&cond, NULL);
    pthread_cond_init(&product_cond, NULL);
    pthread_t tid;
    for(int i = 0; i < THREAD_COUNT; i++)
    {
        //循环两次，每次创建一个吃面的，再创建一个做面的人
        int ret = pthread_create(&tid, NULL, EatNoodle, NULL);
        if(ret < 0)
        {
            perror("pthread_create");
            return -1;
        }

        ret = pthread_create(&tid, NULL, MakeNoodle, NULL);
        if(ret < 0)
        {
            perror("pthread_create");
            return -1;
        }
    }

    while(1)
    {
        sleep(1);
    }

    pthread_mutex_destroy(&g_tex);
    pthread_cond_destroy(&cond);
    pthread_cond_destroy(&product_cond);
    return 0;
}
