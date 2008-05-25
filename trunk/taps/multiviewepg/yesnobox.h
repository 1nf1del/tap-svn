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

#ifndef __YesNo__
  #define __YesNo__

#define __YesNo_VERSION__ "2008-01-16"

enum {
	QUESTION,
	QUESTION2,
	YES,
	NO,
	YESNOOPTIONS
};

#define YesNo_PADDING DISPLAY_BORDER
#define YesNo_GAP DISPLAY_GAP

// Variables used for colours
static dword YesNo_Fc = COLOR_White;
static dword YesNo_HghBc = COLOR_Red;
static dword YesNo_HghFc = COLOR_White;
static dword YesNo_Bc = COLOR_Black;
static dword YesNo_BaseColour = COLOR_Black;

static byte* YesNo_ItemBase = NULL;
static byte* YesNo_ItemHigh = NULL;

static int YesNo_Option_H = 30;
static int YesNo_Option_W = 200;
static bool YesNo_Active = FALSE;
static word YesNo_RGN = 0;
static word YesNo_MemRGN = 0;
static byte* YesNo_Base = NULL;
static bool YesNo_Initalised = FALSE;
static int YesNo_X = 0;
static int YesNo_Y = 0;
static int YesNo_W = 0;
static int YesNo_H = 0;

static char YesNo_Question[128];

static int YesNo_Selected = 0;
static int YesNo_PrevSelected = 0;

void (*YesNo_Callback)( bool );	// Need a routine that accepts an integer so handling of problems/clashing can be dealt with

bool YesNo_IsActive( void );
void YesNo_Init( word, word, int, int, int, int );
bool YesNo_Activate( char *, void (*ReturningProcedure)( bool ) );
void YesNo_Deactivate( int );
void YesNo_DrawBackground( word );
dword YesNo_KeyHandler( dword );
void YesNo_SetColours( dword, dword, dword, dword, dword );
void YesNo_DrawDetails( word );
void YesNo_DrawItem( word, int, int );
void YesNo_CreateGradients( void );
void YesNo_MemFree( void );

#endif
