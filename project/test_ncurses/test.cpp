#include <stdio.h>
#include <iostream>

#include <ncurses.h>

int main()
{
    initscr();
    //printw("hello world!!");
    //refresh();

    //ncurses 库当中提供了两个宏
    //COLS
    //newwin
    //     第三个参数和第四个参数，描述的是当前绘制的窗口左上角从哪开始
    //
    //     第三个参数，表示纵坐标，第四个参数表示横坐标
    WINDOW* header = newwin(LINES/5, COLS, 0, 0);
    //0,0表示默认图形
    box(header, 0, 0);
    wrefresh(header);

    int y, x;
    getmaxyx(header, y, x);//获取当前窗口最大的长和宽
    mvwaddstr(header, y/2, (x-11)/2, "hello world");//将需要打印的数据合理安排好位置
    //并不是线程安全的，如果多线程操作，则需要加锁刷新
    wrefresh(header);
    //关闭窗口
    delwin(header);

    getchar();
    endwin();
    return 0;
}
