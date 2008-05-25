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

#ifndef __MENUSTUFF__
  #define __MENUSTUFF__

#define __MENUSTUFF_VERSION__ "2008-01-15"

#define MENU_ITEMS 11

// MAIN MENU FOR CONFIGURATIONS
static bool Menu_Active = FALSE;
static bool Menu_Redraw = TRUE;
static int Menu_Min = 0;
static int Menu_Max = MENU_ITEMS;
static int Menu_Selected = 0;
static int Menu_PrevSelected = 0;

enum {
	MAIN_OPTIONS,
	RENDERBACKGROUND,
	MESSAGE_TIMEOUT,
	RELOADHOUR,
	LOADDELAY,
	FADE_IN,
	FADE_OUT,
	GRADIENT_FACTOR,
	XOFFSET,
	YOFFSET,
	TIMERPADDINGSTART,
	TIMERPADDINGEND,
	APPENDEPISODE,
	TIMERRENAME,
	
	LINEAREPG_OPTIONS,
	LINEAREPGKEY,
	DEFAULT_SORT,
	SHOW_PARTIAL,
	SHOW_TIMES,
	PIG,
	SEARCH_DELAY,

	GRIDEPG_OPTIONS,
	GRIDEPGKEY,
	GRIDEPGSIZE,
	GRIDEPGCHANNELS,
	GRIDEPGLOGOS,
	
	MULTILIST_OPTIONS,
	MULTILISTKEY,

	TIMERLIST_OPTIONS,
	TIMERLISTKEY,
	TIMERLISTMODE,
	TIMERLISTLOGOS,

	RSSNEWS_OPTIONS,
	RSSLISTKEY,

	FAVMANGE_OPTIONS,
	FAVLISTKEY,

	TAP_EXIT,
	MENU_OPTIONS
};

static byte* Menu_ItemBase = NULL;
static byte* Menu_ItemHigh = NULL;

// Here i'm copy the values from the GUIDE but allowing for any changes that are need
#define MENU_ITEM_W DISPLAY_ITEM_W
#define MENU_ITEM_H DISPLAY_ITEM_H
#define MENU_ITEM DISPLAY_ITEM
#define MENU_SELECTEDITEM DISPLAY_ITEMSELECTED
#define MENU_MAIN DISPLAY_MAIN
#define MENU_OPTION_W 120
#define MENU_GAP DISPLAY_GAP

bool Menu_IsActive( void );
void Menu_Hide( void );
void Menu_RenderItem( word, int, char *, char *, int );
void Menu_OptionInfo( word, char * );
void Menu_ItemDesc( word, int );
void Guide_MenuItem( word, int, int );
void Menu_Render( void );
dword Menu_Keyhandler( dword );
void Menu_MemFree( void );
void Menu_CreateGradients( void );
void Menu_RenderSectionTitle( word rgn, int position, char *option );
void Menu_RenderExisting( void );

#endif
