/***************************************************************************
                        io.c  -  Input Output stuff
                             -------------------
    begin                : Sat Sep 7 2002
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

#define MAXKEY  (1024)

static int IsKeyPressed[MAXKEY];            // Key pressed flags for keyboard controller

//
//
//                      Initialise any I/O Devices or associated data
//
//
void IOInitialise(void)
{
    int i;
    for (i = 0;i < MAXKEY;i++) IsKeyPressed[i] = 0;
}

//
//
//                      Check current status of the controller
//
//
int IOController(void)
{
    SDL_Event e;
    int r;
    while (SDL_PollEvent(&e))               // Look for keyboard events
        if (e.type == SDL_KEYDOWN || e.type == SDL_KEYUP)
        {
            r = e.key.keysym.sym;           // Key symbol changed
            ASSERT(r >= 0 && r < MAXKEY);   // range check
            IsKeyPressed[e.key.keysym.sym]  // Set the flag
                                        = (e.type == SDL_KEYDOWN);
        }
    r = 0;                                  // Build the controller byte
    if (IsKeyPressed[SDLK_z]) r |= KEYLEFT;
    if (IsKeyPressed[SDLK_x]) r |= KEYRIGHT;
    if (IsKeyPressed[SDLK_k]) r |= KEYUP;
    if (IsKeyPressed[SDLK_m]) r |= KEYDOWN;
    if (IsKeyPressed[SDLK_SPACE]) r |= KEYFIRE;
    if (IsKeyPressed[SDLK_ESCAPE]) r |= KEYEXIT;

    return r;
}
