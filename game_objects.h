#ifndef GAME_OBJECTS_H_INCLUDED
#define GAME_OBJECTS_H_INCLUDED

enum move_dir {UP, RIGHT, DOWN, LEFT};
bool playermove[4] = {false, false, false, false};
int gravity = 2;
bool isGroundVisible = true;

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
    bool onGround;
    bool canJump;
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
