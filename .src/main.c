#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <time.h>
#include <math.h>

#define WIDTH 800
#define HEIGHT 800
#define TITLE "Balls and their admirers"
#define BALL_COUNT 50
#define FPS 60
#define VEL_MAX 10
#define RADIUS_MAX 20
#define RADIUS_MIN 5

// acceleration multiplier
#define ACCEL_MULT 0.000001f
#define ACCEL_MAX 100
#define ACCEL_MIN 1

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
    float posx, posy, velx, vely, accelPower;
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
    p->accelPower = (float)((rand() % ACCEL_MAX) + ACCEL_MIN);

    // makes the initial velocity a random value between -VEL_MAX and +VEL_MAX
    p->velx = ((rand() % (2 * VEL_MAX + 1)) - VEL_MAX) / VEL_MAX;
    p->vely = ((rand() % (2 * VEL_MAX + 1)) - VEL_MAX) / VEL_MAX;

    p->maxSpeed = 1 + rand() % VEL_MAX;
    // p->maxSpeed = VEL_MAX;

    // Find a leading ball other than the initialized ball itself.
    Ball *leader; // Represents the leading ball that this ball will follow

    // YOUR CODE HERE
    // imma do it outside ngl
    return p;
}

void SetNewleader(Ball *p)
{
    size_t otherBallID = rand() % BALL_COUNT;
    if (&balls[otherBallID] == p)
    { // if the leader is itself
        p->velx = rand() % VEL_MAX;
        p->vely = rand() % VEL_MAX;
        p->follows = NULL; // follow nobody — keep initial velocity
    }
    else
        p->follows = &balls[otherBallID];
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
        SetNewleader(&balls[i]);
    }
}

/**
 * @brief draw ball
 *
 * @param p pointer to ball to draw
 * @return Ball*
 */
void *draw_ball(Ball *p)
{
    DrawCircle(p->posx, p->posy, p->radius, p->colour);
    return p;
}

void DrawTargetLine(Ball *p)
{
    if (p->follows != NULL)
    {
        // direction vector from p to follower
        float dx = p->follows->posx - p->posx;
        float dy = p->follows->posy - p->posy;
        // normalize
        float len = sqrtf(dx * dx + dy * dy);
        if (len > 0.0001f)
        {
            dx /= len;
            dy /= len;
            // fixed length you want (change 30.0f)
            float fixedLen = 20.0f;
            float x2 = p->posx + dx * fixedLen;
            float y2 = p->posy + dy * fixedLen;
            DrawLine(p->posx, p->posy, x2, y2, p->follows->colour);
        }
        else
        {
            // follower exactly on position: draw nothing or a short default line
            DrawLineV((Vector2){p->posx, p->posy}, (Vector2){p->posx + 5, p->posy}, p->follows->colour);
        }
    }
}

/**
 * @brief Updates the positions of balls according to their velocities
 *
 * @param p pointer to ball to update
 * @return Ball*
 */
void *update_pos(Ball *p)
{
    // update position
    p->posx += p->velx;
    p->posy += p->vely;

    // check if outside on x axis
    if (p->posx > WIDTH - p->radius)
    {
        p->posx = WIDTH - p->radius;
        p->velx *= -1;
    }
    if (p->posx < p->radius)
    {
        p->posx = p->radius;
        p->velx *= -1;
    }

    // check if outside on y axis
    if (p->posy > HEIGHT - p->radius)
    {
        p->posy = HEIGHT - p->radius;
        p->vely *= -1;
    }
    if (p->posy < p->radius)
    {
        p->posy = p->radius;
        p->vely *= -1;
    }

    return p;
}

/**
 * @brief retruns the distance between 2 balls
 * @note uses sqrtf and thusly math.h
 * @headerfile math.h
 * @param _p1 ball 1
 * @param _p2 ball 2
 * @return float
 */
float DistanceBetween(Ball *_p1, Ball *_p2)
{
    float disX = _p2->posx - _p1->posx;
    float disY = _p2->posy - _p1->posy;
    return sqrtf(disX * disX + disY * disY);
}

/**
 * @brief Determins if the ball has reached its target, and then gives it a new target.
 *
 * @param p Ball to check
 */
void ReachedTarget(Ball *p)
{
    if (p == NULL || p->follows == NULL)
        return;

    Ball *t = p->follows;
    float distance = DistanceBetween(p, t);

    // float threshold = (p->radius * 0.5f) + (t->radius * 0.5f);
    float threshold = (p->radius) + (t->radius);

    if (distance <= threshold)
    {
        SetNewleader(p);
    }
}

// Updates the velocity of a ball so that it follows the leading ball
void *update_vel_for_following(Ball *p)
{
    // just keep own velocity if following no otherr ball
    if (p == NULL || p->follows == NULL)
        return p;

    // calculate error in x and y direction
    float errx = p->follows->posx - p->posx;
    float erry = p->follows->posy - p->posy;

    // scale error by acceleration multiplier
    p->velx += errx * p->accelPower * ACCEL_MULT;
    p->vely += erry * p->accelPower * ACCEL_MULT;

    // find the movement vector's magnitude
    float speed = sqrt(p->velx * p->velx + p->vely * p->vely);

    // scale the movement vector to max speed
    float scale = (speed > p->maxSpeed) ? (p->maxSpeed / speed) : 1.0f;
    p->velx *= scale;
    p->vely *= scale;

    return p;
}

/**
 * @brief add bouncing / collision to all balls
 *
 * @param _p
 */
void BallBounce(Ball *_p)
{
    if (_p == NULL)
        return;

    for (size_t j = 0; j < BALL_COUNT; ++j)
    {
        Ball *b = &balls[j];
        if (b == _p)
            continue;

        float dx = b->posx - _p->posx;
        float dy = b->posy - _p->posy;
        float dist = sqrtf(dx * dx + dy * dy);
        float minDist = (_p->radius) + (b->radius);

        if (dist <= 0.0f)
        {
            // avoid division by zero: nudge slightly
            dx = ((rand() % 100) / 100.0f) + 0.01f;
            dy = ((rand() % 100) / 100.0f) + 0.01f;
            dist = sqrtf(dx * dx + dy * dy);
        }

        if (dist > minDist)
            continue;

        // Normalize collision normal
        float nx = dx / dist;
        float ny = dy / dist;

        // Push balls apart to resolve overlap (half to each)
        float overlap = 0.5f * (minDist - dist);
        _p->posx -= nx * overlap;
        _p->posy -= ny * overlap;
        b->posx += nx * overlap;
        b->posy += ny * overlap;

        // simple mass ~ area (radius^2)
        // float massA = _p->radius * _p->radius;
        // float massB = b->radius * b->radius;
        float massA = 0.01f;
        float massB = 0.01f;

        // relative velocity
        float rvx = b->velx - _p->velx;
        float rvy = b->vely - _p->vely;

        // velocity along normal
        float velAlongNormal = rvx * nx + rvy * ny;
        if (velAlongNormal > 0)
            continue; // already separating

        // restitution (1.0 = elastic)
        float e = 1.0f;

        // impulse scalar
        float j = -(1.0f + e) * velAlongNormal;
        j /= (1.0f / massA) + (1.0f / massB);

        // apply impulse
        float impulseX = j * nx;
        float impulseY = j * ny;

        _p->velx -= impulseX / massA;
        _p->vely -= impulseY / massA;
        b->velx += impulseX / massB;
        b->vely += impulseY / massB;
    }
}

// Update all elements
void update_elements()
{
    // Update positions, velocities, targets and draw, and then resolve collisions per ball
    for (size_t i = 0; i < BALL_COUNT; ++i)
    {
        update_pos(&balls[i]);
        update_vel_for_following(&balls[i]);
        ReachedTarget(&balls[i]);
    }

    // collisions for ball
    for (size_t i = 0; i < BALL_COUNT; ++i)
    {
        BallBounce(&balls[i]);
    }

    
    // Draw after physics updates
    for (size_t i = 0; i < BALL_COUNT; ++i)
    {
        draw_ball(&balls[i]);
    }
    
    for (size_t i = 0; i < BALL_COUNT; ++i)
    {
    DrawTargetLine(&balls[i]);
    }

    // draw velocity line
    for (size_t i = 0; i < BALL_COUNT; ++i)
    {
        DrawLine(balls[i].posx, balls[i].posy, balls[i].posx + balls[i].velx * 20, balls[i].posy + balls[i].vely * 20, GREEN);
    }
    // // draw velocity line
    // for (size_t i = 0; i < BALL_COUNT; ++i)
    // {
    //     DrawLine(balls[i].posx, balls[i].posy, balls[i].posx + balls[i].velx * 10, balls[i].posy + balls[i].vely * 10, GREEN);
    // }
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