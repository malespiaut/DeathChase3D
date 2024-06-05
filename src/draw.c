/***************************************************************************
                          draw.c  -  description
                             -------------------
    begin                : Fri Sep 6 2002
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

static void _DRAWObject(int Type, int x, int y, int Size, int yLogical);

//
//
//                                      Draw basic landscape
//
//
void
DRAWLandscape(int IsNight, int Bright)
{
  SDL_Rect rc;
  int c;
  rc.x = rc.y = 0;
  c = (IsNight == 0) ? 255 : 0; // Black night, Cyan day
  c = c * Bright / 100;         // Brightness
  rc.w = Glo.Drawing->w;        // Work out top rectangle and draw it
  rc.h = Glo.Drawing->h * HORIZONPERCENT / 100;
  SDL_FillRect(Glo.Drawing, &rc, SDL_MapRGB(Glo.Drawing->format, 0, c, c));
  rc.y = rc.h;
  rc.h = Glo.Drawing->h - rc.y; // Work out bottom rectangle and draw it
  SDL_FillRect(Glo.Drawing, &rc, SDL_MapRGB(Glo.Drawing->format, 0, (IsNight == 0) ? Bright * 3 / 2 : Bright * 2 / 3, 0));
}

//
//
//                Translate -MAXX..MAXX, 0..MAXY coordinates to screen ones.
//
//
void
DRAWTranslate(int* px, int* py, int* Size)
{
  int x, y;
  double dx, dy;
  ASSERT(px != NULL);
  ASSERT(py != NULL);
  ASSERT(Size != NULL);
  x = *px;
  y = *py; // Read values

  dx = (double)x / MAXX;               // now X position = -1..1 floating.
  dy = 1.0 - (double)y / (double)MAXY; // 0-1 (1 is at the bottom)
  dy = pow(dy, 2.2);                   // Makes the back-front flow right

  x = (int)(Glo.xSize * dx * // Calculate x position
            (dy + 0.4) / 1.0) +
      Glo.xSize / 2;

  y = Glo.ySize - ((1.0 - dy) * Glo.Drawing->h * // Work out y position
                   (100 - HORIZONPERCENT) / 100);
  *Size = dy * 100; // Work out the size
  *px = x;
  *py = y; // Copy to variables
}

//
//
//                          Draw a specified object
//
//
void
DRAWObject(OBJECT* ob)
{
  ASSERT(ob != NULL);
  ob->xPhysical = ob->x; // Copy logical coords to physical ones
  ob->yPhysical = ob->y;
  DRAWTranslate(&(ob->xPhysical), // Convert to physical coordinates
                &(ob->yPhysical),
                &(ob->Size));
  ob->OnScreen = 1; // Work out if on screen
  if (ob->xPhysical < -Glo.Drawing->w / 8)
    ob->OnScreen = 0;
  if (ob->xPhysical > Glo.Drawing->w * 9 / 8)
    ob->OnScreen = 0;
  if (ob->y < 0 || ob->y > MAXY)
    ob->OnScreen = 0;
  if (ob->OnScreen)
    _DRAWObject(ob->Type, ob->xPhysical, ob->yPhysical, ob->Size, ob->y);
}

//
//
//                          Actually draw the object
//
//
static void
_DRAWObject(int Type, int x, int y, int Size, int yLogical)
{
  SDL_Rect rc, rcs;
  int i, Best;
  if (Type == GR_HIT)
  {
    if (SDL_GetTicks() / 100 % 2 == 0)
      return;
    Type = GR_CENTRE;
  }
  switch (Type)
  {
    case GR_TREE:
      Best = 1000;
      for (i = 0; i < Glo.Sizes; i++)
        if (abs(Size - Glo.TreePercent[i]) < Best)
        {
          Best = abs(Size - Glo.TreePercent[i]);
          rcs = Glo.TreeRect[i];
        }
      ASSERT(Best != 1000);
      if (rcs.y < 0)
      {
        rcs.h += rcs.y;
        rcs.y = 0;
      }
      rc.x = x - rcs.w / 2;
      rc.y = y - rcs.h;
      SDL_BlitSurface(Glo.Sprites, &rcs, Glo.Drawing, &rc);
      break;

    case GR_LEFT:
    case GR_CENTRE:
    case GR_RIGHT:
      ASSERT(yLogical >= MAXY / 2);
      i = 7 * (yLogical - MAXY / 2) / (MAXY / 2);
      if (i < 0)
        i = 0;
      if (i > 7)
        i = 7;
      GLOGetSpriteRect(Type, i, &rc);
      rcs.x = x - rc.w / 2;
      rcs.y = y - rc.h / 2;
      SDL_BlitSurface(Glo.Sprites, &rc, Glo.Drawing, &rcs);
      break;

    case GR_MISSILE:
      i = 7 * yLogical / MAXY;
      if (i < 0)
        i = 0;
      if (i > 7)
        i = 7;
      GLOGetSpriteRect(Type, i, &rc);
      rcs.x = x - rc.w / 2;
      rcs.y = y - rc.h / 2;
      SDL_BlitSurface(Glo.Sprites, &rc, Glo.Drawing, &rcs);
      break;

    default:
      ERROR();
  }
}
