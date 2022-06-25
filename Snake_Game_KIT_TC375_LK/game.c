#include <math.h>
#include "game.h"

// LED matrix brightness: between 0(darkest) and 15(brightest) TODO: check this
const short intensity = 8;

// lower = faster message scrolling
const short messageSpeed = 5;

short win = 0;
short gameOver = 0;

struct Point {
    int row;
    int col;
};

struct Coordinate {
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
const short up     = 1;
const short right  = 2;
const short down   = 3; // 'down - 2' must be 'up'
const short left   = 4; // 'left - 2' must be 'right'

// threshold where movement of the joystick will be accepted
const int joystickThreshold = 1000;

// artificial logarithmity (steepness) of the potentiometer (-1 = linear, 1 = natural, bigger = steeper (recommended 0...1))
const float logarithmity = 0.4;

// snake body segments storage
int gameboard[8][8];

//////////////////////////////////////////////
//              UTILS
/////////////////////////////////////////////

// standard map function, but with floats
float mapf(float x, float in_min, float in_max, float out_min, float out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

// calibrate the joystick home for 10 times
void calibrateJoystick() {
    struct Coordinate values = {0, 0};

    for (int i = 0; i < 10; i++) {
        readEVADC();
        int j_x= (int) g_results[1].B.RESULT;
        int j_y= (int) g_results[2].B.RESULT;
        values.x = values.x + j_x;
        values.y = values.y + j_y;
    }

    joystickHome.x = values.x / 10;
    joystickHome.y = values.y / 10;
}

// watches joystick movements & blinks with food
void scanJoystick() {
    int previousDirection = snakeDirection; // save the last direction
    long timestamp = now();

    while (now() < timestamp + snakeSpeed) {
        // calculate snake speed exponentially (10...1000ms)
        //float raw = mapf(analogRead(Pin::potentiometer), 0, 1023, 0, 1); TODO
        float raw = mapf(512, 0, 1023, 0, 1);
        snakeSpeed = mapf(pow(raw, 3.5), 0, 1, 10, 1000); // change the speed exponentially
        if (snakeSpeed == 0) snakeSpeed = 500; // safety: speed can not be 0

        // determine the direction of the snake
        g_results[2].B.RESULT < joystickHome.y - joystickThreshold ? snakeDirection = up    : 0;
        g_results[2].B.RESULT > joystickHome.y + joystickThreshold ? snakeDirection = down  : 0;
        g_results[1].B.RESULT < joystickHome.x - joystickThreshold ? snakeDirection = left  : 0;
        g_results[1].B.RESULT > joystickHome.x + joystickThreshold ? snakeDirection = right : 0;

        // ignore directional change by 180 degrees (no effect for non-moving snake)
        snakeDirection + 2 == previousDirection && previousDirection != 0 ? snakeDirection = previousDirection : 0;
        snakeDirection - 2 == previousDirection && previousDirection != 0 ? snakeDirection = previousDirection : 0;

        // intelligently blink with the food TODO
        //matrix.setLed(0, food.row, food.col, millis() % 100 < 50 ? 1 : 0);
    }
}





void initGame(void) {
    initEVADC();
    init_UART();        // Initialization of the UART communication
    calibrateJoystick(); // calibrate the joystick home
}

void runGame(void) {
    /* Function to initialize the game with default parameters */
    initGame();
    while (1) {
        readEVADC();
        scanJoystick();
    }
}


