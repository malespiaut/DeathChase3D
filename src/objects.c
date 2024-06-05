/***************************************************************************
                 objects.c  -  game objects - bikes,missiles etc.
                             -------------------
    begin                : Sat Sep 14 2002
    copyright            : (C) 2002 by Paul Robson
    email                : autismuk@autismuk.freeserve.co.uk
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "dc.h"

//
//
//          Create basic sprite objects (the bikes), initialise object structure
//
//
void OBJCreate(OBJECT *ob,LEVELDESC *Lvl)
{
    int i;
    for (i = 0;i < SPRCOUNT;i++)            // Erase all objects
                        ob[i].InUse = 0;
    ASSERT(Lvl->EnemyCount <= MAXBIKES);    // Can't ask for too many
    for (i = 0;i < Lvl->EnemyCount;i++)     // Create required no of bikes
    {
        ob[i].Type = GR_CENTRE;             // Centre bike
        ob[i].x = rand()%(MAXX);          // X position
        if (rand()%2==0) ob[i].x *= -1;     // either side.
        ob[i].y = (rand()%40+60)*MAXY /100; // Y position
        ob[i].InUse = 1;                    // Objects in use flag set.
        ob[i].TimeNext = 0;
    }
}

//
//
//                  Sort an object array into furthest-first order.
//
//
void OBJSort(OBJECT *ob)
{
    int i,Unsorted = 1;
    OBJECT tmp;
    for (i = 0;i < SPRCOUNT;i++)            // Chuck unused objects way off sort to the back
        if (ob[i].InUse == 0) ob[i].y = -MAXY*10;

    while (Unsorted)                        // A very basic sort, but it's only a few objects
    {                                       // most of which will be roughly sorted anyway.
        Unsorted = 0;
        for (i = 0;i < SPRCOUNT-1;i++)
            if (ob[i].y < ob[i+1].y)
            {
                tmp = ob[i];ob[i] = ob[i+1];ob[i+1] = tmp;
                Unsorted = 1;
            }
    }
}

//
//
//                              Enemy movement routines
//
//
void OBJMoveEnemy(OBJECT *ob,Uint32 Time,int xPMove,int yPMove,int TMove)
{
        int n;
        ob->y += TMove *                    // Move in y, allowing for player movement
                    ob->Speed / 100 - yPMove;
        if (ob->y > MAXY*11/10)             // Can go off screen, just not very much
                    ob->y = MAXY*11/10;
        n = Glo.MinEnemyPC*MAXY/100;        // This is as near as they can come
        if (ob->y < n) ob->y = n;
        if (Time > ob->TimeNext)            // Rethink movement.
        {
            ob->TimeNext =                  // Time we next move
                    Time+(2000+rand()%5000);
            do                              // Which way
                n = GR_LEFT+rand()%3;
            while (abs(ob->Type-n) != 1);
            ob->Type = n;                
            ob->Speed = Glo.EMinSpeed +     // How fast ?
                        rand()%(Glo.EMaxSpeed-Glo.EMinSpeed);
        }
        n = 0;                              // Direction - depends on type
        if (ob->Type == GR_LEFT) n = -1;
        if (ob->Type == GR_RIGHT) n = 1;
        n = n * Glo.TurningCircle *         // Work out x movement
                    ob->Speed * TMove / 100 / 100;
        ob->x += n - xPMove;                // move in x, allowing for player movement
        if (ob->x < -MAXX)                  // force in limits
                ob->x = -MAXX,ob->Type = GR_RIGHT;;
        if (ob->x >  MAXX)
                ob->x = MAXX,ob->Type = GR_LEFT;
}

//
//
//      Check if missile has hit an enemy
//
//
int  OBJCheckHitEnemy(OBJECT *ob,int x,int y)
{
    int i,n;
    for (i = 0;i < SPRCOUNT;i++)
        if (ob[i].InUse && ob[i].Type >= GR_LEFT && ob[i].Type <= GR_RIGHT)
        {
            if (TREEHasHit(ob[i].x,ob[i].y,x,y))
            {
                ob[i].Type = GR_HIT;
                ob[i].TimeNext = SDL_GetTicks()+1000;
                n = ob[i].y * 100 / MAXY / 5 * 20;
                Glo.Score += n;
                return 1;
            }
        }
    return 0;
}


        