#include <stdlib.h>
#include <time.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/timer.h>
#include "game_objects.h"

//globals===========
const int WIDTH = 800;
const int HEIGHT = 800;

void InitPlayer(PlayerObj &player);
void DrawPlayer(PlayerObj &player);
void MovePlayer(PlayerObj &player);
void PlayerJump(PlayerObj &player);
void PlayerGravity(PlayerObj &player, Platform* steps, int count);
void StopGravity(PlayerObj &player);

void InitPlatforms(Platform *steps, int count);
void DrawPlatforms(Platform *steps, int count);

int main (void)
{
    // primitive vars
    bool done = false;
    bool redraw = true;
    srand(time(0));

    // object vars
    PlayerObj player;
    Platform stairs[9];

    // Allegro vars
    ALLEGRO_DISPLAY *display = NULL;
    ALLEGRO_EVENT_QUEUE *evQueue = NULL;
    ALLEGRO_TIMER *timer = NULL;

    if(!al_init())
    {
        al_show_native_message_box(display, "ALLEGRO ERROR", "Allegro couldn't Initialize!", 0,0, ALLEGRO_MESSAGEBOX_ERROR);
        return -1;
    }

    display = al_create_display(WIDTH, HEIGHT);
    if(!display)
    {
        al_show_native_message_box(display, "ALLEGRO ERROR", "Allegro couldn't create display!", 0,0, ALLEGRO_MESSAGEBOX_ERROR);
        return -1;
    }

    al_install_keyboard();
    al_init_primitives_addon();

    evQueue = al_create_event_queue();
    timer = al_create_timer(1/60.0);

    // Game Init
    InitPlayer(player);
    InitPlatforms(stairs, 9);

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
            }
        }
        else if(ev.type==ALLEGRO_EVENT_KEY_UP)
        {
            switch(ev.keyboard.keycode)
            {
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
            PlayerGravity(player, stairs, 9);
            DrawPlatforms(stairs, 9);
            DrawPlayer(player);
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
    player.accelY = -5;
    player.accelX = 0;
    player.speedY = 0;
    player.boundX = 10;
    player.boundY = 10;
    player.speedX = 0;
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
    else if(player.x > WIDTH-player.boundX)
    {
        player.x=WIDTH-player.boundX;
        player.accelX=0;
        player.speedX = 0;
    }
    if(player.speedX > -15 && player.speedX < 15)
        player.speedX += player.accelX;
    player.x+=player.speedX;
}

void PlayerJump(PlayerObj &player)
{
    if(states[JUMPS] > -50)
    {
        player.speedY += player.accelY;
        player.y += player.speedY*playermove[UP];
        states[JUMPS] += player.speedY;
    }
}

void PlayerGravity(PlayerObj &player, Platform *steps, int count)
{
    if(player.y < HEIGHT-player.boundY-2)
    {
        player.speedY += gravity;
        player.y += player.speedY;
        for(int i =0; i<count; i++)
        {
            if((player.y+player.boundY)>(steps[i].y-steps[i].boundY-10) && (player.y+player.boundY)<=(steps[i].y-steps[i].boundY) && player.x>(steps[i].x-steps[i].boundX) && player.x<(steps[i].x+steps[i].boundX))
                StopGravity(player);
        }
    }
    else
    {
        player.y = HEIGHT-player.boundY;
        StopGravity(player);
    }
}

void StopGravity(PlayerObj &player)
{
    player.speedY = 0;
    states[JUMPS] = 0;
    playermove[UP] = false;
}

void InitPlatforms(Platform *steps, int count)
{
    for(int i=0; i<count; i++)
    {
        steps[i].boundY = 5;
        steps[i].boundX = 100;
        steps[i].x = 100+rand()%(WIDTH-200);
        steps[i].y = HEIGHT-(i+1)*100;
    }
}

void DrawPlatforms(Platform *steps, int count)
{
    for(int i=0; i<count; i++)
    {
        al_draw_filled_rectangle(steps[i].x-steps[i].boundX, steps[i].y-steps[i].boundY, steps[i].x+steps[i].boundX, steps[i].y+steps[i].boundY, al_map_rgb(0,255,255));
        al_draw_line(steps[i].x-steps[i].boundX, steps[i].y-steps[i].boundY, steps[i].x+steps[i].boundX, steps[i].y-steps[i].boundY, al_map_rgb(255,255,255), 2);
    }
}
