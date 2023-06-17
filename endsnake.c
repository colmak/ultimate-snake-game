/*
    By Roland Van Duine & Victor Setaro 
    This is a snake game that is played in the terminal. The user uses the arrow keys to control the snake's movement.
*/

// Libraries needed for this project
#include <curses.h> 
#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>   

// Define const settings for the gameplay
#define WIDTH 70
#define HEIGHT 30
#define SPEED 300000
#define LENGTH 3 


// Enum directions. Use the arrow keys from keyboard to guide the snake
enum Direction { LEFT, RIGHT, UP, DOWN };


/*  Created a snake Struct 
 This is the snake object   */
struct Snake{
    int x,y;    // Snake position 
    struct Snake *next;
};

// Created a Fruit Struct 
struct Fruit{
    int x, y;
};

// Declaring all global variables
int speed, snakeLength, numNewBodies, numFruit, FruitExpireTime;
float speedRate = 0.98;
bool gameOver = false, exitGame = false;
enum Direction currDir;
struct Snake *head;
struct Fruit Fruit;
time_t FruitGenTime, now;

void init();                   // Initialize the game and sets the snake's intial position in the pit
void updateDirection(int);     // Updates the direction of the snake
void snakeMove();              // Control the snake
void render();                 // Builds the screen
void endMessage();             // Game over message
void fruitGen();               // Generates the fruit

/*
 Function : main()
 Name: Victor Setaro
 Functionality: Driver function
*/
int main(){
    init();       
    // loop repeats until gameOver = 1, when the snake dies or end key is pressed
    while(!exitGame){
        if(gameOver){
            endMessage("Game over\n");  // game over message
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

/*
 Function : updateDirection()
 Name: Victor Setaro
 Functionality: Control the direction of the snake 
*/
void updateDirection(int keypress){

    switch (keypress) {     
        case KEY_UP:    
            if(currDir == DOWN){
                endMessage("Game over\n");
                break;
            }
            currDir = UP;
            break;

        case KEY_DOWN:
            if(currDir == UP){
                    endMessage("Game over\n");
                    break;
                }
                currDir = DOWN;
                break;

        case KEY_LEFT:
            if(currDir == RIGHT){
                    endMessage("Game over\n");
                    break;
            }
            currDir = LEFT;
            break;
        
        case KEY_RIGHT:
            if(currDir == LEFT){
                    endMessage("Game over\n");
                    break;
            }
            currDir = RIGHT;
            break;
            // If the user presses 'x' or 'X' the game will be over
            case 'x':
            case 'X':
                exitGame = 1;
    }
}

/*
 Function : snakeMove()
 Name: Victor Setaro
 Functionality: Control the snake's movement
*/
void snakeMove(){
     // Snake head
     struct Snake *newHead;
     newHead = (struct Snake*) malloc(sizeof(struct Snake));
     newHead->next = head;
     newHead->x = head->x;
     newHead->y = head->y;
     head = newHead; 
    // The movement of the snake's head changes based on the user's key presses.
    // The position of the snake is changed by updating its coordinates within the snake pit
     if(currDir == UP) {
        head->y--; 
     } 
     else if(currDir == DOWN) {
        head->y++; 
     } 
     else if(currDir == LEFT) {
        head->x--;
     }  
     else if(currDir == RIGHT) head->x++;  
    // Ends the game if the snake hits the border/wall of the snake pit
    if (head->x >= WIDTH || head->x <= 0 || head->y >= HEIGHT || head->y <= 0) {
        gameOver = 1;   // ends the game
        endMessage("Game over \n");
    }
    // if the snake eat Fruit
    if (head->x == Fruit.x && head->y == Fruit.y) {
        numNewBodies += numFruit;
        snakeLength += numFruit;
        fruitGen();        
    }
    // Ends the game if the snake hits itself
    struct Snake *currBody;
    currBody = head;
    // loop will keep going unless 
    while (currBody->next->next != NULL) {
        // if the snake's head collides with a part of its body, end the game
        if (currBody->next->x == head->x && currBody->next->y == head->y) {
            gameOver = 1;    // ends the game
            endMessage("Game over \n");
        }
        currBody = currBody->next;
    }
    // if there is no new body left to add, remove the tail
    if (numNewBodies <= 0) {
        mvaddch(currBody->next->y, currBody->next->x, ' '); // remove that part of the snake from the screen
        free(currBody->next);   // frees memory
        currBody->next = NULL;
    }
    else {
        numNewBodies--;
        speed = (int)(speed * speedRate);
    }
     }

/*
 Function : endMessage()
 Name: Roland Van Duine
 functionality  :Display Gamoe over message on the screen
*/

void endMessage(char* result) {
    clear();    // clear terminal 
    mvprintw(10, 50, result);    // print the scoreboard
    mvprintw(12, 42, "Game over. Press a key to end");   // print text to terminal
    refresh();
    sleep(4);   // Wait 4 second
    getchar();  // get any key input from user keyboard to Quit the Screen
    endwin();   // end 
    exit(0);    // QUIT the program or KILL IT
}

/*
 Function : render()
 Name: Roland Van Duine
 functionality  :Build the pit and overall Gameplay Area 
*/
void render() {
    // Draw Border
    move(0, 0); // move cursor to the uppder left corner so border will be draw in the right position
    for (int i = 0; i <= HEIGHT; i++) {
        for (int j = 0; j <= WIDTH; j++) {
            // only print hashtages for firsts and lasts, which creates a border
            if (i == 0 || i == HEIGHT || j == 0 || j == WIDTH)
                mvaddch(i, j, '#'); // The border is made of '#' Symbol
        }
        addch('\n');    // add new line
    }
    // Draw Snake
    struct Snake *currBody;
    currBody = head;
    mvaddch(currBody->y, currBody->x, 'O');
    while (currBody->next != NULL) {
        mvaddch(currBody->next->y, currBody->next->x, 'O');
        currBody = currBody->next;
    }

    // When the Fruit expired, update location
    now = time(NULL);
    int timeSpent = (now - FruitGenTime);   
    int remainingTime = FruitExpireTime - timeSpent;
    if (remainingTime <= 0) {
        mvaddch(Fruit.y, Fruit.x, ' '); // clear current Fruit from the screen
        remainingTime = 0;
        fruitGen();
    } 

    //Stat of the game
    mvprintw(0, WIDTH + 2, "Fruit expires in: %d seconds", remainingTime);

    // game progress
    // User Wins when The user wins the game if the snake's length grows to the length equal to half the perimeter of the border.
    float halfPerimeter = HEIGHT + WIDTH;
    float length = snakeLength;
    float progress = (length / halfPerimeter) * 100;
    mvprintw(4, WIDTH + 2, "Progress: %d %%", (int)progress);
    if ((int)progress >= 100) {
        endMessage("You won!!!\n");     // User Won!!
    }
   // render
    move(HEIGHT + 1, 0); // move cursor out of the game screen 
    refresh(); // render screen, screen buffer -> real screen
}

/*
 Function : fruitGen()
 Name: Roland Van Duine
 functionality  :This funtion generates fruits on the screen for the snake to eat.
*/
void fruitGen() {
    bool overlapping = true;
    /* Check if Fruit spawn on the location already taken by snake
     because the wtropthy can not appear on a space already occupy by the snake.*/
    while(overlapping) {
        overlapping = false;
        
        // Fruit X, Y random position
        Fruit.x = random() % (WIDTH - 1) + 1;
        Fruit.y = random() % (HEIGHT - 1) + 1;

        struct Snake *currBody;
        currBody = head;
        if (Fruit.x == currBody->x && Fruit.y == currBody->y) {
            overlapping = true;
        }
        while (currBody->next != NULL) {
            // if Fruit appear on the snake body, regenrate a new Fruit
            if (Fruit.x == currBody->next->x && Fruit.y == currBody->next->y) {
                overlapping = true; 
            }
            currBody = currBody->next;
        }
    }
    // Randomly generate fruit on the screen
    numFruit = rand() % 9 + 1;
    FruitExpireTime = rand() % 9 + 1;
    FruitGenTime = time(NULL);
    char numFruitChar = numFruit + '0';  
    mvaddch(Fruit.y, Fruit.x, numFruitChar);    
}


/*
 Function : init()
 Name: Roland Van Duine.
 functionality  : This function set the game and init snake position and Direction ect...
*/
void init(){
    clear();    // clear the screen/ terminal

    srand(time(NULL));

    int random_num = rand() % 4;
    
    // map the random number to a direction
    enum Direction random_direction;
    switch (random_num) {
        case 0:
            random_direction = LEFT;
            break;
        case 1:
            random_direction = RIGHT;
            break;
        case 2:
            random_direction = UP;
            break;
        case 3:
            random_direction = DOWN;
            break;
    }

    // Initialize 
    initscr();  // start curses or init curse
    keypad(stdscr, TRUE);   // let us use arrow keys from keyboard
    timeout(0);     // dont stop. this prevent getchar from waiting for input

    noecho();
    curs_set(0);    // disable cursor, So it does not interfer with gamepaly

    raw();  //disables line buffering and erase/kill character-processing 

    // Initialise Snake 
    currDir = random_direction;  // Start the snake movement to  random direction
    gameOver = false;          // Game initial state.
    speed = SPEED;             // Initial speed to the defined constant.
    snakeLength = LENGTH;      // Initial LENGTH to the defined constant

    // Snake starts at the middle of the screen 
    int x = WIDTH/2;
    int y = HEIGHT/2;

    // Create the snake, This snake is basically a Linked LIST
    head = (struct Snake*) malloc(sizeof(struct Snake));
    head->next = NULL;
    head->x = x;
    head->y = y;

    // Create the rest of the snake body behind the head in the direction it is facing
    struct Snake *currBody = head;
    for (int i = 1; i < LENGTH; i++) {
        struct Snake *newBody = (struct Snake*) malloc(sizeof(struct Snake));
        newBody->next = NULL;
        switch (currDir) {
            case LEFT:
                newBody->x = x + i;
                newBody->y = y;
                break;
            case RIGHT:
                newBody->x = x - i;
                newBody->y = y;
                break;
            case UP:
                newBody->x = x;
                newBody->y = y + i;
                break;
            case DOWN:
                newBody->x = x;
                newBody->y = y - i;
                break;
        }
        currBody->next = newBody;
        currBody = newBody;
    }
}