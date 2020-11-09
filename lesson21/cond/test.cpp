#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

//0 没有面
//1 有面

int g_bowl = 0;

pthread_mutex_t g_tex;
pthread_cond_t cond;

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
        pthread_cond_signal(&cond);

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
            pthread_cond_wait(&cond, &g_tex);
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
    pthread_t tid;
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

    while(1)
    {
        sleep(1);
    }

    pthread_mutex_destroy(&g_tex);
    pthread_cond_destroy(&cond);
    return 0;
}
