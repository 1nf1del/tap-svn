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

#ifndef __GuideSortOrder__
  #define __GuideSortOrder__

#define __GuideSortOrder_VERSION__ "2008-01-16"

#define GuideSortOrder_PADDING DISPLAY_BORDER
#define GuideSortOrder_GAP DISPLAY_GAP

enum {
	GUIDE_SORTTITLE,
	GUIDE_SORTNAME,
	GUIDE_SORTCHANNEL,
	GUIDE_SORTSTART,
	GUIDE_SORTRATING,
	GUIDE_SORTTYPES
};

// Variables used for colours
static dword GuideSortOrder_Fc = COLOR_White;
static dword GuideSortOrder_HghBc = COLOR_Red;
static dword GuideSortOrder_HghFc = COLOR_White;
static dword GuideSortOrder_Bc = COLOR_Black;
static dword GuideSortOrder_BaseColour = COLOR_Black;

static int GuideSortOrder_Option_H = 30;
static int GuideSortOrder_Option_W = 200;
static bool GuideSortOrder_Active = FALSE;
static word GuideSortOrder_RGN = 0;
static word GuideSortOrder_MemRGN = 0;
static byte* GuideSortOrder_Base = NULL;
static bool GuideSortOrder_Initalised = FALSE;
static int GuideSortOrder_X = 0;
static int GuideSortOrder_Y = 0;
static int GuideSortOrder_W = 0;
static int GuideSortOrder_H = 0;

static byte* GuideSortOrder_ItemBase = NULL;
static byte* GuideSortOrder_ItemHigh = NULL;

static char GuideSortOrder_Question[128];

static int GuideSortOrder_Selected = 0;
static int GuideSortOrder_PrevSelected = 0;

void (*GuideSortOrder_Callback)( bool, int );	// Need a routine that accepts an integer so handling of problems/clashing can be dealt with

bool GuideSortOrder_IsActive( void );
void GuideSortOrder_Init( word, word, int, int, int, int );
bool GuideSortOrder_Activate( char *, void (*ReturningProcedure)( bool, int ) );
void GuideSortOrder_Deactivate( bool );
void GuideSortOrder_DrawBackground( word );
dword GuideSortOrder_KeyHandler( dword );
void GuideSortOrder_SetColours( dword, dword, dword, dword, dword );
void GuideSortOrder_DrawDetails( word );
void GuideSortOrder_DrawItem( word, int, int );
void GuideSortOrder_CreateGradients( void );
void GuideSortOrder_MemFree( void );

#endif
