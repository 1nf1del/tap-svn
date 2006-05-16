/************************************************************
				Part of the ukEPG project/*
	Copyright (C) 2005-2006 Darkmatter

	This file is part of the TAPs for Topfield PVRs project.
		http://tap.berlios.de/

	This is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	The software is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this software; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

// This module handles the definition of all common items


//-----------------
// Dependent files
//
#include "Menu.inc"
#include "RemoteKeys.inc"


//-----------------
// Common Graphics
//
#include "gif.h"
#include "graphics/rowA.gd"
#include "graphics/rowB.gd"
//#include "graphics/rowC.gd"
//#include "graphics/row.gd"
#include "graphics/highlight.gd"
#include "graphics/redcircle.gd"
#include "graphics/greencircle.gd"
#include "graphics/top.gd"
#include "graphics/side.gd"
#include "graphics/timeBar.gd"

#include "graphics/smallGreenBar.gd"

#include "graphics/MenuRow.gd"
#include "graphics/MenuRow_Highlight.gd"
#include "graphics/Menu_Title.gd"


//#define FILL_COLOUR RGB(0,0,102)
#define FILL_COLOUR COLOR_Black
#define MAIN_TEXT_COLOUR RGB(29,29,29)
#define TITLE_COLOUR RGB(18,18,18)
#define INFO_COLOUR RGB(18,18,18)
#define TIME_COLOUR RGB(4,4,4)
#define CALENDAR_BACKGROUND_COLOUR COLOR_User9
#define CALENDAR_DAY_TEXT MAIN_TEXT_COLOUR
//#define COLOR_White RGB(31,31,31)
//#define COLOR_OffWhite RGB(29,29,29)
//#define COLOR_LightGray RGB(24,24,24)
//#define COLOR_Gray RGB(16,16,16)



//*****************
// Prototypes
//*****************
//
void GenerateExitRequest( void );				// UkTimers.c
void DrawGraphicBoarders(void);

void MakeNewTimer( byte chosenWeekDay );		// TimerEdit.c

int ActivateTimerSaveRestore( void );			// TimerSaveRestore.c
void ReadTimerFile( void );
void GenerateTimerFile( void );

void ActivateMenu(void);						// MainMenu.c
void RedrawMainMenu( void );
void TerminateMenu( void );

void DisplayConfigWindow( void );				// ConfigMenu.c

void FormatNameString( char *source, int *index, char *dest, int width);		// logo.c

void SaveConfigurationToFile( void );			// IniFile.c

struct _reent *_impure_ptr; 					// need this declared for reentrant functions in ANSI C library [comment by Sunstealer]



//*****************
//global variables
//*****************
//
static dword mainActivationKey;
static TYPE_ModelType unitModelType;

static bool exitFlag;
static bool terminateFlag;
static word rgn;														// one region used for all our graphics

static bool editWindowShowing;
static bool channelListWindowShowing;
static bool calendarWindowShowing;
static bool timeEditExitWindowShowing;
static bool creditsShowing;
static bool menuShowing;
static bool configWindowShowing;
static bool keyboardWindowShowing;

