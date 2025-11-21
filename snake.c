#include <curses.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>

void screenBorder(){
//width = COLS
//height = LINES

//CORNERS:
    //top left = (0,0)
    //bottom left = (LINES-1, 0)
    //bottom right = (LINES-1, COLS-1)
    //top right = (0, COLS-1)

//create a horizontal line by printing # "COLS" times
hline('#', COLS);
//create a vertical line by printing # "LINES" times
vline('#', LINES);
move(0,COLS-1);
vline('#', LINES);
move(LINES-1,0);
hline('#', COLS);
refresh();
}


int main(int argc, char const *argv[]){

    initscr();
    clear();

    screenBorder();

    getchar();
    endwin();
    return 0;
}
