#pragma once
#include <stdio.h>
#include <unistd.h>

void print()
{
    while(1)
    {
        printf("hello world\n");
        sleep(1);
    }
}
