/***************************************************************************
                       treedraw.c  -  tree drawing code
                             -------------------
    begin                : Thu Sep 5 2002
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

static void _TreeDraw(SDL_Surface *s,SDL_Rect *rc);

//
//
//                              Draw trees on the given surface
//
//
void TREEDraw(SDL_Surface *s)
{
    int i;
    ASSERT(s != NULL);
    for (i = 0;i < Glo.Sizes;i++)
        _TreeDraw(s,&(Glo.TreeRect[i]));
}

//
//
//                            Draw a tree in the given rectangle
//
//
static void _TreeDraw(SDL_Surface *s,SDL_Rect *rc)
{
    int x,w,y,d;
    SDL_Rect r;
    ASSERT(s != NULL);
    ASSERT(rc != NULL);
    w = rc->w/4;if (w < 2) w = 2;
    y = rc->h/2;
    for (x = -w;x <= w;x++)
    {
        r.x = rc->x+x+rc->w/2;
        r.y = rc->y+rc->w/2;
        r.w = 1;r.h = rc->h-rc->w/2;
        d = w-(int)sqrt(w*w-x*x);r.h -= d;
        SDL_FillRect(s,&r,TREEGetColour(s,x-w/3,w*3/2,182,104,44));
    }
    w = rc->w/2;if (w < 2) w = 2;
    for (x = -w;x <= w;x++)
    {
        r.x = rc->x+x+rc->w/2;
        r.y = rc->y;r.w = 1;r.h = y;
        d = w-(int)sqrt(w*w-x*x);r.y += d;r.h -= d*2;
        SDL_FillRect(s,&r,TREEGetColour(s,x-w/3,w*3/2,0,255,0));
    }
}

//
//
//                      Do the lighting effect for drawn objects
//
//
Uint32 TREEGetColour(SDL_Surface *s,int v,int m,int r,int g,int b)
{
    int pc;
    ASSERT(s != NULL);
    pc = 30+70*abs(v)/m;if (pc > 100) pc = 100;
    pc = 100-pc;
    return SDL_MapRGB(s->format,r*pc/100,g*pc/100,b*pc/100);
}
