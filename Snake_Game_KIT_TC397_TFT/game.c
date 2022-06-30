#include <math.h>
#include <string.h>
#include <stdlib.h>
#include "game.h"

// initial snake length (1...63, recommended 3)
const short initialSnakeLength = 3;

short win = 0;
short gameOver = 0;

//DEBUG FLAG: 1=active debug
short DEBUG = 0;
//counter for dumping gameboard (for DEBUG purposes, don't touch)
int dump = 0;

struct Point
{
    int row;
    int col;
};

struct Coordinate
{
    int x;
    int y;
};

struct Coordinate joystickHome = {2100, 2100};

// primary snake head coordinates (snake head), it will be randomly generated
struct Point snake;

// food is not anywhere yet
struct Point food = {-1, -1};

// snake parameters
// initial snake length (1...63, recommended 3)
int snakeLength = 3; // choosed by the user in the config section
int snakeSpeed = 500; // will be set according to potentiometer value, cannot be 0
short snakeDirection = 0; // if it is 0, the snake does not move

// direction constants
const short up = 1;
const short right = 2;
const short down = 3; // 'down - 2' must be 'up'
const short left = 4; // 'left - 2' must be 'right'

// threshold where movement of the joystick will be accepted
const int joystickThreshold = 600;

// snake body segments storage
int gameboard[8][8];

//////////////////////////////////////////////
//              UTILS
/////////////////////////////////////////////

// calibrate the joystick home for 10 times
void calibrateJoystick ()
{
    struct Coordinate values = {0, 0};

    for (int i = 0; i < 10; i++)
    {
        readEVADC();
        int j_x = (int) g_results[1].B.RESULT; //AN21
        int j_y = (int) g_results[2].B.RESULT; //AN20
        values.x = values.x + j_x;
        values.y = values.y + j_y;
    }

    joystickHome.x = values.x / 10;
    joystickHome.y = values.y / 10;
}

// watches joystick movements & blinks with food
void scanJoystick ()
{
    short previousDirection = snakeDirection; // save the last direction
    sint64 timestamp = now();
    readEVADC();

    while (now() < timestamp + 15000000) //this is bugged, either because the board is low or because of the debug settings: if snakespeed is low, the cycle is never executed
    {
        // determine the direction of the snake
        g_results[2].B.RESULT < joystickHome.y - joystickThreshold ? snakeDirection = up : 0;
        g_results[2].B.RESULT > joystickHome.y + joystickThreshold ? snakeDirection = down : 0;
        g_results[1].B.RESULT < joystickHome.x - joystickThreshold ? snakeDirection = left : 0;
        g_results[1].B.RESULT > joystickHome.x + joystickThreshold ? snakeDirection = right : 0;

        // ignore directional change by 180 degrees (no effect for non-moving snake)
        snakeDirection + 2 == previousDirection && previousDirection != 0 ? snakeDirection = previousDirection : 0;
        snakeDirection - 2 == previousDirection && previousDirection != 0 ? snakeDirection = previousDirection : 0;

        // intelligently blink with the food
        setLed(food.row, food.col, now() % 100 < 50 ? 1 : 0);
        //setLed(food.row, food.col, 1);

    }
}

// if there is no food, generate one, also check for victory
void generateFood ()
{
    if (food.row == -1 || food.col == -1)
    {
        // self-explanatory
        if (snakeLength >= 64)
        {
            win = 1;
            return; // prevent the food generator from running, in this case it would run forever, because it will not be able to find a pixel without a snake
        }

        // generate food until it is in the right position
        do
        {
            food.col = rand() % 8;
            food.row = rand() % 8;
        }while (gameboard[food.row][food.col] > 0);
    }
}

// causes the snake to appear on the other side of the screen if it gets out of the edge
void fixEdge ()
{
    snake.col < 0 ? snake.col += 8 : 0;
    snake.col > 7 ? snake.col -= 8 : 0;
    snake.row < 0 ? snake.row += 8 : 0;
    snake.row > 7 ? snake.row -= 8 : 0;
}

// calculate snake movement data
void calculateSnake ()
{
    switch (snakeDirection)
    {
        case 1 : // up
            snake.row--;
            fixEdge();
            setLed(snake.row, snake.col, 1);
            break;

        case 2 : // right
            snake.col++;
            fixEdge();
            setLed(snake.row, snake.col, 1);
            break;

        case 3 :  // down
            snake.row++;
            fixEdge();
            setLed(snake.row, snake.col, 1);
            break;

        case 4 :  //  left
            snake.col--;
            fixEdge();
            setLed(snake.row, snake.col, 1);
            break;

        default : // if the snake is not moving, exit
            return;
    }

    // if there is a snake body segment, this will cause the end of the game (snake must be moving)
    if (gameboard[snake.row][snake.col] > 1 && snakeDirection != 0)
    {
        gameOver = 1;
        return;
    }

    // check if the food was eaten
    if (snake.row == food.row && snake.col == food.col)
    {
        food.row = -1; // reset food
        food.col = -1;

        // increment snake length
        snakeLength++;

        // increment all the snake body segments
        for (int row = 0; row < 8; row++)
        {
            for (int col = 0; col < 8; col++)
            {
                if (gameboard[row][col] > 0)
                {
                    gameboard[row][col]++;
                }
            }
        }
    }

    // add new segment at the snake head location
    gameboard[snake.row][snake.col] = snakeLength + 1; // will be decremented in a moment

    // decrement all the snake body segments, if segment is 0, turn the corresponding led off
    for (int row = 0; row < 8; row++)
    {
        for (int col = 0; col < 8; col++)
        {
            // if there is a body segment, decrement it's value
            if (gameboard[row][col] > 0)
            {
                gameboard[row][col]--;
            }

            // display the current pixel
            setLed(row, col, gameboard[row][col] == 0 ? 0 : 1);
        }
    }
}

void unrollSnake ()
{
    // flash the screen 5 times
    for (int i = 0; i < 5; i++)
    {
        // invert the screen
        test(1);
        wait(IfxStm_getTicksFromMilliseconds(&MODULE_STM0, 150));
        test(0);
        wait(IfxStm_getTicksFromMilliseconds(&MODULE_STM0, 150));
    }
}

//handle gameover and win states: if gameOver || win restarts the game
void handleGameStates ()
{
    if (gameOver || win)
    {
        unrollSnake();

        // re-init the game
        win = 0;
        gameOver = 0;
        snake.row = rand() % 8;
        snake.col = rand() % 8;
        food.row = -1;
        food.col = -1;
        snakeLength = initialSnakeLength;
        snakeDirection = 0;
        // set all gameboard cells at 0
        memset(gameboard, 0, sizeof(gameboard[0][0]) * 8 * 8);
        setup();
    }
}

void append (int *actualcharposition, char *buffer, const char *str, int length)
{
    for (int a = 0; a < length; a++)
    {
        int i = *actualcharposition;
        buffer[i] = str[a];
        (*actualcharposition)++;
    }
}

//this function prints the gameboard to console for debug purpose
void dumpGameBoard ()
{
    // change dumping frequency
    dump == 5000 ? dump = 0 : ++dump;
    if (!dump)
    {
        int counter = 0;
        char dumped[128] = "";
        append(&counter, dumped, "\n\n\n", 3);
        //char* buff = "\n\n\n";
        for (int row = 0; row < 8; row++)
        {
            for (int col = 0; col < 8; col++)
            {
                if (gameboard[row][col] < 10)
                    append(&counter, dumped, " ", 1);
                char c = gameboard[row][col] + '0';
                if (gameboard[row][col] > 0)
                    append(&counter, dumped, &c, 1);
                else if (col == food.col && row == food.row)
                    append(&counter, dumped, "@", 1);
                else
                    append(&counter, dumped, "-", 1);
                append(&counter, dumped, " ", 1);
            }
            append(&counter, dumped, "\r\n", 2);
        }
        IfxStdIf_DPipe_print(&g_stdInterface, dumped);
    }
}

void initGame (void)
{
    /* Initialize the QSPI modules and the LED */
    initPeripherals();
    setup();
    initEVADC();
    init_UART();        // Initialization of the UART communication
    calibrateJoystick(); // calibrate the joystick home
    printMessage();
}

void runGame (void)
{
    /* Function to initialize the game with default parameters */
    initGame();
    if (DEBUG == 1)
    {
        while (1)
        {
            generateFood();
            scanJoystick();
            calculateSnake();  // calculates snake parameters
            handleGameStates();
            dumpGameBoard(); // executed only in case of active debug
        }
    }
    else
        while (1)
        {
            generateFood();
            scanJoystick();
            calculateSnake();  // calculates snake parameters
            handleGameStates();
        }
}

