#include <stdio.h>
#include <string.h>

int main()
{
    FILE* fp = fopen("./linux84", "w+");
    if(!fp)
    {
        printf("open file failed\n");
        return -1;
    }

    printf("open file success\n");

    const char* str = "linux so easy";
    ssize_t ret = fwrite(str, 1, strlen(str), fp);
    //此时文件流指针指向了文件的尾部，要想读文件，需将文件流指针指向文件的头部
    printf("write block count is %d\n", ret);

    fseek(fp, 0, SEEK_SET);

    char buf[1024] = {0};
    ret = fread(buf, 1, 4, fp);
    printf("ret = %d, buf=%s\n", ret, buf);
    fclose(fp);//使用完文件指针函数后需要关闭文件流指针，否则会导致内存泄漏
    return 0;
}
