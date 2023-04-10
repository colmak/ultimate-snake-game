// Snake game 
// Roland Van Duine, Victor Setaro

// Libraries 
#include <curses.h> 
#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>   

// constants
#define WIDTH 80
#define HEIGHT 40
#define SPEED 300000
#define LENGTH 5 

// Snake struct
struct Snake{
    int x,y;    
    struct Snake *next;
};

// Direction enum
enum Direction { LEFT, RIGHT, UP, DOWN };

// Global variables
int speed, snakeLength, numNewBodies;
float speedRate = 0.98;
bool gameOver = false, exitGame = false;
enum Direction movementDirection;
struct Snake *head;

// Prototypes 
void init();                     // Initialize game
void updateDirection(int);       // Direction control
void snakeMove();               // Snake movement
void render();              // Build the pit and overall gameplay area
void end();    // This function is called when you kill the snake


// #### Main Function #####

/*
 Name: Victor Setaro
 Use: Driver function
*/

int main(){
    init();       // builds the game and sets the snake's intial position in the pit

    // repeats game over condition met
    while(!exitGame){
        if(gameOver){
            end("Game over!\n");
        }
        int keypress = getch();
        updateDirection(keypress);
        snakeMove();
        render();
	    usleep(speed);      // pause between snake movement. The speed is proportional to its length.
                            // the longer the snake, the faster it goes
    }
    endwin();    // turn off curses
    return 0;   
}

// ################ init FUNCTION #################
/*
 Function : init()
 Name: Roland Van Duine
 Use: This function set the game and init snake position and Direction ect...
*/
void init(){
    clear();    // clear the screen/ terminal

    // Initialize 
    initscr();  // start curses or init curse
    keypad(stdscr, TRUE);   // let us use arrow keys from keyboard
    timeout(0);     // dont stop. this prevent getchar from waiting for input

    noecho();
    curs_set(0);    // disable cursor, So it does not interfer with gamepaly

    raw();  //disables line buffering and erase/kill character-processing 

    // Initialise Snake 
    movementDirection =RIGHT;     // Start the snake movement to  RIGHT
    gameOver = false;   // Game is not Over. This is because there is a while loop that's checking
    speed = SPEED;      // Set the initial speed to the defined constant.
    snakeLength = LENGTH;   // Set the initial LENGTH to the defined constant

    // Snake starts at the middle of the screen 
    int x = WIDTH/2;
    int y = HEIGHT/2;

    // Create the snake
    struct Snake *currentBody;
    currentBody = (struct Snake*) malloc(sizeof(struct Snake));
    currentBody->next = NULL;
    currentBody->x = x - (LENGTH - 1);
    currentBody->y = y;

    for (int i = LENGTH - 2; i >= 0; i--) {
        head = (struct Snake*) malloc(sizeof(struct Snake));
        head->next = currentBody;
        head->x = x - i;
        head->y = y;
        currentBody = head;    
    }
}

// ##### updateDirection function #####
/*
 Function : updateDirection()
 Name: Victor Setaro
 Use: This function takes in the user's key presses as input and updates the direction of the snake accordingly.
*/

void updateDirection(int keypress){
    /*  
        Switch case to change the direction of the snake based on the user's key presses
        The game will be over if the snake is moving in one direction and the user tries to make the snake move in the opposite direction.
        ie. Snake moving DOWN and the user presses the UP arrow
    */

    switch (keypress) {     
        case KEY_UP:    
            if(movementDirection == DOWN){
                end("You lose! Snakes do not go BACKWARDS...\n");
                break;
            }
            movementDirection = UP;
            break;

        case KEY_DOWN:
            if(movementDirection == UP){
                    // reverse case
                    end("You lose!\n");
                    break;
                }
                movementDirection = DOWN;
                break;

        case KEY_LEFT:
            if(movementDirection == RIGHT){
                
                    end("You lose!\n");
                    break;
            }
            movementDirection = LEFT;
            break;
        
        case KEY_RIGHT:
            if(movementDirection == LEFT){
                 
                    end("You lose!\n");
                    break;
            }
            movementDirection = RIGHT;
            break;
            // 'q' or 'Q' the game will quit game
            case 'q':
            case 'Q':
                exitGame = 1;
    }
}


// ### snakeMove function ###

/*
 Function : snakeMove()
 Name: Victor Setaro
 Use: Snake movement
*/

void snakeMove(){

     // Creates a new head for the snake
     struct Snake *newHead;
     newHead = (struct Snake*) malloc(sizeof(struct Snake));
     newHead->next = head;
     newHead->x = head->x;
     newHead->y = head->y;
     head = newHead; 

    // The movement of the snake's head changes based on the user's key presses.
    // The position of the snake is changed by updating its coordinates within the snake pit
     if(movementDirection == UP) {
        head->y--; 
     } 
     else if(movementDirection == DOWN) {
        head->y++; 
     } 
     else if(movementDirection == LEFT) {
        head->x--;
     }  
     else if(movementDirection == RIGHT) head->x++;

        
    // Ends the game if the snake hits the border/wall of the snake pit
    if (head->x >= WIDTH || head->x <= 0 || head->y >= HEIGHT || head->y <= 0) {
        gameOver = 1;   // ends the game
        end("You hit the wall loser! \n");
    }

    // Ends the game if the snake hits itself
    struct Snake *currentBody;
    currentBody = head;

    // loop will keep going unless 
    while (currentBody->next->next != NULL) {
        // if the snake's head collides with its body, end the game
        if (currentBody->next->x == head->x && currentBody->next->y == head->y) {
            gameOver = 1;    // ends the game
            end("You lose! Stop trying to eat yourself :( \n");
        }
        currentBody = currentBody->next;
    }

    // if no new body left to add, remove the tail
    if (numNewBodies <= 0) {
        mvaddch(currentBody->next->y, currentBody->next->x, ' '); // remove unneeded part
        free(currentBody->next);   // frees memory
        currentBody->next = NULL;
    }
    else {
        numNewBodies--;
        speed = (int)(speed * speedRate);
    }
     }

// ################ end FUNCTION #################
/*
 Function : render()
 Name: Roland Van Duine
 Use: Build the pit and overall Gameplay Area 
*/

void end(char* result) {
    clear();    // clear terminal 
    mvprintw(10, 50, result);    // print the scoreboard
    mvprintw(12, 42, "Game Over! Hit any key to close game");   // print text to terminal
    refresh();
    sleep(4);   
    getchar();  
    endwin();   
    exit(0);    
}

// ################ render FUNCTION #################
/*
 Function : render()
 Name: Roland Van Duine
 Use: Render the pit and rest of rendering to terminal
*/
void render() {
    // Render Border
    move(0, 0); // move cursor to upper left corner for border creation
    for (int i = 0; i <= HEIGHT; i++) {
        for (int j = 0; j <= WIDTH; j++) {
            if (i == 0 || i == HEIGHT || j == 0 || j == WIDTH)
                mvaddch(i, j, '#'); // The border of '#'
        }
        addch('\n');    // add new line
    }
    // Render snake
    struct Snake *currentBody;
    currentBody = head;
    mvaddch(currentBody->y, currentBody->x, 'O');
    while (currentBody->next != NULL) {
        mvaddch(currentBody->next->y, currentBody->next->x, 'O');
        currentBody = currentBody->next;
    }
    // game progress
    // User wins when The user wins the game if the snake's length grows to the length equal to half the perimeter of the border.
    float halfPerimeter = HEIGHT + WIDTH;
    float length = snakeLength;
    float progress = (length / halfPerimeter) * 100;
    mvprintw(4, WIDTH + 2, "Progress: %d %%", (int)progress);
    if ((int)progress >= 100) {
        end("You won!!!\n");     // User Won!!
    }
   // render
    move(HEIGHT + 1, 0); // move cursor out of the game screen 
    refresh(); 
}