/***************************************************************************
                           main.c  -  Main Program
                             -------------------
    begin                : Thu Sep  5 21:52:32 BST 2002
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

static void _MAINPlayGame(void);
static void _MAINTitle(void);

int
main(int argc, char* argv[])
{
  int n, Quit = 0;
  GLOInitialise(argc, argv);
  while (Quit == 0)
  {
    while (IOController() != 0)
    {
    }
    _MAINTitle();
    while (n = IOController(), n == 0)
    {
    }
    Quit = (n & KEYEXIT);
    if (Quit == 0)
      _MAINPlayGame();
  }
  return EXIT_SUCCESS;
}

//
//
//                                              Play one round
//
//
static void
_MAINPlayGame(void)
{
  LEVELDESC Lvl;
  int Lives, Quit, Level, Completed;
  Lives = Glo.StartLives;
  Quit = 0;
  Glo.Score = 0;
  Level = Glo.StartLevel;
  while (Lives > 0 && Quit == 0) // play till quit or out of lives
  {
    Lvl.TreeCount = Glo.BaseTrees + // set up the level
                    (Level - 1) * Glo.IncTrees;
    if (Lvl.TreeCount > MAXTREES)
      Lvl.TreeCount = MAXTREES;
    Lvl.IsNight = (Level % 2 == 0); // levels 2,4,6 are night patrol
    Lvl.EnemyCount = Glo.Enemies;
    Completed = 0;
    while (Quit == 0 && // this loop is per level
           Completed == 0 && Lives > 0)
    {
      Quit = LEVELRun(&Lvl); // do it !
      if (Quit == 0)         // if didn't ESC then completed or died
      {
        if (Lvl.EnemyCount == 0)
          Completed = 1, Level++;
        else
          Lives--;
      }
    }
  }
}

static void
_MAINTitle(void)
{
  FONTINFO f;
  char Line[64];
  SDL_FillRect(Glo.Screen, NULL, SDL_MapRGB(Glo.Screen->format, 0, 0, 0));

  f.xSize = 90;
  f.ySize = 300;
  f.r = 255;
  f.g = 255;
  f.b = 0;
  FONTString(Glo.Screen, &f, -1, 5, "DeathChase");
  f.xSize = f.ySize = 200;

  f.r = 0;
  f.g = 0;
  f.b = 255;
  FONTString(Glo.Screen, &f, 58, 18, "3D");
  f.r = 0;
  f.g = f.b = 255;
  FONTString(Glo.Screen, &f, 60, 20, "3D");

  f.xSize = 30;
  f.ySize = 60;
  f.r = f.b = 0;
  f.g = 255;
  FONTString(Glo.Screen, &f, -1, 45, "Written by Paul Robson");
  FONTString(Glo.Screen, &f, -1, 52, "Original by Mervyn Estcourt");

  sprintf(Line, "High Score %d", Glo.HighScore);
  FONTString(Glo.Screen, &f, -1, 70, Line);
  f.xSize = 24;
  f.ySize = 55;
  f.r = f.g = f.b = 255;
  FONTString(Glo.Screen, &f, -1, 90, "Press Fire to Start  Press Escape to Exit");
  SDL_UpdateRect(Glo.Screen, 0, 0, Glo.Screen->w, Glo.Screen->h);
}
