#include <curses.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <time.h>

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

//structure for initializing X,Y coordinate-based variables
typedef struct{
    int x;
    int y;
} vec2;

int score = 0;

int main(){

    initscr();
    srand(time(NULL));
    //enable the initialized screen to read key bindings
    keypad(stdscr, true);
    //prevent key inputs from being repeated by the program
    noecho();
    //hides the curser
    curs_set(0);
    //enables the program to run and recieve inputs at the same time
    nodelay(stdscr, true);

//=================player snake=================
    //set default position to left end of the middle row
    vec2 head = {2,LINES/2};
    //set default movement to rightwards
    vec2 dir = {1.0};

//=================apples================= 
    //set the default position of the apple to be random (-2 to prevent the apple from spawning in the border)
    vec2 apple = {1+rand() % (COLS-2), 1+rand() % (LINES-2)};

//=================Controls=================
    // int pressed;
    //while screen is initialized
    while(true){
        //pressed = character read from window(main window)
        int pressed = getch();

        //if a key was intentionally pressed (not an error)
        if(pressed != ERR){
            // if the character read is a directional key, move X or Y directions based on the key.
            if(pressed == KEY_LEFT){
                //prevents snake from going into itself (turning the opposite direction). Same application for other input keys
                if(dir.x == 1){
                    continue;
                }
                dir.x = -1;
                dir.y = 0;
            }
            else if(pressed == KEY_RIGHT){
                if(dir.x == -1){
                    continue;
                }
                dir.x = 1;
                dir.y = 0;
            }
            else if(pressed == KEY_DOWN){
                if(dir.y == -1){
                    continue;
                }
                dir.x = 0;
                dir.y = 1;
            }
            else if(pressed == KEY_UP){
                if(dir.y == 1){
                    continue;
                }
                dir.x = 0;
                dir.y = -1;
            }

            //if ESC is read, kill the game
            else if (pressed == '\e'){
                break;
            }
        }

        //position of snake head will continuously move X and Y spaces based on the read key input
        head.x += dir.x;
        head.y += dir.y;

        //if the snake head collides with the apple: 
        //increase the score | move the apple to a random spot (/2 to counteract the *2 grid positioning | -2 to prevent the apple from spawning in the border)
        if (head.x == apple.x && head.y == apple.y){
            score += 1;
            apple.x = 1+rand() % ((COLS/2)-2);
            apple.y = 1+rand() % (LINES-2);
        }

//=================draw=================
        //clear the screen
        clear();
        //draw the screen border
        screenBorder();

        //print the apple character at position Y,X. 
            //*2 to keep consistency with the snake's own grid movement.
        mvaddch(apple.y, apple.x*2, '@');

        //print the given character (Z) at position Y,X (defined above)
            //*2 to increase and match vertical movement speed to horizontal movement speed (could play with later to make the game harder as it goes)
        mvaddch(head.y, head.x*2, '~');
        
        refresh();
        //slow the program down by milliseconds to make it reactable
        usleep(125000);
    }
    endwin();
    return 0;
}
