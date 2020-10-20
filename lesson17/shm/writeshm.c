#include <stdio.h>
#include <unistd.h>
#include <sys/shm.h>
#include <string.h>

#define KEY 0x99999999

int main()
{
    int shmid = shmget(KEY, 1024, IPC_CREAT | 0664);
    if(shmid < 0)
    {
        perror("shmget");
        return -1;
    }

    void *lp = shmat(shmid, NULL, 0);
    {
        char buf[1024] = {0};
        sprintf(buf, "%s-%d\n", "Linux84", 1);
        strcpy((char*)lp, buf);//将buf的字符读到lp所指向的共享内存中
    }

    sleep(10);

    shmdt(lp);//10秒之后将lp所指向的共享内存与当前进程脱离

    struct shmid_ds buf;//描述共享内存的结构体
    shmctl(shmid, IPC_STAT, &buf);
    
    printf("%lu\n", buf.shm_segsz);//打印一下buf中共享内存的大小
    shmctl(shmid, IPC_RMID, NULL);
    while(1)
    {
       sleep(1); 
    }
    return 0;
}
