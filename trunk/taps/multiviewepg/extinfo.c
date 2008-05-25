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

#include "extinfo.h"
#include "fontman.h"
#include "timers.h"
#include "display.h"

// Desc: Routine to return whether the extended information is active
// OUT:
//      bool - Activate status

bool ExtInfo_IsActive( void ){
	return ExtInfo_Showing;
}

// Desc: Routine to initalise the extended information display
// IN:
//     rgn - Display region
//     memrgn - Memory region for drawing
//     x - X position of the extended information
//     y - Y position of the extended information
//     w - Width of the extended information
//     h - Height of the extended information

void ExtInfo_Init( word rgn, word memrgn, int x, int y, int w, int h ){
	ExtInfo_RGN = rgn;
	ExtInfo_MemRGN = memrgn;
	ExtInfo_X = x;
	ExtInfo_Y = y;
	ExtInfo_W = w;
	ExtInfo_H = h;
	ExtInfo_ShowName = FALSE;
	ExtInfo_Initalised = TRUE;
}

// Desc: Routine to initalise the extended information display
// IN:
//     rgn - Display region
//     memrgn - Memory region for drawing
//     x - X position of the extended information
//     y - Y position of the extended information
//     w - Width of the extended information
//     h - Height of the extended information
//     name - Whether to show the name of the event in the top of the display

void ExtInfo_Init2( word rgn, word memrgn, int x, int y, int w, int h, bool name ){
	ExtInfo_RGN = rgn;
	ExtInfo_MemRGN = memrgn;
	ExtInfo_X = x;
	ExtInfo_Y = y;
	ExtInfo_W = w;
	ExtInfo_H = h;
	ExtInfo_ShowName = name;
	ExtInfo_Initalised = TRUE;
}

// Desc: Routine to activate the extended information display
// IN:
//     row - Pointer to event in events[] of event to display the extended information

void ExtInfo_Activate( int row ){
	if( !ExtInfo_Initalised ) return;
	if( row < 0 ) return;
	if( row > eventsPointer ) return;
	ExtInfo_Row = row;
	ExtInfo_RenderDetails(ExtInfo_Row);
}

// Desc: Routine to close the extended information display

void ExtInfo_Close( void ){
	TAP_Osd_RestoreBox(ExtInfo_RGN, ExtInfo_X, ExtInfo_Y, ExtInfo_W, ExtInfo_H, ExtInfo_Base);
	TAP_MemFree(ExtInfo_Base);
	TAP_Osd_Copy( ExtInfo_RGN, ExtInfo_MemRGN, 0, 0, 720, 576, 0, 0, FALSE);	// Copy the main region to memory to avoid corruption from the previous OSD
	ExtInfo_Base = NULL;
	ExtInfo_Showing = FALSE;
	LastWrapPutStr_P = 0;
}

// Desc: Routine to handle key presses when display is active
// IN:
//     param1 - User pressed key
// OUT:
//     dword - Key to return to process that called the key handler

dword ExtInfo_Keyhandler( dword param1 ){
	if( param1 == RKEY_Exit || param1 == RKEY_Info ){	// Close the extended information panel if user presses exit
		ExtInfo_Close();
		return 0;
	}
	if( param1 == RKEY_ChDown || param1 == RKEY_ChUp ){
		ExtInfo_RenderDetails(ExtInfo_Row);
		return 0;
	}
	return 0;
}

// Desc: Routine to render the extended information of teh event. Contains support for description extender
// IN:
//     row - Pointer to event in events[] of event to display the extended information

void ExtInfo_RenderDetails( int row ){
	byte *eventDesc = NULL;
	char *desc;
	int x_add;
	char text[512];
	dword fc = DISPLAY_EVENT;
	dword bc = DISPLAY_ITEM;
	TYPE_TapEvent event;
	x_add = 0;

	// Sanity check
	if( ExtInfo_Y > 575 ) return;
	if( ExtInfo_X > 719 ) return;
	if( ExtInfo_Y < 0 ) return;
	if( ExtInfo_X < 0 ) return;
	
	if( ExtInfo_Base == NULL )
		ExtInfo_Base = TAP_Osd_SaveBox(ExtInfo_RGN, ExtInfo_X, ExtInfo_Y, ExtInfo_W, ExtInfo_H);

	// Seperate the display into sowing event name and just extended information
	if( ExtInfo_ShowName ){
		TAP_Osd_FillBox(ExtInfo_MemRGN, ExtInfo_X, ExtInfo_Y, ExtInfo_W, ExtInfo_H, bc);
		// Check for full or partial and render appropriatly
		// graphics libray needs a visible area when drawing so here we will just add the offset
		switch( Timers_HasTimer( row ) ){
			case TIMER_FULL:
				x_add = (10+10+5);
				break;
			case TIMER_PARTIAL:
				x_add = (10+10+5);
				break;
		}
		event = Events_GetEvent(row);
		// Render the name and the rating
		TAP_SPrint( text, "%s\0",event.eventName);	// Get the channel name and place in local text array for printing
		if( strlen(text) > 25 ){
			Font_Osd_PutString1419( ExtInfo_MemRGN, (ExtInfo_X+DISPLAY_GAP+DISPLAY_GAP)+x_add, ExtInfo_Y+DISPLAY_GAP+((28)*0)+3, ExtInfo_X+ExtInfo_W-(DISPLAY_BORDER*2), text, fc, bc );
		} else {
			Font_Osd_PutString1622( ExtInfo_MemRGN, (ExtInfo_X+DISPLAY_GAP+DISPLAY_GAP)+x_add, ExtInfo_Y+DISPLAY_GAP+((28)*0), ExtInfo_X+ExtInfo_W-(DISPLAY_BORDER*2), text, fc, bc );
		}
		// Description Extender supprt
		if( PatchIsInstalled((dword*) 0x80000000, "De") || strlen(event.description) > 128 ){
			// Description extender is present so use the eventname + event description for the extended info
			desc = event.eventName + event.description[127];
			strcpy(text,desc);
			if( strlen(text) < 1 ){
				WrapPutStr(ExtInfo_MemRGN, "No description available", ExtInfo_X+DISPLAY_GAP+DISPLAY_GAP, ExtInfo_Y+DISPLAY_GAP+((28)*1), ExtInfo_W-(DISPLAY_BORDER*2), fc, bc, 12, FNT_Size_1419, DISPLAY_GAP);
			} else {
				WrapPutStr(ExtInfo_MemRGN, text, ExtInfo_X+DISPLAY_GAP+DISPLAY_GAP, ExtInfo_Y+DISPLAY_GAP+((28)*1), ExtInfo_W-(DISPLAY_BORDER*2), fc, bc, 12, FNT_Size_1419, DISPLAY_GAP);
			}
		} else {
			// No description extender support so use the default extended information
			eventDesc = TAP_EPG_GetExtInfo( &event );	
			if( eventDesc ){
				WrapPutStr_StartPos = LastWrapPutStr_P;
				WrapPutStr(ExtInfo_MemRGN, eventDesc, ExtInfo_X+DISPLAY_GAP+DISPLAY_GAP, ExtInfo_Y+DISPLAY_GAP+((28)*1), ExtInfo_W-(DISPLAY_BORDER*2), fc, bc, 12, FNT_Size_1419, DISPLAY_GAP);
				TAP_MemFree(eventDesc);
			} else {
				WrapPutStr(ExtInfo_MemRGN, "No extended information.", ExtInfo_X+DISPLAY_GAP+DISPLAY_GAP, ExtInfo_Y+DISPLAY_GAP+((28)*1), ExtInfo_W-(DISPLAY_BORDER*2), fc, bc, 12, FNT_Size_1419, DISPLAY_GAP);
			}
		}
		TAP_Osd_Copy( ExtInfo_MemRGN, ExtInfo_RGN, ExtInfo_X, ExtInfo_Y, ExtInfo_W, ExtInfo_H, ExtInfo_X, ExtInfo_Y, FALSE);
		// Check for full or partial and render appropriatly.
		switch( Timers_HasTimer( row ) ){
			case TIMER_FULL:
				drawDisk(ExtInfo_RGN, ExtInfo_X+5+10, ExtInfo_Y+10+5, 10, DISPLAY_RECORDINGFULL);
				break;
			case TIMER_PARTIAL:
				drawDisk(ExtInfo_RGN, ExtInfo_X+5+10, ExtInfo_Y+10+5, 10, DISPLAY_RECORDINGPARTIAL);
				break;
		}
	} else {
		TAP_Osd_FillBox(ExtInfo_MemRGN, ExtInfo_X, ExtInfo_Y, ExtInfo_W, ExtInfo_H, bc);
		// Check for full or partial and render appropriatly
		event = Events_GetEvent(row);
		// Description Extender supprt
		if( PatchIsInstalled((dword*) 0x80000000, "De") || strlen(event.description) > 128 ){
			// Description extender is present so use the eventname + event description for the extended info
			desc = event.eventName + event.description[127];
			strcpy(text,desc);
			if( strlen(text) < 1 ){
				WrapPutStr(ExtInfo_MemRGN, "No description available", ExtInfo_X+DISPLAY_GAP+DISPLAY_GAP, ExtInfo_Y+DISPLAY_GAP+((28)*0), ExtInfo_W-(DISPLAY_BORDER*2), fc, bc, 10, FNT_Size_1622, DISPLAY_GAP);
			} else {
				WrapPutStr(ExtInfo_MemRGN, text, ExtInfo_X+DISPLAY_GAP+DISPLAY_GAP, ExtInfo_Y+DISPLAY_GAP+((28)*0), ExtInfo_W-(DISPLAY_BORDER*2), fc, bc, 10, FNT_Size_1622, DISPLAY_GAP);
			}
		} else {
			// No description extender support so use the default extended information
			eventDesc = TAP_EPG_GetExtInfo( &event );	
			if( eventDesc ){
				WrapPutStr_StartPos = LastWrapPutStr_P;
				WrapPutStr(ExtInfo_MemRGN, eventDesc, ExtInfo_X+DISPLAY_GAP+DISPLAY_GAP, ExtInfo_Y+DISPLAY_GAP+((28)*0), ExtInfo_W-(DISPLAY_BORDER*2), fc, bc, 10, FNT_Size_1622, DISPLAY_GAP);
				TAP_MemFree(eventDesc);
			} else {
				WrapPutStr(ExtInfo_MemRGN, "No extended information.", ExtInfo_X+DISPLAY_GAP+DISPLAY_GAP, ExtInfo_Y+DISPLAY_GAP+((28)*0), ExtInfo_W-(DISPLAY_BORDER*2), fc, bc, 10, FNT_Size_1622, DISPLAY_GAP);
			}
		}
		TAP_Osd_Copy( ExtInfo_MemRGN, ExtInfo_RGN, ExtInfo_X, ExtInfo_Y, ExtInfo_W, ExtInfo_H, ExtInfo_X, ExtInfo_Y, FALSE);
	}
	ExtInfo_Showing = TRUE;
}
