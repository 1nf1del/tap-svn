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

#ifndef __FAVNEW__
  #define __FAVNEW__

#define __FAVNEW_VERSION__ "2008-01-16"

enum {
	FAVNEW_OK,
	FAVNEW_CANCEL
};

enum {
	FAVNEW_SECTIONTITLE,
	FAVNEW_TERM,
	FAVNEW_CHANNEL,
	FAVNEW_DAY,
	FAVNEW_LOCATION,
	FAVNEW_START,
	FAVNEW_END,
	FAVNEW_PRIORITY,
	FAVNEW_RPT,
	FAVNEW_ADJ,
	FAVNEW_SELECTION,
	FAVNEW_HELP,
	FAVNEW_OPTIONS
};

#define FAVNEW_PADDING DISPLAY_BORDER
#define FAVNEW_GAP DISPLAY_GAP

// Variables used for colours
static dword FavNew_Fc = COLOR_White;
static dword FavNew_HghBc = COLOR_Red;
static dword FavNew_HghFc = COLOR_White;
static dword FavNew_Bc = COLOR_Black;
static dword FavNew_BaseColour = COLOR_Black;
static int FavNew_Option_H = 30;
static int FavNew_Option_W = 200;
static bool FavNew_Active = FALSE;
static word FavNew_RGN = 0;
static word FavNew_MemRGN = 0;
static byte* FavNew_Base = NULL;
static bool FavNew_Initalised = FALSE;
static int FavNew_X = 0;
static int FavNew_Y = 0;
static int FavNew_W = 0;
static int FavNew_H = 0;
static byte* FavNew_ItemBase = NULL;
static byte* FavNew_ItemHigh = NULL;
static byte* FavNew_OkItemHigh = NULL;
static byte* FavNew_CancelItemHigh = NULL;
static int FavNew_Selected = 0;
static int FavNew_PrevSelected = 0;
static int FavNew_OptionSelected = 0;

static favitem FavNew_Item;

void (*FavNew_Callback)( favitem, bool );	// Need a routine that accepts an integer so handling of problems/clashing can be dealt with

bool FavNew_IsActive( void );
void FavNew_Init( word, word, int, int, int, int );
bool FavNew_Activate( void (*ReturningProcedure)( favitem, bool ) );
bool FavNew_ActivateWithItem( favitem item, void (*ReturningProcedure)( favitem, bool ) );
void FavNew_Deactivate( bool );
void FavNew_DrawBackground( word );
dword FavNew_KeyHandler( dword );
void FavNew_SetColours( dword, dword, dword, dword, dword );
void FavNew_DrawDetails( word );
void FavNew_DrawItem( word, int, int );
void FavNew_MemFree( void );
void FavNew_CreateGradients( void );

#endif
