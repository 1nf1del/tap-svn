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

#ifndef __ThreeOption__
  #define __ThreeOption__

#define __ThreeOption_VERSION__ "2008-01-16"

enum {
	THREEOPTION_QUESTION,
	THREEOPTION_OPTION1,
	THREEOPTION_OPTION2,
	THREEOPTION_NO,
	ThreeOptionOPTIONS
};

#define ThreeOption_PADDING DISPLAY_BORDER
#define ThreeOption_GAP DISPLAY_GAP

// Variables used for colours
static dword ThreeOption_Fc = COLOR_White;
static dword ThreeOption_HghBc = COLOR_Red;
static dword ThreeOption_HghFc = COLOR_White;
static dword ThreeOption_Bc = COLOR_Black;
static dword ThreeOption_BaseColour = COLOR_Black;

static byte* ThreeOption_ItemBase = NULL;
static byte* ThreeOption_ItemHigh = NULL;

static int ThreeOption_Option_H = 30;
static int ThreeOption_Option_W = 200;
static bool ThreeOption_Active = FALSE;
static word ThreeOption_RGN = 0;
static word ThreeOption_MemRGN = 0;
static byte* ThreeOption_Base = NULL;
static bool ThreeOption_Initalised = FALSE;
static int ThreeOption_X = 0;
static int ThreeOption_Y = 0;
static int ThreeOption_W = 0;
static int ThreeOption_H = 0;

static char ThreeOption_Question[128];
static char ThreeOption_Option1[128];
static char ThreeOption_Option2[128];

static int ThreeOption_Selected = 0;
static int ThreeOption_PrevSelected = 0;

void (*ThreeOption_Callback)( int );	// Need a routine that accepts an integer so handling of problems/clashing can be dealt with

bool ThreeOption_IsActive( void );
void ThreeOption_Init( word, word, int, int, int, int );
bool ThreeOption_Activate( char * question, char * option1, char * option2, int selected, void (*ReturningProcedure)( int option ) );
void ThreeOption_Deactivate( int );
void ThreeOption_DrawBackground( word );
dword ThreeOption_KeyHandler( dword );
void ThreeOption_SetColours( dword, dword, dword, dword, dword );
void ThreeOption_DrawDetails( word );
void ThreeOption_DrawItem( word, int, int );
void ThreeOption_CreateGradients( void );
void ThreeOption_MemFree( void );

#endif
