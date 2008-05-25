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

#include <time.h>
#include <stdarg.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "tap.h"

// Required Program Details
#define _PROGRAM_NAME_ "MultiView EPG - 1.0"
#define _AUTHOR_ "Jim16"
#define ID_TAP 0x8026211B

TAP_ID( ID_TAP );
TAP_PROGRAM_NAME( _PROGRAM_NAME_ );
TAP_AUTHOR_NAME( _AUTHOR_ );
TAP_DESCRIPTION( "EPG Views, Timerlist, Fav Manager, Guide Searching, RSS News" );
TAP_ETCINFO( __DATE__ );

// TAP is to be TSR compat
#include "TSRCommander.inc"

// Include all the functions that i use from firebirds library
#include "firebirdlib.h"

// Graphic library taken from Media Manager
#include "gfxlib.c"
#include "graphics.h"

// Include all the .c files
#include "uklogo.c"
#include "fontman.c"
#include "skin.c"
#include "tools.c"
#include "rssnews.c"
#include "yesnobox.c"
#include "threeoption.c"
#include "timerconflict.c"
#include "extinfo.c"
#include "settings.c"
#include "display.c"
#include "guidesortorder.c"
#include "messagebox.c"
#include "timerlist.c"
#include "screentools.c"
#include "favsortorder.c"
#include "timereditsmall.c"
#include "timeredit.c"
#include "timereditfull.c"
#include "recordingedit.c"
#include "timernew.c"
#include "favnew.c"
#include "favedit.c"
#include "timereditnew.c"
#include "favmanage.c"
#include "multilistepg.c"
#include "events.c"
#include "timers.c"
#include "menu.c"
#include "keyboard.c"
#include "guide.c"
#include "gridepg.c"

// TSR Function for exiting
bool TSRCommanderExitTAP (void){
	Events_TAPExit();	// Call the memory freeing routine in events
	RssNews_FreeMemory();
	FavManage_FreeMemory();
	Display_Hide();
	TAP_Channel_Scale( 1, 0, 0, 720, 576, FALSE );
	Settings_Save();
	TAP_Osd_FillBox(0, 0, 0, 720, 576, COLOR_None);
	return TRUE;
}

// TSR Function to access Options menu
void TSRCommanderConfigDialog(){
	Menu_Redraw = TRUE;
	Menu_Min = 0;
	Menu_Max = MENU_ITEMS;
	Menu_Selected = 1;
	Menu_PrevSelected = 1;
	Menu_Render();	// This will automatically build the display
}

// Main Key Handler
dword TAP_KeyHandler( dword param1 ){
	static dword key;
	TSRCommanderWork();
	lastPress = TAP_GetTick();	// Log when the key press occured
	if( param1 == RKEY_Mute ){	// Never catch the mute key as it's annoying if a TAP captures this key and you want to kill the volume. Handle here for ease.
		return param1;
	}
	if( Guide_isActive() ){	// Guide is onscreen at the mo so send the key to its handler
		key = Guide_KeyHandler(param1);
		prevPress = lastPress;
		return key;
	}
	if( Menu_IsActive() ){
		return Menu_Keyhandler(param1);
	}
	if( TimerList_IsActive() ){
		return TimerList_Keyhandler(param1);
	}
	if( RSS_IsActive() ){
		return RSS_Keyhandler(param1);
	}
	if( GridEPG_IsActive() ){
		return GridEPG_Keyhandler(param1);
	}
	if( FavManage_IsActive() ){
		return FavManage_KeyHandler(param1);
	}
	if( MultiList_isActive() ){
		return MultiList_Keyhandler(param1);
	}
	if( Settings_LinearEPGKey != 0 ){
		if( param1 == Settings_LinearEPGKey ){
			if( State_Normal() ){
				if( !isAnyOSDVisible(200,200,10,400) ){	// Fav and nothing visible
					Guide_Activate();
					prevPress = lastPress;
					return 0;
				}
			}
		}
	}
	if( Settings_TimerListKey != 0 ){
		if( param1 == Settings_TimerListKey ){
			if( State_Normal() ){
				if( !isAnyOSDVisible(200,200,10,400) ){	// Fav and nothing visible
					TimerList_Activate();
					prevPress = lastPress;
					return 0;
				}
			}
		}
	}
	if( Settings_RSSListKey != 0 ){
		if( param1 == Settings_RSSListKey ){
			if( State_Normal() ){
				if( !isAnyOSDVisible(200,200,10,400) ){	// Fav and nothing visible
					RSS_Activate();
					prevPress = lastPress;
					return 0;
				}
			}
		}
	}
	if( Settings_GridEPGKey != 0 ){
		if( param1 == Settings_GridEPGKey ){
			if( State_Normal() ){
				if( !isAnyOSDVisible(200,200,10,400) ){	// Fav and nothing visible
					GridEPG_Activate();
					prevPress = lastPress;
					return 0;
				}
			}
		}
	}	
	if( Settings_FavListKey != 0 ){
		if( param1 == Settings_FavListKey ){
			if( State_Normal() ){
				if( !isAnyOSDVisible(200,200,10,400) ){	// Fav and nothing visible
					FavManage_Activate();
					prevPress = lastPress;
					return 0;
				}
			}
		}
	}
	if( Settings_MultiListEPGKey != 0 ){
		if( param1 == Settings_MultiListEPGKey ){
			if( State_Normal() ){
				if( !isAnyOSDVisible(200,200,10,400) ){	// Fav and nothing visible
					MultiList_Activate();
					prevPress = lastPress;
					return 0;
				}
			}
		}
	}
	prevPress = lastPress;
	return param1;
}

//
void TAP_IdleHandler( void ){
	TSRCommanderWork();
	if( Settings_LoadFile ){
		Settings_Load();
		CacheLogos();
	}
	Events_LoadAtStartIdle();
	Events_ReloadHourIdle();
	// Update the recording details
	Update_RecordingInfo(0);
	Update_RecordingInfo(1);
	TAP_GetState( &mainstate, &substate );
	Guide_RecordingNowDisplay();
	Display_DrawTime();
	Guide_SearchStringIdle();
	Message_Idle();
	GridEPG_Idle();
	Guide_Idle();
	TimerList_Idle();
	Keyboard_CursorBlink();
}

//
dword TAP_EventHandler( word event, dword param1, dword param2 ){
	if( event == EVT_KEY ){
		return TAP_KeyHandler( param1 );
	}
	if( event == EVT_IDLE ){
		TAP_IdleHandler();
	}
	return param1;
}

//
int TAP_Main(void){
	TSRCommanderInit( 0, TRUE );	// Initalise TSR compat
	InitTAPex();
	InitTAPAPIFix();				// Initalise Firebirds API fixing for the anooying bugs
	TAP_EnterNormal();				// Enter normal
	setupGraphics();				// Setup the graphic library
	return 1;
}

