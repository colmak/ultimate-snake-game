#include <curses.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define BORDER 1
#define BORDER_SYMBOL "#"
#define SNAKE_INITIAL_LENGTH 5
#define SNAKE_BODY_SYMBOL "o"
#define SNAKE_HEAD_SYMBOL "O"
#define DIRECTION_UP 0
#define DIRECTION_RIGHT 1
#define DIRECTION_DOWN 2
#define DIRECTION_LEFT 3
#define TROPHY_SYMBOL "x"

typedef struct
{
    int x;
    int y;
} point;

typedef struct
{
    point head;
    point trail[100];
    int direction;
    int length;
} snake_t;

typedef struct
{
    point location;
    int value;
    int timer;
} trophy_t;

snake_t snake;
trophy_t trophy;

const int dx[] = {0, 1, 0, -1};
const int dy[] = {-1, 0, 1, 0};

void spawn_trophy()
{
    trophy.location.x = rand() % (COLS - 2 * BORDER - 1) + BORDER + 1;
    trophy.location.y = rand() % (LINES - 2 * BORDER - 1) + BORDER + 1;
    trophy.value = rand() % 9 + 1;
    trophy.timer = trophy.value;
}

void draw_trophy()
{
    mvprintw(trophy.location.y, trophy.location.x, TROPHY_SYMBOL);
}

void check_trophy_collision()
{
    if (snake.head.x == trophy.location.x && snake.head.y == trophy.location.y)
    {
        snake.length += trophy.value;
        spawn_trophy();
    }
}

void update_score()
{
    mvprintw(0, 0, "Score: %d", snake.length);
}

void check_collisions()
{
    // Check border collision
    if (snake.head.x <= BORDER || snake.head.x >= COLS - BORDER - 1 ||
        snake.head.y <= BORDER || snake.head.y >= LINES - BORDER - 1)
    {
        endwin();
        exit(0);
    }

    // Check self-collision
    for (int i = 1; i < snake.length; i++)
    {
        if (snake.head.x == snake.trail[i].x && snake.head.y == snake.trail[i].y)
        {
            endwin();
            exit(0);
        }
    }
}

void init_game()
{
    // Initialize ncurses
    initscr();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);
    curs_set(0);

    // Initialize random seed
    srand(time(NULL));

    // Initialize snake
    snake.head.x = rand() % (COLS - 2 * BORDER - 1) + BORDER + 1;
    snake.head.y = rand() % (LINES - 2 * BORDER - 1) + BORDER + 1;
    snake.direction = rand() % 4;
    snake.length = SNAKE_INITIAL_LENGTH;
    for (int i = 0; i < snake.length; i++)
    {
        snake.trail[i].x = snake.head.x - i * dx[snake.direction];
        snake.trail[i].y = snake.head.y - i * dy[snake.direction];
    }

    // Initialize trophy
    spawn_trophy();

    // Draw the border
    for (int x = BORDER; x < COLS - BORDER; x++)
    {
        mvprintw(BORDER, x, BORDER_SYMBOL);
        mvprintw(LINES - BORDER - 1, x, BORDER_SYMBOL);

        for (int y = BORDER + 1; y < LINES - BORDER - 1; y++)
        {
            mvprintw(y, BORDER, BORDER_SYMBOL);
            mvprintw(y, COLS - BORDER - 1, BORDER_SYMBOL);
        }

        // Draw the snake
        for (int i = 0; i < snake.length; i++)
        {
            if (i == 0)
            {
                mvprintw(snake.head.y, snake.head.x, SNAKE_HEAD_SYMBOL);
            }
            else
            {
                mvprintw(snake.trail[i].y, snake.trail[i].x, SNAKE_BODY_SYMBOL);
            }
        }

        // Draw the trophy
        draw_trophy();
    }

    void move_snake()
    {
        // Move the tail
        for (int i = snake.length - 1; i >= 1; i--)
        {
            snake.trail[i] = snake.trail[i - 1];
        }
        snake.trail[0] = snake.head;
        // Move the head
        snake.head.x += dx[snake.direction];
        snake.head.y += dy[snake.direction];

        // Wrap around the screen
        if (snake.head.x < BORDER + 1)
        {
            snake.head.x = COLS - BORDER - 2;
        }
        else if (snake.head.x >= COLS - BORDER - 1)
        {
            snake.head.x = BORDER + 1;
        }
        if (snake.head.y < BORDER + 1)
        {
            snake.head.y = LINES - BORDER - 2;
        }
        else if (snake.head.y >= LINES - BORDER - 1)
        {
            snake.head.y = BORDER + 1;
        }
    }

    void handle_input(int ch)
    {
        switch (ch)
        {
        case KEY_UP:
            if (snake.direction != DIRECTION_DOWN)
            {
                snake.direction = DIRECTION_UP;
            }
            break;
        case KEY_RIGHT:
            if (snake.direction != DIRECTION_LEFT)
            {
                snake.direction = DIRECTION_RIGHT;
            }
            break;
        case KEY_DOWN:
            if (snake.direction != DIRECTION_UP)
            {
                snake.direction = DIRECTION_DOWN;
            }
            break;
        case KEY_LEFT:
            if (snake.direction != DIRECTION_RIGHT)
            {
                snake.direction = DIRECTION_LEFT;
            }
            break;
        case 'q':
        case 'Q':
            endwin();
            exit(0);
            break;
        }
    }

    int main()
    {
        // Initialize the game
        init_game();
        // Game loop
        while (1)
        {
            // Check for input
            int ch = getch();
            handle_input(ch);

            // Update the snake
            move_snake();
            check_collisions();
            check_trophy_collision();

            // Draw the game
            clear();
            draw_trophy();
            update_score();
            for (int x = BORDER; x < COLS - BORDER; x++)
            {
                mvprintw(BORDER, x, BORDER_SYMBOL);
                mvprintw(LINES - BORDER - 1, x, BORDER_SYMBOL);
            }
            for (int y = BORDER + 1; y < LINES - BORDER - 1; y++)
            {
                mvprintw(y, BORDER, BORDER_SYMBOL);
                mvprintw(y, COLS - BORDER - 1, BORDER_SYMBOL);
            }
            for (int i = 0; i < snake.length; i++)
            {
                if (i == 0)
                {
                    mvprintw(snake.head.y, snake.head.x, SNAKE_HEAD_SYMBOL);
                }
                else
                {
                    mvprintw(snake.trail[i].y, snake.trail[i].x, SNAKE_BODY_SYMBOL);
                }
            }

            // Refresh the
            refresh();

            // Sleep to control the speed of the snake
            int sleep_time = 1000000 / (SNAKE_BASE_SPEED + snake.length * SNAKE_SPEED_INCREMENT);
            usleep(sleep_time);
        }

        return 0;
    }