/***************************************************************************
                          sprites.c  -  description
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

static SDL_Surface* _Surface;
static SDL_Rect* _Rect;
static int _Step;
static double _Sin, _Cos;

static void _SPRPlot(int x, int y, Uint32 Colour);
static void _SPRRect(int x, int y, int w, int h, int r, int g, int b);
static void _SPREllipse(int x, int y, int w, int h, int r, int g, int b);

//
//
//                          Set Current Drawing information
//
//
void
SPRSetDraw(SDL_Surface* s, SDL_Rect* rc, int Rotate)
{
  double Angle;
  _Surface = s;
  _Rect = rc;
  _Step = 2048 / (rc->w + rc->h);
  Angle = (double)Rotate * 2.0 * 3.14159 / 360.0;
  _Cos = cos(-Angle);
  _Sin = sin(-Angle);
}

//
//
//                                    Plot a point
//
//
static void
_SPRPlot(int x, int y, Uint32 Colour)
{
  SDL_Rect rc;
  double xf, yf;

  xf = (double)x;
  yf = (double)y;                   // To float
  x = (int)(xf * _Cos - yf * _Sin); // Rotate
  y = (int)(xf * _Sin + yf * _Cos);

  if (abs(x) > 1024 || abs(y) > 1024)
    return;

  rc.x = (x + 1024) * (_Rect->w) / 2048 + _Rect->x;
  rc.y = (y + 1024) * (_Rect->h) / 2048 + _Rect->y;
  rc.w = rc.h = 1;
  SDL_FillRect(_Surface, &rc, Colour);
}

//
//
//                              Draw a rectangle
//
//
static void
_SPRRect(int x, int y, int w, int h, int r, int g, int b)
{
  int x1, y1;
  for (x1 = 0; x1 < w; x1 = x1 + _Step)
    for (y1 = 0; y1 < h; y1 = y1 + _Step)
    {
      if (w > h)
        _SPRPlot(x + x1, y + y1, TREEGetColour(_Surface, y1 - h / 3, h * 2 / 3, r, g, b));
      else
        _SPRPlot(x + x1, y + y1, TREEGetColour(_Surface, x1 - w * 2 / 3, w * 2 / 3, r, g, b));
    }
}

//
//
//                              Draw an Ellipse
//
//
static void
_SPREllipse(int x, int y, int w, int h, int r, int g, int b)
{
  int d, x1, y1, xs, xc, yc;
  for (y1 = -h; y1 < h; y1 = y1 + _Step)
  {
    xs = (int)sqrt(h * h - y1 * y1);
    xs = xs * w / h;
    for (x1 = -xs; x1 < xs; x1 = x1 + _Step)
    {
      xc = x1 - h / 3;
      yc = y1 + h / 3;
      xc = xc * h / w;
      d = sqrt(xc * xc + yc * yc);
      _SPRPlot(x + x1, y + y1, TREEGetColour(_Surface, d, h * 2, r, g, b));
    }
  }
}

//
//
//                          Draw a player (the bike being ridden)
//
//
void
SPRDrawPlayer(void)
{
  double s, c;
  s = _Sin;
  c = _Cos;
  _Sin = 0.0;
  _Cos = 1.0;
  _SPRRect(-300, 0, 600, 1024, 192, 192, 192);
  _Sin = s;
  _Cos = c;
  //    _SPRRect(-100,-512,200,256,128,128,128);
  //    _SPRRect(-50,-812,100,300,128,128,128);
  _SPRRect(-768, -128, 768 * 2, 256, 192, 192, 192);
  _SPREllipse(-768, 0, 100, 100, 255, 255, 0);
  _SPRRect(-868, 0, 200, 1024, 255, 255, 0);
  _SPREllipse(768, 0, 100, 100, 255, 255, 0);
  _SPRRect(668, 0, 200, 1024, 255, 255, 0);
  _SPREllipse(0, 0, 360, 360, 0, 0, 0);
  _SPREllipse(0, 0, 340, 340, 255, 255, 255);
}

//
//
//                              Draw a bike in the background
//
//
void
SPRDrawBike(void)
{
  int i;
  _SPREllipse(0, -700, 300, 300, 255, 255, 255);
  _SPREllipse(-400, -300, 200, 300, 255, 255, 255);
  _SPREllipse(400, -300, 200, 300, 255, 255, 255);
  _SPRRect(-400, -600, 800, 800, 255, 255, 255);
  _SPRRect(-600, 200, 1200, 400, 255, 255, 0);
  for (i = -1; i <= 1; i = i + 2)
  {
    _SPREllipse(i * 600, 400, 350, 350, 0, 0, 0);
    _SPREllipse(i * 600, 400, 320, 320, 255, 255, 0);
    _SPREllipse(i * 600, 400, 210, 210, 255, 0, 0);
  }
}
//
//
//                                      Draw a missile
//
//
void
SPRDrawMissile(void)
{
  _SPREllipse(-600, 0, 500, 500, 255, 0, 0);
  _SPREllipse(600, 0, 500, 500, 255, 0, 0);
}
