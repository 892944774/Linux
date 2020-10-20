#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define KEY 0x09999999
int main()
{
    int shmid = shmget(KEY, 1024, IPC_CREAT | IPC_EXCL | 0664);
    if(shmid < 0)
    {
        perror("shmget");
        return -1;
    }

    void* lp = shmat(shmid, NULL, 0);

    while(1)
    {
        printf("%s\n", ((char*)lp));
        sleep(1);
    }
    return 0;
}
