/***************************************************************************
                          level.c  -  run a level
                             -------------------
    begin                : Sun Sep 8 2002
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
//                                          Do one level
//
//
int LEVELRun(LEVELDESC *Lvl)
{
    int Quit = 0,Escaped = 0;
    int xMove,yMove,MaxSpeed,Speed,x,y,yScore,n,MissileSpr;
    Uint32 Time,TAccel;
    char Score[7];
    SDL_Rect rc;
    TREELIST Trees;
    OBJECT Objects[SPRCOUNT];
    char *Msg;
    
    TREEInitialise(&Trees,Lvl->TreeCount);  // Set up the trees
    OBJCreate(Objects,Lvl);                 // Set up the on screen objects

    MaxSpeed = MAXY*3/4*Glo.ScaleSpeed/100; // Units per second speed
    Speed = 0;
    yScore = 0;                             // The movement score
    TAccel = 0;

    if (Lvl->IsNight)                       // Display the "Night Patrol" message at night
    {
            DRAWLandscape(Lvl->IsNight,100);
            TREEDrawAll(&Trees,0,MAXY);
            Msg = "Night Patrol";
            for (n = 0;n < strlen(Msg);n++)
            {
                rc.w = Glo.Screen->w/20;
                rc.h = Glo.Screen->h/12;
                rc.y = Glo.Screen->h/2-rc.h/2;
                rc.x = Glo.Screen->w/2-strlen(Msg)*rc.w/2+n*rc.w;
                FONTChar(Glo.Screen,&rc,SDL_MapRGB(Glo.Screen->format,255,255,0),Msg[n]);
            }
            SDL_UpdateRect(Glo.Screen,0,0,     
                        Glo.Screen->w,Glo.Screen->h);
            Time = SDL_GetTicks()+1500;
            while (SDL_GetTicks() < Time) {};
    }                
    while (Quit == 0)                       // While level neither abandoned nor completed
    {
        Time = SDL_GetTicks();              // Read the timer
        OBJSort(Objects);                   // Sort the displayed objects into furthest first order
        DRAWLandscape(Lvl->IsNight,100);    // Draw the landscape

        n = 0;y = MAXY;                     // Start with furthest object
        do
        {
            TREEDrawAll(                    // Draw up to that object - or the nearest if it exists
                            &Trees,Objects[n].y,y);// there will be at least one -MAXY unused object post sort
            y = Objects[n].y;               // position of object - where drawn to in trees
            if (Objects[n].InUse) DRAWObject(&(Objects[n]));
            n++;                            // Next one
        } while (y >= 0);                   // While more to do.


        sprintf(Score,"%06d",               // Convert score to text
                    Glo.Score % 1000000);
        for (n = 0;n < 6;n++)               // Write score to screen
        {
            rc.x = 10+Glo.DigitRect[0].w*n;
            rc.y = Glo.Screen->h-10-Glo.DigitRect[0].h;
            SDL_BlitSurface(Glo.Sprites,&(Glo.DigitRect[Score[n]-'0']),Glo.Drawing,&rc);
        }

        n = IOController();                 // Read controller
        if (n & KEYEXIT) Quit = Escaped = 1;// Exit key/button pressed

        x = 0;                              // Direction of movement
        if (n & KEYLEFT) x = -1;
        if (n & KEYRIGHT) x = 1;

        if (Time > TAccel)                  // Time to change speed
        {
            if (n & KEYUP) Speed+=10;       // Do so.
            if (n & KEYDOWN) Speed-=10;
            if (Speed < 0) Speed = 0;       // Force into limits
            if (Speed > 100) Speed = 100;
            TAccel = Time+Glo.AccelTime/10; // Time for next step
        }

                                            // Draw the player cycle at bottom centre
        rc.x = Glo.Screen->w/2-Glo.PlayerRect[0].w/2;
        rc.y = Glo.Screen->h - Glo.PlayerRect[0].h*70/100;
        SDL_BlitSurface(Glo.Sprites,&(Glo.PlayerRect[x+1]),Glo.Screen,&rc);

                                            // Uncomment this to check graphics
//        rc.x = rc.y = 0;rc.w = Glo.Screen->w;rc.h = Glo.Screen->h;
//        SDL_BlitSurface(Glo.Sprites,&rc,Glo.Screen,&rc);

        SDL_UpdateRect(Glo.Screen,0,0,      // Update the display
                    Glo.Screen->w,Glo.Screen->h);


        while (SDL_GetTicks() < Time+10) {};// Limit frame speed to 100 fps
        Time = SDL_GetTicks() - Time;       // ms elapsed this frame

        y = Speed * MaxSpeed / 100;         // Current speed in units per second
        yMove = y = y * Time / 1000;        // Amount to move to maintain speed
        xMove = x *y * Glo.TurningCircle/100;
        if (TREEMoveAll(&Trees,xMove,yMove))// Move, check for collision
        {
            Quit = 1;                       // Level over
            y = 100;                        // Used for brightness :)
            while (y > 0)                   // Fade to black - then this function terminates
            {
                Time = SDL_GetTicks()+50;
                DRAWLandscape(Lvl->IsNight,y);
                TREEDrawAll(&Trees,0,MAXY);
                SDL_UpdateRect(Glo.Screen,0,0,      
                            Glo.Screen->w,Glo.Screen->h);
                while (SDL_GetTicks() < Time) {};
                y = y - 6;
            }
        }
        yScore = yScore + yMove;            // This gives low scores for simply moving
        if (yScore > MAXY/4)                // through the forest
                yScore = 0,Glo.Score++;
        Lvl->EnemyCount = 0;
        MissileSpr = -1;

        for (n = 0;n < SPRCOUNT;n++)        // Work through the objects
            if (Objects[n].InUse)
            {
                if (Objects[n].Type >= GR_LEFT && Objects[n].Type <= GR_RIGHT)
                {
                    Lvl->EnemyCount++;
                    OBJMoveEnemy(&(Objects[n]),SDL_GetTicks(),xMove,yMove,MaxSpeed*Time/1000);
                }
                if (Objects[n].Type == GR_MISSILE)
                    MissileSpr = n;
                if (Objects[n].Type == GR_HIT)
                {
                    Lvl->EnemyCount++;
                    if (SDL_GetTicks() > Objects[n].TimeNext)
                        Objects[n].InUse = 0;
                }
            }
        if (Lvl->EnemyCount == 0)           // No enemies left
                            Quit = 1;       // finished level, and completed it.

        if (TREECheckMissileHit             // Have wMissileHtree ?
                    (Objects[MissileSpr].x,Objects[MissileSpr].y,&Trees))
        {
            Objects[MissileSpr].InUse = 0;  // If so , remove missile
            MissileSpr = -1;                // and there is no current missile
        }

        if (MissileSpr >= 0)                // Missile still in progress ?
        {
            if (OBJCheckHitEnemy(Objects,   // If hit one of the bad guys, cancel missile
                                 Objects[MissileSpr].x,Objects[MissileSpr].y))
            {
                Objects[MissileSpr].InUse = 0;
                MissileSpr = -1;
            }
        }
        if (MissileSpr >= 0)                // Is there a missile
        {
            y = Time*Glo.MissileSpeed*      // Movement into screen
                                MaxSpeed/100/100;
              Objects[MissileSpr].y += y-yMove;
            Objects[MissileSpr].x +=        // Movement across screen
                    Objects[MissileSpr].Dir*y*Glo.TurningCircle/100/2-xMove;
            if (Objects[MissileSpr].y >= MAXY) Objects[MissileSpr].InUse = 0;
        }
        else                                // No missile
        {
            x = IOController();             // Check for fire
            if ((x & KEYFIRE) != 0 &&       // Last sprite empty as they're sorted
                        Objects[SPRCOUNT-1].InUse == 0)
            {
                n = SPRCOUNT-1;             // Convenience - then set it up
                Objects[n].Type = GR_MISSILE;
                Objects[n].x = Objects[n].y = 0;
                Objects[n].MarkDel = 0;Objects[n].InUse = 1;
                Objects[n].Dir = 0;         // Which way to the missile ?
                if (x & KEYLEFT) Objects[n].Dir = -1;
                if (x & KEYRIGHT) Objects[n].Dir = 1;
            }
        }
    }
    if (Glo.Score > Glo.HighScore) Glo.HighScore = Glo.Score;
    return Escaped;
}


