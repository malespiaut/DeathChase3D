/***************************************************************************
                     font.c - scalable bitmapped fonts
                             -------------------
    begin                : Sun Aug 25 2002
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

static int _FONTPixelSet(char* Data, int x, int y);
static void _FONTAngleDraw(SDL_Surface* s, SDL_Rect* rc, int w, int h, Uint32 Colour);

//
//
//              An 8x8 pixel font, just alphanumeric characters
//
//
unsigned char _Font[] = {
  255,
  129,
  129,
  129,
  129,
  129,
  129,
  255,
  0,
  60,
  70,
  74,
  82,
  98,
  60,
  0,
  0,
  24,
  40,
  8,
  8,
  8,
  62,
  0,
  0,
  60,
  66,
  2,
  60,
  64,
  126,
  0,
  0,
  60,
  66,
  12,
  2,
  66,
  60,
  0,
  0,
  8,
  24,
  40,
  72,
  126,
  8,
  0,
  0,
  126,
  64,
  124,
  2,
  66,
  60,
  0,
  0,
  60,
  64,
  124,
  66,
  66,
  60,
  0,
  0,
  126,
  2,
  4,
  8,
  16,
  16,
  0,
  0,
  60,
  66,
  60,
  66,
  66,
  60,
  0,
  0,
  60,
  66,
  66,
  62,
  2,
  60,
  0,
  0,
  60,
  66,
  66,
  126,
  66,
  66,
  0,
  0,
  124,
  66,
  124,
  66,
  66,
  124,
  0,
  0,
  60,
  66,
  64,
  64,
  66,
  60,
  0,
  0,
  120,
  68,
  66,
  66,
  68,
  120,
  0,
  0,
  126,
  64,
  124,
  64,
  64,
  126,
  0,
  0,
  126,
  64,
  124,
  64,
  64,
  64,
  0,
  0,
  60,
  66,
  64,
  78,
  66,
  60,
  0,
  0,
  66,
  66,
  126,
  66,
  66,
  66,
  0,
  0,
  62,
  8,
  8,
  8,
  8,
  62,
  0,
  0,
  2,
  2,
  2,
  66,
  66,
  60,
  0,
  0,
  68,
  72,
  112,
  72,
  68,
  66,
  0,
  0,
  64,
  64,
  64,
  64,
  64,
  126,
  0,
  0,
  66,
  102,
  90,
  66,
  66,
  66,
  0,
  0,
  66,
  98,
  82,
  74,
  70,
  66,
  0,
  0,
  60,
  66,
  66,
  66,
  66,
  60,
  0,
  0,
  124,
  66,
  66,
  124,
  64,
  64,
  0,
  0,
  60,
  66,
  66,
  82,
  74,
  60,
  0,
  0,
  124,
  66,
  66,
  124,
  68,
  66,
  0,
  0,
  60,
  64,
  60,
  2,
  66,
  60,
  0,
  0,
  254,
  16,
  16,
  16,
  16,
  16,
  0,
  0,
  66,
  66,
  66,
  66,
  66,
  60,
  0,
  0,
  66,
  66,
  66,
  66,
  36,
  24,
  0,
  0,
  66,
  66,
  66,
  66,
  90,
  36,
  0,
  0,
  66,
  36,
  24,
  24,
  36,
  66,
  0,
  0,
  130,
  68,
  40,
  16,
  16,
  16,
  0,
  0,
  126,
  4,
  8,
  16,
  32,
  126,
  0};

//
//    Draw an 8x8 pixellated character (for fonts), does some rounding etc.
//
void
FONTChar(SDL_Surface* s, SDL_Rect* rc, Uint32 Colour, int Char)
{
  int x, y, w, h;
  char* GfxData;
  SDL_Rect rc2;
  ASSERT(s != NULL); // Checks
  ASSERT(rc != NULL);
  w = rc->w / 8;
  h = rc->h / 8;            // work out the box sizes
  ASSERT(w != 0 && h != 0); // At least one pixel !
  if (Char == ' ')
    return; // Don't do anything for spaces

  GfxData = _Font; // Convert character to graphic id
  if (isalpha(Char))
    GfxData = _Font + (toupper(Char) - 'A' + 11) * 8;
  if (isdigit(Char))
    GfxData = _Font + (Char - '0') * 8 + 8;

  for (x = 0; x < 8; x++) // Work through the 64 pixel array
    for (y = 0; y < 8; y++)
      if (_FONTPixelSet(GfxData, x, y)) // If set.
      {
        rc2.x = rc->x + w * x; // Calculate the bounding rectangle
        rc2.y = rc->y + h * y;
        rc2.w = w;
        rc2.h = h;
        SDL_FillRect(s, &rc2, Colour); // Draw an pixel there
                                       // Neaten the diagonals
        if (_FONTPixelSet(GfxData, x, y + 1) == 0 &&
            _FONTPixelSet(GfxData, x - 1, y) == 0 &&
            _FONTPixelSet(GfxData, x - 1, y + 1) != 0)
          _FONTAngleDraw(s, &rc2, -w, h, Colour);
        if (_FONTPixelSet(GfxData, x, y + 1) == 0 &&
            _FONTPixelSet(GfxData, x + 1, y) == 0 &&
            _FONTPixelSet(GfxData, x + 1, y + 1) != 0)
          _FONTAngleDraw(s, &rc2, w, h, Colour);
      }
}

//
//                  Check if pixel is set in character
//
static int
_FONTPixelSet(char* Data, int x, int y)
{
  if (x < 0 || y < 0 || x > 7 || y > 7)
    return 0;
  return (Data[y] & (0x80 >> x)) ? 1 : 0;
}

//
//  Draw an angled line - this stops the squared corners on diagonals showing
//
static void
_FONTAngleDraw(SDL_Surface* s, SDL_Rect* rc, int w, int h, Uint32 Colour)
{
  int i, m;
  SDL_Rect rc3;
  ASSERT(s != NULL); // Checks
  ASSERT(rc != NULL);
  m = abs(w);
  if (abs(h) > m)
    m = abs(h);
  for (i = 0; i < m; i++)
  {
    rc3.x = rc->x + w * i / m;
    rc3.y = rc->y + h * i / m;
    rc3.w = rc->w;
    rc3.h = rc->h;
    SDL_FillRect(s, &rc3, Colour);
  }
}

void
FONTString(SDL_Surface* s, FONTINFO* f, int x, int y, char* Msg)
{
  int xSize, ySize;
  SDL_Rect rc;
  xSize = s->w * f->xSize / 1000; // Size of font rectangle
  ySize = s->h * f->ySize / 1000;

  if (x >= 0)
    x = x * s->w / 100;
  else
    x = s->w / 2 - (strlen(Msg) * xSize / 2);
  y = y * s->h / 100;
  while (*Msg != '\0')
  {
    rc.x = x;
    rc.y = y;
    rc.w = xSize;
    rc.h = ySize;
    FONTChar(s, &rc, SDL_MapRGB(s->format, f->r, f->g, f->b), *Msg++);
    x = x + xSize;
  }
}
