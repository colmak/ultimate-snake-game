/*
    Developed by: Giovanni Tshibangu & Dejah Murray 
    BB Team: 1
    Project name: Snake Part 1
    Date: 6/28/2022

    Part 1 Objective:
    Intermediate deliverable (due 06/30/2022) needs to have the following functionality:
    * The game starts with the snake of size 5 moving right
    * Snake movement can be controlled in all directions
    * Snake does not grow
    * Snake pit border is visible
    * No trophies
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



// Global variables
int speed, snakeLength, numNewBodies, numFruit, FruitExpireTime;
float speedRate = 0.98;
bool gameOver = false, exitGame = false;
enum Direction currDir;
struct Snake *head;
struct Fruit Fruit;
time_t FruitGenTime, now;

// Declaring all function prototypes 
void buildGame();                // This function sets the game and initial snake position ect...
void updateDirection(int);       // Change the snake position based on user input.
void guideSnake();               // More snake control, such as head movement and collision handler
void buildScreen();              // Build the pit and overall gameplay area
void gameOverMessageScreen();    // This function is called when you kill the snake
void FruitGenrator();            // This function Generates Trophies


// #### Main Function #####
/*
 Function : main()
 Name: Dejah Murray
 Functionality: Driver function
*/
int main(){
    buildGame();       // builds the game and sets the snake's intial position in the pit
    // loop repeats until user presses 'x' or 'X', which will set gameOver = 1
    while(!exitGame){
        if(gameOver){
            gameOverMessageScreen("YOU LOSE!\n");  // game over message
        }
        int keypress = getch();
        updateDirection(keypress);
        guideSnake();
        buildScreen();
	    usleep(speed);      // pause between snake movement. The speed is proportional to its length.
                            // the longer the snake, the faster it goes
    }
    endwin();    // turn off curses
    return 0;   
}
// ##### updateDirection function #####
/*
 Function : updateDirection()
 Name: Dejah Murray
 Functionality: This function takes in the user's key presses as input and updates the direction of the snake accordingly.
*/
void updateDirection(int keypress){
    /*  
        Switch case to change the direction of the snake based on the user's key presses
        The game will be over if the snake is moving in one direction and the user tries to make the snake move in the opposite direction.
        ie. Snake moving DOWN and the user presses the UP arrow
    */
    switch (keypress) {     
        case KEY_UP:    
            if(currDir == DOWN){
                gameOverMessageScreen("You lose! Snakes do not go BACKWARDS...\n");
                break;
            }
            currDir = UP;
            break;

        case KEY_DOWN:
            if(currDir == UP){
                    // reverse
                    gameOverMessageScreen("You lose! Snakes do not go BACKWARDS...\n");
                    break;
                }
                currDir = DOWN;
                break;

        case KEY_LEFT:
            if(currDir == RIGHT){
                // reverse
                    gameOverMessageScreen("You lose! Snakes do not go BACKWARDS...\n");
                    break;
            }
            currDir = LEFT;
            break;
        
        case KEY_RIGHT:
            if(currDir == LEFT){
                 // reverse
                    gameOverMessageScreen("You lose! Snakes do not go BACKWARDS...\n");
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
// ### guideSnake function ###
/*
 Function : guideSnake()
 Name: Dejah Murray
 Functionality: Allows the user to control the direction of the snake and handles 
 collision events ie. snake runs into the border, runs into itself, user tries to reverse snake direction.
*/
void guideSnake(){
     // Creates a new head for the snake
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
        gameOverMessageScreen("You hit the wall loser! \n");
    }
    // if the snake eat Fruit
    if (head->x == Fruit.x && head->y == Fruit.y) {
        numNewBodies += numFruit;
        snakeLength += numFruit;
        FruitGenrator();        
    }
    // Ends the game if the snake hits itself
    struct Snake *currBody;
    currBody = head;
    // loop will keep going unless 
    while (currBody->next->next != NULL) {
        // if the snake's head collides with a part of its body, end the game
        if (currBody->next->x == head->x && currBody->next->y == head->y) {
            gameOver = 1;    // ends the game
            gameOverMessageScreen("You lose! Stop trying to eat yourself :( \n");
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

// ################ gameOverMessageScreen FUNCTION #################
/*
 Function : gameOverMessageScreen()
 Name: Giovanni Tshibangu
 functionality  :Display Gamoe over message on the screen
*/

void gameOverMessageScreen(char* result) {
    clear();    // clear terminal 
    mvprintw(10, 50, result);    // print the scoreboard
    mvprintw(12, 42, "Game Over! Hit any key to close game");   // print text to terminal
    refresh();
    sleep(4);   // Wait 4 second
    getchar();  // get any key input from user keyboard to Quit the Screen
    endwin();   // end 
    exit(0);    // QUIT the program or KILL IT
}

// ################ buildScreen FUNCTION #################
/*
 Function : buildScreen()
 Name: Giovanni Tshibangu
 functionality  :Build the pit and overall Gameplay Area 
*/
void buildScreen() {
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
        FruitGenrator();
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
        gameOverMessageScreen("You won!!!\n");     // User Won!!
    }
   // render
    move(HEIGHT + 1, 0); // move cursor out of the game screen 
    refresh(); // render screen, screen buffer -> real screen
}


// ################ FruitGenrator FUNCTION #################
/*
 Function : FruitGenrator()
 Name: Giovanni Tshibangu
 functionality  :This funtion generates fruits on the screen for the snake to eat.
*/
void FruitGenrator() {
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


// ################ buildGame FUNCTION #################
/*
 Function : buildGame()
 Name: Giovanni Tshibangu.
 functionality  : This function set the game and init snake position and Direction ect...
*/
void buildGame(){
    clear();    // clear the screen/ terminal

    // Initialize 
    initscr();  // start curses or init curse
    keypad(stdscr, TRUE);   // let us use arrow keys from keyboard
    timeout(0);     // dont stop. this prevent getchar from waiting for input

    noecho();
    curs_set(0);    // disable cursor, So it does not interfer with gamepaly

    raw();  //disables line buffering and erase/kill character-processing 

    // Initialise Snake 
    currDir =RIGHT;     // Start the snake movement to  RIGHT
    gameOver = false;   // Game is not Over. This is because there is a while loop that's checking
    speed = SPEED;      // Set the initial speed to the defined constant.
    snakeLength = LENGTH;   // Set the initial LENGTH to the defined constant

    // Snake starts at the middle of the screen 
    int x = WIDTH/2;
    int y = HEIGHT/2;

    // Create the snake, This snake is basically a Linked LIST
    struct Snake *currBody;
    currBody = (struct Snake*) malloc(sizeof(struct Snake));
    currBody->next = NULL;
    currBody->x = x - (LENGTH - 1);
    currBody->y = y;

    for (int i = LENGTH - 2; i >= 0; i--) {
        head = (struct Snake*) malloc(sizeof(struct Snake));
        head->next = currBody;
        head->x = x - i;
        head->y = y;
        currBody = head;    
    }
}