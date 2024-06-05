/***************************************************************************
                    globals.c  -  globals, setup, terminate
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

GLOBAL Glo;

static void GLOTerminate(void);
static int _GLOCalculateTreeRect(void);
static int _GLOParameter(int argc, char** argv, char* Name, int Default, int IsHelp);
//
//
//                  Initialise globals and general setup
//
//

#define PARAM(name, default) _GLOParameter(argc, argv, name, default, Help)

void
GLOInitialise(int argc, char* argv[])
{
  int i, j, Err, Help;
  SDL_Rect *rc, rc2;
  ASSERT(argc > 0);
  ASSERT(argv != NULL);

  Help = 0;
  for (i = 0; i < argc; i++)
    if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0)
      Help = 1;

  if (Help)
    printf("Parameters");
  Glo.xSize = PARAM("xscreen", 800);
  Glo.ySize = PARAM("yscreen", 600);
  Glo.Depth = PARAM("depth", 16);
  Glo.SpriteScaler = PARAM("scaler", 4);
  Glo.ScaleSpeed = PARAM("speed", 100);
  Glo.AccelTime = PARAM("accel", 100) * 6;
  Glo.TurningCircle = PARAM("turning", 100);
  Glo.MinEnemyPC = PARAM("enemynear", 75);
  Glo.EMinSpeed = PARAM("emin", 80);
  Glo.EMaxSpeed = PARAM("emax", 120);
  Glo.MissileSpeed = PARAM("mspeed", 100) / 3;
  Glo.CollidePercent = PARAM("collide", 100) * 11 / 100;
  Glo.Precision = PARAM("precision", 100);
  Glo.BaseTrees = PARAM("basetrees", 3);
  Glo.IncTrees = PARAM("inctrees", 1);
  Glo.StartLevel = PARAM("level", 1);
  Glo.Enemies = PARAM("enemies", 2);
  Glo.StartLives = PARAM("lives", 3);

  if (Help) // Help display, do nothing else.
  {
    printf("\n\n");
    exit(0);
  }

  if (SDL_Init(SDL_INIT_VIDEO | // Initialise SDL
               SDL_INIT_TIMER) < 0)
    ERROR();
  atexit(SDL_Quit);
  Glo.Screen = // Create main video screen
    SDL_SetVideoMode(Glo.xSize, Glo.ySize, Glo.Depth, SDL_SWSURFACE | SDL_ANYFORMAT);
  if (Glo.Screen == NULL)
    ERROR();
  SDL_WM_SetCaption("Deathchase 3D", NULL);
  Glo.Drawing = Glo.Screen;

  atexit(GLOTerminate);
  Glo.Sprites = // Create sprites surface
    SDL_CreateRGBSurface(SDL_SWSURFACE | SDL_SRCCOLORKEY,
                         Glo.xSize * Glo.SpriteScaler,
                         Glo.ySize,
                         Glo.Screen->format->BytesPerPixel * 8,
                         0,
                         0,
                         0,
                         0);
  Glo.ColourKey = // Sprites colour key
    SDL_MapRGB(Glo.Sprites->format, 32, 64, 192);
  SDL_SetColorKey(Glo.Sprites, // Set the colour key
                  SDL_SRCCOLORKEY,
                  Glo.ColourKey);
  SDL_FillRect(Glo.Sprites, NULL, Glo.ColourKey);
  if (Glo.Sprites == NULL)
    ERROR();
  Glo.Score = 0;
  Glo.HighScore = 1000;
  Glo.Sizes = MAXTREESIZE; // Calculate number of scaled tree graphics, start max.
  do
  {
    Glo.Sizes--;                   // next down.
    Err = _GLOCalculateTreeRect(); // will it fit the display
  } while (Err != 0);              // no, keep going
  printf("Creating %d different size trees.\n", Glo.Sizes);

  TREEDraw(Glo.Sprites); // Create the tree sprites

  printf("Creating the player sprites.\n");
  for (i = 0; i < 3; i++) // Create three players sprites
  {
    rc = &(Glo.PlayerRect[i]); // Get the rectangle ptr
    rc->x = 0;                 // Set up the rectangle
    rc->w = rc->h = Glo.Sprites->h / 3;
    rc->w = rc->w * 4 / 3;
    rc->y = Glo.Sprites->h - (rc->h) * (i + 1);
    SPRSetDraw(Glo.Sprites, rc, (1 - i) * 15); // Set the drawing space and tilt and draw it
    SPRDrawPlayer();
  }
  printf("Creating enemy and missile sprites.\n");
  for (i = 0; i < 4; i++)   // 3 x enemies [1..3],1 x missile [0]
    for (j = 0; j < 8; j++) // 8 scales
    {
      GLOGetSpriteRect(i, j, &rc2);      // Get drawing area
      if (i >= GR_LEFT && i <= GR_RIGHT) // Enemies
      {
        SPRSetDraw(Glo.Sprites, &rc2, (i - 2) * -20);
        SPRDrawBike();
      }
      if (i == GR_MISSILE) // Missile
      {
        SPRSetDraw(Glo.Sprites, &rc2, 0);
        SPRDrawMissile();
      }
    }
  printf("Creating digit graphics.\n"); // Create digits 0-9 for fast blitting
  for (i = 0; i < 10; i++)
  {
    rc2.w = Glo.Screen->w / 3 / 6;
    rc2.h = rc2.w;
    rc2.y = rc2.h * i;
    rc2.x = Glo.Screen->w - rc2.w;
    Glo.DigitRect[i] = rc2;
    j = rc2.w / 16;
    if (j == 0)
      j = 1;
    rc2.w -= j;
    rc2.h -= j;
    FONTChar(Glo.Sprites, &rc2, SDL_MapRGB(Glo.Sprites->format, 0, 0, 0), '0' + i);
    rc2.x += j;
    rc2.y += j;
    FONTChar(Glo.Sprites, &rc2, SDL_MapRGB(Glo.Sprites->format, 255, 255, 255), '0' + i);
  }
}

//
//
//                                  Tidy up on exit
//
//
static void
GLOTerminate(void)
{
}

//
//
//                                  Error : Crash out
//
//
void
GLOError(int Line, char* File)
{
  fprintf(stderr, "\n\nDeathchase 3D Error : %s [%d]\n\n", File, Line);
  exit(1);
}

//
//
//                      Calculate tree rectangles for a given size
//
//
static int
_GLOCalculateTreeRect(void)
{
  int i, w, x, pc;
  double p;
  SDL_Rect* rc;
  x = Glo.Screen->w; // First bit belongs to sprites
  w = Glo.xSize *    // Width of tree at point blank range
      MAXTREEWIDTHPERCENT / 100;
  for (i = 0; i < Glo.Sizes; i++) // For each in current number
  {
    rc = &(Glo.TreeRect[i]); // Handy pointer
    p = (double)(i) /        // Work out percentage of full size as decimal
          (double)(Glo.Sizes) * 0.90 +
        0.10;
    p = pow(p, 1.5);                          // skews towards smaller trees
    Glo.TreePercent[i] = pc = (int)(p * 100); // Percentage
    rc->w = w * pc / 100;                     // Width
    rc->h = Glo.ySize * 3 / 2 * pc / 100;     // Height
    rc->x = x;
    rc->y = Glo.Screen->h - rc->h;
    x = x + rc->w + 4; // Shift 'next pos' pointer right
  }
  return (x >= Glo.Sprites->w); // if gone off the end, this one failed
}

//
//
//                              Get Rectangle for a sprite
//
//
void
GLOGetSpriteRect(int Sprite, int Entry, SDL_Rect* rc)
{
  int pc;
  rc->w = Glo.Screen->h / 8; // Actual bounding rectangle
  rc->h = rc->w;
  rc->x = rc->w * Sprite + Glo.PlayerRect[0].w;
  rc->y = rc->h * Entry;

  rc->x = rc->x + rc->w / 2; // Move X to centre point
  rc->y = rc->y + rc->h / 2;
  pc = 100 - Entry * 10; // Percentage size

  rc->w = rc->w * pc / 100; // Scale size
  rc->h = rc->h * pc / 100;
  rc->x = rc->x - rc->w / 2; // Fix top left
  rc->y = rc->y - rc->h / 2;
}

static int
_GLOParameter(int argc, char** argv, char* Name, int Default, int IsHelp)
{
  int i;
  char* p;
  if (IsHelp)
  {
    printf(" --%s [%d]", Name, Default);
    return Default;
  }
  for (i = 0; i < argc; i++)
  {
    p = argv[i];
    if (p[0] == '-' && p[1] == '-')
      if (strcmp(p + 2, Name) == 0)
        return atoi(argv[i + 1]);
  }
  return Default;
}
