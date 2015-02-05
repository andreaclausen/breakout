//
// breakout.c
//
// Computer Science 50
// Problem Set 4
//

// standard libraries
#define _XOPEN_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Stanford Portable Library
#include "gevents.h"
#include "gobjects.h"
#include "gwindow.h"

// height and width of game's window in pixels
#define HEIGHT 600
#define WIDTH 400

// number of rows of bricks
#define ROWS 5

// number of columns of bricks
#define COLS 10

// radius of ball in pixels
#define RADIUS 10

// width and height of paddle
#define P_HEIGHT 10
#define P_WIDTH 60

// lives
#define LIVES 3

// speed
#define SPEED 3.0

// prototypes
void initBricks(GWindow window);
GOval initBall(GWindow window);
GRect initPaddle(GWindow window);
GLabel initScoreboard(GWindow window);
void updateScoreboard(GWindow window, GLabel label, int points);
GObject detectCollision(GWindow window, GOval ball);

int main(void)
{
    // seed pseudorandom number generator
    srand48(time(NULL));

    // instantiate window
    GWindow window = newGWindow(WIDTH, HEIGHT);

    // instantiate bricks
    initBricks(window);

    // instantiate ball, centered in middle of window
    GOval ball = initBall(window);

    // instantiate paddle, centered at bottom of window
    GRect paddle = initPaddle(window);

    // instantiate scoreboard, centered in middle of window, just above ball
    GLabel label = initScoreboard(window);

    // number of bricks initially
    int bricks = COLS * ROWS;

    // number of lives initially
    int lives = LIVES;

    // number of points initially
    int points = 0;
    
    // balls's velocity initially
    double x_velocity = drand48() * 2;
    double y_velocity = SPEED;
    
    waitForClick();
    
    // keep playing until game over
    while (lives > 0 && bricks > 0)
    {
        // move ball
        move(ball, x_velocity, y_velocity);
        
        // check for mouse event
        GEvent event = getNextEvent(MOUSE_EVENT);

        // if we heard one
        if (event != NULL)
        {
            // if the event was movement
            if (getEventType(event) == MOUSE_MOVED)
            {
                // ensure circle follows top cursor
                double x = getX(event) - getWidth(paddle) / 2;
                double y = HEIGHT - (HEIGHT / 10);
                setLocation(paddle, x, y);
            }
        }
        
        // detect collision
        GObject object = detectCollision(window, ball);
        
        if (object != NULL)
        {
            if (strcmp(getType(object), "GRect") == 0)
            {
                y_velocity = -y_velocity;
                if (object != paddle)
                {
                    removeGWindow(window, object);
                    bricks -= 1;
                    points += 1;
                    
                    updateScoreboard(window, label, points);
                }
            }
        }

        // bounce off right edge of window
        if (getX(ball) + getWidth(ball) >= getWidth(window))
        {
            x_velocity = -x_velocity;
        }

        // bounce off left edge of window
        else if (getX(ball) <= 0)
        {
            x_velocity = -x_velocity;
        }
        
        // bounce off top edge of window
        if (getY(ball) <= 0)
        {
            y_velocity = -y_velocity;
        }
        
        // lose a life if ball hits bottom of window
        else if  (getY(ball) + getWidth(ball) >= getHeight(window))
        {
            lives -= 1;
            removeGWindow(window, ball);
            
            // return ball to center of the window and wait for click
            ball = initBall(window);
            x_velocity = 0.0;
            y_velocity = 0.0; 
            waitForClick();
            x_velocity = drand48() * 2;
            y_velocity = SPEED;   
        }
        
        // linger before moving again
        pause(10);
        }
    
    // return ball to center if game is won
    if (bricks == 0)
    {
        removeGWindow(window, ball);       
        ball = initBall(window);
        x_velocity = 0.0;
        y_velocity = 0.0; 
    }
    
    // wait for click before exiting
    waitForClick();

    // game over
    closeGWindow(window);
    return 0;
}
        

/**
 * Initializes window with a grid of bricks.
 */
void initBricks(GWindow window)
{
    // set brick height and width
    int b_width = (WIDTH / COLS) - 4;
    int b_height = 10;
    char* color;
    
    // initialize a 10 x 5 grid of bricks
    for (int i = 0; i < ROWS; i++)
    {
        // make each row a different color
        switch (i)
        {
            case 0 :
                color = "RED";
                break;
            case 1 :
                color = "ORANGE";
                break;
            case 2 :
                color = "YELLOW";
                break;
            case 3 :
                color = "GREEN";
                break;
            case 4 :
                color = "BLUE";
                break;
            default :
                color = "RED";
        }
            
        for (int j = 0; j < COLS; j++)
        {
            
            int x = ((j + 1) * (WIDTH / COLS)) - (WIDTH / COLS) + 2;
            int y = (i * b_height * 1.5) + 40;
            
            GRect brick = newGRect (x, y, b_width, b_height);
            setFilled(brick, true);
            setColor(brick, color);
            add(window, brick);
        }
    }
}

/**
 * Instantiates ball in center of window.  Returns ball.
 */
GOval initBall(GWindow window)
{
    // calculate coordinates to center ball
    int x = (WIDTH / 2) - RADIUS;
    int y = (HEIGHT / 2) - RADIUS;
    
    // instantiate ball
    GOval ball = newGOval (x, y, 2 * RADIUS, 2 * RADIUS);
    setFilled(ball, true);
    setColor(ball, "BLACK");
    add(window, ball);
    return ball;
}

/**
 * Instantiates paddle in bottom-middle of window.
 */
GRect initPaddle(GWindow window)
{
    // calculate coordinates to center paddle
    int x = (WIDTH / 2) - (P_WIDTH / 2);
    int y = HEIGHT - (HEIGHT / 10);
    
    // instantiate paddle
    GRect paddle = newGRect (x, y, P_WIDTH, P_HEIGHT);
    setFilled(paddle, true);
    setColor(paddle, "BLACK");
    add(window, paddle);
    return paddle;
}

/**
 * Instantiates, configures, and returns label for scoreboard.
 */
GLabel initScoreboard(GWindow window)
{
    
    // instantiate label
    GLabel label = newGLabel("0");
    setFont(label, "SansSerif-36");
    add(window, label);
    
    // center label
    double x = (getWidth(window) - getWidth(label)) / 2;
    double y = (getHeight(window) - getHeight(label)) / 2;
    setLocation(label, x, y);
    setColor(label, "MAGENTA");
    return label;
}

/**
 * Updates scoreboard's label, keeping it centered in window.
 */
void updateScoreboard(GWindow window, GLabel label, int points)
{
    // update label
    char s[12];
    sprintf(s, "%i", points);
    setLabel(label, s);

    // center label in window
    double x = (getWidth(window) - getWidth(label)) / 2;
    double y = (getHeight(window) - getHeight(label)) / 2;
    setLocation(label, x, y);
}

/**
 * Detects whether ball has collided with some object in window
 * by checking the four corners of its bounding box (which are
 * outside the ball's GOval, and so the ball can't collide with
 * itself).  Returns object if so, else NULL.
 */
GObject detectCollision(GWindow window, GOval ball)
{
    // ball's location
    double x = getX(ball);
    double y = getY(ball);

    // for checking for collisions
    GObject object;

    // check for collision at ball's top-left corner
    object = getGObjectAt(window, x, y);
    if (object != NULL)
    {
        return object;
    }

    // check for collision at ball's top-right corner
    object = getGObjectAt(window, x + 2 * RADIUS, y);
    if (object != NULL)
    {
        return object;
    }

    // check for collision at ball's bottom-left corner
    object = getGObjectAt(window, x, y + 2 * RADIUS);
    if (object != NULL)
    {
        return object;
    }

    // check for collision at ball's bottom-right corner
    object = getGObjectAt(window, x + 2 * RADIUS, y + 2 * RADIUS);
    if (object != NULL)
    {
        return object;
    }

    // no collision
    return NULL;
}
