#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#define THREAD_COUNT 4

struct ThreadData
{
    int num_;
    ThreadData()
    {
        num_ = -1;
    }
};

class Data
{
    public:
        void SetData(int data)
        {
            data_ = data;
        }

        int GetData()
        {
            return data_;
        }

    private:
        int data_;
};

void* ThreadStrat(void* arg)
{
    pthread_detach(pthread_self());
    Data* d = (Data*)arg;

    while(1)
    {
        printf("i am working threai:%d\n", d->GetData());
        sleep(1);
    }

    sleep(3);
    pthread_cancel(pthread_self());
    while(1)
    {
        sleep(1);
    }
    delete d;
}

int main()
{
    pthread_t tid[THREAD_COUNT];
    for(int i = 0; i< THREAD_COUNT; ++i)
    {
        Data* d = new Data;
        d->SetData(i);
        int ret = pthread_create(&tid[i], NULL, ThreadStrat, (void*)d);
        if(ret < 0)
        {
            perror("pthread_create");
            return -1;
        }
    }

    printf("hhhhhhhhhhhh\n");
    while(1)
    {
        printf("i am main thread\n");
        sleep(1);
    }
    return 0;
}
