#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

class test
{
    public:
        test()
        {}

        ~test()
        {}

        static void* ThreadStart(void* arg)
        {
            test* lp = (test*)arg;
        }
        
        int InitSvr()
        {
            pthread_create(&tid, NULL, ThreadStart, (void*)this);
        }

    private:
        pthread_t tid_;
}

