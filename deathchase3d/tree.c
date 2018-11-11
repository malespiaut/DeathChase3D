/***************************************************************************
                      tree.c  -  tree linked list code
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

static void _TREEAddNewTree(OBJECT *t,TREELIST *List,int yPos);
static OBJECT *_TREEDeleteFromList(OBJECT *t);

//
//
//              Initialise a treelist structure - a list of treelistobjects
//              always sorted by furthest first.
//
//
void TREEInitialise(TREELIST *List,int n)
{
    int i;
    ASSERT(List != NULL);
    ASSERT(n > 0 && n <= MAXTREES);
    List->TreeCount = n;                    // Save number of trees
    List->First = NULL;                     // No trees in the list
    for (i = 0;i < n;i++)                   // Clear all "trees in use" flag.
        _TREEAddNewTree(&(List->Trees[i]),  // Add a new tree to the list
                        List,
                        (40+60*(i+1)/n)*MAXY/100);
}

//
//
//                              Add a new list in
//
//
static void _TREEAddNewTree(OBJECT *t,TREELIST *List,int yPos)
{
    int x;
    ASSERT(t != NULL);
    ASSERT(List != NULL);
    if (List->First != NULL)                // If not the first, check legitimate order
        if (yPos < List->First->y) ERROR();
    x = (int)pow(MAXX,1.8);                 // Horizontal position
    t->x = (rand()%x)*MAXX/x;
    ASSERT(t->x <= MAXX);
    if (rand()%2 == 0) t->x = -t->x;
    t->y = yPos;                            // Vertical position
    t->Next = List->First;                  // Link in to list
    List->First = t;
    t->MarkDel = 0;                         // Not due to be deleted
    t->Type = GR_TREE;
}

//
//
//      Redraw all the trees between y1 and y2
//
//
void TREEDrawAll(TREELIST *t,int y1,int y2)
{
    OBJECT *tl;
    int i;
    ASSERT(t != NULL);
    if (y1 == y2) return;                   // Nothing to do :)
    tl = t->First;                          // Point to first in list
    ASSERT(tl != NULL);
    while (tl != NULL)
    {
        tl->MarkDel = 0;                    // Not deleted
        if (tl->Next != NULL)               // Validate the list order
            if (tl->y < tl->Next->y) ERROR();
        if (tl->y >= y1 && tl->y < y2)      // if in range
        {
            ASSERT(tl->Type == GR_TREE);
            DRAWObject(tl);                 // And draw it
            if (tl->OnScreen == 0)          // If not on screen
                        tl->MarkDel = 1;    // Delete it the next time around
        }
        if (tl->y < 0 || tl->y > MAXY)      // Kill if out of range.
                            tl->MarkDel = 1;
        tl = tl->Next;                      // Work through the list
    }
    t->First=_TREEDeleteFromList(t->First); // Physically remove from the list
    for (i = 0;i < t->TreeCount;i++)        // Any deleted ones recreate.
        if (t->Trees[i].MarkDel)
            _TREEAddNewTree(&(t->Trees[i]),t,MAXY);
}

//
//
//                          Recursive list element deleter
//
//
static OBJECT *_TREEDeleteFromList(OBJECT *t)
{
    if (t == NULL) return NULL;             // Empty list
    if (t->MarkDel)                         // Being deleted
    {
        return _TREEDeleteFromList(t->Next);// Delete it out.
    }
    else                                    // Not being deleted ?
    {
        t->Next =                           // Recursively delete from the rest of the list
            _TREEDeleteFromList(t->Next);
        return t;                           // Return this one
    }
}

//
//
//                                  Move All Trees, Check for collision
//
//
int TREEMoveAll(TREELIST *t,int xMove,int yMove)
{
    OBJECT *tl;
    int n,Crashed = 0;
    ASSERT(t != NULL);
    tl = t->First;                          // Point to first in list
    ASSERT(tl != NULL);
    while (tl != NULL)
    {
        if (tl->Next != NULL)               // Validate the list order
            if (tl->y < tl->Next->y) ERROR();
        tl->y -= yMove;
        if (tl->y <= 0)                     // Tree gone through the front
        {
            n = abs(tl->x) * 100 / MAXX;    // How far off centre ?
            if (n < Glo.CollidePercent) Crashed = 1;
        }        
        tl->x -= xMove;
        tl = tl->Next;                      // Work through the list
    }
    return Crashed;
}

//
//
//                                  Check if a tree has been hit
//
//

int TREECheckMissileHit(int x,int y,TREELIST *t)
{
    OBJECT *tl;
    ASSERT(t != NULL);
    tl = t->First;                          // Work through trees
    while (tl != NULL)
    {
        if (TREEHasHit(x,y,tl->x,tl->y))    // Return true if hit
                                        return 1;
        tl = tl->Next;        
    }
    return 0;
}

//
//
//                                Check if missile has hit an object
//
//
int TREEHasHit(int xm,int ym,int xo,int yo)
{
    int x,y;
    x = abs(xm-xo);y = abs(ym-yo);
    if (y < Glo.Precision*MAXY/35/100)
        if (x < Glo.Precision*MAXX/35/100) return 1;
    return 0;
}

