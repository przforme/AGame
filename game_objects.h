#ifndef GAME_OBJECTS_H_INCLUDED
#define GAME_OBJECTS_H_INCLUDED

enum move_dir {UP, RIGHT, DOWN, LEFT};
bool playermove[4] = {false, false, false, false};
enum state {IDLE, JUMPS, DIES};
int states[3] = {0,0,0};
int gravity = 2;

struct PlayerObj
{
    signed int accelY;
    int accelX;
    int x;
    int y;
    int speedY;
    int speedX;
    int boundX;
    int boundY;
    int score;
};

struct Platform
{
    int x;
    int y;
    int boundX;
    int boundY;
};

#endif // GAME_OBJECTS_H_INCLUDED
