/***************************************************************************
                          dc.h  -  DeathChase Headers
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <string.h>
#include <SDL.h>

typedef struct _Colour                      // Colour descriptor structure
{
    int r,g,b;
} COLOUR;

#define MAXTREESIZE (80)                    // Max # tree sizes
#define MAXTREEWIDTHPERCENT (14)            // Largest tree width as percent screen size
#define SPEEDWIDTHPERCENT   (35)            // Speeder width as percent of screen width
#define MAXX        (1024)                  // Maximum horizontal extent (-ve left, +ve right)
#define MAXY        (1024)                  // Maximum vertical extent (0 player,+ve into screen)
#define HORIZONPERCENT (40)                 // Horizon position down screen
#define MAXTREES    (100)                   // Maximum number of trees

#define MAXBIKES    (16)                    // Max number of bikes
#define MAXOTHERS   (0)                     // Max number of other sprites
#define SPRCOUNT    (MAXBIKES+MAXOTHERS+2)  // No of sprites (+1 for player missile,+1 for end marker)

#define GR_MISSILE  (0)                     // Missile graphics
#define GR_LEFT     (1)
#define GR_CENTRE   (2)
#define GR_RIGHT    (3)
#define GR_HIT      (4)                     // a hit bad guy
#define GR_TREE     (-1)

#define ASSERT(x)   if (!(x)) ERROR()
#define ERROR()     GLOError(__LINE__,__FILE__)

#define KEYUP       (0x01)                  // Key bits
#define KEYDOWN     (0x02)
#define KEYLEFT     (0x04)
#define KEYRIGHT    (0x08)
#define KEYFIRE     (0x10)
#define KEYEXIT     (0x20)

typedef struct _Global                      // Globals structure
{
    SDL_Surface *Screen;                    // Main display
    SDL_Surface *Drawing;                   // Drawing display
    SDL_Surface *Sprites;                   // Sprites surface
    int xSize,ySize,Depth;                  // Size and Depth
    int Sizes;                              // Number of different tree sizes
    int SpriteScaler;                       // Multiplier for sprite screen vs real screen
    Uint32 ColourKey;                       // Colour Key (sprites)
    SDL_Rect TreeRect[MAXTREESIZE];         // Tree Graphic Rectangles
    int TreePercent[MAXTREESIZE];           // Percentage represented by those rectangles
    SDL_Rect PlayerRect[3];                 // Player Rectangles
    SDL_Rect DigitRect[10];                 // Rectangle for score digits
    int Score;                              // Current score
    int HighScore;
    int ScaleSpeed;                         // Scaling of game speed
    int AccelTime;                          // Time to accelerate to 100%
    int TurningCircle;                      // Turning circle rate
    int MinEnemyPC;                         // Minimum Enemy position in screen, percent
    int EMinSpeed,EMaxSpeed;                // Min and Max Speeds, as percentage
    int MissileSpeed;                       // Missile Speed as percentage
    int CollidePercent;                     // Percentage collision with trees adjustment
    int Precision;                          // Shot precision
    int BaseTrees;                          // Base number of trees
    int IncTrees;                           // Additional trees per level.
    int StartLevel;                         // Starting Level
    int Enemies;                            // Enemy count per level
    int StartLives;                         // Lives at the start
} GLOBAL;

typedef struct _Object                      // Item in a treelist entry
{
    int xPhysical,yPhysical;                // Position (logical)
    int Size;                               // Size as a percentage
    int Type;                               // Object type
    int OnScreen;                           // Is currently visible
    int x,y;                                // Position of tree
    int MarkDel;                            // Mark for deletion flag
    int InUse;                              // Is currently in use
    Uint32 TimeNext;                        // Time of next action
    int Speed;                              // Speed (% of flat out)
    int Dir;                                // Direction (for missiles)
    struct _Object *Next;                   // Next (list must be sorted in Y high order first)
} OBJECT;

typedef struct _LevelDesc                   // Level Descriptor
{
    int IsNight;                            // Non-zero on night run
    int EnemyCount;                         // Number of enemies
    int TreeCount;                          // Number of trees
} LEVELDESC;

typedef struct _TreeList                    // Tree list entry
{
    int TreeCount;                          // Number of permissible trees
    OBJECT *First;                          // First entry in the list (highest Y tree)
    OBJECT Trees[MAXTREES];                 // Structure containing all possible trees
} TREELIST;

typedef struct _FontInfo
{
    int xSize,ySize;                        // Char sizes as percent of the screen
    int r,g,b;                              // Colour (0-255 each value)
} FONTINFO;

extern GLOBAL Glo;


void GLOInitialise(int argc,char *argv[]);
void GLOError(int Line,char *File);
void GLOGetSpriteRect(int Sprite,int Entry,SDL_Rect *rc);

void TREEDraw(SDL_Surface *s);
Uint32 TREEGetColour(SDL_Surface *s,int v,int m,int r,int g,int b);

void DRAWLandscape(int IsNight,int Bright);
void DRAWTranslate(int *px,int *py,int *Size);
void DRAWObject(OBJECT *ob);

void OBJCreate(OBJECT *ob,LEVELDESC *Lvl);
void OBJSort(OBJECT *ob);
void OBJMoveEnemy(OBJECT *ob,Uint32 Time,int xPMove,int yPMove,int TMove);
int  OBJCheckHitEnemy(OBJECT *ob,int x,int y);

void TREEInitialise(TREELIST *List,int n);
void TREEDrawAll(TREELIST *t,int y1,int y2);
int  TREEMoveAll(TREELIST *t,int xMove,int yMove);
int TREECheckMissileHit(int x,int y,TREELIST *t);
int TREEHasHit(int xm,int ym,int xo,int yo);

void IOInitialise(void);
int IOController(void);

void SPRSetDraw(SDL_Surface *s,SDL_Rect *rc,int Rotate);
void SPRDrawPlayer(void);
void SPRDrawBike(void);
void SPRDrawMissile(void);

void FONTChar(SDL_Surface *s,SDL_Rect *rc,Uint32 Colour,int Char);
void FONTString(SDL_Surface *s,FONTINFO *f,int x,int y,char *Msg);

int LEVELRun(LEVELDESC *Lvl);
