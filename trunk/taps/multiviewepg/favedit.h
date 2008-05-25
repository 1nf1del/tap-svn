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

#ifndef __FAVEDIT__
  #define __FAVEDIT__

#define __FAVEDIT_VERSION__ "2008-01-16"

enum {
	FAVEDIT_OK,
	FAVEDIT_CANCEL
};

enum {
	FAVEDIT_SECTIONTITLE,
	FAVEDIT_TERM,
	FAVEDIT_CHANNEL,
	FAVEDIT_DAY,
	FAVEDIT_LOCATION,
	FAVEDIT_START,
	FAVEDIT_END,
	FAVEDIT_PRIORITY,
	FAVEDIT_RPT,
	FAVEDIT_ADJ,
	FAVEDIT_SELECTION,
	FAVEDIT_HELP,
	FAVEDIT_OPTIONS
};

#define FAVEDIT_PADDING DISPLAY_BORDER
#define FAVEDIT_GAP DISPLAY_GAP

// Variables used for colours
static dword FavEdit_Fc = COLOR_White;
static dword FavEdit_HghBc = COLOR_Red;
static dword FavEdit_HghFc = COLOR_White;
static dword FavEdit_Bc = COLOR_Black;
static dword FavEdit_BaseColour = COLOR_Black;
static int FavEdit_Option_H = 30;
static int FavEdit_Option_W = 200;
static bool FavEdit_Active = FALSE;
static word FavEdit_RGN = 0;
static word FavEdit_MemRGN = 0;
static byte* FavEdit_Base = NULL;
static bool FavEdit_Initalised = FALSE;
static int FavEdit_X = 0;
static int FavEdit_Y = 0;
static int FavEdit_W = 0;
static int FavEdit_H = 0;
static byte* FavEdit_ItemBase = NULL;
static byte* FavEdit_ItemHigh = NULL;
static byte* FavEdit_OkItemHigh = NULL;
static byte* FavEdit_CancelItemHigh = NULL;
static int FavEdit_Selected = 0;
static int FavEdit_PrevSelected = 0;
static int FavEdit_OptionSelected = 0;
static favitem FavEdit_Item;

void (*FavEdit_Callback)( favitem, bool );	// Need a routine that accepts an integer so handling of problems/clashing can be dealt with

bool FavEdit_IsActive( void );
void FavEdit_Init( word, word, int, int, int, int );
bool FavEdit_Activate( int, void (*ReturningProcedure)( favitem, bool ) );
void FavEdit_Deactivate( bool );
void FavEdit_DrawBackground( word );
dword FavEdit_KeyHandler( dword );
void FavEdit_SetColours( dword, dword, dword, dword, dword );
void FavEdit_DrawDetails( word );
void FavEdit_DrawItem( word, int, int );
void FavEdit_MemFree( void );
void FavEdit_CreateGradients( void );



#endif
