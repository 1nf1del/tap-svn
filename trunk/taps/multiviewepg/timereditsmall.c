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

#include "timereditsmall.h"
#include "fontman.h"
#include "timers.h"
#include "keyboard.h"
#include "skin.h"
#include "settings.h"

bool TimerEditSmall_IsActive( void ){
	return TimerEditSmall_Active;
}

// Function to initalize the keyboard - region and location to render
void TimerEditSmall_Init( word rgn, word memrgn, int x, int y, int w, int h ){
	TimerEditSmall_RGN = rgn;	// Store the region to render on
	TimerEditSmall_MemRGN = memrgn;
	TimerEditSmall_X = x;
	TimerEditSmall_Y = y;
	TimerEditSmall_W = w;
	TimerEditSmall_H = h;
	TimerEditSmall_Initalised = TRUE;
	TimerEditSmall_Option_H = (TimerEditSmall_H/TIMEREDITSMALL_OPTIONS)-TIMEREDITSMALL_GAP-1;
	TimerEditSmall_Option_W = TimerEditSmall_W - (TIMEREDITSMALL_PADDING*2);
	TimerEditSmall_CreateGradients();
}

void TimerEditSmall_MemFree( void ){
	if (TimerEditSmall_ItemBase) TAP_MemFree(TimerEditSmall_ItemBase);
	if (TimerEditSmall_ItemHigh) TAP_MemFree(TimerEditSmall_ItemHigh);
	if (TimerEditSmall_OkItemHigh) TAP_MemFree(TimerEditSmall_OkItemHigh);
	if (TimerEditSmall_CancelItemHigh) TAP_MemFree(TimerEditSmall_CancelItemHigh);
	TimerEditSmall_ItemBase = NULL;
	TimerEditSmall_ItemHigh = NULL;
	TimerEditSmall_OkItemHigh = NULL;
	TimerEditSmall_CancelItemHigh = NULL;
}

void TimerEditSmall_CreateGradients( void ){
	word rgn;
	TimerEditSmall_MemFree();
	if( Settings_GradientFactor > 0 ){
		rgn	= TAP_Osd_Create(0, 0, TimerEditSmall_Option_W, TimerEditSmall_Option_H, 0, OSD_Flag_MemRgn);
		// Main default item
		createGradient(rgn, TimerEditSmall_Option_W, TimerEditSmall_Option_H, Settings_GradientFactor, DISPLAY_ITEM);
		TimerEditSmall_ItemBase = TAP_Osd_SaveBox(rgn, 0, 0, TimerEditSmall_Option_W, TimerEditSmall_Option_H);
		createGradient(rgn, TimerEditSmall_Option_W, TimerEditSmall_Option_H, Settings_GradientFactor, DISPLAY_ITEMSELECTED);
		TimerEditSmall_ItemHigh = TAP_Osd_SaveBox(rgn, 0, 0, TimerEditSmall_Option_W, TimerEditSmall_Option_H);
		TimerEditSmall_CancelItemHigh = TAP_Osd_SaveBox(rgn, 0, 0, (TimerEditSmall_Option_W/2)-(TIMEREDITSMALL_GAP/2), TimerEditSmall_Option_H);
		TimerEditSmall_OkItemHigh = TAP_Osd_SaveBox(rgn, 0, 0, (TimerEditSmall_Option_W/2)-(TIMEREDITSMALL_GAP/2)-1, TimerEditSmall_Option_H);
		TAP_Osd_Delete(rgn);
	}
}

// Activate by sending the timer and the number. Used when not using events ala conflict calling
bool TimerEditSmall_ActivateWithTimer( TYPE_TimerInfo editTimer, int editNumber, void (*ReturningProcedure)( int, bool ) ){
	if( !TimerEditSmall_Initalised ) return FALSE;	// We haven't initalised so gracefully fail
	TimerEditSmall_Callback = ReturningProcedure;
	// Get the details of the event and create the timer structure
	TimerEditSmall_Timer = editTimer;
	TimerEditSmall_TimerNum = editNumber;
	StripFileName(TimerEditSmall_Timer.fileName);
	TimerEditSmall_Selected = 1;
	TimerEditSmall_PrevSelected = 1;
	TimerEditSmall_OptionSelected = 0;
	// Now lets render the display
	TimerEditSmall_DrawBackground(TimerEditSmall_MemRGN);
	TimerEditSmall_DrawDetails(TimerEditSmall_MemRGN);
	// Coppy rendered to foreground
	TAP_Osd_Copy( TimerEditSmall_MemRGN, TimerEditSmall_RGN, TimerEditSmall_X, TimerEditSmall_Y, TimerEditSmall_W, TimerEditSmall_H, TimerEditSmall_X, TimerEditSmall_Y, FALSE);
	TimerEditSmall_Active = TRUE;
	return TRUE;
}

bool TimerEditSmall_Activate( int row, void (*ReturningProcedure)( int, bool ) ){
	TYPE_TapEvent event;
	if( !TimerEditSmall_Initalised ) return FALSE;	// We haven't initalised so gracefully fail
	TimerEditSmall_Callback = ReturningProcedure;
	// Get the details of the event and create the timer structure
	event = Events_GetEvent(row);
	// Get the timer covering this event and store in our variables
	if( TimerEditSmall_GetTimer(row)){
		// Remove the .rec from the end of the filename. Will append at the end
		StripFileName(TimerEditSmall_Timer.fileName);
		TimerEditSmall_Selected = 1;
		TimerEditSmall_PrevSelected = 1;
		TimerEditSmall_OptionSelected = 0;
		// Now lets render the display
		TimerEditSmall_DrawBackground(TimerEditSmall_MemRGN);
		TimerEditSmall_DrawDetails(TimerEditSmall_MemRGN);
		// Coppy rendered to foreground
		TAP_Osd_Copy( TimerEditSmall_MemRGN, TimerEditSmall_RGN, TimerEditSmall_X, TimerEditSmall_Y, TimerEditSmall_W, TimerEditSmall_H, TimerEditSmall_X, TimerEditSmall_Y, FALSE);
		TimerEditSmall_Active = TRUE;
		return TRUE;
	}
	return FALSE;	// Didn't find the timer so say we failed
}

// Routine to handle the closing of the timer window
void TimerEditSmall_Deactivate( int retval, bool cancel ){
	TimerEditSmall_Active = FALSE;
	// Restore what we had drawn over
	TAP_Osd_RestoreBox(TimerEditSmall_RGN, TimerEditSmall_X, TimerEditSmall_Y, TimerEditSmall_W, TimerEditSmall_H, TimerEditSmall_Base);
	TAP_MemFree(TimerEditSmall_Base);
	TimerEditSmall_Base = NULL;
	TimerEditSmall_MemFree();
	TimerEditSmall_Callback(retval, cancel);
}


// Here we will just draw the basic layout of the keyboard
void TimerEditSmall_DrawBackground( word rgn ){
	if( TimerEditSmall_Base != NULL ) TAP_MemFree(TimerEditSmall_Base);
	TimerEditSmall_Base = TAP_Osd_SaveBox(TimerEditSmall_RGN, TimerEditSmall_X, TimerEditSmall_Y, TimerEditSmall_W, TimerEditSmall_H);	// Save what is under the Timer screen
	TAP_Osd_FillBox(rgn, TimerEditSmall_X, TimerEditSmall_Y, TimerEditSmall_W, TimerEditSmall_H, TimerEditSmall_BaseColour);
}

void TimerEditSmall_DrawItem( word rgn, int item, int selected ){
	int fntSize = 0;
	int pad_y;
	dword fc = TimerEditSmall_Fc;
	dword bc = TimerEditSmall_Bc;
	// Work out the font size allowed for the height of the option
	if( ( TimerEditSmall_Option_H - 10 ) > 19  ){
		if( ( TimerEditSmall_Option_H - 12 ) > 22 ){
			// Use the big font
			fntSize = FNT_Size_1926;
			pad_y = (TimerEditSmall_Option_H/2)-(26/2);
		} else {
			fntSize = FNT_Size_1622;
			pad_y = (TimerEditSmall_Option_H/2)-(22/2);
		}
	} else {
		pad_y = (TimerEditSmall_Option_H/2)-(19/2);
	}
	// If selected change the colors of the elements
	if( item == selected ){
		fc = TimerEditSmall_HghFc;
		bc = TimerEditSmall_HghBc;
	}
	switch( item ){
		case TIMEREDITSMALL_SECTIONTITLE:
			TAP_Osd_FillBox(rgn, TimerEditSmall_X+TIMEREDITSMALL_PADDING, TimerEditSmall_Y+TIMEREDITSMALL_GAP+((TimerEditSmall_Option_H+TIMEREDITSMALL_GAP)*item), TimerEditSmall_Option_W, TimerEditSmall_Option_H, TimerEditSmall_BaseColour);
			switch( fntSize ){
				case FNT_Size_1419:
					Font_Osd_PutString1419( rgn, TimerEditSmall_X+TIMEREDITSMALL_PADDING+TIMEREDITSMALL_GAP, TimerEditSmall_Y+TIMEREDITSMALL_GAP+((TimerEditSmall_Option_H+TIMEREDITSMALL_GAP)*item)+pad_y, TimerEditSmall_X+TIMEREDITSMALL_PADDING+TimerEditSmall_Option_W-TIMEREDITSMALL_GAP, "Edit Timer Details", fc, COLOR_None);
					break;
				case FNT_Size_1622:
					Font_Osd_PutString1622( rgn, TimerEditSmall_X+TIMEREDITSMALL_PADDING+TIMEREDITSMALL_GAP, TimerEditSmall_Y+TIMEREDITSMALL_GAP+((TimerEditSmall_Option_H+TIMEREDITSMALL_GAP)*item)+pad_y, TimerEditSmall_X+TIMEREDITSMALL_PADDING+TimerEditSmall_Option_W-TIMEREDITSMALL_GAP, "Edit Timer Details", fc, COLOR_None);
					break;
				case FNT_Size_1926:
					Font_Osd_PutString1926( rgn, TimerEditSmall_X+TIMEREDITSMALL_PADDING+TIMEREDITSMALL_GAP, TimerEditSmall_Y+TIMEREDITSMALL_GAP+((TimerEditSmall_Option_H+TIMEREDITSMALL_GAP)*item)+pad_y, TimerEditSmall_X+TIMEREDITSMALL_PADDING+TimerEditSmall_Option_W-TIMEREDITSMALL_GAP, "Edit Timer Details", fc, COLOR_None);
					break;
			}
			break;
		case TIMEREDITSMALL_TIMERNAME:
			if( item == selected ){
				if( TimerEditSmall_ItemHigh != NULL ){
					TAP_Osd_RestoreBox( rgn, TimerEditSmall_X+TIMEREDITSMALL_PADDING, TimerEditSmall_Y+TIMEREDITSMALL_GAP+((TimerEditSmall_Option_H+TIMEREDITSMALL_GAP)*item), TimerEditSmall_Option_W, TimerEditSmall_Option_H, TimerEditSmall_ItemHigh );
				} else {
					TAP_Osd_FillBox(rgn, TimerEditSmall_X+TIMEREDITSMALL_PADDING, TimerEditSmall_Y+TIMEREDITSMALL_GAP+((TimerEditSmall_Option_H+TIMEREDITSMALL_GAP)*item), TimerEditSmall_Option_W, TimerEditSmall_Option_H, bc);
				}
			} else {
				if( TimerEditSmall_ItemBase != NULL ){
					TAP_Osd_RestoreBox( rgn, TimerEditSmall_X+TIMEREDITSMALL_PADDING, TimerEditSmall_Y+TIMEREDITSMALL_GAP+((TimerEditSmall_Option_H+TIMEREDITSMALL_GAP)*item), TimerEditSmall_Option_W, TimerEditSmall_Option_H, TimerEditSmall_ItemBase );
				} else {
					TAP_Osd_FillBox(rgn, TimerEditSmall_X+TIMEREDITSMALL_PADDING, TimerEditSmall_Y+TIMEREDITSMALL_GAP+((TimerEditSmall_Option_H+TIMEREDITSMALL_GAP)*item), TimerEditSmall_Option_W, TimerEditSmall_Option_H, bc);
				}
			}
//			TAP_Osd_FillBox(rgn, TimerEditSmall_X+TIMEREDITSMALL_PADDING, TimerEditSmall_Y+TIMEREDITSMALL_GAP+((TimerEditSmall_Option_H+TIMEREDITSMALL_GAP)*item), TimerEditSmall_Option_W, TimerEditSmall_Option_H, bc);
			switch( fntSize ){
				case FNT_Size_1419:
					Font_Osd_PutString1419( rgn, TimerEditSmall_X+TIMEREDITSMALL_PADDING+TIMEREDITSMALL_GAP, TimerEditSmall_Y+TIMEREDITSMALL_GAP+((TimerEditSmall_Option_H+TIMEREDITSMALL_GAP)*item)+pad_y, TimerEditSmall_X+TIMEREDITSMALL_PADDING+TimerEditSmall_Option_W-TIMEREDITSMALL_GAP, TimerEditSmall_Timer.fileName, fc, COLOR_None);
					break;
				case FNT_Size_1622:
					Font_Osd_PutString1622( rgn, TimerEditSmall_X+TIMEREDITSMALL_PADDING+TIMEREDITSMALL_GAP, TimerEditSmall_Y+TIMEREDITSMALL_GAP+((TimerEditSmall_Option_H+TIMEREDITSMALL_GAP)*item)+pad_y, TimerEditSmall_X+TIMEREDITSMALL_PADDING+TimerEditSmall_Option_W-TIMEREDITSMALL_GAP, TimerEditSmall_Timer.fileName, fc, COLOR_None);
					break;
				case FNT_Size_1926:
					Font_Osd_PutString1926( rgn, TimerEditSmall_X+TIMEREDITSMALL_PADDING+TIMEREDITSMALL_GAP, TimerEditSmall_Y+TIMEREDITSMALL_GAP+((TimerEditSmall_Option_H+TIMEREDITSMALL_GAP)*item)+pad_y, TimerEditSmall_X+TIMEREDITSMALL_PADDING+TimerEditSmall_Option_W-TIMEREDITSMALL_GAP, TimerEditSmall_Timer.fileName, fc, COLOR_None);
					break;
			}
			break;
		case TIMEREDITSMALL_TIMERSTART:
			if( item == selected ){
				if( TimerEditSmall_ItemHigh != NULL ){
					TAP_Osd_RestoreBox( rgn, TimerEditSmall_X+TIMEREDITSMALL_PADDING, TimerEditSmall_Y+TIMEREDITSMALL_GAP+((TimerEditSmall_Option_H+TIMEREDITSMALL_GAP)*item), TimerEditSmall_Option_W, TimerEditSmall_Option_H, TimerEditSmall_ItemHigh );
				} else {
					TAP_Osd_FillBox(rgn, TimerEditSmall_X+TIMEREDITSMALL_PADDING, TimerEditSmall_Y+TIMEREDITSMALL_GAP+((TimerEditSmall_Option_H+TIMEREDITSMALL_GAP)*item), TimerEditSmall_Option_W, TimerEditSmall_Option_H, bc);
				}
			} else {
				if( TimerEditSmall_ItemBase != NULL ){
					TAP_Osd_RestoreBox( rgn, TimerEditSmall_X+TIMEREDITSMALL_PADDING, TimerEditSmall_Y+TIMEREDITSMALL_GAP+((TimerEditSmall_Option_H+TIMEREDITSMALL_GAP)*item), TimerEditSmall_Option_W, TimerEditSmall_Option_H, TimerEditSmall_ItemBase );
				} else {
					TAP_Osd_FillBox(rgn, TimerEditSmall_X+TIMEREDITSMALL_PADDING, TimerEditSmall_Y+TIMEREDITSMALL_GAP+((TimerEditSmall_Option_H+TIMEREDITSMALL_GAP)*item), TimerEditSmall_Option_W, TimerEditSmall_Option_H, bc);
				}
			}
//			TAP_Osd_FillBox(rgn, TimerEditSmall_X+TIMEREDITSMALL_PADDING, TimerEditSmall_Y+TIMEREDITSMALL_GAP+((TimerEditSmall_Option_H+TIMEREDITSMALL_GAP)*item), TimerEditSmall_Option_W, TimerEditSmall_Option_H, bc);
			switch( fntSize ){
				case FNT_Size_1419:
					Font_Osd_PutString1419( rgn, TimerEditSmall_X+TIMEREDITSMALL_PADDING+TIMEREDITSMALL_GAP, TimerEditSmall_Y+TIMEREDITSMALL_GAP+((TimerEditSmall_Option_H+TIMEREDITSMALL_GAP)*item)+pad_y, TimerEditSmall_X+TIMEREDITSMALL_PADDING+TimerEditSmall_Option_W-TIMEREDITSMALL_GAP, Time_HHMM(TimerEditSmall_Timer.startTime), fc, COLOR_None);
					break;
				case FNT_Size_1622:
					Font_Osd_PutString1622( rgn, TimerEditSmall_X+TIMEREDITSMALL_PADDING+TIMEREDITSMALL_GAP, TimerEditSmall_Y+TIMEREDITSMALL_GAP+((TimerEditSmall_Option_H+TIMEREDITSMALL_GAP)*item)+pad_y, TimerEditSmall_X+TIMEREDITSMALL_PADDING+TimerEditSmall_Option_W-TIMEREDITSMALL_GAP, Time_HHMM(TimerEditSmall_Timer.startTime), fc, COLOR_None);
					break;
				case FNT_Size_1926:
					Font_Osd_PutString1926( rgn, TimerEditSmall_X+TIMEREDITSMALL_PADDING+TIMEREDITSMALL_GAP, TimerEditSmall_Y+TIMEREDITSMALL_GAP+((TimerEditSmall_Option_H+TIMEREDITSMALL_GAP)*item)+pad_y, TimerEditSmall_X+TIMEREDITSMALL_PADDING+TimerEditSmall_Option_W-TIMEREDITSMALL_GAP, Time_HHMM(TimerEditSmall_Timer.startTime), fc, COLOR_None);
					break;
			}
			break;
		case TIMEREDITSMALL_TIMEREND:
			if( item == selected ){
				if( TimerEditSmall_ItemHigh != NULL ){
					TAP_Osd_RestoreBox( rgn, TimerEditSmall_X+TIMEREDITSMALL_PADDING, TimerEditSmall_Y+TIMEREDITSMALL_GAP+((TimerEditSmall_Option_H+TIMEREDITSMALL_GAP)*item), TimerEditSmall_Option_W, TimerEditSmall_Option_H, TimerEditSmall_ItemHigh );
				} else {
					TAP_Osd_FillBox(rgn, TimerEditSmall_X+TIMEREDITSMALL_PADDING, TimerEditSmall_Y+TIMEREDITSMALL_GAP+((TimerEditSmall_Option_H+TIMEREDITSMALL_GAP)*item), TimerEditSmall_Option_W, TimerEditSmall_Option_H, bc);
				}
			} else {
				if( TimerEditSmall_ItemBase != NULL ){
					TAP_Osd_RestoreBox( rgn, TimerEditSmall_X+TIMEREDITSMALL_PADDING, TimerEditSmall_Y+TIMEREDITSMALL_GAP+((TimerEditSmall_Option_H+TIMEREDITSMALL_GAP)*item), TimerEditSmall_Option_W, TimerEditSmall_Option_H, TimerEditSmall_ItemBase );
				} else {
					TAP_Osd_FillBox(rgn, TimerEditSmall_X+TIMEREDITSMALL_PADDING, TimerEditSmall_Y+TIMEREDITSMALL_GAP+((TimerEditSmall_Option_H+TIMEREDITSMALL_GAP)*item), TimerEditSmall_Option_W, TimerEditSmall_Option_H, bc);
				}
			}
//			TAP_Osd_FillBox(rgn, TimerEditSmall_X+TIMEREDITSMALL_PADDING, TimerEditSmall_Y+TIMEREDITSMALL_GAP+((TimerEditSmall_Option_H+TIMEREDITSMALL_GAP)*item), TimerEditSmall_Option_W, TimerEditSmall_Option_H, bc);
			switch( fntSize ){
				case FNT_Size_1419:
					Font_Osd_PutString1419( rgn, TimerEditSmall_X+TIMEREDITSMALL_PADDING+TIMEREDITSMALL_GAP, TimerEditSmall_Y+TIMEREDITSMALL_GAP+((TimerEditSmall_Option_H+TIMEREDITSMALL_GAP)*item)+pad_y, TimerEditSmall_X+TIMEREDITSMALL_PADDING+TimerEditSmall_Option_W-TIMEREDITSMALL_GAP, Endtime_HHMM(TimerEditSmall_Timer.startTime,TimerEditSmall_Timer.duration), fc, COLOR_None);
					break;
				case FNT_Size_1622:
					Font_Osd_PutString1622( rgn, TimerEditSmall_X+TIMEREDITSMALL_PADDING+TIMEREDITSMALL_GAP, TimerEditSmall_Y+TIMEREDITSMALL_GAP+((TimerEditSmall_Option_H+TIMEREDITSMALL_GAP)*item)+pad_y, TimerEditSmall_X+TIMEREDITSMALL_PADDING+TimerEditSmall_Option_W-TIMEREDITSMALL_GAP, Endtime_HHMM(TimerEditSmall_Timer.startTime,TimerEditSmall_Timer.duration), fc, COLOR_None);
					break;
				case FNT_Size_1926:
					Font_Osd_PutString1926( rgn, TimerEditSmall_X+TIMEREDITSMALL_PADDING+TIMEREDITSMALL_GAP, TimerEditSmall_Y+TIMEREDITSMALL_GAP+((TimerEditSmall_Option_H+TIMEREDITSMALL_GAP)*item)+pad_y, TimerEditSmall_X+TIMEREDITSMALL_PADDING+TimerEditSmall_Option_W-TIMEREDITSMALL_GAP, Endtime_HHMM(TimerEditSmall_Timer.startTime,TimerEditSmall_Timer.duration), fc, COLOR_None);
					break;
			}
			break;
		case TIMEREDITSMALL_DATE:
			if( item == selected ){
				if( TimerEditSmall_ItemHigh != NULL ){
					TAP_Osd_RestoreBox( rgn, TimerEditSmall_X+TIMEREDITSMALL_PADDING, TimerEditSmall_Y+TIMEREDITSMALL_GAP+((TimerEditSmall_Option_H+TIMEREDITSMALL_GAP)*item), TimerEditSmall_Option_W, TimerEditSmall_Option_H, TimerEditSmall_ItemHigh );
				} else {
					TAP_Osd_FillBox(rgn, TimerEditSmall_X+TIMEREDITSMALL_PADDING, TimerEditSmall_Y+TIMEREDITSMALL_GAP+((TimerEditSmall_Option_H+TIMEREDITSMALL_GAP)*item), TimerEditSmall_Option_W, TimerEditSmall_Option_H, bc);
				}
			} else {
				if( TimerEditSmall_ItemBase != NULL ){
					TAP_Osd_RestoreBox( rgn, TimerEditSmall_X+TIMEREDITSMALL_PADDING, TimerEditSmall_Y+TIMEREDITSMALL_GAP+((TimerEditSmall_Option_H+TIMEREDITSMALL_GAP)*item), TimerEditSmall_Option_W, TimerEditSmall_Option_H, TimerEditSmall_ItemBase );
				} else {
					TAP_Osd_FillBox(rgn, TimerEditSmall_X+TIMEREDITSMALL_PADDING, TimerEditSmall_Y+TIMEREDITSMALL_GAP+((TimerEditSmall_Option_H+TIMEREDITSMALL_GAP)*item), TimerEditSmall_Option_W, TimerEditSmall_Option_H, bc);
				}
			}
//			TAP_Osd_FillBox(rgn, TimerEditSmall_X+TIMEREDITSMALL_PADDING, TimerEditSmall_Y+TIMEREDITSMALL_GAP+((TimerEditSmall_Option_H+TIMEREDITSMALL_GAP)*item), TimerEditSmall_Option_W, TimerEditSmall_Option_H, bc);
			switch( fntSize ){
				case FNT_Size_1419:
					Font_Osd_PutString1419( rgn, TimerEditSmall_X+TIMEREDITSMALL_PADDING+TIMEREDITSMALL_GAP, TimerEditSmall_Y+TIMEREDITSMALL_GAP+((TimerEditSmall_Option_H+TIMEREDITSMALL_GAP)*item)+pad_y, TimerEditSmall_X+TIMEREDITSMALL_PADDING+TimerEditSmall_Option_W-TIMEREDITSMALL_GAP, Time_DOWDDMMSpec(TimerEditSmall_Timer.startTime), fc, COLOR_None);
					break;
				case FNT_Size_1622:
					Font_Osd_PutString1622( rgn, TimerEditSmall_X+TIMEREDITSMALL_PADDING+TIMEREDITSMALL_GAP, TimerEditSmall_Y+TIMEREDITSMALL_GAP+((TimerEditSmall_Option_H+TIMEREDITSMALL_GAP)*item)+pad_y, TimerEditSmall_X+TIMEREDITSMALL_PADDING+TimerEditSmall_Option_W-TIMEREDITSMALL_GAP, Time_DOWDDMMSpec(TimerEditSmall_Timer.startTime), fc, COLOR_None);
					break;
				case FNT_Size_1926:
					Font_Osd_PutString1926( rgn, TimerEditSmall_X+TIMEREDITSMALL_PADDING+TIMEREDITSMALL_GAP, TimerEditSmall_Y+TIMEREDITSMALL_GAP+((TimerEditSmall_Option_H+TIMEREDITSMALL_GAP)*item)+pad_y, TimerEditSmall_X+TIMEREDITSMALL_PADDING+TimerEditSmall_Option_W-TIMEREDITSMALL_GAP, Time_DOWDDMMSpec(TimerEditSmall_Timer.startTime), fc, COLOR_None);
					break;
			}
			break;	
			break;
		case TIMEREDITSMALL_TIMERSELECTION:
			if( TimerEditSmall_ItemBase != NULL ){
				TAP_Osd_RestoreBox( rgn, TimerEditSmall_X+TIMEREDITSMALL_PADDING, TimerEditSmall_Y+TIMEREDITSMALL_GAP+((TimerEditSmall_Option_H+TIMEREDITSMALL_GAP)*item), TimerEditSmall_Option_W, TimerEditSmall_Option_H, TimerEditSmall_ItemBase );
			} else {
				TAP_Osd_FillBox(rgn, TimerEditSmall_X+TIMEREDITSMALL_PADDING, TimerEditSmall_Y+TIMEREDITSMALL_GAP+((TimerEditSmall_Option_H+TIMEREDITSMALL_GAP)*item), TimerEditSmall_Option_W, TimerEditSmall_Option_H, bc);
			}
			TAP_Osd_FillBox(rgn, TimerEditSmall_X+TIMEREDITSMALL_PADDING+(TimerEditSmall_Option_W/2)-(TIMEREDITSMALL_GAP/2), TimerEditSmall_Y+TIMEREDITSMALL_GAP+((TimerEditSmall_Option_H+TIMEREDITSMALL_GAP)*item), TIMEREDITSMALL_GAP-1, TimerEditSmall_Option_H, TimerEditSmall_BaseColour);
			if( item == selected ){
				if( TimerEditSmall_OptionSelected == TIMEREDITSMALL_OK ){
					if( TimerEditSmall_OkItemHigh != NULL ){
						TAP_Osd_RestoreBox( rgn, TimerEditSmall_X+TIMEREDITSMALL_PADDING, TimerEditSmall_Y+TIMEREDITSMALL_GAP+((TimerEditSmall_Option_H+TIMEREDITSMALL_GAP)*item), (TimerEditSmall_Option_W/2)-(TIMEREDITSMALL_GAP/2)-1, TimerEditSmall_Option_H, TimerEditSmall_OkItemHigh );
					} else {
						TAP_Osd_FillBox(rgn, TimerEditSmall_X+TIMEREDITSMALL_PADDING, TimerEditSmall_Y+TIMEREDITSMALL_GAP+((TimerEditSmall_Option_H+TIMEREDITSMALL_GAP)*item), (TimerEditSmall_Option_W/2)-(TIMEREDITSMALL_GAP/2), TimerEditSmall_Option_H, TimerEditSmall_HghBc);
					}
				} else {
					if( TimerEditSmall_OkItemHigh != NULL ){
						TAP_Osd_RestoreBox( rgn, TimerEditSmall_X+TIMEREDITSMALL_PADDING+(TimerEditSmall_Option_W/2)+(TIMEREDITSMALL_GAP/2), TimerEditSmall_Y+TIMEREDITSMALL_GAP+((TimerEditSmall_Option_H+TIMEREDITSMALL_GAP)*item), (TimerEditSmall_Option_W/2)-(TIMEREDITSMALL_GAP/2)-1, TimerEditSmall_Option_H, TimerEditSmall_CancelItemHigh );
					} else {
						TAP_Osd_FillBox(rgn, TimerEditSmall_X+TIMEREDITSMALL_PADDING+(TimerEditSmall_Option_W/2)+(TIMEREDITSMALL_GAP/2), TimerEditSmall_Y+TIMEREDITSMALL_GAP+((TimerEditSmall_Option_H+TIMEREDITSMALL_GAP)*item), (TimerEditSmall_Option_W/2)-(TIMEREDITSMALL_GAP/2)-1, TimerEditSmall_Option_H, TimerEditSmall_HghBc);
					}
				}
				switch( fntSize ){
					case FNT_Size_1419:
						Font_Osd_PutString1419( rgn, TimerEditSmall_X+TIMEREDITSMALL_PADDING+TIMEREDITSMALL_GAP, TimerEditSmall_Y+TIMEREDITSMALL_GAP+((TimerEditSmall_Option_H+TIMEREDITSMALL_GAP)*item)+pad_y, TimerEditSmall_X+TIMEREDITSMALL_PADDING+TimerEditSmall_Option_W-TIMEREDITSMALL_GAP, "OK", fc, COLOR_None);
						break;
					case FNT_Size_1622:
						Font_Osd_PutString1622( rgn, TimerEditSmall_X+TIMEREDITSMALL_PADDING+TIMEREDITSMALL_GAP, TimerEditSmall_Y+TIMEREDITSMALL_GAP+((TimerEditSmall_Option_H+TIMEREDITSMALL_GAP)*item)+pad_y, TimerEditSmall_X+TIMEREDITSMALL_PADDING+TimerEditSmall_Option_W-TIMEREDITSMALL_GAP, "OK", fc, COLOR_None);
						break;
					case FNT_Size_1926:
						Font_Osd_PutString1926( rgn, TimerEditSmall_X+TIMEREDITSMALL_PADDING+TIMEREDITSMALL_GAP, TimerEditSmall_Y+TIMEREDITSMALL_GAP+((TimerEditSmall_Option_H+TIMEREDITSMALL_GAP)*item)+pad_y, TimerEditSmall_X+TIMEREDITSMALL_PADDING+TimerEditSmall_Option_W-TIMEREDITSMALL_GAP, "OK", fc, COLOR_None);
						break;
				}
				switch( fntSize ){
					case FNT_Size_1419:
						Font_Osd_PutString1419( rgn, TimerEditSmall_X+TIMEREDITSMALL_PADDING+TIMEREDITSMALL_GAP+(TimerEditSmall_Option_W/2)+(TIMEREDITSMALL_GAP/2), TimerEditSmall_Y+TIMEREDITSMALL_GAP+((TimerEditSmall_Option_H+TIMEREDITSMALL_GAP)*item)+pad_y, TimerEditSmall_X+TIMEREDITSMALL_PADDING+TimerEditSmall_Option_W-TIMEREDITSMALL_GAP, "Cancel", fc, COLOR_None);
						break;
					case FNT_Size_1622:
						Font_Osd_PutString1622( rgn, TimerEditSmall_X+TIMEREDITSMALL_PADDING+TIMEREDITSMALL_GAP+(TimerEditSmall_Option_W/2)+(TIMEREDITSMALL_GAP/2), TimerEditSmall_Y+TIMEREDITSMALL_GAP+((TimerEditSmall_Option_H+TIMEREDITSMALL_GAP)*item)+pad_y, TimerEditSmall_X+TIMEREDITSMALL_PADDING+TimerEditSmall_Option_W-TIMEREDITSMALL_GAP, "Cancel", fc, COLOR_None);
						break;
					case FNT_Size_1926:
						Font_Osd_PutString1926( rgn, TimerEditSmall_X+TIMEREDITSMALL_PADDING+TIMEREDITSMALL_GAP+(TimerEditSmall_Option_W/2)+(TIMEREDITSMALL_GAP/2), TimerEditSmall_Y+TIMEREDITSMALL_GAP+((TimerEditSmall_Option_H+TIMEREDITSMALL_GAP)*item)+pad_y, TimerEditSmall_X+TIMEREDITSMALL_PADDING+TimerEditSmall_Option_W-TIMEREDITSMALL_GAP, "Cancel", fc, COLOR_None);
						break;
				}	
			} else {
//				TAP_Osd_FillBox(rgn, TimerEditSmall_X+TIMEREDITSMALL_PADDING, TimerEditSmall_Y+TIMEREDITSMALL_GAP+((TimerEditSmall_Option_H+TIMEREDITSMALL_GAP)*item), (TimerEditSmall_Option_W/2)-(TIMEREDITSMALL_GAP/2)-1, TimerEditSmall_Option_H, TimerEditSmall_Bc);
//				TAP_Osd_FillBox(rgn, TimerEditSmall_X+TIMEREDITSMALL_PADDING+(TimerEditSmall_Option_W/2)+(TIMEREDITSMALL_GAP/2), TimerEditSmall_Y+TIMEREDITSMALL_GAP+((TimerEditSmall_Option_H+TIMEREDITSMALL_GAP)*item), (TimerEditSmall_Option_W/2)-(TIMEREDITSMALL_GAP/2)-1, TimerEditSmall_Option_H, TimerEditSmall_Bc);
				switch( fntSize ){
					case FNT_Size_1419:
						Font_Osd_PutString1419( rgn, TimerEditSmall_X+TIMEREDITSMALL_PADDING+TIMEREDITSMALL_GAP, TimerEditSmall_Y+TIMEREDITSMALL_GAP+((TimerEditSmall_Option_H+TIMEREDITSMALL_GAP)*item)+pad_y, TimerEditSmall_X+TIMEREDITSMALL_PADDING+TimerEditSmall_Option_W-TIMEREDITSMALL_GAP, "OK", fc, COLOR_None);
						break;
					case FNT_Size_1622:
						Font_Osd_PutString1622( rgn, TimerEditSmall_X+TIMEREDITSMALL_PADDING+TIMEREDITSMALL_GAP, TimerEditSmall_Y+TIMEREDITSMALL_GAP+((TimerEditSmall_Option_H+TIMEREDITSMALL_GAP)*item)+pad_y, TimerEditSmall_X+TIMEREDITSMALL_PADDING+TimerEditSmall_Option_W-TIMEREDITSMALL_GAP, "OK", fc, COLOR_None);
						break;
					case FNT_Size_1926:
						Font_Osd_PutString1926( rgn, TimerEditSmall_X+TIMEREDITSMALL_PADDING+TIMEREDITSMALL_GAP, TimerEditSmall_Y+TIMEREDITSMALL_GAP+((TimerEditSmall_Option_H+TIMEREDITSMALL_GAP)*item)+pad_y, TimerEditSmall_X+TIMEREDITSMALL_PADDING+TimerEditSmall_Option_W-TIMEREDITSMALL_GAP, "OK", fc, COLOR_None);
						break;
				}
				switch( fntSize ){
					case FNT_Size_1419:
						Font_Osd_PutString1419( rgn, TimerEditSmall_X+TIMEREDITSMALL_PADDING+TIMEREDITSMALL_GAP+(TimerEditSmall_Option_W/2)+(TIMEREDITSMALL_GAP/2), TimerEditSmall_Y+TIMEREDITSMALL_GAP+((TimerEditSmall_Option_H+TIMEREDITSMALL_GAP)*item)+pad_y, TimerEditSmall_X+TIMEREDITSMALL_PADDING+TimerEditSmall_Option_W-TIMEREDITSMALL_GAP, "Cancel", fc, COLOR_None);
						break;
					case FNT_Size_1622:
						Font_Osd_PutString1622( rgn, TimerEditSmall_X+TIMEREDITSMALL_PADDING+TIMEREDITSMALL_GAP+(TimerEditSmall_Option_W/2)+(TIMEREDITSMALL_GAP/2), TimerEditSmall_Y+TIMEREDITSMALL_GAP+((TimerEditSmall_Option_H+TIMEREDITSMALL_GAP)*item)+pad_y, TimerEditSmall_X+TIMEREDITSMALL_PADDING+TimerEditSmall_Option_W-TIMEREDITSMALL_GAP, "Cancel", fc, COLOR_None);
						break;
					case FNT_Size_1926:
						Font_Osd_PutString1926( rgn, TimerEditSmall_X+TIMEREDITSMALL_PADDING+TIMEREDITSMALL_GAP+(TimerEditSmall_Option_W/2)+(TIMEREDITSMALL_GAP/2), TimerEditSmall_Y+TIMEREDITSMALL_GAP+((TimerEditSmall_Option_H+TIMEREDITSMALL_GAP)*item)+pad_y, TimerEditSmall_X+TIMEREDITSMALL_PADDING+TimerEditSmall_Option_W-TIMEREDITSMALL_GAP, "Cancel", fc, COLOR_None);
						break;
				}	
			}
		
			break;
	}
}

void TimerEditSmall_KeyboardClose( char * text, bool changed ){
	// Do nothing atm until it is implement
	if( changed ){
		// We have change the text of the timer so modify the fileName field
		memset( TimerEditSmall_Timer.fileName, '\0', sizeof(TimerEditSmall_Timer.fileName) ); // Blank the filename field
		strncpy(TimerEditSmall_Timer.fileName, text, sizeof(TimerEditSmall_Timer.fileName));
		TimerEditSmall_DrawItem( TimerEditSmall_RGN, TimerEditSmall_Selected, TimerEditSmall_Selected );
	}
}

// Function to draw the timer details to the region
void TimerEditSmall_DrawDetails( word rgn ){
	int i;
	for( i = 0; i < TIMEREDITSMALL_OPTIONS; i++ ){
		TimerEditSmall_DrawItem( rgn, i, TimerEditSmall_Selected );
	}
}

int TimerEditSmall_Modify( void ){
	strcat(TimerEditSmall_Timer.fileName,".rec");
	return TAP_Timer_Modify(TimerEditSmall_TimerNum,&TimerEditSmall_Timer);
}

dword TimerEditSmall_KeyHandler( dword param1 ){
	if( Keyboard_IsActive() ){	// Handle the keyboard
		return Keyboard_Keyhandler(param1);
	}
	if( param1 == RKEY_Exit ){
		TimerEditSmall_Deactivate(0,TRUE);
		return 0;
	}
	// Do the navigation stuff now
	if( param1 == RKEY_ChUp ){
		TimerEditSmall_PrevSelected = TimerEditSmall_Selected;
		TimerEditSmall_Selected--;
		if( TimerEditSmall_Selected < 1 ){
			TimerEditSmall_Selected = TIMEREDITSMALL_OPTIONS-1;
		}
		TimerEditSmall_DrawItem( TimerEditSmall_MemRGN, TimerEditSmall_PrevSelected, TimerEditSmall_Selected );
		TimerEditSmall_DrawItem( TimerEditSmall_MemRGN, TimerEditSmall_Selected, TimerEditSmall_Selected );
		TAP_Osd_Copy( TimerEditSmall_MemRGN, TimerEditSmall_RGN, TimerEditSmall_X+TIMEREDITSMALL_PADDING, TimerEditSmall_Y+TIMEREDITSMALL_GAP+((TimerEditSmall_Option_H+TIMEREDITSMALL_GAP)*TimerEditSmall_PrevSelected), TimerEditSmall_Option_W, TimerEditSmall_Option_H, TimerEditSmall_X+TIMEREDITSMALL_PADDING, TimerEditSmall_Y+TIMEREDITSMALL_GAP+((TimerEditSmall_Option_H+TIMEREDITSMALL_GAP)*TimerEditSmall_PrevSelected), FALSE);
		TAP_Osd_Copy( TimerEditSmall_MemRGN, TimerEditSmall_RGN, TimerEditSmall_X+TIMEREDITSMALL_PADDING, TimerEditSmall_Y+TIMEREDITSMALL_GAP+((TimerEditSmall_Option_H+TIMEREDITSMALL_GAP)*TimerEditSmall_Selected), TimerEditSmall_Option_W, TimerEditSmall_Option_H, TimerEditSmall_X+TIMEREDITSMALL_PADDING, TimerEditSmall_Y+TIMEREDITSMALL_GAP+((TimerEditSmall_Option_H+TIMEREDITSMALL_GAP)*TimerEditSmall_Selected), FALSE);
	}
	// Do the navigation stuff now
	if( param1 == RKEY_ChDown ){
		TimerEditSmall_PrevSelected = TimerEditSmall_Selected;
		TimerEditSmall_Selected++;
		if( TimerEditSmall_Selected > TIMEREDITSMALL_OPTIONS-1 ){
			TimerEditSmall_Selected = 1;
		}
		TimerEditSmall_DrawItem( TimerEditSmall_MemRGN, TimerEditSmall_PrevSelected, TimerEditSmall_Selected );
		TimerEditSmall_DrawItem( TimerEditSmall_MemRGN, TimerEditSmall_Selected, TimerEditSmall_Selected );
		TAP_Osd_Copy( TimerEditSmall_MemRGN, TimerEditSmall_RGN, TimerEditSmall_X+TIMEREDITSMALL_PADDING, TimerEditSmall_Y+TIMEREDITSMALL_GAP+((TimerEditSmall_Option_H+TIMEREDITSMALL_GAP)*TimerEditSmall_PrevSelected), TimerEditSmall_Option_W, TimerEditSmall_Option_H, TimerEditSmall_X+TIMEREDITSMALL_PADDING, TimerEditSmall_Y+TIMEREDITSMALL_GAP+((TimerEditSmall_Option_H+TIMEREDITSMALL_GAP)*TimerEditSmall_PrevSelected), FALSE);
		TAP_Osd_Copy( TimerEditSmall_MemRGN, TimerEditSmall_RGN, TimerEditSmall_X+TIMEREDITSMALL_PADDING, TimerEditSmall_Y+TIMEREDITSMALL_GAP+((TimerEditSmall_Option_H+TIMEREDITSMALL_GAP)*TimerEditSmall_Selected), TimerEditSmall_Option_W, TimerEditSmall_Option_H, TimerEditSmall_X+TIMEREDITSMALL_PADDING, TimerEditSmall_Y+TIMEREDITSMALL_GAP+((TimerEditSmall_Option_H+TIMEREDITSMALL_GAP)*TimerEditSmall_Selected), FALSE);
	}
	// Do the navigation stuff now
	if( param1 == RKEY_VolDown || param1 == RKEY_VolUp || param1 == RKEY_Forward || param1 == RKEY_Rewind ){
		switch( TimerEditSmall_Selected ){
			case TIMEREDITSMALL_TIMERSTART:
				TimerEditSmall_AlterStartTime(param1);
				TimerEditSmall_DrawItem( TimerEditSmall_MemRGN, TIMEREDITSMALL_TIMEREND, TimerEditSmall_Selected );
				TAP_Osd_Copy( TimerEditSmall_MemRGN, TimerEditSmall_RGN, TimerEditSmall_X+TIMEREDITSMALL_PADDING, TimerEditSmall_Y+TIMEREDITSMALL_GAP+((TimerEditSmall_Option_H+TIMEREDITSMALL_GAP)*TIMEREDITSMALL_TIMEREND), TimerEditSmall_Option_W, TimerEditSmall_Option_H, TimerEditSmall_X+TIMEREDITSMALL_PADDING, TimerEditSmall_Y+TIMEREDITSMALL_GAP+((TimerEditSmall_Option_H+TIMEREDITSMALL_GAP)*TIMEREDITSMALL_TIMEREND), FALSE);
				break;
			case TIMEREDITSMALL_TIMEREND:
				TimerEditSmall_AlterDuration(param1);
				break;
			case TIMEREDITSMALL_TIMERSELECTION:
				TimerEditSmall_OptionSelected++;
				if( TimerEditSmall_OptionSelected > 1 ){
					TimerEditSmall_OptionSelected  = 0;
				}
				break;
			case TIMEREDITSMALL_DATE:
				TimerEditSmall_AdjustDate( param1 );
				break;
		}
		TimerEditSmall_DrawItem( TimerEditSmall_MemRGN, TimerEditSmall_Selected, TimerEditSmall_Selected );
		TAP_Osd_Copy( TimerEditSmall_MemRGN, TimerEditSmall_RGN, TimerEditSmall_X+TIMEREDITSMALL_PADDING, TimerEditSmall_Y+TIMEREDITSMALL_GAP+((TimerEditSmall_Option_H+TIMEREDITSMALL_GAP)*TimerEditSmall_Selected), TimerEditSmall_Option_W, TimerEditSmall_Option_H, TimerEditSmall_X+TIMEREDITSMALL_PADDING, TimerEditSmall_Y+TIMEREDITSMALL_GAP+((TimerEditSmall_Option_H+TIMEREDITSMALL_GAP)*TimerEditSmall_Selected), FALSE);
	}
	if( param1 == RKEY_Ok ){
		switch( TimerEditSmall_Selected ){
			case TIMEREDITSMALL_TIMERNAME:
				Keyboard_Init( TimerEditSmall_RGN, TimerEditSmall_MemRGN, TimerEditSmall_X, TimerEditSmall_Y, TimerEditSmall_W, TimerEditSmall_H, "Timer Name" );	// Initalise the keyboard
				Keyboard_SetColours( DISPLAY_KEYBOARDITEM, DISPLAY_KEYBOARDITEMTEXT, DISPLAY_KEYBOARDITEMSELECTED, DISPLAY_KEYBOARDITEMSELECTEDTEXT, DISPLAY_KEYBOARDFIELDTEXT, DISPLAY_KEYBOARDFIELD, DISPLAY_KEYBOARDBACKGROUND );
				if( Keyboard_Activate(TimerEditSmall_Timer.fileName,&TimerEditSmall_KeyboardClose) ){
				}
				break;
			case TIMEREDITSMALL_TIMERSELECTION:
				if( TimerEditSmall_OptionSelected == TIMEREDITSMALL_OK ){
					TimerEditSmall_Deactivate(TimerEditSmall_Modify(),FALSE);
				} else {
					TimerEditSmall_Deactivate(0,TRUE);
				}
				break;
		}
	}
	return 0;
}

void TimerEditSmall_AdjustDate( dword param1 ){
	word 	mjd;
	int 	startTime; // , duration
	startTime = TimerEditSmall_Timer.startTime&0xffff;		// extract the time
	mjd = TimerEditSmall_Timer.startTime>>16;
	switch ( param1 ){
		case RKEY_VolUp:	mjd++;	break;			// Date up
		case RKEY_VolDown:	mjd--;	break;			// Date down
		default :			break;
	}
	TimerEditSmall_Timer.startTime = (mjd<<16) | startTime;
}

void TimerEditSmall_AlterDuration( dword param1 ){
	switch( param1 ){
		case RKEY_VolUp:							// Time up
			TimerEditSmall_Timer.duration++;
			break;
		case RKEY_VolDown:							// Time down
			if ( TimerEditSmall_Timer.duration > 0 ) TimerEditSmall_Timer.duration--;
			break;
		case RKEY_Forward:							// Time up
			TimerEditSmall_Timer.duration+=60;
			break;
		case RKEY_Rewind:							// Time down
			if ( TimerEditSmall_Timer.duration > 60 ) TimerEditSmall_Timer.duration-=60;
			break;
		default :
			break;
	}
}

void TimerEditSmall_AlterStartTime( dword param1 ){
	word 	mjd;
	int 	min, hour;
	hour = (TimerEditSmall_Timer.startTime&0xff00)>>8;			// extract the time
	min = (TimerEditSmall_Timer.startTime&0xff);
	mjd = TimerEditSmall_Timer.startTime>>16;
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
	TimerEditSmall_Timer.startTime = (mjd<<16) | ((hour&0xff)<<8) | (min&0xff);
}

// Function to set the colours of the Timer window
void TimerEditSmall_SetColours( dword fc, dword bc, dword fhc, dword bhc, dword base ){
	TimerEditSmall_Fc = fc;
	TimerEditSmall_HghBc = bhc;
	TimerEditSmall_HghFc = fhc;
	TimerEditSmall_Bc = bc;
	TimerEditSmall_BaseColour = base;
}


// Function to get and set the variable associated with editing the timer
bool TimerEditSmall_GetTimer( int row ){
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
					TimerEditSmall_Timer = currentTimerDefault;
					TimerEditSmall_TimerNum = i;
					return TRUE;
				}
				break;
			case RESERVE_TYPE_Everyday:
				// Initial check to determine against currentTimer start
				if( (result=Timer_TimerFind( currentChInfo.svcId, Events_GetEventSvcId(row), currentTimer.startTime, AddTime(currentTimer.startTime, currentTimer.duration), event.startTime, event.endTime )) != TIMER_NONE ){
					TimerEditSmall_Timer = currentTimerDefault;
					TimerEditSmall_TimerNum = i;
					return TRUE;
				}
				// Not found on today so we need to go and increment for the next 7 days and check on weekdays
				for( x = 0; x < TIMERS_MAXDAYSAHEAD; x++ ){
					currentTimer.startTime = AddTime( currentTimer.startTime, 1440 );
					if( (result=Timer_TimerFind( currentChInfo.svcId, Events_GetEventSvcId(row), currentTimer.startTime, AddTime(currentTimer.startTime, currentTimer.duration), event.startTime, event.endTime )) != TIMER_NONE ){
						TimerEditSmall_Timer = currentTimerDefault;
						TimerEditSmall_TimerNum = i;
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
							TimerEditSmall_Timer = currentTimerDefault;
							TimerEditSmall_TimerNum = i;
							return TRUE;
						}
					}
					// Not found on today so we need to go and increment for the next 7 days and check on weekdays
					for( x = 0; x < TIMERS_MAXDAYSAHEAD; x++ ){
						currentTimer.startTime = AddTime( currentTimer.startTime, 1440 );
						TAP_ExtractMjd( currentTimer.startTime>>16, &year, &month, &day, &weekDay);
						if( weekDay == SAT || weekDay == SUN  ){	// Sanity check to make sure timer is on a weekday
							if( (result=Timer_TimerFind( currentChInfo.svcId, Events_GetEventSvcId(row), currentTimer.startTime, AddTime(currentTimer.startTime, currentTimer.duration), event.startTime, event.endTime )) != TIMER_NONE ){
								TimerEditSmall_Timer = currentTimerDefault;
								TimerEditSmall_TimerNum = i;
								return TRUE;
							}
						}
					}
				}
				break;
			case RESERVE_TYPE_Weekly:
				if( (result=Timer_TimerFind( currentChInfo.svcId, Events_GetEventSvcId(row), currentTimer.startTime, AddTime(currentTimer.startTime, currentTimer.duration), event.startTime, event.endTime )) != TIMER_NONE ){
					TimerEditSmall_Timer = currentTimerDefault;
					TimerEditSmall_TimerNum = i;
					return TRUE;
				}
				for( x = 0; x < 7; x++ ){	// AddTime only handle 1440 max so loop the adding of the time
					currentTimer.startTime = AddTime(currentTimer.startTime, 1440 );
				}
				// Only search a week ahead atm as data shouldn't be anymore than that
				if( (result=Timer_TimerFind( currentChInfo.svcId, Events_GetEventSvcId(row), currentTimer.startTime, AddTime(currentTimer.startTime, currentTimer.duration), event.startTime, event.endTime )) != TIMER_NONE ){
					TimerEditSmall_Timer = currentTimerDefault;
					TimerEditSmall_TimerNum = i;
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
							TimerEditSmall_Timer = currentTimerDefault;
							TimerEditSmall_TimerNum = i;
							return TRUE;
						}
					}
					// Not found on today so we need to go and increment for the next 7 days and check on weekdays
					for( x = 0; x < TIMERS_MAXDAYSAHEAD; x++ ){
						currentTimer.startTime = AddTime( currentTimer.startTime, 1440 );
						TAP_ExtractMjd( currentTimer.startTime>>16, &year, &month, &day, &weekDay);
						if( weekDay != SAT && weekDay != SUN  ){	// Sanity check to make sure timer is on a weekday
							if( (result=Timer_TimerFind( currentChInfo.svcId, Events_GetEventSvcId(row), currentTimer.startTime, AddTime(currentTimer.startTime, currentTimer.duration), event.startTime, event.endTime )) != TIMER_NONE ){
								TimerEditSmall_Timer = currentTimerDefault;
								TimerEditSmall_TimerNum = i;
								return TRUE;
							}
						}
					}
				}
				break;
		}
	}


//	TYPE_TimerInfo	currentTimer;
//	int totalTimers = 0;
//	int i;
//	word year;
//	byte month, day, weekDay;
//	TYPE_TapChInfo	currentChInfo;
//	TYPE_TapEvent event;
//	event = Events_GetEvent(row);
//	totalTimers = TAP_Timer_GetTotalNum();
//	for ( i = 0; i < totalTimers; i++ ){
//		TAP_Timer_GetInfo(i, &currentTimer); // Sequentially get timer details and compare with those in the display range
//		TAP_ExtractMjd( currentTimer.startTime>>16, &year, &month, &day, &weekDay);
//		TAP_Channel_GetInfo( currentTimer.svcType, currentTimer.svcNum, &currentChInfo );
//		if( currentChInfo.svcId == Events_GetEventSvcId(row) ){
//			// Check whether the timer completely covers the event
//			if( currentTimer.startTime <= event.startTime &&
//				AddTime(currentTimer.startTime, currentTimer.duration) >= event.endTime ){
//				TimerEditSmall_Timer = currentTimer;
//				TimerEditSmall_TimerNum = i;
//				return TRUE;
//			}
//			if( Setting_ShowPartialTimers ){
//				// Check for partial cover at end and start
//				if( 
//					// Start overlap
//					(currentTimer.startTime <= event.startTime &&
//					AddTime(currentTimer.startTime, currentTimer.duration) >= event.startTime )
//					||
//					// End overlap
//					(currentTimer.startTime <= event.endTime &&
//					AddTime(currentTimer.startTime, currentTimer.duration) >= event.endTime )){
//					TimerEditSmall_Timer = currentTimer;
//					TimerEditSmall_TimerNum = i;
//					return TRUE;
//				}
//				if( currentTimer.startTime >= event.endTime &&
//					AddTime(currentTimer.startTime, currentTimer.duration) <= event.endTime ){
//					// Recording is middle part of the show
//					TimerEditSmall_Timer = currentTimer;
//					TimerEditSmall_TimerNum = i;
//					return TRUE;
//				}
//			}
//		}
//	}
	return FALSE;
}


