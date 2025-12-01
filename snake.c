#include <curses.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

#define MAX_SCORE 256
#define FRAME_TIME 110000

//structure for initializing X,Y coordinate-based variables
typedef struct{
    int x;
    int y;
} vec2;

//forward declarations - best practice and convenience
void quit_game(void);
void draw_border(int top, int left, int width, int height); //variables are local
void screenBorder(void);
void process_input(void);
void game_over(void);
void update(void);
vec2 spawn_apple(void);

//variable declaration
int score = 0;
char score_message[16];

bool skip = false;
bool is_running = true;

const int screen_width = 40;
const int screen_height = 20;

//initialize gameplay window (screen)
WINDOW *win;

//snake creation
vec2 head = {2,screen_height/2};
vec2 segments[MAX_SCORE];
vec2 dir = {1,0};

vec2 apple;

//---helper functions---

//quit the game
void quit_game() {
    is_running = false;
}

//draw a rectangular border
void draw_border(int top, int left, int width, int height) {
    mvaddch(top, left, '+'); // corners (little visual flair)
    mvaddch(top, left + width - 1, '+');
    mvaddch(top + height - 1, left, '+');
    mvaddch(top + height - 1, left + width - 1, '+');

    //horizontal lines
    for (int x = left + 1; x < left + width - 1; x++) {
        mvaddch(top, x, '-');
        mvaddch(top + height - 1, x, '-');
    }

    //vertical lines
    for (int y = top + 1; y < top + height - 1; y++) {
        mvaddch(y, left, '|');
        mvaddch(y, left + width - 1, '|');
    }
}

//draw the main screen border
void screenBorder() {
    attron(COLOR_PAIR(1)); // red
    draw_border(0, 0, screen_width * 2, screen_height);
    attroff(COLOR_PAIR(1));

}

//collision logic (checks if both x and y are the same for the two objects passed in)
bool collide(vec2 a, vec2 b) {
    if (a.x == b.x && a.y == b.y) {
        return true;
    }
    else return false;
}

//snake self collision logic (checks if head is in the same position as a body segment)
bool collide_snake_body(vec2 point) {
    for (int i = 0; i < score; i++) {
        if (collide(point, segments[i])) {
            return true;
        }
    }
    return false;
}

//apple spawning logic (tweaked for consistency and self sufficiency)
vec2 spawn_apple(){
    vec2 a;
    do {
        a.x = 1 + rand() % (screen_width - 2);
        a.y = 1 + rand() % (screen_height - 2);
    } while (collide(head, a) || collide_snake_body(a));
    return a;
}

void update() {
    //snake body movement and collision
    for (int i = score; i > 0; i--) {
        segments[i] = segments[i-1];
    }
    segments[0] = head;

    //position of snake head will continuously move X and Y spaces based on the read key input
    head.x += dir.x;
    head.y += dir.y;

    //lose condition
    if (collide_snake_body(head) || head.x < 0 || head.y < 0 || head.x >= screen_width || head.y >= screen_height) {
        is_running = false;
        game_over();
    }

    //if the snake head collides with the apple: 
    //increase the score | move the apple to a random spot
    if (collide(head, apple)) {
        if (score < MAX_SCORE) {
            score += 1;
            sprintf(score_message, "[ Score: %d ]", score);
        }
        else {
            printf("You win!"); //win condition (obviously)
        }

        apple = spawn_apple();
    }
    usleep(FRAME_TIME);
}

void init() {
    srand(time(NULL));
    
    //initialize window
    win = initscr();
    set_escdelay(25);  // wait only 25 ms instead of 1000
    //accept player input, hide cursor
    keypad(win, true);
    noecho();
    nodelay(win, true);
    curs_set(0);

    //informs the user if they cannot use color
    if (has_colors() == false) {
        endwin();
        fprintf(stderr, "your terminal doesn't support color.\n");
        exit(1);
    }

    //color options :)
    start_color();
    use_default_colors();
    init_pair(1, COLOR_RED, -1);
    init_pair(2, COLOR_GREEN, -1);
    init_pair(3, COLOR_YELLOW, -1);

    apple = spawn_apple();

    sprintf(score_message, "[ SCORE: %d ]", score);
}

//resets after a game ends, if user chooses to play again
void restart_game() {
    head.x = 5;
    head.y = 5;
    dir.x = 1;
    dir.y = 0;
    score = 0;
    sprintf(score_message, " [ Score: %d ]", score);
    is_running = true;
}

void process_input() {
    //pressed = character read from window(main window)
    int pressed = wgetch(win);

    //if a key was intentionally pressed (not an error)
    if(pressed != ERR){
        // if the character read is a directional key, move X or Y directions based on the key.
        if(pressed == KEY_LEFT){
            //prevents snake from going into itself (turning the opposite direction). Same application for other input keys
            if(dir.x == 1){
                return;
                skip = true;
            }
            dir.x = -1;
            dir.y = 0;
        }
        else if(pressed == KEY_RIGHT){
            if(dir.x == -1){
                return;
                skip = true;
            }
            dir.x = 1;
            dir.y = 0;
        }
        else if(pressed == KEY_DOWN){
            if(dir.y == -1){
                return;
                skip = true;
            }
            dir.x = 0;
            dir.y = 1;
        }
        else if(pressed == KEY_UP){
            if(dir.y == 1){
                return;
                skip = true;
            }
            dir.x = 0;
            dir.y = -1;
        }

        else if (pressed == ' '){
            if (!is_running)
                restart_game();
        }

        //*************** this needs to be fixed ***************
        //if ESC is read, kill the game (27 = ESC)
        else if (pressed == '\e'){
            is_running = false;
            quit_game();
        }
    }
}

//after a loss, prints to inform the user of their options
void game_over() {
    while (is_running == false) {
        process_input();

        mvaddstr(screen_height / 2, screen_width - 16, "            Game Over            ");
        mvaddstr(screen_height / 2 + 1, screen_width - 16, "Space to restart, ESC to quit");
        attron(COLOR_PAIR(3));
        draw_border(screen_height / 2 - 1, screen_width - 27, 17, 2);
        attroff(COLOR_PAIR(3));

        usleep(FRAME_TIME);
    }
}

int main(){
    init();
    //=================Controls=================
    while(true){
        process_input();
        update();

        //=================draw=================
        //clear the screen
        clear();
        //draw the screen border
        screenBorder();

        //draw score at top-left
        mvaddstr(0, 2, score_message);

        //print the apple character at position Y,X. 
        //*2 to keep consistency with the snake's own grid movement.
        mvaddch(apple.y, apple.x*2, '@');

        //print the given character (Z) at position Y,X (defined above)
        //*2 to increase and match vertical movement speed to horizontal movement speed (could play with later to make the game harder as it goes)
        for (int i = 0; i < score; i++) {
            mvaddch(segments[i].y, segments[i].x * 2, 'o');
        }
        mvaddch(head.y, head.x*2, '~');
        
        refresh();
        //slow the program down by milliseconds to make it reactable
        usleep(125000);
    }
    
    endwin();
    return 0;
}