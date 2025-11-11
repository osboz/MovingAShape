#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <time.h>
#include <math.h>

#define WIDTH 800
#define HEIGHT 800
#define TITLE "Balls and their admirers"
#define BALL_COUNT 100
#define FPS 60
#define VEL_MAX 5
#define RADIUS_MAX 20
#define RADIUS_MIN 5

// acceleration multiplier
#define ACCEL_MULT 0.001f

Color COLORS[] = {
    LIGHTGRAY,
    GRAY,
    DARKGRAY,
    YELLOW,
    GOLD,
    ORANGE,
    PINK,
    RED,
    MAROON,
    GREEN,
    LIME,
    DARKGREEN,
    SKYBLUE,
    BLUE,
    DARKBLUE,
    PURPLE,
    VIOLET,
    DARKPURPLE,
    BEIGE,
    BROWN,
    DARKBROWN,
};
// set count correctly
const int ColoursCount = sizeof(COLORS) / sizeof(COLORS[0]);

// Definition of Ball

// YOUR CODE HERE
/**
 * @brief  Ball stores state and other properties
 *
 */
typedef struct Ball
{
    float posx, posy, velx, vely;
    float radius;
    Color colour;
    struct Ball *follows;
    int maxSpeed;
} Ball;

Ball balls[BALL_COUNT];

// Initializes a ball with random values
Ball *init_ball_random(Ball *p, int i)
{
    // Randomly initialize state and properties
    p->posx = rand() % WIDTH;
    p->posy = rand() % HEIGHT;
    p->radius = rand() % RADIUS_MAX + RADIUS_MIN;
    p->colour = COLORS[rand() % ColoursCount];

    // makes the initial velocity a random value between -VEL_MAX and +VEL_MAX
    p->velx = (rand() % (2 * VEL_MAX + 1)) - VEL_MAX;
    p->vely = (rand() % (2 * VEL_MAX + 1)) - VEL_MAX;

    p->maxSpeed = 1 + rand() % VEL_MAX;

    // Find a leading ball other than the initialized ball itself.
    Ball *leader; // Represents the leading ball that this ball will follow

    // YOUR CODE HERE
    // imma do it outside ngl
    return p;
}

/**
 * @brief Initialize all `balls` with random values.
 *
 */
void init_balls_random()
{
    // init all balls with random values
    for (size_t i = 0; i < BALL_COUNT; ++i)
        init_ball_random(&balls[i], i);

    // give each ball a ball to follow
    for (size_t i = 0; i < BALL_COUNT; ++i)
    {
        // since we're instantiation all balls, we can make even an undefined ball to the leader since it will be inistaniteded before we run it.
        // so we can just make a random number and it will be in the range of 0 to BALL_COUNT - 1
        size_t otherBallID = rand() % BALL_COUNT;
        if (otherBallID == i)
            balls[i].follows = NULL; // follow nobody — keep initial velocity
        else
            balls[i].follows = &balls[otherBallID];
    }
}

/**
 * @brief draw ball
 *
 * @param p pointer to ball to draw
 * @return Ball*
 */
Ball *draw_ball(Ball *p)
{
    DrawCircle(p->posx, p->posy, p->radius, p->colour);
    return p;
}

/**
 * @brief Updates the positions of balls according to their velocities
 *
 * @param p pointer to ball to update
 * @return Ball*
 */
Ball *update_pos(Ball *p)
{
    // update position
    p->posx += p->velx;
    p->posy += p->vely;

    // check if outside on x axis
    if (p->posx > WIDTH - p->radius / 2)
    {
        p->posx = WIDTH - p->radius / 2;
        p->velx *= -1;
    }
    if (p->posx < p->radius / 2)
    {
        p->posx = p->radius / 2;
        p->velx *= -1;
    }

    // check if outside on y axis
    if (p->posy > HEIGHT - p->radius / 2)
    {
        p->posy = HEIGHT - p->radius / 2;
        p->vely *= -1;
    }
    if (p->posy < p->radius / 2)
    {
        p->posy = p->radius / 2;
        p->vely *= -1;
    }

    return p;
}

// Updates the velocity of a ball so that it follows the leading ball
Ball *update_vel_for_following(Ball *p)
{
    // just keep own velocity if following no otherr ball
    if (p == NULL || p->follows == NULL)
        return p;

    // calculate error in x and y direction
    float errx = p->follows->posx - p->posx;
    float erry = p->follows->posy - p->posy;

    // scale error by acceleration multiplier
    p->velx += errx * ACCEL_MULT;
    p->vely += erry * ACCEL_MULT;

    // find the movement vector's magnitude
    float speed = sqrt(p->velx * p->velx + p->vely * p->vely);


    // scale the movement vector to max speed
    float scale = (speed > p->maxSpeed) ? (p->maxSpeed / speed) : 1.0f;
    p->velx *= scale;
    p->vely *= scale;

    return p;
}

// Update all elements
void update_elements()
{
    for (size_t i = 0; i < BALL_COUNT; ++i)
    {
        draw_ball(update_pos(update_vel_for_following(&balls[i])));
    }
}

int main()
{
    srand((unsigned)time(NULL));

    InitWindow(WIDTH, HEIGHT, TITLE);
    SetTargetFPS(FPS);

    init_balls_random();

    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        BeginDrawing();
        update_elements();
        ClearBackground(BLACK);
        EndDrawing();
    }
}