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

#include "timereditfull.h"
#include "fontman.h"
#include "skin.h"
#include "settings.h"
#include "keyboard.h"

bool TimerEditFull_IsActive( void ){
	return TimerEditFull_Active;
}

// Function to initalize the keyboard - region and location to render
void TimerEditFull_Init( word rgn, word memrgn, int x, int y, int w, int h ){
	TimerEditFull_RGN = rgn;	// Store the region to render on
	TimerEditFull_MemRGN = memrgn;
	TimerEditFull_X = x;
	TimerEditFull_Y = y;
	TimerEditFull_W = w;
	TimerEditFull_H = h;
	TimerEditFull_Initalised = TRUE;
	TimerEditFull_Option_H = (TimerEditFull_H/TIMEREDITFULL_OPTIONS)-TIMEREDITFULL_GAP;
	TimerEditFull_Option_W = TimerEditFull_W - (TIMEREDITFULL_PADDING*2);
	TimerEditFull_CreateGradients();
}

void TimerEditFull_MemFree( void ){
	if (TimerEditFull_ItemBase) TAP_MemFree(TimerEditFull_ItemBase);
	if (TimerEditFull_ItemHigh) TAP_MemFree(TimerEditFull_ItemHigh);
	if (TimerEditFull_OkItemHigh) TAP_MemFree(TimerEditFull_OkItemHigh);
	if (TimerEditFull_CancelItemHigh) TAP_MemFree(TimerEditFull_CancelItemHigh);
	TimerEditFull_ItemBase = NULL;
	TimerEditFull_ItemHigh = NULL;
	TimerEditFull_OkItemHigh = NULL;
	TimerEditFull_CancelItemHigh = NULL;
}

void TimerEditFull_CreateGradients( void ){
	word rgn;
	TimerEditFull_MemFree();
	if( Settings_GradientFactor > 0 ){
		rgn	= TAP_Osd_Create(0, 0, TimerEditFull_Option_W, TimerEditFull_Option_H, 0, OSD_Flag_MemRgn);
		// Main default item
		createGradient(rgn, TimerEditFull_Option_W, TimerEditFull_Option_H, Settings_GradientFactor, DISPLAY_ITEM);
		TimerEditFull_ItemBase = TAP_Osd_SaveBox(rgn, 0, 0, TimerEditFull_Option_W, TimerEditFull_Option_H);
		createGradient(rgn, TimerEditFull_Option_W, TimerEditFull_Option_H, Settings_GradientFactor, DISPLAY_ITEMSELECTED);
		TimerEditFull_ItemHigh = TAP_Osd_SaveBox(rgn, 0, 0, TimerEditFull_Option_W, TimerEditFull_Option_H);
		TimerEditFull_CancelItemHigh = TAP_Osd_SaveBox(rgn, 0, 0, (TimerEditFull_Option_W/2)-(TIMEREDITFULL_GAP/2), TimerEditFull_Option_H);
		TimerEditFull_OkItemHigh = TAP_Osd_SaveBox(rgn, 0, 0, (TimerEditFull_Option_W/2)-(TIMEREDITFULL_GAP/2)-1, TimerEditFull_Option_H);
		TAP_Osd_Delete(rgn);
	}
}
// Activate by sending the timer and the number. Used when not using events ala conflict calling
bool TimerEditFull_ActivateWithTimer( TYPE_TimerInfo editTimer, int editNumber, void (*ReturningProcedure)( int, bool ) ){
	if( !TimerEditFull_Initalised ) return FALSE;	// We haven't initalised so gracefully fail
	if( editNumber < 0 ) return FALSE;
	TimerEditFull_Callback = ReturningProcedure;
	// Get the details of the event and create the timer structure
	TimerEditFull_Timer = editTimer;
	TimerEditFull_TimerNum = editNumber;
	StripFileName(TimerEditFull_Timer.fileName);
	TimerEditFull_Selected = 1;
	TimerEditFull_PrevSelected = 1;
	TimerEditFull_OptionSelected = 0;
	// Now lets render the display
	TimerEditFull_DrawBackground(TimerEditFull_MemRGN);
	TimerEditFull_DrawDetails(TimerEditFull_MemRGN);
	// Coppy rendered to foreground
	TAP_Osd_Copy( TimerEditFull_MemRGN, TimerEditFull_RGN, TimerEditFull_X, TimerEditFull_Y, TimerEditFull_W, TimerEditFull_H, TimerEditFull_X, TimerEditFull_Y, FALSE);
	TimerEditFull_Active = TRUE;
	return TRUE;
}

bool TimerEditFull_Activate( int row, void (*ReturningProcedure)( int, bool ) ){
	TYPE_TapEvent event;
	if( !TimerEditFull_Initalised ) return FALSE;	// We haven't initalised so gracefully fail
	TimerEditFull_Callback = ReturningProcedure;
	// Get the details of the event and create the timer structure
	event = Events_GetEvent(row);
	// Get the timer covering this event and store in our variables
	if( TimerEditFull_GetTimer(row)){
		// Remove the .rec from the end of the filename. Will append at the end
		StripFileName(TimerEditFull_Timer.fileName);
		TimerEditFull_Selected = 1;
		TimerEditFull_PrevSelected = 1;
		TimerEditFull_OptionSelected = 0;
		// Now lets render the display
		TimerEditFull_DrawBackground(TimerEditFull_MemRGN);
		TimerEditFull_DrawDetails(TimerEditFull_MemRGN);
		// Coppy rendered to foreground
		TAP_Osd_Copy( TimerEditFull_MemRGN, TimerEditFull_RGN, TimerEditFull_X, TimerEditFull_Y, TimerEditFull_W, TimerEditFull_H, TimerEditFull_X, TimerEditFull_Y, FALSE);
		TimerEditFull_Active = TRUE;
		return TRUE;
	}
	return FALSE;	// Didn't find the timer so say we failed
}

// Routine to handle the closing of the timer window
void TimerEditFull_Deactivate( int retval, bool cancel ){
	TimerEditFull_Active = FALSE;
	// Restore what we had drawn over
	TAP_Osd_RestoreBox(TimerEditFull_RGN, TimerEditFull_X, TimerEditFull_Y, TimerEditFull_W, TimerEditFull_H, TimerEditFull_Base);
	TAP_MemFree(TimerEditFull_Base);
	TimerEditFull_Base = NULL;
	TimerEditFull_MemFree();
	TimerEditFull_Callback(retval, cancel);
}


// Here we will just draw the basic layout of the keyboard
void TimerEditFull_DrawBackground( word rgn ){
	if( TimerEditFull_Base != NULL ) TAP_MemFree(TimerEditFull_Base);
	TimerEditFull_Base = TAP_Osd_SaveBox(TimerEditFull_RGN, TimerEditFull_X, TimerEditFull_Y, TimerEditFull_W, TimerEditFull_H);	// Save what is under the Timer screen
	TAP_Osd_FillBox(rgn, TimerEditFull_X, TimerEditFull_Y, TimerEditFull_W, TimerEditFull_H, TimerEditFull_BaseColour);
}

void TimerEditFull_DrawItem( word rgn, int item, int selected ){
	int fntSize = 0;
	int pad_y;
	dword fc = TimerEditFull_Fc;
	dword bc = TimerEditFull_Bc;
	// Work out the font size allowed for the height of the option
	if( ( TimerEditFull_Option_H - 8 ) > 19  ){
		if( ( TimerEditFull_Option_H - 12 ) > 22 ){
			// Use the big font
			fntSize = FNT_Size_1926;
			pad_y = (TimerEditFull_Option_H/2)-(26/2);
		} else {
			fntSize = FNT_Size_1622;
			pad_y = (TimerEditFull_Option_H/2)-(22/2);
		}
	} else {
		pad_y = (TimerEditFull_Option_H/2)-(19/2);
	}
	// If selected change the colors of the elements
	if( item == selected ){
		fc = TimerEditFull_HghFc;
		bc = TimerEditFull_HghBc;
	}
	switch( item ){
		case TIMEREDITFULL_SECTIONTITLE:
			TAP_Osd_FillBox(rgn, TimerEditFull_X+TIMEREDITFULL_PADDING, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*item), TimerEditFull_Option_W, TimerEditFull_Option_H, TimerEditFull_BaseColour);
			switch( fntSize ){
				case FNT_Size_1419:
					Font_Osd_PutString1419( rgn, TimerEditFull_X+TIMEREDITFULL_PADDING+TIMEREDITFULL_GAP, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*item)+pad_y, TimerEditFull_X+TIMEREDITFULL_PADDING+TimerEditFull_Option_W-TIMEREDITFULL_GAP, "Edit Timer Details", fc, COLOR_None);
					break;
				case FNT_Size_1622:
					Font_Osd_PutString1622( rgn, TimerEditFull_X+TIMEREDITFULL_PADDING+TIMEREDITFULL_GAP, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*item)+pad_y, TimerEditFull_X+TIMEREDITFULL_PADDING+TimerEditFull_Option_W-TIMEREDITFULL_GAP, "Edit Timer Details", fc, COLOR_None);
					break;
				case FNT_Size_1926:
					Font_Osd_PutString1926( rgn, TimerEditFull_X+TIMEREDITFULL_PADDING+TIMEREDITFULL_GAP, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*item)+pad_y, TimerEditFull_X+TIMEREDITFULL_PADDING+TimerEditFull_Option_W-TIMEREDITFULL_GAP, "Edit Timer Details", fc, COLOR_None);
					break;
			}
			break;
		case TIMEREDITFULL_TIMERNAME:
			if( item == selected ){
				if( TimerEditFull_ItemHigh != NULL ){
					TAP_Osd_RestoreBox( rgn, TimerEditFull_X+TIMEREDITFULL_PADDING, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*item), TimerEditFull_Option_W, TimerEditFull_Option_H, TimerEditFull_ItemHigh );
				} else {
					TAP_Osd_FillBox(rgn, TimerEditFull_X+TIMEREDITFULL_PADDING, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*item), TimerEditFull_Option_W, TimerEditFull_Option_H, bc);
				}
			} else {
				if( TimerEditFull_ItemBase != NULL ){
					TAP_Osd_RestoreBox( rgn, TimerEditFull_X+TIMEREDITFULL_PADDING, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*item), TimerEditFull_Option_W, TimerEditFull_Option_H, TimerEditFull_ItemBase );
				} else {
					TAP_Osd_FillBox(rgn, TimerEditFull_X+TIMEREDITFULL_PADDING, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*item), TimerEditFull_Option_W, TimerEditFull_Option_H, bc);
				}
			}
			TAP_Osd_FillBox(rgn, TimerEditFull_X+TIMEREDITFULL_PADDING+TIMEREDITFULL_GAP+120, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*item), TIMEREDITFULL_GAP, TimerEditFull_Option_H, TimerEditFull_BaseColour);
			switch( fntSize ){
				case FNT_Size_1419:
					Font_Osd_PutString1419( rgn, TimerEditFull_X+TIMEREDITFULL_PADDING+TIMEREDITFULL_GAP, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*item)+pad_y, TimerEditFull_X+TIMEREDITFULL_PADDING+TIMEREDITFULL_GAP+120, "File Name", fc, COLOR_None);
					Font_Osd_PutString1419( rgn, TimerEditFull_X+TIMEREDITFULL_PADDING+TIMEREDITFULL_GAP+130, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*item)+pad_y, TimerEditFull_X+TIMEREDITFULL_PADDING+TimerEditFull_Option_W-TIMEREDITFULL_GAP, TimerEditFull_Timer.fileName, fc, COLOR_None);
					break;
				case FNT_Size_1622:
					Font_Osd_PutString1622( rgn, TimerEditFull_X+TIMEREDITFULL_PADDING+TIMEREDITFULL_GAP, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*item)+pad_y, TimerEditFull_X+TIMEREDITFULL_PADDING+TIMEREDITFULL_GAP+120, "File Name", fc, COLOR_None);
					Font_Osd_PutString1622( rgn, TimerEditFull_X+TIMEREDITFULL_PADDING+TIMEREDITFULL_GAP+130, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*item)+pad_y, TimerEditFull_X+TIMEREDITFULL_PADDING+TimerEditFull_Option_W-TIMEREDITFULL_GAP, TimerEditFull_Timer.fileName, fc, COLOR_None);
					break;
				case FNT_Size_1926:
					Font_Osd_PutString1926( rgn, TimerEditFull_X+TIMEREDITFULL_PADDING+TIMEREDITFULL_GAP, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*item)+pad_y, TimerEditFull_X+TIMEREDITFULL_PADDING+TIMEREDITFULL_GAP+120, "File Name", fc, COLOR_None);
					Font_Osd_PutString1926( rgn, TimerEditFull_X+TIMEREDITFULL_PADDING+TIMEREDITFULL_GAP+130, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*item)+pad_y, TimerEditFull_X+TIMEREDITFULL_PADDING+TimerEditFull_Option_W-TIMEREDITFULL_GAP, TimerEditFull_Timer.fileName, fc, COLOR_None);
					break;
			}
			break;
		case TIMEREDITFULL_TIMERSTART:
			if( item == selected ){
				if( TimerEditFull_ItemHigh != NULL ){
					TAP_Osd_RestoreBox( rgn, TimerEditFull_X+TIMEREDITFULL_PADDING, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*item), TimerEditFull_Option_W, TimerEditFull_Option_H, TimerEditFull_ItemHigh );
				} else {
					TAP_Osd_FillBox(rgn, TimerEditFull_X+TIMEREDITFULL_PADDING, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*item), TimerEditFull_Option_W, TimerEditFull_Option_H, bc);
				}
			} else {
				if( TimerEditFull_ItemBase != NULL ){
					TAP_Osd_RestoreBox( rgn, TimerEditFull_X+TIMEREDITFULL_PADDING, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*item), TimerEditFull_Option_W, TimerEditFull_Option_H, TimerEditFull_ItemBase );
				} else {
					TAP_Osd_FillBox(rgn, TimerEditFull_X+TIMEREDITFULL_PADDING, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*item), TimerEditFull_Option_W, TimerEditFull_Option_H, bc);
				}
			}
			TAP_Osd_FillBox(rgn, TimerEditFull_X+TIMEREDITFULL_PADDING+TIMEREDITFULL_GAP+120, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*item), TIMEREDITFULL_GAP, TimerEditFull_Option_H, TimerEditFull_BaseColour);
			switch( fntSize ){
				case FNT_Size_1419:
					Font_Osd_PutString1419( rgn, TimerEditFull_X+TIMEREDITFULL_PADDING+TIMEREDITFULL_GAP, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*item)+pad_y, TimerEditFull_X+TIMEREDITFULL_PADDING+TIMEREDITFULL_GAP+120, "Start Time", fc, COLOR_None);
					Font_Osd_PutString1419( rgn, TimerEditFull_X+TIMEREDITFULL_PADDING+TIMEREDITFULL_GAP+130, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*item)+pad_y, TimerEditFull_X+TIMEREDITFULL_PADDING+TimerEditFull_Option_W-TIMEREDITFULL_GAP, Time_HHMM(TimerEditFull_Timer.startTime), fc, COLOR_None);
					break;
				case FNT_Size_1622:
					Font_Osd_PutString1622( rgn, TimerEditFull_X+TIMEREDITFULL_PADDING+TIMEREDITFULL_GAP, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*item)+pad_y, TimerEditFull_X+TIMEREDITFULL_PADDING+TIMEREDITFULL_GAP+120, "Start Time", fc, COLOR_None);
					Font_Osd_PutString1622( rgn, TimerEditFull_X+TIMEREDITFULL_PADDING+TIMEREDITFULL_GAP+130, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*item)+pad_y, TimerEditFull_X+TIMEREDITFULL_PADDING+TimerEditFull_Option_W-TIMEREDITFULL_GAP, Time_HHMM(TimerEditFull_Timer.startTime), fc, COLOR_None);
					break;
				case FNT_Size_1926:
					Font_Osd_PutString1926( rgn, TimerEditFull_X+TIMEREDITFULL_PADDING+TIMEREDITFULL_GAP, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*item)+pad_y, TimerEditFull_X+TIMEREDITFULL_PADDING+TIMEREDITFULL_GAP+120, "Start Time", fc, COLOR_None);
					Font_Osd_PutString1926( rgn, TimerEditFull_X+TIMEREDITFULL_PADDING+TIMEREDITFULL_GAP+130, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*item)+pad_y, TimerEditFull_X+TIMEREDITFULL_PADDING+TimerEditFull_Option_W-TIMEREDITFULL_GAP, Time_HHMM(TimerEditFull_Timer.startTime), fc, COLOR_None);
					break;
			}
			break;
		case TIMEREDITFULL_TIMEREND:
			if( item == selected ){
				if( TimerEditFull_ItemHigh != NULL ){
					TAP_Osd_RestoreBox( rgn, TimerEditFull_X+TIMEREDITFULL_PADDING, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*item), TimerEditFull_Option_W, TimerEditFull_Option_H, TimerEditFull_ItemHigh );
				} else {
					TAP_Osd_FillBox(rgn, TimerEditFull_X+TIMEREDITFULL_PADDING, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*item), TimerEditFull_Option_W, TimerEditFull_Option_H, bc);
				}
			} else {
				if( TimerEditFull_ItemBase != NULL ){
					TAP_Osd_RestoreBox( rgn, TimerEditFull_X+TIMEREDITFULL_PADDING, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*item), TimerEditFull_Option_W, TimerEditFull_Option_H, TimerEditFull_ItemBase );
				} else {
					TAP_Osd_FillBox(rgn, TimerEditFull_X+TIMEREDITFULL_PADDING, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*item), TimerEditFull_Option_W, TimerEditFull_Option_H, bc);
				}
			}
			TAP_Osd_FillBox(rgn, TimerEditFull_X+TIMEREDITFULL_PADDING+TIMEREDITFULL_GAP+120, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*item), TIMEREDITFULL_GAP, TimerEditFull_Option_H, TimerEditFull_BaseColour);
			switch( fntSize ){
				case FNT_Size_1419:
					Font_Osd_PutString1419( rgn, TimerEditFull_X+TIMEREDITFULL_PADDING+TIMEREDITFULL_GAP, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*item)+pad_y, TimerEditFull_X+TIMEREDITFULL_PADDING+TIMEREDITFULL_GAP+120, "End Time", fc, COLOR_None);
					Font_Osd_PutString1419( rgn, TimerEditFull_X+TIMEREDITFULL_PADDING+TIMEREDITFULL_GAP+130, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*item)+pad_y, TimerEditFull_X+TIMEREDITFULL_PADDING+TimerEditFull_Option_W-TIMEREDITFULL_GAP, Endtime_HHMM(TimerEditFull_Timer.startTime,TimerEditFull_Timer.duration), fc, COLOR_None);
					break;
				case FNT_Size_1622:
					Font_Osd_PutString1622( rgn, TimerEditFull_X+TIMEREDITFULL_PADDING+TIMEREDITFULL_GAP, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*item)+pad_y, TimerEditFull_X+TIMEREDITFULL_PADDING+TIMEREDITFULL_GAP+120, "End Time", fc, COLOR_None);
					Font_Osd_PutString1622( rgn, TimerEditFull_X+TIMEREDITFULL_PADDING+TIMEREDITFULL_GAP+130, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*item)+pad_y, TimerEditFull_X+TIMEREDITFULL_PADDING+TimerEditFull_Option_W-TIMEREDITFULL_GAP, Endtime_HHMM(TimerEditFull_Timer.startTime,TimerEditFull_Timer.duration), fc, COLOR_None);
					break;
				case FNT_Size_1926:
					Font_Osd_PutString1926( rgn, TimerEditFull_X+TIMEREDITFULL_PADDING+TIMEREDITFULL_GAP, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*item)+pad_y, TimerEditFull_X+TIMEREDITFULL_PADDING+TIMEREDITFULL_GAP+120, "End Time", fc, COLOR_None);
					Font_Osd_PutString1926( rgn, TimerEditFull_X+TIMEREDITFULL_PADDING+TIMEREDITFULL_GAP+130, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*item)+pad_y, TimerEditFull_X+TIMEREDITFULL_PADDING+TimerEditFull_Option_W-TIMEREDITFULL_GAP, Endtime_HHMM(TimerEditFull_Timer.startTime,TimerEditFull_Timer.duration), fc, COLOR_None);
					break;
			}
			break;
		case TIMEREDITFULL_RESERVETYPE:
			if( item == selected ){
				if( TimerEditFull_ItemHigh != NULL ){
					TAP_Osd_RestoreBox( rgn, TimerEditFull_X+TIMEREDITFULL_PADDING, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*item), TimerEditFull_Option_W, TimerEditFull_Option_H, TimerEditFull_ItemHigh );
				} else {
					TAP_Osd_FillBox(rgn, TimerEditFull_X+TIMEREDITFULL_PADDING, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*item), TimerEditFull_Option_W, TimerEditFull_Option_H, bc);
				}
			} else {
				if( TimerEditFull_ItemBase != NULL ){
					TAP_Osd_RestoreBox( rgn, TimerEditFull_X+TIMEREDITFULL_PADDING, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*item), TimerEditFull_Option_W, TimerEditFull_Option_H, TimerEditFull_ItemBase );
				} else {
					TAP_Osd_FillBox(rgn, TimerEditFull_X+TIMEREDITFULL_PADDING, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*item), TimerEditFull_Option_W, TimerEditFull_Option_H, bc);
				}
			}
			TAP_Osd_FillBox(rgn, TimerEditFull_X+TIMEREDITFULL_PADDING+TIMEREDITFULL_GAP+120, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*item), TIMEREDITFULL_GAP, TimerEditFull_Option_H, TimerEditFull_BaseColour);
			switch( fntSize ){
				case FNT_Size_1419:
					Font_Osd_PutString1419( rgn, TimerEditFull_X+TIMEREDITFULL_PADDING+TIMEREDITFULL_GAP, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*item)+pad_y, TimerEditFull_X+TIMEREDITFULL_PADDING+TIMEREDITFULL_GAP+120, "Repeat Type", fc, COLOR_None);
					Font_Osd_PutString1419( rgn, TimerEditFull_X+TIMEREDITFULL_PADDING+TIMEREDITFULL_GAP+130, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*item)+pad_y, TimerEditFull_X+TIMEREDITFULL_PADDING+TimerEditFull_Option_W-TIMEREDITFULL_GAP, TimerEditFull_ReserveType(TimerEditFull_Timer.reservationType), fc, COLOR_None);
					break;
				case FNT_Size_1622:
					Font_Osd_PutString1622( rgn, TimerEditFull_X+TIMEREDITFULL_PADDING+TIMEREDITFULL_GAP, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*item)+pad_y, TimerEditFull_X+TIMEREDITFULL_PADDING+TIMEREDITFULL_GAP+120, "Repeat Type", fc, COLOR_None);
					Font_Osd_PutString1622( rgn, TimerEditFull_X+TIMEREDITFULL_PADDING+TIMEREDITFULL_GAP+130, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*item)+pad_y, TimerEditFull_X+TIMEREDITFULL_PADDING+TimerEditFull_Option_W-TIMEREDITFULL_GAP, TimerEditFull_ReserveType(TimerEditFull_Timer.reservationType), fc, COLOR_None);
					break;
				case FNT_Size_1926:
					Font_Osd_PutString1926( rgn, TimerEditFull_X+TIMEREDITFULL_PADDING+TIMEREDITFULL_GAP, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*item)+pad_y, TimerEditFull_X+TIMEREDITFULL_PADDING+TIMEREDITFULL_GAP+120, "Repeat Type", fc, COLOR_None);
					Font_Osd_PutString1926( rgn, TimerEditFull_X+TIMEREDITFULL_PADDING+TIMEREDITFULL_GAP+130, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*item)+pad_y, TimerEditFull_X+TIMEREDITFULL_PADDING+TimerEditFull_Option_W-TIMEREDITFULL_GAP, TimerEditFull_ReserveType(TimerEditFull_Timer.reservationType), fc, COLOR_None);
					break;
			}
			break;		
		case TIMEREDITFULL_DATE:
			if( item == selected ){
				if( TimerEditFull_ItemHigh != NULL ){
					TAP_Osd_RestoreBox( rgn, TimerEditFull_X+TIMEREDITFULL_PADDING, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*item), TimerEditFull_Option_W, TimerEditFull_Option_H, TimerEditFull_ItemHigh );
				} else {
					TAP_Osd_FillBox(rgn, TimerEditFull_X+TIMEREDITFULL_PADDING, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*item), TimerEditFull_Option_W, TimerEditFull_Option_H, bc);
				}
			} else {
				if( TimerEditFull_ItemBase != NULL ){
					TAP_Osd_RestoreBox( rgn, TimerEditFull_X+TIMEREDITFULL_PADDING, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*item), TimerEditFull_Option_W, TimerEditFull_Option_H, TimerEditFull_ItemBase );
				} else {
					TAP_Osd_FillBox(rgn, TimerEditFull_X+TIMEREDITFULL_PADDING, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*item), TimerEditFull_Option_W, TimerEditFull_Option_H, bc);
				}
			}
			TAP_Osd_FillBox(rgn, TimerEditFull_X+TIMEREDITFULL_PADDING+TIMEREDITFULL_GAP+120, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*item), TIMEREDITFULL_GAP, TimerEditFull_Option_H, TimerEditFull_BaseColour);
			switch( fntSize ){
				case FNT_Size_1419:
					Font_Osd_PutString1419( rgn, TimerEditFull_X+TIMEREDITFULL_PADDING+TIMEREDITFULL_GAP, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*item)+pad_y, TimerEditFull_X+TIMEREDITFULL_PADDING+TIMEREDITFULL_GAP+120, "Date", fc, COLOR_None);
					Font_Osd_PutString1419( rgn, TimerEditFull_X+TIMEREDITFULL_PADDING+TIMEREDITFULL_GAP+130, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*item)+pad_y, TimerEditFull_X+TIMEREDITFULL_PADDING+TimerEditFull_Option_W-TIMEREDITFULL_GAP, Time_DOWDDMMSpec(TimerEditFull_Timer.startTime), fc, COLOR_None);
					break;
				case FNT_Size_1622:
					Font_Osd_PutString1622( rgn, TimerEditFull_X+TIMEREDITFULL_PADDING+TIMEREDITFULL_GAP, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*item)+pad_y, TimerEditFull_X+TIMEREDITFULL_PADDING+TIMEREDITFULL_GAP+120, "Date", fc, COLOR_None);
					Font_Osd_PutString1622( rgn, TimerEditFull_X+TIMEREDITFULL_PADDING+TIMEREDITFULL_GAP+130, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*item)+pad_y, TimerEditFull_X+TIMEREDITFULL_PADDING+TimerEditFull_Option_W-TIMEREDITFULL_GAP, Time_DOWDDMMSpec(TimerEditFull_Timer.startTime), fc, COLOR_None);
					break;
				case FNT_Size_1926:
					Font_Osd_PutString1926( rgn, TimerEditFull_X+TIMEREDITFULL_PADDING+TIMEREDITFULL_GAP, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*item)+pad_y, TimerEditFull_X+TIMEREDITFULL_PADDING+TIMEREDITFULL_GAP+120, "Date", fc, COLOR_None);
					Font_Osd_PutString1926( rgn, TimerEditFull_X+TIMEREDITFULL_PADDING+TIMEREDITFULL_GAP+130, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*item)+pad_y, TimerEditFull_X+TIMEREDITFULL_PADDING+TimerEditFull_Option_W-TIMEREDITFULL_GAP, Time_DOWDDMMSpec(TimerEditFull_Timer.startTime), fc, COLOR_None);
					break;
			}
			break;	
			
		case TIMEREDITFULL_RECORD:
			if( item == selected ){
				if( TimerEditFull_ItemHigh != NULL ){
					TAP_Osd_RestoreBox( rgn, TimerEditFull_X+TIMEREDITFULL_PADDING, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*item), TimerEditFull_Option_W, TimerEditFull_Option_H, TimerEditFull_ItemHigh );
				} else {
					TAP_Osd_FillBox(rgn, TimerEditFull_X+TIMEREDITFULL_PADDING, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*item), TimerEditFull_Option_W, TimerEditFull_Option_H, bc);
				}
			} else {
				if( TimerEditFull_ItemBase != NULL ){
					TAP_Osd_RestoreBox( rgn, TimerEditFull_X+TIMEREDITFULL_PADDING, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*item), TimerEditFull_Option_W, TimerEditFull_Option_H, TimerEditFull_ItemBase );
				} else {
					TAP_Osd_FillBox(rgn, TimerEditFull_X+TIMEREDITFULL_PADDING, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*item), TimerEditFull_Option_W, TimerEditFull_Option_H, bc);
				}
			}
			TAP_Osd_FillBox(rgn, TimerEditFull_X+TIMEREDITFULL_PADDING+TIMEREDITFULL_GAP+120, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*item), TIMEREDITFULL_GAP, TimerEditFull_Option_H, TimerEditFull_BaseColour);
			switch( fntSize ){
				case FNT_Size_1419:
					Font_Osd_PutString1419( rgn, TimerEditFull_X+TIMEREDITFULL_PADDING+TIMEREDITFULL_GAP, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*item)+pad_y, TimerEditFull_X+TIMEREDITFULL_PADDING+TIMEREDITFULL_GAP+120, "Record Mode", fc, COLOR_None);
					Font_Osd_PutString1419( rgn, TimerEditFull_X+TIMEREDITFULL_PADDING+TIMEREDITFULL_GAP+130, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*item)+pad_y, TimerEditFull_X+TIMEREDITFULL_PADDING+TimerEditFull_Option_W-TIMEREDITFULL_GAP, TimerEditFull_Timer.isRec == 0?"Watch":"Record", fc, COLOR_None);
					break;
				case FNT_Size_1622:
					Font_Osd_PutString1622( rgn, TimerEditFull_X+TIMEREDITFULL_PADDING+TIMEREDITFULL_GAP, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*item)+pad_y, TimerEditFull_X+TIMEREDITFULL_PADDING+TIMEREDITFULL_GAP+120, "Record Mode", fc, COLOR_None);
					Font_Osd_PutString1622( rgn, TimerEditFull_X+TIMEREDITFULL_PADDING+TIMEREDITFULL_GAP+130, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*item)+pad_y, TimerEditFull_X+TIMEREDITFULL_PADDING+TimerEditFull_Option_W-TIMEREDITFULL_GAP, TimerEditFull_Timer.isRec == 0?"Watch":"Record", fc, COLOR_None);
					break;
				case FNT_Size_1926:
					Font_Osd_PutString1926( rgn, TimerEditFull_X+TIMEREDITFULL_PADDING+TIMEREDITFULL_GAP, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*item)+pad_y, TimerEditFull_X+TIMEREDITFULL_PADDING+TIMEREDITFULL_GAP+120, "Record Mode", fc, COLOR_None);
					Font_Osd_PutString1926( rgn, TimerEditFull_X+TIMEREDITFULL_PADDING+TIMEREDITFULL_GAP+130, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*item)+pad_y, TimerEditFull_X+TIMEREDITFULL_PADDING+TimerEditFull_Option_W-TIMEREDITFULL_GAP, TimerEditFull_Timer.isRec == 0?"Watch":"Record", fc, COLOR_None);
					break;
			}
			break;	
			
		case TIMEREDITFULL_TYPE:
			if( item == selected ){
				if( TimerEditFull_ItemHigh != NULL ){
					TAP_Osd_RestoreBox( rgn, TimerEditFull_X+TIMEREDITFULL_PADDING, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*item), TimerEditFull_Option_W, TimerEditFull_Option_H, TimerEditFull_ItemHigh );
				} else {
					TAP_Osd_FillBox(rgn, TimerEditFull_X+TIMEREDITFULL_PADDING, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*item), TimerEditFull_Option_W, TimerEditFull_Option_H, bc);
				}
			} else {
				if( TimerEditFull_ItemBase != NULL ){
					TAP_Osd_RestoreBox( rgn, TimerEditFull_X+TIMEREDITFULL_PADDING, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*item), TimerEditFull_Option_W, TimerEditFull_Option_H, TimerEditFull_ItemBase );
				} else {
					TAP_Osd_FillBox(rgn, TimerEditFull_X+TIMEREDITFULL_PADDING, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*item), TimerEditFull_Option_W, TimerEditFull_Option_H, bc);
				}
			}
			// Draw black divider
			TAP_Osd_FillBox(rgn, TimerEditFull_X+TIMEREDITFULL_PADDING+TIMEREDITFULL_GAP+120, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*item), TIMEREDITFULL_GAP, TimerEditFull_Option_H, TimerEditFull_BaseColour);
			switch( fntSize ){
				case FNT_Size_1419:
					Font_Osd_PutString1419( rgn, TimerEditFull_X+TIMEREDITFULL_PADDING+TIMEREDITFULL_GAP, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*item)+pad_y, TimerEditFull_X+TIMEREDITFULL_PADDING+TIMEREDITFULL_GAP+120, "Service", fc, COLOR_None);
					Font_Osd_PutString1419( rgn, TimerEditFull_X+TIMEREDITFULL_PADDING+TIMEREDITFULL_GAP+130, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*item)+pad_y, TimerEditFull_X+TIMEREDITFULL_PADDING+TimerEditFull_Option_W-TIMEREDITFULL_GAP, TimerEditFull_Timer.svcType == SVC_TYPE_Tv?"TV":"Radio", fc, COLOR_None);
					break;
				case FNT_Size_1622:
					Font_Osd_PutString1622( rgn, TimerEditFull_X+TIMEREDITFULL_PADDING+TIMEREDITFULL_GAP, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*item)+pad_y, TimerEditFull_X+TIMEREDITFULL_PADDING+TIMEREDITFULL_GAP+120, "Service", fc, COLOR_None);
					Font_Osd_PutString1622( rgn, TimerEditFull_X+TIMEREDITFULL_PADDING+TIMEREDITFULL_GAP+130, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*item)+pad_y, TimerEditFull_X+TIMEREDITFULL_PADDING+TimerEditFull_Option_W-TIMEREDITFULL_GAP, TimerEditFull_Timer.svcType == SVC_TYPE_Tv?"TV":"Radio", fc, COLOR_None);
					break;
				case FNT_Size_1926:
					Font_Osd_PutString1926( rgn, TimerEditFull_X+TIMEREDITFULL_PADDING+TIMEREDITFULL_GAP, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*item)+pad_y, TimerEditFull_X+TIMEREDITFULL_PADDING+TIMEREDITFULL_GAP+120, "Service", fc, COLOR_None);
					Font_Osd_PutString1926( rgn, TimerEditFull_X+TIMEREDITFULL_PADDING+TIMEREDITFULL_GAP+130, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*item)+pad_y, TimerEditFull_X+TIMEREDITFULL_PADDING+TimerEditFull_Option_W-TIMEREDITFULL_GAP, TimerEditFull_Timer.svcType == SVC_TYPE_Tv?"TV":"Radio", fc, COLOR_None);
					break;
			}
			break;
			
		case TIMEREDITFULL_CHANNEL:
			if( item == selected ){
				if( TimerEditFull_ItemHigh != NULL ){
					TAP_Osd_RestoreBox( rgn, TimerEditFull_X+TIMEREDITFULL_PADDING, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*item), TimerEditFull_Option_W, TimerEditFull_Option_H, TimerEditFull_ItemHigh );
				} else {
					TAP_Osd_FillBox(rgn, TimerEditFull_X+TIMEREDITFULL_PADDING, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*item), TimerEditFull_Option_W, TimerEditFull_Option_H, bc);
				}
			} else {
				if( TimerEditFull_ItemBase != NULL ){
					TAP_Osd_RestoreBox( rgn, TimerEditFull_X+TIMEREDITFULL_PADDING, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*item), TimerEditFull_Option_W, TimerEditFull_Option_H, TimerEditFull_ItemBase );
				} else {
					TAP_Osd_FillBox(rgn, TimerEditFull_X+TIMEREDITFULL_PADDING, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*item), TimerEditFull_Option_W, TimerEditFull_Option_H, bc);
				}
			}
			TAP_Osd_FillBox(rgn, TimerEditFull_X+TIMEREDITFULL_PADDING+TIMEREDITFULL_GAP+120, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*item), TIMEREDITFULL_GAP, TimerEditFull_Option_H, TimerEditFull_BaseColour);
			switch( fntSize ){
				case FNT_Size_1419:
					Font_Osd_PutString1419( rgn, TimerEditFull_X+TIMEREDITFULL_PADDING+TIMEREDITFULL_GAP, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*item)+pad_y, TimerEditFull_X+TIMEREDITFULL_PADDING+TIMEREDITFULL_GAP+120, "Channel", fc, COLOR_None);
					Font_Osd_PutString1419( rgn, TimerEditFull_X+TIMEREDITFULL_PADDING+TIMEREDITFULL_GAP+130, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*item)+pad_y, TimerEditFull_X+TIMEREDITFULL_PADDING+TimerEditFull_Option_W-TIMEREDITFULL_GAP, Channel_Name( TimerEditFull_Timer.svcType, TimerEditFull_Timer.svcNum ), fc, COLOR_None);
					break;
				case FNT_Size_1622:
					Font_Osd_PutString1622( rgn, TimerEditFull_X+TIMEREDITFULL_PADDING+TIMEREDITFULL_GAP, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*item)+pad_y, TimerEditFull_X+TIMEREDITFULL_PADDING+TIMEREDITFULL_GAP+120, "Channel", fc, COLOR_None);
					Font_Osd_PutString1622( rgn, TimerEditFull_X+TIMEREDITFULL_PADDING+TIMEREDITFULL_GAP+130, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*item)+pad_y, TimerEditFull_X+TIMEREDITFULL_PADDING+TimerEditFull_Option_W-TIMEREDITFULL_GAP, Channel_Name( TimerEditFull_Timer.svcType, TimerEditFull_Timer.svcNum ), fc, COLOR_None);
					break;
				case FNT_Size_1926:
					Font_Osd_PutString1926( rgn, TimerEditFull_X+TIMEREDITFULL_PADDING+TIMEREDITFULL_GAP, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*item)+pad_y, TimerEditFull_X+TIMEREDITFULL_PADDING+TIMEREDITFULL_GAP+120, "Channel", fc, COLOR_None);
					Font_Osd_PutString1926( rgn, TimerEditFull_X+TIMEREDITFULL_PADDING+TIMEREDITFULL_GAP+130, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*item)+pad_y, TimerEditFull_X+TIMEREDITFULL_PADDING+TimerEditFull_Option_W-TIMEREDITFULL_GAP, Channel_Name( TimerEditFull_Timer.svcType, TimerEditFull_Timer.svcNum ), fc, COLOR_None);
					break;
			}
			break;
			
		case TIMEREDITFULL_DURATION:
			if( item == selected ){
				if( TimerEditFull_ItemHigh != NULL ){
					TAP_Osd_RestoreBox( rgn, TimerEditFull_X+TIMEREDITFULL_PADDING, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*item), TimerEditFull_Option_W, TimerEditFull_Option_H, TimerEditFull_ItemHigh );
				} else {
					TAP_Osd_FillBox(rgn, TimerEditFull_X+TIMEREDITFULL_PADDING, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*item), TimerEditFull_Option_W, TimerEditFull_Option_H, bc);
				}
			} else {
				if( TimerEditFull_ItemBase != NULL ){
					TAP_Osd_RestoreBox( rgn, TimerEditFull_X+TIMEREDITFULL_PADDING, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*item), TimerEditFull_Option_W, TimerEditFull_Option_H, TimerEditFull_ItemBase );
				} else {
					TAP_Osd_FillBox(rgn, TimerEditFull_X+TIMEREDITFULL_PADDING, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*item), TimerEditFull_Option_W, TimerEditFull_Option_H, bc);
				}
			}
			TAP_Osd_FillBox(rgn, TimerEditFull_X+TIMEREDITFULL_PADDING+TIMEREDITFULL_GAP+120, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*item), TIMEREDITFULL_GAP, TimerEditFull_Option_H, TimerEditFull_BaseColour);
			switch( fntSize ){
				case FNT_Size_1419:
					Font_Osd_PutString1419( rgn, TimerEditFull_X+TIMEREDITFULL_PADDING+TIMEREDITFULL_GAP, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*item)+pad_y, TimerEditFull_X+TIMEREDITFULL_PADDING+TIMEREDITFULL_GAP+120, "Duration", fc, COLOR_None);
					Font_Osd_PutString1419( rgn, TimerEditFull_X+TIMEREDITFULL_PADDING+TIMEREDITFULL_GAP+130, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*item)+pad_y, TimerEditFull_X+TIMEREDITFULL_PADDING+TimerEditFull_Option_W-TIMEREDITFULL_GAP, MinsToString(TimerEditFull_Timer.duration), fc, COLOR_None);
					break;
				case FNT_Size_1622:
					Font_Osd_PutString1622( rgn, TimerEditFull_X+TIMEREDITFULL_PADDING+TIMEREDITFULL_GAP, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*item)+pad_y, TimerEditFull_X+TIMEREDITFULL_PADDING+TIMEREDITFULL_GAP+120, "Duration", fc, COLOR_None);
					Font_Osd_PutString1622( rgn, TimerEditFull_X+TIMEREDITFULL_PADDING+TIMEREDITFULL_GAP+130, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*item)+pad_y, TimerEditFull_X+TIMEREDITFULL_PADDING+TimerEditFull_Option_W-TIMEREDITFULL_GAP, MinsToString(TimerEditFull_Timer.duration), fc, COLOR_None);
					break;
				case FNT_Size_1926:
					Font_Osd_PutString1926( rgn, TimerEditFull_X+TIMEREDITFULL_PADDING+TIMEREDITFULL_GAP, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*item)+pad_y, TimerEditFull_X+TIMEREDITFULL_PADDING+TIMEREDITFULL_GAP+120, "Duration", fc, COLOR_None);
					Font_Osd_PutString1926( rgn, TimerEditFull_X+TIMEREDITFULL_PADDING+TIMEREDITFULL_GAP+130, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*item)+pad_y, TimerEditFull_X+TIMEREDITFULL_PADDING+TimerEditFull_Option_W-TIMEREDITFULL_GAP, MinsToString(TimerEditFull_Timer.duration), fc, COLOR_None);
					break;
			}
			break;
			
		case TIMEREDITFULL_TIMERSELECTION:
			if( TimerEditFull_ItemBase != NULL ){
				TAP_Osd_RestoreBox( rgn, TimerEditFull_X+TIMEREDITFULL_PADDING, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*item), TimerEditFull_Option_W, TimerEditFull_Option_H, TimerEditFull_ItemBase );
			} else {
				TAP_Osd_FillBox(rgn, TimerEditFull_X+TIMEREDITFULL_PADDING, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*item), TimerEditFull_Option_W, TimerEditFull_Option_H, bc);
			}
			TAP_Osd_FillBox(rgn, TimerEditFull_X+TIMEREDITFULL_PADDING+(TimerEditFull_Option_W/2)-(TIMEREDITFULL_GAP/2), TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*item), TIMEREDITFULL_GAP-1, TimerEditFull_Option_H, TimerEditFull_BaseColour);
			if( item == selected ){
				if( TimerEditFull_OptionSelected == TIMEREDITFULL_OK ){
					if( TimerEditFull_OkItemHigh != NULL ){
						TAP_Osd_RestoreBox( rgn, TimerEditFull_X+TIMEREDITFULL_PADDING, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*item), (TimerEditFull_Option_W/2)-(TIMEREDITFULL_GAP/2)-1, TimerEditFull_Option_H, TimerEditFull_OkItemHigh );
					} else {
						TAP_Osd_FillBox(rgn, TimerEditFull_X+TIMEREDITFULL_PADDING, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*item), (TimerEditFull_Option_W/2)-(TIMEREDITFULL_GAP/2), TimerEditFull_Option_H, TimerEditFull_HghBc);
					}
				} else {
					if( TimerEditFull_OkItemHigh != NULL ){
						TAP_Osd_RestoreBox( rgn, TimerEditFull_X+TIMEREDITFULL_PADDING+(TimerEditFull_Option_W/2)+(TIMEREDITFULL_GAP/2), TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*item), (TimerEditFull_Option_W/2)-(TIMEREDITFULL_GAP/2)-1, TimerEditFull_Option_H, TimerEditFull_CancelItemHigh );
					} else {
						TAP_Osd_FillBox(rgn, TimerEditFull_X+TIMEREDITFULL_PADDING+(TimerEditFull_Option_W/2)+(TIMEREDITFULL_GAP/2), TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*item), (TimerEditFull_Option_W/2)-(TIMEREDITFULL_GAP/2)-1, TimerEditFull_Option_H, TimerEditFull_HghBc);
					}
				}
				switch( fntSize ){
					case FNT_Size_1419:
						Font_Osd_PutString1419( rgn, TimerEditFull_X+TIMEREDITFULL_PADDING+TIMEREDITFULL_GAP, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*item)+pad_y, TimerEditFull_X+TIMEREDITFULL_PADDING+TimerEditFull_Option_W-TIMEREDITFULL_GAP, "OK", fc, COLOR_None);
						break;
					case FNT_Size_1622:
						Font_Osd_PutString1622( rgn, TimerEditFull_X+TIMEREDITFULL_PADDING+TIMEREDITFULL_GAP, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*item)+pad_y, TimerEditFull_X+TIMEREDITFULL_PADDING+TimerEditFull_Option_W-TIMEREDITFULL_GAP, "OK", fc, COLOR_None);
						break;
					case FNT_Size_1926:
						Font_Osd_PutString1926( rgn, TimerEditFull_X+TIMEREDITFULL_PADDING+TIMEREDITFULL_GAP, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*item)+pad_y, TimerEditFull_X+TIMEREDITFULL_PADDING+TimerEditFull_Option_W-TIMEREDITFULL_GAP, "OK", fc, COLOR_None);
						break;
				}
				switch( fntSize ){
					case FNT_Size_1419:
						Font_Osd_PutString1419( rgn, TimerEditFull_X+TIMEREDITFULL_PADDING+TIMEREDITFULL_GAP+(TimerEditFull_Option_W/2)+(TIMEREDITFULL_GAP/2), TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*item)+pad_y, TimerEditFull_X+TIMEREDITFULL_PADDING+TimerEditFull_Option_W-TIMEREDITFULL_GAP, "Cancel", fc, COLOR_None);
						break;
					case FNT_Size_1622:
						Font_Osd_PutString1622( rgn, TimerEditFull_X+TIMEREDITFULL_PADDING+TIMEREDITFULL_GAP+(TimerEditFull_Option_W/2)+(TIMEREDITFULL_GAP/2), TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*item)+pad_y, TimerEditFull_X+TIMEREDITFULL_PADDING+TimerEditFull_Option_W-TIMEREDITFULL_GAP, "Cancel", fc, COLOR_None);
						break;
					case FNT_Size_1926:
						Font_Osd_PutString1926( rgn, TimerEditFull_X+TIMEREDITFULL_PADDING+TIMEREDITFULL_GAP+(TimerEditFull_Option_W/2)+(TIMEREDITFULL_GAP/2), TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*item)+pad_y, TimerEditFull_X+TIMEREDITFULL_PADDING+TimerEditFull_Option_W-TIMEREDITFULL_GAP, "Cancel", fc, COLOR_None);
						break;
				}	
			} else {
//				TAP_Osd_FillBox(rgn, TimerEditFull_X+TIMEREDITFULL_PADDING, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*item), (TimerEditFull_Option_W/2)-(TIMEREDITFULL_GAP/2)-1, TimerEditFull_Option_H, TimerEditFull_Bc);
//				TAP_Osd_FillBox(rgn, TimerEditFull_X+TIMEREDITFULL_PADDING+(TimerEditFull_Option_W/2)+(TIMEREDITFULL_GAP/2), TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*item), (TimerEditFull_Option_W/2)-(TIMEREDITFULL_GAP/2)-1, TimerEditFull_Option_H, TimerEditFull_Bc);
				switch( fntSize ){
					case FNT_Size_1419:
						Font_Osd_PutString1419( rgn, TimerEditFull_X+TIMEREDITFULL_PADDING+TIMEREDITFULL_GAP, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*item)+pad_y, TimerEditFull_X+TIMEREDITFULL_PADDING+TimerEditFull_Option_W-TIMEREDITFULL_GAP, "OK", fc, COLOR_None);
						break;
					case FNT_Size_1622:
						Font_Osd_PutString1622( rgn, TimerEditFull_X+TIMEREDITFULL_PADDING+TIMEREDITFULL_GAP, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*item)+pad_y, TimerEditFull_X+TIMEREDITFULL_PADDING+TimerEditFull_Option_W-TIMEREDITFULL_GAP, "OK", fc, COLOR_None);
						break;
					case FNT_Size_1926:
						Font_Osd_PutString1926( rgn, TimerEditFull_X+TIMEREDITFULL_PADDING+TIMEREDITFULL_GAP, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*item)+pad_y, TimerEditFull_X+TIMEREDITFULL_PADDING+TimerEditFull_Option_W-TIMEREDITFULL_GAP, "OK", fc, COLOR_None);
						break;
				}
				switch( fntSize ){
					case FNT_Size_1419:
						Font_Osd_PutString1419( rgn, TimerEditFull_X+TIMEREDITFULL_PADDING+TIMEREDITFULL_GAP+(TimerEditFull_Option_W/2)+(TIMEREDITFULL_GAP/2), TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*item)+pad_y, TimerEditFull_X+TIMEREDITFULL_PADDING+TimerEditFull_Option_W-TIMEREDITFULL_GAP, "Cancel", fc, COLOR_None);
						break;
					case FNT_Size_1622:
						Font_Osd_PutString1622( rgn, TimerEditFull_X+TIMEREDITFULL_PADDING+TIMEREDITFULL_GAP+(TimerEditFull_Option_W/2)+(TIMEREDITFULL_GAP/2), TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*item)+pad_y, TimerEditFull_X+TIMEREDITFULL_PADDING+TimerEditFull_Option_W-TIMEREDITFULL_GAP, "Cancel", fc, COLOR_None);
						break;
					case FNT_Size_1926:
						Font_Osd_PutString1926( rgn, TimerEditFull_X+TIMEREDITFULL_PADDING+TIMEREDITFULL_GAP+(TimerEditFull_Option_W/2)+(TIMEREDITFULL_GAP/2), TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*item)+pad_y, TimerEditFull_X+TIMEREDITFULL_PADDING+TimerEditFull_Option_W-TIMEREDITFULL_GAP, "Cancel", fc, COLOR_None);
						break;
				}	
			}
		
			break;
	}
}

char * TimerEditFull_ReserveType( int type ){
	static char text[128];
	switch( type ){
		case RESERVE_TYPE_Onetime:
			sprintf( text, "Onetime" );
			break;
		case RESERVE_TYPE_Everyday:
			sprintf( text, "Everyday" );
			break;
		case RESERVE_TYPE_EveryWeekend:
			sprintf( text, "Weekends" );
			break;
		case RESERVE_TYPE_Weekly:
			sprintf( text, "Weekly" );
			break;
		case RESERVE_TYPE_WeekDay:
			sprintf( text, "Weekday" );
			break;
	}
	return text;
}

void TimerEditFull_KeyboardClose( char * text, bool changed ){
	// Do nothing atm until it is implement
	if( changed ){
		// We have change the text of the timer so modify the fileName field
		memset( TimerEditFull_Timer.fileName, '\0', sizeof(TimerEditFull_Timer.fileName) ); // Blank the filename field
		strncpy(TimerEditFull_Timer.fileName, text, sizeof(TimerEditFull_Timer.fileName));
		TimerEditFull_DrawItem( TimerEditFull_RGN, TimerEditFull_Selected, TimerEditFull_Selected );
	}
}

// Function to draw the timer details to the region
void TimerEditFull_DrawDetails( word rgn ){
	int i;
	for( i = 0; i < TIMEREDITFULL_OPTIONS; i++ ){
		TimerEditFull_DrawItem( rgn, i, TimerEditFull_Selected );
	}
}

int TimerEditFull_Modify( void ){
	strcat(TimerEditFull_Timer.fileName,".rec");
	return TAP_Timer_Modify(TimerEditFull_TimerNum,&TimerEditFull_Timer);
}

dword TimerEditFull_KeyHandler( dword param1 ){
	if( Keyboard_IsActive() ){	// Handle the keyboard
		return Keyboard_Keyhandler(param1);
	}
	if( param1 == RKEY_Exit ){
		TimerEditFull_Deactivate(0,TRUE);
		return 0;
	}
	// Do the navigation stuff now
	if( param1 == RKEY_ChUp ){
		TimerEditFull_PrevSelected = TimerEditFull_Selected;
		TimerEditFull_Selected--;
		if( TimerEditFull_Selected < 1 ){
			TimerEditFull_Selected = TIMEREDITFULL_OPTIONS-1;
		}
		TimerEditFull_DrawItem( TimerEditFull_MemRGN, TimerEditFull_PrevSelected, TimerEditFull_Selected );
		TimerEditFull_DrawItem( TimerEditFull_MemRGN, TimerEditFull_Selected, TimerEditFull_Selected );
		TAP_Osd_Copy( TimerEditFull_MemRGN, TimerEditFull_RGN, TimerEditFull_X+TIMEREDITFULL_PADDING, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*TimerEditFull_PrevSelected), TimerEditFull_Option_W, TimerEditFull_Option_H, TimerEditFull_X+TIMEREDITFULL_PADDING, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*TimerEditFull_PrevSelected), FALSE);
		TAP_Osd_Copy( TimerEditFull_MemRGN, TimerEditFull_RGN, TimerEditFull_X+TIMEREDITFULL_PADDING, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*TimerEditFull_Selected), TimerEditFull_Option_W, TimerEditFull_Option_H, TimerEditFull_X+TIMEREDITFULL_PADDING, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*TimerEditFull_Selected), FALSE);
	}
	// Do the navigation stuff now
	if( param1 == RKEY_ChDown ){
		TimerEditFull_PrevSelected = TimerEditFull_Selected;
		TimerEditFull_Selected++;
		if( TimerEditFull_Selected > TIMEREDITFULL_OPTIONS-1 ){
			TimerEditFull_Selected = 1;
		}
		TimerEditFull_DrawItem( TimerEditFull_MemRGN, TimerEditFull_PrevSelected, TimerEditFull_Selected );
		TimerEditFull_DrawItem( TimerEditFull_MemRGN, TimerEditFull_Selected, TimerEditFull_Selected );
		TAP_Osd_Copy( TimerEditFull_MemRGN, TimerEditFull_RGN, TimerEditFull_X+TIMEREDITFULL_PADDING, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*TimerEditFull_PrevSelected), TimerEditFull_Option_W, TimerEditFull_Option_H, TimerEditFull_X+TIMEREDITFULL_PADDING, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*TimerEditFull_PrevSelected), FALSE);
		TAP_Osd_Copy( TimerEditFull_MemRGN, TimerEditFull_RGN, TimerEditFull_X+TIMEREDITFULL_PADDING, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*TimerEditFull_Selected), TimerEditFull_Option_W, TimerEditFull_Option_H, TimerEditFull_X+TIMEREDITFULL_PADDING, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*TimerEditFull_Selected), FALSE);
	}
	// Do the navigation stuff now
	if( param1 == RKEY_VolDown || param1 == RKEY_VolUp || param1 == RKEY_Forward || param1 == RKEY_Rewind ){
		switch( TimerEditFull_Selected ){
			case TIMEREDITFULL_TIMERSTART:
				TimerEditFull_AlterStartTime(param1);
				TimerEditFull_DrawItem( TimerEditFull_MemRGN, TIMEREDITFULL_TIMEREND, TimerEditFull_Selected );
				TAP_Osd_Copy( TimerEditFull_MemRGN, TimerEditFull_RGN, TimerEditFull_X+TIMEREDITFULL_PADDING, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*TIMEREDITFULL_TIMEREND), TimerEditFull_Option_W, TimerEditFull_Option_H, TimerEditFull_X+TIMEREDITFULL_PADDING, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*TIMEREDITFULL_TIMEREND), FALSE);
				break;
			case TIMEREDITFULL_TIMEREND:
			case TIMEREDITFULL_DURATION:
				TimerEditFull_AlterDuration(param1);
				TimerEditFull_DrawItem( TimerEditFull_MemRGN, TIMEREDITFULL_TIMEREND, TimerEditFull_Selected );
				TimerEditFull_DrawItem( TimerEditFull_MemRGN, TIMEREDITFULL_DURATION, TimerEditFull_Selected );
				TAP_Osd_Copy( TimerEditFull_MemRGN, TimerEditFull_RGN, TimerEditFull_X+TIMEREDITFULL_PADDING, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*TIMEREDITFULL_TIMEREND), TimerEditFull_Option_W, TimerEditFull_Option_H, TimerEditFull_X+TIMEREDITFULL_PADDING, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*TIMEREDITFULL_TIMEREND), FALSE);
				TAP_Osd_Copy( TimerEditFull_MemRGN, TimerEditFull_RGN, TimerEditFull_X+TIMEREDITFULL_PADDING, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*TIMEREDITFULL_DURATION), TimerEditFull_Option_W, TimerEditFull_Option_H, TimerEditFull_X+TIMEREDITFULL_PADDING, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*TIMEREDITFULL_DURATION), FALSE);
				return 0;
				break;
			case TIMEREDITFULL_TIMERSELECTION:
				TimerEditFull_OptionSelected++;
				if( TimerEditFull_OptionSelected > 1 ){
					TimerEditFull_OptionSelected  = 0;
				}
				break;
			case TIMEREDITFULL_RESERVETYPE:
				switch ( param1 ){
					case RKEY_VolUp:
						switch ( TimerEditFull_Timer.reservationType ){
							case RESERVE_TYPE_Onetime: 		TimerEditFull_Timer.reservationType = RESERVE_TYPE_Everyday; break;
							case RESERVE_TYPE_Everyday: 	TimerEditFull_Timer.reservationType = RESERVE_TYPE_EveryWeekend; break;
							case RESERVE_TYPE_EveryWeekend: TimerEditFull_Timer.reservationType = RESERVE_TYPE_Weekly; break;
							case RESERVE_TYPE_Weekly: 		TimerEditFull_Timer.reservationType = RESERVE_TYPE_WeekDay; break;
							case RESERVE_TYPE_WeekDay: 		TimerEditFull_Timer.reservationType = RESERVE_TYPE_Onetime; break;
							default : break;
						}
						break;
					case RKEY_VolDown:
						switch ( TimerEditFull_Timer.reservationType ){
							case RESERVE_TYPE_Onetime: 		TimerEditFull_Timer.reservationType = RESERVE_TYPE_WeekDay; break;
							case RESERVE_TYPE_Everyday: 	TimerEditFull_Timer.reservationType = RESERVE_TYPE_Onetime; break;
							case RESERVE_TYPE_EveryWeekend: TimerEditFull_Timer.reservationType = RESERVE_TYPE_Everyday; break;
							case RESERVE_TYPE_Weekly: 		TimerEditFull_Timer.reservationType = RESERVE_TYPE_EveryWeekend; break;
							case RESERVE_TYPE_WeekDay: 		TimerEditFull_Timer.reservationType = RESERVE_TYPE_Weekly; break;
							default : break;
					}
					break;
					default :
						break;
				}
				break;
			case TIMEREDITFULL_DATE:
				TimerEditFull_AdjustDate( param1 );
				break;
			case TIMEREDITFULL_RECORD:
				TimerEditFull_ToggleRec();
				break;
			case TIMEREDITFULL_TYPE:
				TimerEditFull_ToggleType();
				break;
			case TIMEREDITFULL_CHANNEL:
				TimerEditFull_AlterChannel(param1);
				break;
			
		}
		TimerEditFull_DrawItem( TimerEditFull_MemRGN, TimerEditFull_Selected, TimerEditFull_Selected );
		TAP_Osd_Copy( TimerEditFull_MemRGN, TimerEditFull_RGN, TimerEditFull_X+TIMEREDITFULL_PADDING, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*TimerEditFull_Selected), TimerEditFull_Option_W, TimerEditFull_Option_H, TimerEditFull_X+TIMEREDITFULL_PADDING, TimerEditFull_Y+TIMEREDITFULL_GAP+((TimerEditFull_Option_H+TIMEREDITFULL_GAP)*TimerEditFull_Selected), FALSE);
	}
	if( param1 == RKEY_Ok ){
		switch( TimerEditFull_Selected ){
			case TIMEREDITFULL_TIMERNAME:
				Keyboard_Init( TimerEditFull_RGN, TimerEditFull_MemRGN, TimerEditFull_X, TimerEditFull_Y, TimerEditFull_W, TimerEditFull_H, "Timer Name" );	// Initalise the keyboard
				Keyboard_SetColours( DISPLAY_KEYBOARDITEM, DISPLAY_KEYBOARDITEMTEXT, DISPLAY_KEYBOARDITEMSELECTED, DISPLAY_KEYBOARDITEMSELECTEDTEXT, DISPLAY_KEYBOARDFIELDTEXT, DISPLAY_KEYBOARDFIELD, DISPLAY_KEYBOARDBACKGROUND );
				if( Keyboard_Activate(TimerEditFull_Timer.fileName,&TimerEditFull_KeyboardClose) ){
				}
				break;
			case TIMEREDITFULL_TIMERSELECTION:
				if( TimerEditFull_OptionSelected == TIMEREDITFULL_OK ){
					TimerEditFull_Deactivate(TimerEditFull_Modify(),FALSE);
				} else {
					TimerEditFull_Deactivate(0,TRUE);
				}
				break;
		}
	}
	return 0;
}

void TimerEditFull_AlterChannel( dword key ){
	int count = 0;
	int newSvcNum;
	int max, result;
	int maxTvChannels, maxRadioChannels;
	TYPE_TapChInfo	currentChInfo;
	newSvcNum = TimerEditFull_Timer.svcNum;
	TAP_Channel_GetTotalNum( &maxTvChannels, &maxRadioChannels );
	if ( TimerEditFull_Timer.svcType == SVC_TYPE_Tv) max = maxTvChannels;
	else max = maxRadioChannels;
	count = 0;
	switch ( key ){
		case RKEY_VolUp:								// Channel up
			do {
			    if ( newSvcNum < max ) newSvcNum++;		// skip to the next channel
				else newSvcNum = 1;
				count++;								// if the channel is not selectable, go round again
				result = TAP_Channel_GetInfo( TimerEditFull_Timer.svcType, newSvcNum, &currentChInfo);
			} while ((result == 0) && (count < 3000));	// until either we find a channel that is ok,
			if (result == 0) break;
			TimerEditFull_Timer.svcNum = newSvcNum;
			break;
		case RKEY_VolDown:								// Channel down
			do {
			    if ( newSvcNum > 0 ) newSvcNum--;
				else newSvcNum = max;
				count++;
				result = TAP_Channel_GetInfo( TimerEditFull_Timer.svcType, newSvcNum, &currentChInfo);
			} while ((result == 0) && (count < 3000));
			if (result == 0) break;
			TimerEditFull_Timer.svcNum = newSvcNum;
			break;
	}
}

void TimerEditFull_ToggleType( void ){
	int maxTvChannels, maxRadioChannels;
	TAP_Channel_GetTotalNum( &maxTvChannels, &maxRadioChannels );
	if ( TimerEditFull_Timer.svcType == SVC_TYPE_Tv ){
		TimerEditFull_Timer.svcType = SVC_TYPE_Radio;
		if( maxRadioChannels < 1 ){
			TimerEditFull_Timer.svcType = SVC_TYPE_Tv;
		} else {
			if ( TimerEditFull_Timer.svcNum >= maxRadioChannels ) TimerEditFull_Timer.svcNum = 0;
		}
	} else {
	    TimerEditFull_Timer.svcType = SVC_TYPE_Tv;
		if( maxTvChannels < 1 ){
			TimerEditFull_Timer.svcType = SVC_TYPE_Radio;
		} else {
			if ( TimerEditFull_Timer.svcNum >= maxTvChannels ) TimerEditFull_Timer.svcNum = 0;
		}
	}
}

void TimerEditFull_ToggleRec( void ){
	if(TimerEditFull_Timer.isRec == 1) TimerEditFull_Timer.isRec = 0;
	else TimerEditFull_Timer.isRec = 1;
}

void TimerEditFull_AdjustDate( dword param1 ){
	word 	mjd;
	int 	startTime; // , duration
	startTime = TimerEditFull_Timer.startTime&0xffff;		// extract the time
	mjd = TimerEditFull_Timer.startTime>>16;
	switch ( param1 ){
		case RKEY_VolUp:	mjd++;	break;			// Date up
		case RKEY_VolDown:	mjd--;	break;			// Date down
		default :			break;
	}
	TimerEditFull_Timer.startTime = (mjd<<16) | startTime;
}

void TimerEditFull_AlterDuration( dword param1 ){
	switch( param1 ){
		case RKEY_VolUp:							// Time up
			TimerEditFull_Timer.duration++;
			break;
		case RKEY_VolDown:							// Time down
			if ( TimerEditFull_Timer.duration > 0 ) TimerEditFull_Timer.duration--;
			break;
		case RKEY_Forward:							// Time up
			TimerEditFull_Timer.duration+=60;
			break;
		case RKEY_Rewind:							// Time down
			if ( TimerEditFull_Timer.duration > 60 ) TimerEditFull_Timer.duration-=60;
			break;
		default :
			break;
	}
}

void TimerEditFull_AlterStartTime( dword param1 ){
	word 	mjd;
	int 	min, hour;
	hour = (TimerEditFull_Timer.startTime&0xff00)>>8;			// extract the time
	min = (TimerEditFull_Timer.startTime&0xff);
	mjd = TimerEditFull_Timer.startTime>>16;
	switch ( param1 ){
		case RKEY_VolUp:								// Time up
			if ( min < 59 ){
				min++;
			} else {
			    if ( hour < 23 ) {
				    min = 0;
					hour++;
				} else {
				    min = 0;
					hour = 0;
					mjd++;
				}
			}
			break;
		case RKEY_VolDown:								// Time down
			if ( min > 0 ){
				min--;
			} else {
			    if ( hour > 0 ){
				    min = 59;
					hour--;
				} else {
				    min = 59;
					hour = 23;
					mjd--;
				}
			}
			break;
		case RKEY_Forward:								// Time up
			if ( hour < 23 ){
				hour++;
			} else {
				hour = 0;
				mjd++;
			}
			break;
		case RKEY_Rewind:								// Time down
			if ( hour > 0 ){
				hour--;
			} else {
				hour = 0;
				mjd--;
			}
			break;
		default :
			break;
	}
	TimerEditFull_Timer.startTime = (mjd<<16) | ((hour&0xff)<<8) | (min&0xff);
}

// Function to set the colours of the Timer window
void TimerEditFull_SetColours( dword fc, dword bc, dword fhc, dword bhc, dword base ){
	TimerEditFull_Fc = fc;
	TimerEditFull_HghBc = bhc;
	TimerEditFull_HghFc = fhc;
	TimerEditFull_Bc = bc;
	TimerEditFull_BaseColour = base;
}


// Function to get and set the variable associated with editing the timer
// TO DO - Find the bug that results in a non match being found but timereditsmall works
bool TimerEditFull_GetTimer( int row ){
	TYPE_TimerInfo	currentTimer;
	TYPE_TimerInfo	currentTimerDefault;
	int totalTimers = 0;
	int i, x;
	word year;
	byte month, day, weekDay;
	word year2;
	byte month2, day2, weekDay2;
	int result;
	TYPE_TapChInfo	currentChInfo;
	TYPE_TapEvent event;
	if( row < 0 ) return -1;
	event = Events_GetEvent(row);
	totalTimers = TAP_Timer_GetTotalNum();
	// Store the event day of occurance
	TAP_ExtractMjd( event.startTime>>16, &year2, &month2, &day2, &weekDay2);
	for ( i = 0; i < totalTimers; i++ ){
		TAP_Timer_GetInfo(i, &currentTimer); // Sequentially get timer details and compare with those in the display range
		TAP_Timer_GetInfo(i, &currentTimerDefault); // Sequentially get timer details and compare with those in the display range
		TAP_ExtractMjd( currentTimer.startTime>>16, &year, &month, &day, &weekDay);
		TAP_Channel_GetInfo( currentTimer.svcType, currentTimer.svcNum, &currentChInfo );
		switch( currentTimer.reservationType ){
			case RESERVE_TYPE_Onetime:
				if( (result=Timer_TimerFind( currentChInfo.svcId, Events_GetEventSvcId(row), currentTimer.startTime, AddTime(currentTimer.startTime, currentTimer.duration), event.startTime, event.endTime )) != TIMER_NONE ){
					TimerEditFull_Timer = currentTimerDefault;
					TimerEditFull_TimerNum = i;
					return TRUE;
				}
				break;
			case RESERVE_TYPE_Everyday:
				// Initial check to determine against currentTimer start
				if( (result=Timer_TimerFind( currentChInfo.svcId, Events_GetEventSvcId(row), currentTimer.startTime, AddTime(currentTimer.startTime, currentTimer.duration), event.startTime, event.endTime )) != TIMER_NONE ){
					TimerEditFull_Timer = currentTimerDefault;
					TimerEditFull_TimerNum = i;
					return TRUE;
				}
				// Not found on today so we need to go and increment for the next 7 days and check on weekdays
				for( x = 0; x < TIMERS_MAXDAYSAHEAD; x++ ){
					currentTimer.startTime = AddTime( currentTimer.startTime, 1440 );
					if( (result=Timer_TimerFind( currentChInfo.svcId, Events_GetEventSvcId(row), currentTimer.startTime, AddTime(currentTimer.startTime, currentTimer.duration), event.startTime, event.endTime )) != TIMER_NONE ){
						TimerEditFull_Timer = currentTimerDefault;
						TimerEditFull_TimerNum = i;
						return TRUE;
					}
				}
				break;
			case RESERVE_TYPE_EveryWeekend:
				// if event is not on the weekend we need to check every week day from the start to determine
				// whether event has timer
				if( weekDay2 == SAT || weekDay2 == SUN ){
					// Initial check to determine against currentTimer start
					if( weekDay == SAT || weekDay == SUN  ){	// Sanity check to make sure timer is on a weekend
						if( (result=Timer_TimerFind( currentChInfo.svcId, Events_GetEventSvcId(row), currentTimer.startTime, AddTime(currentTimer.startTime, currentTimer.duration), event.startTime, event.endTime )) != TIMER_NONE ){
							TimerEditFull_Timer = currentTimerDefault;
							TimerEditFull_TimerNum = i;
							return TRUE;
						}
					}
					// Not found on today so we need to go and increment for the next 7 days and check on weekdays
					for( x = 0; x < TIMERS_MAXDAYSAHEAD; x++ ){
						currentTimer.startTime = AddTime( currentTimer.startTime, 1440 );
						TAP_ExtractMjd( currentTimer.startTime>>16, &year, &month, &day, &weekDay);
						if( weekDay == SAT || weekDay == SUN  ){	// Sanity check to make sure timer is on a weekday
							if( (result=Timer_TimerFind( currentChInfo.svcId, Events_GetEventSvcId(row), currentTimer.startTime, AddTime(currentTimer.startTime, currentTimer.duration), event.startTime, event.endTime )) != TIMER_NONE ){
								TimerEditFull_Timer = currentTimerDefault;
								TimerEditFull_TimerNum = i;
								return TRUE;
							}
						}
					}
				}
				break;
			case RESERVE_TYPE_Weekly:
				if( (result=Timer_TimerFind( currentChInfo.svcId, Events_GetEventSvcId(row), currentTimer.startTime, AddTime(currentTimer.startTime, currentTimer.duration), event.startTime, event.endTime )) != TIMER_NONE ){
					TimerEditFull_Timer = currentTimerDefault;
					TimerEditFull_TimerNum = i;
					return TRUE;
				}
				for( x = 0; x < 7; x++ ){	// AddTime only handle 1440 max so loop the adding of the time
					currentTimer.startTime = AddTime(currentTimer.startTime, 1440 );
				}
				// Only search a week ahead atm as data shouldn't be anymore than that
				if( (result=Timer_TimerFind( currentChInfo.svcId, Events_GetEventSvcId(row), currentTimer.startTime, AddTime(currentTimer.startTime, currentTimer.duration), event.startTime, event.endTime )) != TIMER_NONE ){
					TimerEditFull_Timer = currentTimerDefault;
					TimerEditFull_TimerNum = i;
					return TRUE;
				}
				break;
			case RESERVE_TYPE_WeekDay:
				// if event is not on the weekend we need to check every week day from the start to determine
				// whether event has timer
				if( weekDay2 != SAT && weekDay2 != SUN ){
					// Initial check to determine against currentTimer start
					if( weekDay != SAT && weekDay != SUN  ){	// Sanity check to make sure timer is on a weekday
						if( (result=Timer_TimerFind( currentChInfo.svcId, Events_GetEventSvcId(row), currentTimer.startTime, AddTime(currentTimer.startTime, currentTimer.duration), event.startTime, event.endTime )) != TIMER_NONE ){
							TimerEditFull_Timer = currentTimerDefault;
							TimerEditFull_TimerNum = i;
							return TRUE;
						}
					}
					// Not found on today so we need to go and increment for the next 7 days and check on weekdays
					for( x = 0; x < TIMERS_MAXDAYSAHEAD; x++ ){
						currentTimer.startTime = AddTime( currentTimer.startTime, 1440 );
						TAP_ExtractMjd( currentTimer.startTime>>16, &year, &month, &day, &weekDay);
						if( weekDay != SAT && weekDay != SUN  ){	// Sanity check to make sure timer is on a weekday
							if( (result=Timer_TimerFind( currentChInfo.svcId, Events_GetEventSvcId(row), currentTimer.startTime, AddTime(currentTimer.startTime, currentTimer.duration), event.startTime, event.endTime )) != TIMER_NONE ){
								TimerEditFull_Timer = currentTimerDefault;
								TimerEditFull_TimerNum = i;
								return TRUE;
							}
						}
					}
				}
				break;
		}
	}
	return FALSE;
}


