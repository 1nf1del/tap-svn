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

#ifndef __FavSortOrder__
  #define __FavSortOrder__

#define __FavSortOrder_VERSION__ "2008-01-16"

enum {
	QUESTIONLINE,
	SORTFAVPRIORITY,
	SORTFAVNAME,
	FavSortOrderOPTIONS
};

#define FavSortOrder_PADDING DISPLAY_BORDER
#define FavSortOrder_GAP DISPLAY_GAP

// Variables used for colours
static dword FavSortOrder_Fc = COLOR_White;
static dword FavSortOrder_HghBc = COLOR_Red;
static dword FavSortOrder_HghFc = COLOR_White;
static dword FavSortOrder_Bc = COLOR_Black;
static dword FavSortOrder_BaseColour = COLOR_Black;

static int FavSortOrder_Option_H = 30;
static int FavSortOrder_Option_W = 200;
static bool FavSortOrder_Active = FALSE;
static word FavSortOrder_RGN = 0;
static word FavSortOrder_MemRGN = 0;
static byte* FavSortOrder_Base = NULL;
static bool FavSortOrder_Initalised = FALSE;
static int FavSortOrder_X = 0;
static int FavSortOrder_Y = 0;
static int FavSortOrder_W = 0;
static int FavSortOrder_H = 0;

static byte* FavSortOrder_ItemBase = NULL;
static byte* FavSortOrder_ItemHigh = NULL;

static char FavSortOrder_Question[128];

static int FavSortOrder_Selected = 0;
static int FavSortOrder_PrevSelected = 0;

void (*FavSortOrder_Callback)( bool, int );	// Need a routine that accepts an integer so handling of problems/clashing can be dealt with

bool FavSortOrder_IsActive( void );
void FavSortOrder_Init( word, word, int, int, int, int );
bool FavSortOrder_Activate( char *, void (*ReturningProcedure)( bool, int ) );
void FavSortOrder_Deactivate( bool );
void FavSortOrder_DrawBackground( word );
dword FavSortOrder_KeyHandler( dword );
void FavSortOrder_SetColours( dword, dword, dword, dword, dword );
void FavSortOrder_DrawDetails( word );
void FavSortOrder_DrawItem( word, int, int );
void FavSortOrder_CreateGradients( void );
void FavSortOrder_MemFree( void );

#endif
