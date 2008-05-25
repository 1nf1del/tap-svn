//
//	  MultiViewEPG jim16 Tue 1 Apr 12:12:00
//
//	  Copyright (C) 2008 Jim16
//
//	  This is free software; you can redistribute it and/or modify
//	  it under the terms of the GNU General Public License as published by
//	  the Free Software Foundation; either version 2 of the License, or
//	  (at your option) any later version.
//
//	  The software is distributed in the hope that it will be useful,
//	  but WITHOUT ANY WARRANTY; without even the implied warranty of
//	  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//	  GNU General Public License for more details.
//
//	  You should have received a copy of the GNU General Public License
//	  along with this software; if not, write to the Free Software
//	  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

/*
GfxLIB - A utility library to assist in doing graphics on the OSD on Topfield 5000.
Author : Kjetil Naess, July 2005
Version: 1.0

You may copy and/or use GfxLIB in either source or compiled form as you wish. Any suggestions and/or
improvements are welcome so we can make this a good public resource.
*/

#ifndef GFXLIB_H
#define GFXLIB_H 1
#endif

#include "tap.h"

#define VideoResX 720
#define VideoResY 576
#define xOffset 40
#define yOffset  28
#define xLimit  680
#define yLimit  552

void setupGraphics();
void endGraphics();
void setPixel(word region, int x, int y, word color);
word getPixel(word region, int x, int y);
void drawLine(word region, int x1, int y1, int x2, int y2, word color);
void drawDisk(word region, int xc, int yc, int radius, word color);
bool drawCircle(word region, int xc, int yc, int radius, word color);
void drawRectangle(word region, int x1, int y1, int x2, int y2, word color, byte Filled);
void drawArea(word region, int x1, int y1, int w, int h, word color);
void gxRoundRectangle(word region, int x1, int y1, int x2, int y2, word color, byte Filled, int r);
void gxFloodFill(word region, int x, int y, word newColor, word oldColor);
void gxCircleQuadrant(word region, int xc, int yc, int radius, word color, int quadrant, byte Filled);

void DuffsDevice(register unsigned char *to, register unsigned char *from, register unsigned int count);

#define blt_ToScreen 0
#define blt_FromScreen 1

#define blt_SRCAND    0
#define blt_SRCCOPY   1
#define blt_SRCINVERT 2
#define blt_SRCPAINT  3
#define blt_SRCCLEAR  4

//DLLIMPORT byte setROP(byte ROP);

//DLLIMPORT void bitBlt(int x, int y, int w, int h, word *Data, unsigned char Direction, unsigned char Operation);

//extern bool masterpiece_mode;