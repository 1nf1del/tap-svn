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

#ifndef __DISPLAYSTUFF__
  #define __DISPLAYSTUFF__

#define __DISPLAYSTUFF_VERSION__ "2008-01-15"

#include "settings.h"

// Colour definitions. When skinning is implemented these will become variables rather than definitions
#define DISPLAY_BACKGROUND COLOR_DarkBlue
#define DISPLAY_MAIN COLOR_Black
#define DISPLAY_ITEM RGB8888(90,104,213)
#define DISPLAY_ITEMSELECTED RGB8888(42,158,0)
#define DISPLAY_NOWEVENT COLOR_Yellow
#define DISPLAY_EVENT RGB8888(200,200,200)
#define DISPLAY_EVENTPAST COLOR_Black
#define DISPLAY_TITLETEXT COLOR_Gray
#define DISPLAY_RECORDINGFULL COLOR_Red
#define DISPLAY_RECORDINGPARTIAL RGB8888(255,180,0)
#define DISPLAY_RECORDINGALT COLOR_Yellow
#define DISPLAY_NOWEVENTTEXT COLOR_Yellow
#define DISPLAY_NOWEVENTBACKGROUND COLOR_Black
#define DISPLAY_CHANNELNOTAVAL COLOR_Gray
#define DISPLAY_EPGTIMES COLOR_Yellow
#define DISPLAY_EPGREC	COLOR_Red
#define DISPLAY_EPGCURRENTREC	COLOR_Red
#define DISPLAY_KEYBOARDITEM RGB8888(90,104,213)
#define DISPLAY_KEYBOARDITEMTEXT RGB8888(200,200,200)
#define DISPLAY_KEYBOARDITEMSELECTEDTEXT RGB8888(200,200,200)
#define DISPLAY_KEYBOARDFIELDTEXT RGB8888(200,200,200)
#define DISPLAY_KEYBOARDFIELD RGB8888(90,104,213)
#define DISPLAY_KEYBOARDITEMSELECTED RGB8888(42,158,0)
#define DISPLAY_KEYBOARDBACKGROUND COLOR_Black

// Define global positions and widths
#define DISPLAY_X 55
#define DISPLAY_Y 45
#define DISPLAY_W 720-(DISPLAY_X*2)
#define DISPLAY_H 576-(DISPLAY_Y*2)
#define DISPLAY_CORNER 12
#define DISPLAY_BORDER 4
#define DISPLAY_HEADER 45
#define DISPLAY_GAP 3
#define DISPLAY_ITEM_H 28
#define DISPLAY_ITEM_W 330

// Display variables
static int DISPLAY_X_OFFSET = 0;
static int DISPLAY_Y_OFFSET = 0;
static int Panel_Left_X = 0;
static int Panel_Left_Y = 0;
static int DISPLAY_ROW_WIDTH = 610;
static int Panel_Top_X = 0;
static int Panel_Top_Y = 0;
static int Panel_Top_W = 0;
static int Panel_Top_H = 0;
static int Panel_Bottom_X = 0;
static int Panel_Bottom_Y = 0;
static int Panel_Bottom_W = 0;
static int Panel_Bottom_H = 0;
static bool Display_Initalised = FALSE;
static bool Display_ClockEnabled = TRUE;
static word Display_RGN = 0;
static word Display_MemRGN = 0;
static bool Display_BackgroundDrawn = FALSE;
static bool Display_TitleDrawn = FALSE;

// Function defines
void Display_CreateRegions( void );
void Display_DrawBackground( void );
void Display_DrawBackgroundSize( int );
void Display_DrawTime( void );
void Display_Init( void );
void Display_Hide( void );
void Display_PIGRestore( void );
void Display_PIGScale( void );
void Display_RenderTitle( char * );

#endif
