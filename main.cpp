#include <stdlib.h>
#include <time.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_font.h>
#include <allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/timer.h>
#include "game_objects.h"

//globals===========
const int WIDTH = 800;
const int HEIGHT = 800;
const int LEVELS = 1000;

void InitPlayer(PlayerObj &player);
void DrawPlayer(PlayerObj &player);
void MovePlayer(PlayerObj &player);
void PlayerJump(PlayerObj &player);
void PlayerGravity(PlayerObj &player);
void PlayerCollision(PlayerObj &player, Platform *steps);
void StopGravity(PlayerObj &player);

void InitPlatforms(Platform *steps);
void DrawPlatforms(Platform *steps);

void ScrollUp(PlayerObj &player, Platform *steps);
void ScoreCount(PlayerObj &player, Platform *steps);
void GameEnd(PlayerObj &player, ALLEGRO_FONT *endfont, ALLEGRO_FONT *scorefont);

int main (void)
{
    // primitive vars
    bool done = false;
    bool redraw = true;
    srand(time(0));

    // object vars
    PlayerObj player;
    Platform stairs[LEVELS];

    // Allegro vars
    ALLEGRO_DISPLAY *display = NULL;
    ALLEGRO_EVENT_QUEUE *evQueue = NULL;
    ALLEGRO_TIMER *timer = NULL;
    ALLEGRO_FONT *scorefont = NULL, *endfont = NULL;

    if(!al_init())
    {
        al_show_native_message_box(display, "ALLEGRO ERROR", "Allegro couldn\'t Initialize!", 0,0, ALLEGRO_MESSAGEBOX_ERROR);
        return -1;
    }

    display = al_create_display(WIDTH, HEIGHT);
    if(!display)
    {
        al_show_native_message_box(display, "ALLEGRO ERROR", "Allegro couldn\'t create display!", 0,0, ALLEGRO_MESSAGEBOX_ERROR);
        return -1;
    }

    al_install_keyboard();
    al_init_primitives_addon();
    al_init_font_addon();
    al_init_ttf_addon();

    evQueue = al_create_event_queue();
    timer = al_create_timer(1/60.0);
    scorefont = al_load_ttf_font("res/font.ttf", 16, 0);
    endfont = al_load_ttf_font("res/font.ttf", 72, 0);

    // Game Init
    InitPlayer(player);
    InitPlatforms(stairs);

    al_register_event_source(evQueue, al_get_keyboard_event_source());
    al_register_event_source(evQueue, al_get_timer_event_source(timer));
    al_register_event_source(evQueue, al_get_display_event_source(display));

    al_start_timer(timer);
    while(!done)
    {
        ALLEGRO_EVENT ev;
        al_wait_for_event(evQueue, &ev);
        if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
        {
            done = true;
        }
        else if (ev.type == ALLEGRO_EVENT_TIMER)
        {
            MovePlayer(player);
            if(player.canJump && player.onGround)
                PlayerJump(player);
            redraw=true;
        }
        else if(ev.type==ALLEGRO_EVENT_KEY_DOWN)
        {
            switch(ev.keyboard.keycode)
            {
            case ALLEGRO_KEY_UP:
                playermove[UP]=true;
                break;
            case ALLEGRO_KEY_DOWN:
                playermove[DOWN] = true;
                break;
            case ALLEGRO_KEY_LEFT:
                playermove[LEFT] = true;
                break;
            case ALLEGRO_KEY_RIGHT:
                playermove[RIGHT] = true;
                break;
            case ALLEGRO_KEY_ESCAPE:
                done = true;
                break;
            }
        }
        else if(ev.type==ALLEGRO_EVENT_KEY_UP)
        {
            switch(ev.keyboard.keycode)
            {
            case ALLEGRO_KEY_UP:
                playermove[UP] = false;
                break;
            case ALLEGRO_KEY_DOWN:
                playermove[DOWN] = false;
                break;
            case ALLEGRO_KEY_LEFT:
                playermove[LEFT] = false;
                player.accelX = 0;
                player.speedX = 0;
                break;
            case ALLEGRO_KEY_RIGHT:
                playermove[RIGHT] = false;
                player.accelX = 0;
                player.speedX = 0;
                break;
            }
        }
        if(redraw && al_is_event_queue_empty(evQueue))
        {
            redraw = false;
            PlayerGravity(player);
            PlayerCollision(player, stairs);
            ScrollUp(player, stairs);
            ScoreCount(player, stairs);
            DrawPlatforms(stairs);
            DrawPlayer(player);
            al_draw_textf(scorefont, al_map_rgb(255, 255, 0), 10, 10, 0, "%d", player.score);
            GameEnd(player, endfont, scorefont);
            al_flip_display();
            al_clear_to_color(al_map_rgb(0,0,0));
        }

    }
    al_destroy_display(display);

    return 0;
}

void InitPlayer(PlayerObj &player)
{
    player.x = 0.5*WIDTH;
    player.y = HEIGHT-player.boundY;
    player.accelY = -20;
    player.accelX = 0;
    player.speedY = 0;
    player.boundX = 10;
    player.boundY = 15;
    player.speedX = 0;
    player.onGround = true;
    player.canJump = true;
    player.score = 0;
}

void DrawPlayer(PlayerObj &player)
{
    al_draw_filled_rectangle(player.x-player.boundX, player.y-player.boundY, player.x+player.boundX, player.y+player.boundY, al_map_rgb(255,0,255));
}

void MovePlayer(PlayerObj &player)
{
    if(playermove[RIGHT])
        player.accelX = 1;
    else if(playermove[LEFT])
        player.accelX = -1;
    if(player.x < player.boundX)
    {
        player.x = player.boundX;
        player.accelX = 0;
        player.speedX = 0;
    }
    else if(player.x > WIDTH - player.boundX)
    {
        player.x = WIDTH - player.boundX;
        player.accelX = 0;
        player.speedX = 0;
    }
    if(player.speedX > -15 && player.speedX < 15)
        player.speedX += player.accelX;
    player.x+=player.speedX;
}

void PlayerJump(PlayerObj &player)
{
    player.speedY += player.accelY * playermove[UP];
    if(player.speedY < player.boundY) {
        player.canJump = false;
    }
    player.y += player.speedY;
    player.onGround = false;
}

void PlayerCollision(PlayerObj &player, Platform *steps)
{
    for(int i = 0; i<LEVELS; i++)
    {
        if(player.y >= steps[i].y - steps[i].boundY - player.boundY &&
                player.y <= steps[i].y - steps[i].boundY &&
                player.x + player.boundX > (steps[i].x - steps[i].boundX) &&
                player.x - player.boundX < (steps[i].x + steps[i].boundX) &&
                player.speedY >= 0)
        {
            player.y = steps[i].y - (steps[i].boundY + player.boundY);
            StopGravity(player);
        }
        else if(player.x + player.boundX < (steps[i].x - steps[i].boundX) &&
                player.x - player.boundX > (steps[i].x + steps[i].boundX))
            player.onGround = false;
    }
    if(player.y >= HEIGHT - player.boundY && isGroundVisible)
    {
        player.y = HEIGHT - player.boundY;
        StopGravity(player);
    }
}

void PlayerGravity(PlayerObj &player)
{
    if(!player.onGround)
    {
        player.speedY += gravity;
        if(player.speedY > player.boundY)
            player.speedY = player.boundY;
        player.y += player.speedY;
    }
}

void StopGravity(PlayerObj &player)
{
    player.speedY = 0;
    player.onGround = true;
    player.canJump = true;
}

void InitPlatforms(Platform *steps)
{
    for(int i=0; i<LEVELS; i++)
    {
        steps[i].boundY = 5;
        steps[i].boundX = 100;
        steps[i].x = 200+rand()%(WIDTH-400);
        steps[i].y = HEIGHT-(i+1)*100;
    }
}

void DrawPlatforms(Platform *steps)
{
    for(int i=0; i<LEVELS; i++)
    {
        al_draw_filled_rectangle(steps[i].x-steps[i].boundX, steps[i].y-steps[i].boundY, steps[i].x+steps[i].boundX, steps[i].y+steps[i].boundY, al_map_rgb(0,255,255));
        al_draw_line(steps[i].x-steps[i].boundX, steps[i].y-steps[i].boundY, steps[i].x+steps[i].boundX, steps[i].y-steps[i].boundY, al_map_rgb(255,255,255), 2);
    }
}

void ScrollUp(PlayerObj &player, Platform *steps) {
    if(player.y<HEIGHT*0.1) {
        isGroundVisible = false;
        player.y += player.boundY;
        for(int i =0; i<LEVELS; i++) {
            steps[i].y += player.boundY;
        }
    }
    else if(!isGroundVisible) {
        player.y += 1;
        for(int i =0; i<LEVELS; i++) {
            steps[i].y += 1;
        }
    }
}

void ScoreCount(PlayerObj &player, Platform *steps) {
    for(int i=0; i<LEVELS; i++)
        if(player.y <= steps[i].y && (i+1)*100 > player.score && player.onGround)
            player.score = (i+1) * 100;
}

void GameEnd(PlayerObj &player, ALLEGRO_FONT *endfont, ALLEGRO_FONT *scorefont) {
    if(player.y >= HEIGHT - player.boundY && !isGroundVisible) {
        playermove[UP] = playermove[DOWN] = playermove[LEFT] = playermove[RIGHT] = false;
        al_draw_textf(endfont, al_map_rgb(255, 255, 255), WIDTH/2, HEIGHT/2-60, ALLEGRO_ALIGN_CENTER, "GAME OVER");
        al_draw_textf(scorefont, al_map_rgb(255, 255, 0), WIDTH/2, HEIGHT/2+60, ALLEGRO_ALIGN_CENTER, "Your score: %d", player.score);
    }
}


