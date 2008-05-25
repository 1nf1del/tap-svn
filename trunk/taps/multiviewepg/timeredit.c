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

#include "timeredit.h"
#include "fontman.h"
#include "skin.h"
#include "settings.h"
#include "keyboard.h"

bool TimerEdit_IsActive( void ){
	return TimerEdit_Active;
}

// Function to initalize the keyboard - region and location to render
void TimerEdit_Init( word rgn, word memrgn, int x, int y, int w, int h ){
	TimerEdit_RGN = rgn;	// Store the region to render on
	TimerEdit_MemRGN = memrgn;
	TimerEdit_X = x;
	TimerEdit_Y = y;
	TimerEdit_W = w;
	TimerEdit_H = h;
	TimerEdit_Initalised = TRUE;
	TimerEdit_Option_H = (TimerEdit_H/TIMEREDIT_OPTIONS)-TIMEREDIT_GAP-1;
	TimerEdit_Option_W = TimerEdit_W - (TIMEREDIT_PADDING*2);
	TimerEdit_CreateGradients();
}

void TimerEdit_MemFree( void ){
	if (TimerEdit_ItemBase) TAP_MemFree(TimerEdit_ItemBase);
	if (TimerEdit_ItemHigh) TAP_MemFree(TimerEdit_ItemHigh);
	if (TimerEdit_OkItemHigh) TAP_MemFree(TimerEdit_OkItemHigh);
	if (TimerEdit_CancelItemHigh) TAP_MemFree(TimerEdit_CancelItemHigh);
	TimerEdit_ItemBase = NULL;
	TimerEdit_ItemHigh = NULL;
	TimerEdit_OkItemHigh = NULL;
	TimerEdit_CancelItemHigh = NULL;
}

void TimerEdit_CreateGradients( void ){
	word rgn;
	TimerEdit_MemFree();
	if( Settings_GradientFactor > 0 ){
		rgn	= TAP_Osd_Create(0, 0, TimerEdit_Option_W, TimerEdit_Option_H, 0, OSD_Flag_MemRgn);
		// Main default item
		createGradient(rgn, TimerEdit_Option_W, TimerEdit_Option_H, Settings_GradientFactor, DISPLAY_ITEM);
		TimerEdit_ItemBase = TAP_Osd_SaveBox(rgn, 0, 0, TimerEdit_Option_W, TimerEdit_Option_H);
		createGradient(rgn, TimerEdit_Option_W, TimerEdit_Option_H, Settings_GradientFactor, DISPLAY_ITEMSELECTED);
		TimerEdit_ItemHigh = TAP_Osd_SaveBox(rgn, 0, 0, TimerEdit_Option_W, TimerEdit_Option_H);
		TimerEdit_CancelItemHigh = TAP_Osd_SaveBox(rgn, 0, 0, (TimerEdit_Option_W/2)-(TIMEREDIT_GAP/2), TimerEdit_Option_H);
		TimerEdit_OkItemHigh = TAP_Osd_SaveBox(rgn, 0, 0, (TimerEdit_Option_W/2)-(TIMEREDIT_GAP/2)-1, TimerEdit_Option_H);
		TAP_Osd_Delete(rgn);
	}
}
// Activate by sending the timer and the number. Used when not using events ala conflict calling
bool TimerEdit_ActivateWithTimer( TYPE_TimerInfo editTimer, int editNumber, void (*ReturningProcedure)( int, bool ) ){
	if( !TimerEdit_Initalised ) return FALSE;	// We haven't initalised so gracefully fail
	if( editNumber < 0 ) return FALSE;
	TimerEdit_Callback = ReturningProcedure;
	// Get the details of the event and create the timer structure
	TimerEdit_Timer = editTimer;
	TimerEdit_TimerNum = editNumber;
	StripFileName(TimerEdit_Timer.fileName);
	TimerEdit_Selected = 1;
	TimerEdit_PrevSelected = 1;
	TimerEdit_OptionSelected = 0;
	// Now lets render the display
	TimerEdit_DrawBackground(TimerEdit_MemRGN);
	TimerEdit_DrawDetails(TimerEdit_MemRGN);
	// Coppy rendered to foreground
	TAP_Osd_Copy( TimerEdit_MemRGN, TimerEdit_RGN, TimerEdit_X, TimerEdit_Y, TimerEdit_W, TimerEdit_H, TimerEdit_X, TimerEdit_Y, FALSE);
	TimerEdit_Active = TRUE;
	return TRUE;
}

bool TimerEdit_Activate( int row, void (*ReturningProcedure)( int, bool ) ){
	TYPE_TapEvent event;
	if( !TimerEdit_Initalised ) return FALSE;	// We haven't initalised so gracefully fail
	TimerEdit_Callback = ReturningProcedure;
	// Get the details of the event and create the timer structure
	event = Events_GetEvent(row);
	// Get the timer covering this event and store in our variables
	if( TimerEdit_GetTimer(row)){
		// Remove the .rec from the end of the filename. Will append at the end
		StripFileName(TimerEdit_Timer.fileName);
		TimerEdit_Selected = 1;
		TimerEdit_PrevSelected = 1;
		TimerEdit_OptionSelected = 0;
		// Now lets render the display
		TimerEdit_DrawBackground(TimerEdit_MemRGN);
		TimerEdit_DrawDetails(TimerEdit_MemRGN);
		// Coppy rendered to foreground
		TAP_Osd_Copy( TimerEdit_MemRGN, TimerEdit_RGN, TimerEdit_X, TimerEdit_Y, TimerEdit_W, TimerEdit_H, TimerEdit_X, TimerEdit_Y, FALSE);
		TimerEdit_Active = TRUE;
		return TRUE;
	}
	return FALSE;	// Didn't find the timer so say we failed
}

// Routine to handle the closing of the timer window
void TimerEdit_Deactivate( int retval, bool cancel ){
	TimerEdit_Active = FALSE;
	// Restore what we had drawn over
	TAP_Osd_RestoreBox(TimerEdit_RGN, TimerEdit_X, TimerEdit_Y, TimerEdit_W, TimerEdit_H, TimerEdit_Base);
	TAP_MemFree(TimerEdit_Base);
	TimerEdit_Base = NULL;
	TimerEdit_MemFree();
	TimerEdit_Callback(retval, cancel);
}


// Here we will just draw the basic layout of the keyboard
void TimerEdit_DrawBackground( word rgn ){
	if( TimerEdit_Base != NULL ) TAP_MemFree(TimerEdit_Base);
	TimerEdit_Base = TAP_Osd_SaveBox(TimerEdit_RGN, TimerEdit_X, TimerEdit_Y, TimerEdit_W, TimerEdit_H);	// Save what is under the Timer screen
	TAP_Osd_FillBox(rgn, TimerEdit_X, TimerEdit_Y, TimerEdit_W, TimerEdit_H, TimerEdit_BaseColour);
}

void TimerEdit_DrawItem( word rgn, int item, int selected ){
	int fntSize = 0;
	int pad_y;
	dword fc = TimerEdit_Fc;
	dword bc = TimerEdit_Bc;
	// Work out the font size allowed for the height of the option
	if( ( TimerEdit_Option_H - 8 ) > 19  ){
		if( ( TimerEdit_Option_H - 12 ) > 22 ){
			// Use the big font
			fntSize = FNT_Size_1926;
			pad_y = (TimerEdit_Option_H/2)-(26/2);
		} else {
			fntSize = FNT_Size_1622;
			pad_y = (TimerEdit_Option_H/2)-(22/2);
		}
	} else {
		pad_y = (TimerEdit_Option_H/2)-(19/2);
	}
	// If selected change the colors of the elements
	if( item == selected ){
		fc = TimerEdit_HghFc;
		bc = TimerEdit_HghBc;
	}
	switch( item ){
		case TIMEREDIT_SECTIONTITLE:
			TAP_Osd_FillBox(rgn, TimerEdit_X+TIMEREDIT_PADDING, TimerEdit_Y+TIMEREDIT_GAP+((TimerEdit_Option_H+TIMEREDIT_GAP)*item), TimerEdit_Option_W, TimerEdit_Option_H, TimerEdit_BaseColour);
			switch( fntSize ){
				case FNT_Size_1419:
					Font_Osd_PutString1419( rgn, TimerEdit_X+TIMEREDIT_PADDING+TIMEREDIT_GAP, TimerEdit_Y+TIMEREDIT_GAP+((TimerEdit_Option_H+TIMEREDIT_GAP)*item)+pad_y, TimerEdit_X+TIMEREDIT_PADDING+TimerEdit_Option_W-TIMEREDIT_GAP, "Edit Timer Details", fc, COLOR_None);
					break;
				case FNT_Size_1622:
					Font_Osd_PutString1622( rgn, TimerEdit_X+TIMEREDIT_PADDING+TIMEREDIT_GAP, TimerEdit_Y+TIMEREDIT_GAP+((TimerEdit_Option_H+TIMEREDIT_GAP)*item)+pad_y, TimerEdit_X+TIMEREDIT_PADDING+TimerEdit_Option_W-TIMEREDIT_GAP, "Edit Timer Details", fc, COLOR_None);
					break;
				case FNT_Size_1926:
					Font_Osd_PutString1926( rgn, TimerEdit_X+TIMEREDIT_PADDING+TIMEREDIT_GAP, TimerEdit_Y+TIMEREDIT_GAP+((TimerEdit_Option_H+TIMEREDIT_GAP)*item)+pad_y, TimerEdit_X+TIMEREDIT_PADDING+TimerEdit_Option_W-TIMEREDIT_GAP, "Edit Timer Details", fc, COLOR_None);
					break;
			}
			break;
		case TIMEREDIT_TIMERNAME:
			if( item == selected ){
				if( TimerEdit_ItemHigh != NULL ){
					TAP_Osd_RestoreBox( rgn, TimerEdit_X+TIMEREDIT_PADDING, TimerEdit_Y+TIMEREDIT_GAP+((TimerEdit_Option_H+TIMEREDIT_GAP)*item), TimerEdit_Option_W, TimerEdit_Option_H, TimerEdit_ItemHigh );
				} else {
					TAP_Osd_FillBox(rgn, TimerEdit_X+TIMEREDIT_PADDING, TimerEdit_Y+TIMEREDIT_GAP+((TimerEdit_Option_H+TIMEREDIT_GAP)*item), TimerEdit_Option_W, TimerEdit_Option_H, bc);
				}
			} else {
				if( TimerEdit_ItemBase != NULL ){
					TAP_Osd_RestoreBox( rgn, TimerEdit_X+TIMEREDIT_PADDING, TimerEdit_Y+TIMEREDIT_GAP+((TimerEdit_Option_H+TIMEREDIT_GAP)*item), TimerEdit_Option_W, TimerEdit_Option_H, TimerEdit_ItemBase );
				} else {
					TAP_Osd_FillBox(rgn, TimerEdit_X+TIMEREDIT_PADDING, TimerEdit_Y+TIMEREDIT_GAP+((TimerEdit_Option_H+TIMEREDIT_GAP)*item), TimerEdit_Option_W, TimerEdit_Option_H, bc);
				}
			}
//			TAP_Osd_FillBox(rgn, TimerEdit_X+TIMEREDIT_PADDING, TimerEdit_Y+TIMEREDIT_GAP+((TimerEdit_Option_H+TIMEREDIT_GAP)*item), TimerEdit_Option_W, TimerEdit_Option_H, bc);
			switch( fntSize ){
				case FNT_Size_1419:
					Font_Osd_PutString1419( rgn, TimerEdit_X+TIMEREDIT_PADDING+TIMEREDIT_GAP, TimerEdit_Y+TIMEREDIT_GAP+((TimerEdit_Option_H+TIMEREDIT_GAP)*item)+pad_y, TimerEdit_X+TIMEREDIT_PADDING+TimerEdit_Option_W-TIMEREDIT_GAP, TimerEdit_Timer.fileName, fc, COLOR_None);
					break;
				case FNT_Size_1622:
					Font_Osd_PutString1622( rgn, TimerEdit_X+TIMEREDIT_PADDING+TIMEREDIT_GAP, TimerEdit_Y+TIMEREDIT_GAP+((TimerEdit_Option_H+TIMEREDIT_GAP)*item)+pad_y, TimerEdit_X+TIMEREDIT_PADDING+TimerEdit_Option_W-TIMEREDIT_GAP, TimerEdit_Timer.fileName, fc, COLOR_None);
					break;
				case FNT_Size_1926:
					Font_Osd_PutString1926( rgn, TimerEdit_X+TIMEREDIT_PADDING+TIMEREDIT_GAP, TimerEdit_Y+TIMEREDIT_GAP+((TimerEdit_Option_H+TIMEREDIT_GAP)*item)+pad_y, TimerEdit_X+TIMEREDIT_PADDING+TimerEdit_Option_W-TIMEREDIT_GAP, TimerEdit_Timer.fileName, fc, COLOR_None);
					break;
			}
			break;
		case TIMEREDIT_TIMERSTART:
			if( item == selected ){
				if( TimerEdit_ItemHigh != NULL ){
					TAP_Osd_RestoreBox( rgn, TimerEdit_X+TIMEREDIT_PADDING, TimerEdit_Y+TIMEREDIT_GAP+((TimerEdit_Option_H+TIMEREDIT_GAP)*item), TimerEdit_Option_W, TimerEdit_Option_H, TimerEdit_ItemHigh );
				} else {
					TAP_Osd_FillBox(rgn, TimerEdit_X+TIMEREDIT_PADDING, TimerEdit_Y+TIMEREDIT_GAP+((TimerEdit_Option_H+TIMEREDIT_GAP)*item), TimerEdit_Option_W, TimerEdit_Option_H, bc);
				}
			} else {
				if( TimerEdit_ItemBase != NULL ){
					TAP_Osd_RestoreBox( rgn, TimerEdit_X+TIMEREDIT_PADDING, TimerEdit_Y+TIMEREDIT_GAP+((TimerEdit_Option_H+TIMEREDIT_GAP)*item), TimerEdit_Option_W, TimerEdit_Option_H, TimerEdit_ItemBase );
				} else {
					TAP_Osd_FillBox(rgn, TimerEdit_X+TIMEREDIT_PADDING, TimerEdit_Y+TIMEREDIT_GAP+((TimerEdit_Option_H+TIMEREDIT_GAP)*item), TimerEdit_Option_W, TimerEdit_Option_H, bc);
				}
			}
//			TAP_Osd_FillBox(rgn, TimerEdit_X+TIMEREDIT_PADDING, TimerEdit_Y+TIMEREDIT_GAP+((TimerEdit_Option_H+TIMEREDIT_GAP)*item), TimerEdit_Option_W, TimerEdit_Option_H, bc);
			switch( fntSize ){
				case FNT_Size_1419:
					Font_Osd_PutString1419( rgn, TimerEdit_X+TIMEREDIT_PADDING+TIMEREDIT_GAP, TimerEdit_Y+TIMEREDIT_GAP+((TimerEdit_Option_H+TIMEREDIT_GAP)*item)+pad_y, TimerEdit_X+TIMEREDIT_PADDING+TimerEdit_Option_W-TIMEREDIT_GAP, Time_HHMM(TimerEdit_Timer.startTime), fc, COLOR_None);
					break;
				case FNT_Size_1622:
					Font_Osd_PutString1622( rgn, TimerEdit_X+TIMEREDIT_PADDING+TIMEREDIT_GAP, TimerEdit_Y+TIMEREDIT_GAP+((TimerEdit_Option_H+TIMEREDIT_GAP)*item)+pad_y, TimerEdit_X+TIMEREDIT_PADDING+TimerEdit_Option_W-TIMEREDIT_GAP, Time_HHMM(TimerEdit_Timer.startTime), fc, COLOR_None);
					break;
				case FNT_Size_1926:
					Font_Osd_PutString1926( rgn, TimerEdit_X+TIMEREDIT_PADDING+TIMEREDIT_GAP, TimerEdit_Y+TIMEREDIT_GAP+((TimerEdit_Option_H+TIMEREDIT_GAP)*item)+pad_y, TimerEdit_X+TIMEREDIT_PADDING+TimerEdit_Option_W-TIMEREDIT_GAP, Time_HHMM(TimerEdit_Timer.startTime), fc, COLOR_None);
					break;
			}
			break;
		case TIMEREDIT_TIMEREND:
			if( item == selected ){
				if( TimerEdit_ItemHigh != NULL ){
					TAP_Osd_RestoreBox( rgn, TimerEdit_X+TIMEREDIT_PADDING, TimerEdit_Y+TIMEREDIT_GAP+((TimerEdit_Option_H+TIMEREDIT_GAP)*item), TimerEdit_Option_W, TimerEdit_Option_H, TimerEdit_ItemHigh );
				} else {
					TAP_Osd_FillBox(rgn, TimerEdit_X+TIMEREDIT_PADDING, TimerEdit_Y+TIMEREDIT_GAP+((TimerEdit_Option_H+TIMEREDIT_GAP)*item), TimerEdit_Option_W, TimerEdit_Option_H, bc);
				}
			} else {
				if( TimerEdit_ItemBase != NULL ){
					TAP_Osd_RestoreBox( rgn, TimerEdit_X+TIMEREDIT_PADDING, TimerEdit_Y+TIMEREDIT_GAP+((TimerEdit_Option_H+TIMEREDIT_GAP)*item), TimerEdit_Option_W, TimerEdit_Option_H, TimerEdit_ItemBase );
				} else {
					TAP_Osd_FillBox(rgn, TimerEdit_X+TIMEREDIT_PADDING, TimerEdit_Y+TIMEREDIT_GAP+((TimerEdit_Option_H+TIMEREDIT_GAP)*item), TimerEdit_Option_W, TimerEdit_Option_H, bc);
				}
			}
//			TAP_Osd_FillBox(rgn, TimerEdit_X+TIMEREDIT_PADDING, TimerEdit_Y+TIMEREDIT_GAP+((TimerEdit_Option_H+TIMEREDIT_GAP)*item), TimerEdit_Option_W, TimerEdit_Option_H, bc);
			switch( fntSize ){
				case FNT_Size_1419:
					Font_Osd_PutString1419( rgn, TimerEdit_X+TIMEREDIT_PADDING+TIMEREDIT_GAP, TimerEdit_Y+TIMEREDIT_GAP+((TimerEdit_Option_H+TIMEREDIT_GAP)*item)+pad_y, TimerEdit_X+TIMEREDIT_PADDING+TimerEdit_Option_W-TIMEREDIT_GAP, Endtime_HHMM(TimerEdit_Timer.startTime,TimerEdit_Timer.duration), fc, COLOR_None);
					break;
				case FNT_Size_1622:
					Font_Osd_PutString1622( rgn, TimerEdit_X+TIMEREDIT_PADDING+TIMEREDIT_GAP, TimerEdit_Y+TIMEREDIT_GAP+((TimerEdit_Option_H+TIMEREDIT_GAP)*item)+pad_y, TimerEdit_X+TIMEREDIT_PADDING+TimerEdit_Option_W-TIMEREDIT_GAP, Endtime_HHMM(TimerEdit_Timer.startTime,TimerEdit_Timer.duration), fc, COLOR_None);
					break;
				case FNT_Size_1926:
					Font_Osd_PutString1926( rgn, TimerEdit_X+TIMEREDIT_PADDING+TIMEREDIT_GAP, TimerEdit_Y+TIMEREDIT_GAP+((TimerEdit_Option_H+TIMEREDIT_GAP)*item)+pad_y, TimerEdit_X+TIMEREDIT_PADDING+TimerEdit_Option_W-TIMEREDIT_GAP, Endtime_HHMM(TimerEdit_Timer.startTime,TimerEdit_Timer.duration), fc, COLOR_None);
					break;
			}
			break;
		case TIMEREDIT_RESERVETYPE:
			if( item == selected ){
				if( TimerEdit_ItemHigh != NULL ){
					TAP_Osd_RestoreBox( rgn, TimerEdit_X+TIMEREDIT_PADDING, TimerEdit_Y+TIMEREDIT_GAP+((TimerEdit_Option_H+TIMEREDIT_GAP)*item), TimerEdit_Option_W, TimerEdit_Option_H, TimerEdit_ItemHigh );
				} else {
					TAP_Osd_FillBox(rgn, TimerEdit_X+TIMEREDIT_PADDING, TimerEdit_Y+TIMEREDIT_GAP+((TimerEdit_Option_H+TIMEREDIT_GAP)*item), TimerEdit_Option_W, TimerEdit_Option_H, bc);
				}
			} else {
				if( TimerEdit_ItemBase != NULL ){
					TAP_Osd_RestoreBox( rgn, TimerEdit_X+TIMEREDIT_PADDING, TimerEdit_Y+TIMEREDIT_GAP+((TimerEdit_Option_H+TIMEREDIT_GAP)*item), TimerEdit_Option_W, TimerEdit_Option_H, TimerEdit_ItemBase );
				} else {
					TAP_Osd_FillBox(rgn, TimerEdit_X+TIMEREDIT_PADDING, TimerEdit_Y+TIMEREDIT_GAP+((TimerEdit_Option_H+TIMEREDIT_GAP)*item), TimerEdit_Option_W, TimerEdit_Option_H, bc);
				}
			}
//			TAP_Osd_FillBox(rgn, TimerEdit_X+TIMEREDIT_PADDING, TimerEdit_Y+TIMEREDIT_GAP+((TimerEdit_Option_H+TIMEREDIT_GAP)*item), TimerEdit_Option_W, TimerEdit_Option_H, bc);
			switch( fntSize ){
				case FNT_Size_1419:
					Font_Osd_PutString1419( rgn, TimerEdit_X+TIMEREDIT_PADDING+TIMEREDIT_GAP, TimerEdit_Y+TIMEREDIT_GAP+((TimerEdit_Option_H+TIMEREDIT_GAP)*item)+pad_y, TimerEdit_X+TIMEREDIT_PADDING+TimerEdit_Option_W-TIMEREDIT_GAP, TimerEdit_ReserveType(TimerEdit_Timer.reservationType), fc, COLOR_None);
					break;
				case FNT_Size_1622:
					Font_Osd_PutString1622( rgn, TimerEdit_X+TIMEREDIT_PADDING+TIMEREDIT_GAP, TimerEdit_Y+TIMEREDIT_GAP+((TimerEdit_Option_H+TIMEREDIT_GAP)*item)+pad_y, TimerEdit_X+TIMEREDIT_PADDING+TimerEdit_Option_W-TIMEREDIT_GAP, TimerEdit_ReserveType(TimerEdit_Timer.reservationType), fc, COLOR_None);
					break;
				case FNT_Size_1926:
					Font_Osd_PutString1926( rgn, TimerEdit_X+TIMEREDIT_PADDING+TIMEREDIT_GAP, TimerEdit_Y+TIMEREDIT_GAP+((TimerEdit_Option_H+TIMEREDIT_GAP)*item)+pad_y, TimerEdit_X+TIMEREDIT_PADDING+TimerEdit_Option_W-TIMEREDIT_GAP, TimerEdit_ReserveType(TimerEdit_Timer.reservationType), fc, COLOR_None);
					break;
			}
			break;		
		case TIMEREDIT_DATE:
			if( item == selected ){
				if( TimerEdit_ItemHigh != NULL ){
					TAP_Osd_RestoreBox( rgn, TimerEdit_X+TIMEREDIT_PADDING, TimerEdit_Y+TIMEREDIT_GAP+((TimerEdit_Option_H+TIMEREDIT_GAP)*item), TimerEdit_Option_W, TimerEdit_Option_H, TimerEdit_ItemHigh );
				} else {
					TAP_Osd_FillBox(rgn, TimerEdit_X+TIMEREDIT_PADDING, TimerEdit_Y+TIMEREDIT_GAP+((TimerEdit_Option_H+TIMEREDIT_GAP)*item), TimerEdit_Option_W, TimerEdit_Option_H, bc);
				}
			} else {
				if( TimerEdit_ItemBase != NULL ){
					TAP_Osd_RestoreBox( rgn, TimerEdit_X+TIMEREDIT_PADDING, TimerEdit_Y+TIMEREDIT_GAP+((TimerEdit_Option_H+TIMEREDIT_GAP)*item), TimerEdit_Option_W, TimerEdit_Option_H, TimerEdit_ItemBase );
				} else {
					TAP_Osd_FillBox(rgn, TimerEdit_X+TIMEREDIT_PADDING, TimerEdit_Y+TIMEREDIT_GAP+((TimerEdit_Option_H+TIMEREDIT_GAP)*item), TimerEdit_Option_W, TimerEdit_Option_H, bc);
				}
			}
//			TAP_Osd_FillBox(rgn, TimerEdit_X+TIMEREDIT_PADDING, TimerEdit_Y+TIMEREDIT_GAP+((TimerEdit_Option_H+TIMEREDIT_GAP)*item), TimerEdit_Option_W, TimerEdit_Option_H, bc);
			switch( fntSize ){
				case FNT_Size_1419:
					Font_Osd_PutString1419( rgn, TimerEdit_X+TIMEREDIT_PADDING+TIMEREDIT_GAP, TimerEdit_Y+TIMEREDIT_GAP+((TimerEdit_Option_H+TIMEREDIT_GAP)*item)+pad_y, TimerEdit_X+TIMEREDIT_PADDING+TimerEdit_Option_W-TIMEREDIT_GAP, Time_DOWDDMMSpec(TimerEdit_Timer.startTime), fc, COLOR_None);
					break;
				case FNT_Size_1622:
					Font_Osd_PutString1622( rgn, TimerEdit_X+TIMEREDIT_PADDING+TIMEREDIT_GAP, TimerEdit_Y+TIMEREDIT_GAP+((TimerEdit_Option_H+TIMEREDIT_GAP)*item)+pad_y, TimerEdit_X+TIMEREDIT_PADDING+TimerEdit_Option_W-TIMEREDIT_GAP, Time_DOWDDMMSpec(TimerEdit_Timer.startTime), fc, COLOR_None);
					break;
				case FNT_Size_1926:
					Font_Osd_PutString1926( rgn, TimerEdit_X+TIMEREDIT_PADDING+TIMEREDIT_GAP, TimerEdit_Y+TIMEREDIT_GAP+((TimerEdit_Option_H+TIMEREDIT_GAP)*item)+pad_y, TimerEdit_X+TIMEREDIT_PADDING+TimerEdit_Option_W-TIMEREDIT_GAP, Time_DOWDDMMSpec(TimerEdit_Timer.startTime), fc, COLOR_None);
					break;
			}
			break;	
			break;
		case TIMEREDIT_TIMERSELECTION:
			if( TimerEdit_ItemBase != NULL ){
				TAP_Osd_RestoreBox( rgn, TimerEdit_X+TIMEREDIT_PADDING, TimerEdit_Y+TIMEREDIT_GAP+((TimerEdit_Option_H+TIMEREDIT_GAP)*item), TimerEdit_Option_W, TimerEdit_Option_H, TimerEdit_ItemBase );
			} else {
				TAP_Osd_FillBox(rgn, TimerEdit_X+TIMEREDIT_PADDING, TimerEdit_Y+TIMEREDIT_GAP+((TimerEdit_Option_H+TIMEREDIT_GAP)*item), TimerEdit_Option_W, TimerEdit_Option_H, bc);
			}
			TAP_Osd_FillBox(rgn, TimerEdit_X+TIMEREDIT_PADDING+(TimerEdit_Option_W/2)-(TIMEREDIT_GAP/2), TimerEdit_Y+TIMEREDIT_GAP+((TimerEdit_Option_H+TIMEREDIT_GAP)*item), TIMEREDIT_GAP-1, TimerEdit_Option_H, TimerEdit_BaseColour);
			if( item == selected ){
				if( TimerEdit_OptionSelected == TIMEREDIT_OK ){
					if( TimerEdit_OkItemHigh != NULL ){
						TAP_Osd_RestoreBox( rgn, TimerEdit_X+TIMEREDIT_PADDING, TimerEdit_Y+TIMEREDIT_GAP+((TimerEdit_Option_H+TIMEREDIT_GAP)*item), (TimerEdit_Option_W/2)-(TIMEREDIT_GAP/2)-1, TimerEdit_Option_H, TimerEdit_OkItemHigh );
					} else {
						TAP_Osd_FillBox(rgn, TimerEdit_X+TIMEREDIT_PADDING, TimerEdit_Y+TIMEREDIT_GAP+((TimerEdit_Option_H+TIMEREDIT_GAP)*item), (TimerEdit_Option_W/2)-(TIMEREDIT_GAP/2), TimerEdit_Option_H, TimerEdit_HghBc);
					}
				} else {
					if( TimerEdit_OkItemHigh != NULL ){
						TAP_Osd_RestoreBox( rgn, TimerEdit_X+TIMEREDIT_PADDING+(TimerEdit_Option_W/2)+(TIMEREDIT_GAP/2), TimerEdit_Y+TIMEREDIT_GAP+((TimerEdit_Option_H+TIMEREDIT_GAP)*item), (TimerEdit_Option_W/2)-(TIMEREDIT_GAP/2)-1, TimerEdit_Option_H, TimerEdit_CancelItemHigh );
					} else {
						TAP_Osd_FillBox(rgn, TimerEdit_X+TIMEREDIT_PADDING+(TimerEdit_Option_W/2)+(TIMEREDIT_GAP/2), TimerEdit_Y+TIMEREDIT_GAP+((TimerEdit_Option_H+TIMEREDIT_GAP)*item), (TimerEdit_Option_W/2)-(TIMEREDIT_GAP/2)-1, TimerEdit_Option_H, TimerEdit_HghBc);
					}
				}
				switch( fntSize ){
					case FNT_Size_1419:
						Font_Osd_PutString1419( rgn, TimerEdit_X+TIMEREDIT_PADDING+TIMEREDIT_GAP, TimerEdit_Y+TIMEREDIT_GAP+((TimerEdit_Option_H+TIMEREDIT_GAP)*item)+pad_y, TimerEdit_X+TIMEREDIT_PADDING+TimerEdit_Option_W-TIMEREDIT_GAP, "OK", fc, COLOR_None);
						break;
					case FNT_Size_1622:
						Font_Osd_PutString1622( rgn, TimerEdit_X+TIMEREDIT_PADDING+TIMEREDIT_GAP, TimerEdit_Y+TIMEREDIT_GAP+((TimerEdit_Option_H+TIMEREDIT_GAP)*item)+pad_y, TimerEdit_X+TIMEREDIT_PADDING+TimerEdit_Option_W-TIMEREDIT_GAP, "OK", fc, COLOR_None);
						break;
					case FNT_Size_1926:
						Font_Osd_PutString1926( rgn, TimerEdit_X+TIMEREDIT_PADDING+TIMEREDIT_GAP, TimerEdit_Y+TIMEREDIT_GAP+((TimerEdit_Option_H+TIMEREDIT_GAP)*item)+pad_y, TimerEdit_X+TIMEREDIT_PADDING+TimerEdit_Option_W-TIMEREDIT_GAP, "OK", fc, COLOR_None);
						break;
				}
				switch( fntSize ){
					case FNT_Size_1419:
						Font_Osd_PutString1419( rgn, TimerEdit_X+TIMEREDIT_PADDING+TIMEREDIT_GAP+(TimerEdit_Option_W/2)+(TIMEREDIT_GAP/2), TimerEdit_Y+TIMEREDIT_GAP+((TimerEdit_Option_H+TIMEREDIT_GAP)*item)+pad_y, TimerEdit_X+TIMEREDIT_PADDING+TimerEdit_Option_W-TIMEREDIT_GAP, "Cancel", fc, COLOR_None);
						break;
					case FNT_Size_1622:
						Font_Osd_PutString1622( rgn, TimerEdit_X+TIMEREDIT_PADDING+TIMEREDIT_GAP+(TimerEdit_Option_W/2)+(TIMEREDIT_GAP/2), TimerEdit_Y+TIMEREDIT_GAP+((TimerEdit_Option_H+TIMEREDIT_GAP)*item)+pad_y, TimerEdit_X+TIMEREDIT_PADDING+TimerEdit_Option_W-TIMEREDIT_GAP, "Cancel", fc, COLOR_None);
						break;
					case FNT_Size_1926:
						Font_Osd_PutString1926( rgn, TimerEdit_X+TIMEREDIT_PADDING+TIMEREDIT_GAP+(TimerEdit_Option_W/2)+(TIMEREDIT_GAP/2), TimerEdit_Y+TIMEREDIT_GAP+((TimerEdit_Option_H+TIMEREDIT_GAP)*item)+pad_y, TimerEdit_X+TIMEREDIT_PADDING+TimerEdit_Option_W-TIMEREDIT_GAP, "Cancel", fc, COLOR_None);
						break;
				}	
			} else {
//				TAP_Osd_FillBox(rgn, TimerEdit_X+TIMEREDIT_PADDING, TimerEdit_Y+TIMEREDIT_GAP+((TimerEdit_Option_H+TIMEREDIT_GAP)*item), (TimerEdit_Option_W/2)-(TIMEREDIT_GAP/2)-1, TimerEdit_Option_H, TimerEdit_Bc);
//				TAP_Osd_FillBox(rgn, TimerEdit_X+TIMEREDIT_PADDING+(TimerEdit_Option_W/2)+(TIMEREDIT_GAP/2), TimerEdit_Y+TIMEREDIT_GAP+((TimerEdit_Option_H+TIMEREDIT_GAP)*item), (TimerEdit_Option_W/2)-(TIMEREDIT_GAP/2)-1, TimerEdit_Option_H, TimerEdit_Bc);
				switch( fntSize ){
					case FNT_Size_1419:
						Font_Osd_PutString1419( rgn, TimerEdit_X+TIMEREDIT_PADDING+TIMEREDIT_GAP, TimerEdit_Y+TIMEREDIT_GAP+((TimerEdit_Option_H+TIMEREDIT_GAP)*item)+pad_y, TimerEdit_X+TIMEREDIT_PADDING+TimerEdit_Option_W-TIMEREDIT_GAP, "OK", fc, COLOR_None);
						break;
					case FNT_Size_1622:
						Font_Osd_PutString1622( rgn, TimerEdit_X+TIMEREDIT_PADDING+TIMEREDIT_GAP, TimerEdit_Y+TIMEREDIT_GAP+((TimerEdit_Option_H+TIMEREDIT_GAP)*item)+pad_y, TimerEdit_X+TIMEREDIT_PADDING+TimerEdit_Option_W-TIMEREDIT_GAP, "OK", fc, COLOR_None);
						break;
					case FNT_Size_1926:
						Font_Osd_PutString1926( rgn, TimerEdit_X+TIMEREDIT_PADDING+TIMEREDIT_GAP, TimerEdit_Y+TIMEREDIT_GAP+((TimerEdit_Option_H+TIMEREDIT_GAP)*item)+pad_y, TimerEdit_X+TIMEREDIT_PADDING+TimerEdit_Option_W-TIMEREDIT_GAP, "OK", fc, COLOR_None);
						break;
				}
				switch( fntSize ){
					case FNT_Size_1419:
						Font_Osd_PutString1419( rgn, TimerEdit_X+TIMEREDIT_PADDING+TIMEREDIT_GAP+(TimerEdit_Option_W/2)+(TIMEREDIT_GAP/2), TimerEdit_Y+TIMEREDIT_GAP+((TimerEdit_Option_H+TIMEREDIT_GAP)*item)+pad_y, TimerEdit_X+TIMEREDIT_PADDING+TimerEdit_Option_W-TIMEREDIT_GAP, "Cancel", fc, COLOR_None);
						break;
					case FNT_Size_1622:
						Font_Osd_PutString1622( rgn, TimerEdit_X+TIMEREDIT_PADDING+TIMEREDIT_GAP+(TimerEdit_Option_W/2)+(TIMEREDIT_GAP/2), TimerEdit_Y+TIMEREDIT_GAP+((TimerEdit_Option_H+TIMEREDIT_GAP)*item)+pad_y, TimerEdit_X+TIMEREDIT_PADDING+TimerEdit_Option_W-TIMEREDIT_GAP, "Cancel", fc, COLOR_None);
						break;
					case FNT_Size_1926:
						Font_Osd_PutString1926( rgn, TimerEdit_X+TIMEREDIT_PADDING+TIMEREDIT_GAP+(TimerEdit_Option_W/2)+(TIMEREDIT_GAP/2), TimerEdit_Y+TIMEREDIT_GAP+((TimerEdit_Option_H+TIMEREDIT_GAP)*item)+pad_y, TimerEdit_X+TIMEREDIT_PADDING+TimerEdit_Option_W-TIMEREDIT_GAP, "Cancel", fc, COLOR_None);
						break;
				}	
			}
		
			break;
	}
}

char * TimerEdit_ReserveType( int type ){
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

void TimerEdit_KeyboardClose( char * text, bool changed ){
	// Do nothing atm until it is implement
	if( changed ){
		// We have change the text of the timer so modify the fileName field
		memset( TimerEdit_Timer.fileName, '\0', sizeof(TimerEdit_Timer.fileName) ); // Blank the filename field
		strncpy(TimerEdit_Timer.fileName, text, sizeof(TimerEdit_Timer.fileName));
		TimerEdit_DrawItem( TimerEdit_RGN, TimerEdit_Selected, TimerEdit_Selected );
	}
}

// Function to draw the timer details to the region
void TimerEdit_DrawDetails( word rgn ){
	int i;
	for( i = 0; i < TIMEREDIT_OPTIONS; i++ ){
		TimerEdit_DrawItem( rgn, i, TimerEdit_Selected );
	}
}

int TimerEdit_Modify( void ){
	strcat(TimerEdit_Timer.fileName,".rec");
	return TAP_Timer_Modify(TimerEdit_TimerNum,&TimerEdit_Timer);
}

dword TimerEdit_KeyHandler( dword param1 ){
	if( Keyboard_IsActive() ){	// Handle the keyboard
		return Keyboard_Keyhandler(param1);
	}
	if( param1 == RKEY_Exit ){
		TimerEdit_Deactivate(0,TRUE);
		return 0;
	}
	// Do the navigation stuff now
	if( param1 == RKEY_ChUp ){
		TimerEdit_PrevSelected = TimerEdit_Selected;
		TimerEdit_Selected--;
		if( TimerEdit_Selected < 1 ){
			TimerEdit_Selected = TIMEREDIT_OPTIONS-1;
		}
		TimerEdit_DrawItem( TimerEdit_MemRGN, TimerEdit_PrevSelected, TimerEdit_Selected );
		TimerEdit_DrawItem( TimerEdit_MemRGN, TimerEdit_Selected, TimerEdit_Selected );
		TAP_Osd_Copy( TimerEdit_MemRGN, TimerEdit_RGN, TimerEdit_X+TIMEREDIT_PADDING, TimerEdit_Y+TIMEREDIT_GAP+((TimerEdit_Option_H+TIMEREDIT_GAP)*TimerEdit_PrevSelected), TimerEdit_Option_W, TimerEdit_Option_H, TimerEdit_X+TIMEREDIT_PADDING, TimerEdit_Y+TIMEREDIT_GAP+((TimerEdit_Option_H+TIMEREDIT_GAP)*TimerEdit_PrevSelected), FALSE);
		TAP_Osd_Copy( TimerEdit_MemRGN, TimerEdit_RGN, TimerEdit_X+TIMEREDIT_PADDING, TimerEdit_Y+TIMEREDIT_GAP+((TimerEdit_Option_H+TIMEREDIT_GAP)*TimerEdit_Selected), TimerEdit_Option_W, TimerEdit_Option_H, TimerEdit_X+TIMEREDIT_PADDING, TimerEdit_Y+TIMEREDIT_GAP+((TimerEdit_Option_H+TIMEREDIT_GAP)*TimerEdit_Selected), FALSE);
	}
	// Do the navigation stuff now
	if( param1 == RKEY_ChDown ){
		TimerEdit_PrevSelected = TimerEdit_Selected;
		TimerEdit_Selected++;
		if( TimerEdit_Selected > TIMEREDIT_OPTIONS-1 ){
			TimerEdit_Selected = 1;
		}
		TimerEdit_DrawItem( TimerEdit_MemRGN, TimerEdit_PrevSelected, TimerEdit_Selected );
		TimerEdit_DrawItem( TimerEdit_MemRGN, TimerEdit_Selected, TimerEdit_Selected );
		TAP_Osd_Copy( TimerEdit_MemRGN, TimerEdit_RGN, TimerEdit_X+TIMEREDIT_PADDING, TimerEdit_Y+TIMEREDIT_GAP+((TimerEdit_Option_H+TIMEREDIT_GAP)*TimerEdit_PrevSelected), TimerEdit_Option_W, TimerEdit_Option_H, TimerEdit_X+TIMEREDIT_PADDING, TimerEdit_Y+TIMEREDIT_GAP+((TimerEdit_Option_H+TIMEREDIT_GAP)*TimerEdit_PrevSelected), FALSE);
		TAP_Osd_Copy( TimerEdit_MemRGN, TimerEdit_RGN, TimerEdit_X+TIMEREDIT_PADDING, TimerEdit_Y+TIMEREDIT_GAP+((TimerEdit_Option_H+TIMEREDIT_GAP)*TimerEdit_Selected), TimerEdit_Option_W, TimerEdit_Option_H, TimerEdit_X+TIMEREDIT_PADDING, TimerEdit_Y+TIMEREDIT_GAP+((TimerEdit_Option_H+TIMEREDIT_GAP)*TimerEdit_Selected), FALSE);
	}
	// Do the navigation stuff now
	if( param1 == RKEY_VolDown || param1 == RKEY_VolUp || param1 == RKEY_Forward || param1 == RKEY_Rewind ){
		switch( TimerEdit_Selected ){
			case TIMEREDIT_TIMERSTART:
				TimerEdit_AlterStartTime(param1);
				TimerEdit_DrawItem( TimerEdit_MemRGN, TIMEREDIT_TIMEREND, TimerEdit_Selected );
				TAP_Osd_Copy( TimerEdit_MemRGN, TimerEdit_RGN, TimerEdit_X+TIMEREDIT_PADDING, TimerEdit_Y+TIMEREDIT_GAP+((TimerEdit_Option_H+TIMEREDIT_GAP)*TIMEREDIT_TIMEREND), TimerEdit_Option_W, TimerEdit_Option_H, TimerEdit_X+TIMEREDIT_PADDING, TimerEdit_Y+TIMEREDIT_GAP+((TimerEdit_Option_H+TIMEREDIT_GAP)*TIMEREDIT_TIMEREND), FALSE);
				break;
			case TIMEREDIT_TIMEREND:
				TimerEdit_AlterDuration(param1);
				break;
			case TIMEREDIT_TIMERSELECTION:
				TimerEdit_OptionSelected++;
				if( TimerEdit_OptionSelected > 1 ){
					TimerEdit_OptionSelected  = 0;
				}
				break;
			case TIMEREDIT_RESERVETYPE:
				switch ( param1 ){
					case RKEY_VolUp:
						switch ( TimerEdit_Timer.reservationType ){
							case RESERVE_TYPE_Onetime: 		TimerEdit_Timer.reservationType = RESERVE_TYPE_Everyday; break;
							case RESERVE_TYPE_Everyday: 	TimerEdit_Timer.reservationType = RESERVE_TYPE_EveryWeekend; break;
							case RESERVE_TYPE_EveryWeekend: TimerEdit_Timer.reservationType = RESERVE_TYPE_Weekly; break;
							case RESERVE_TYPE_Weekly: 		TimerEdit_Timer.reservationType = RESERVE_TYPE_WeekDay; break;
							case RESERVE_TYPE_WeekDay: 		TimerEdit_Timer.reservationType = RESERVE_TYPE_Onetime; break;
							default : break;
						}
						break;
					case RKEY_VolDown:
						switch ( TimerEdit_Timer.reservationType ){
							case RESERVE_TYPE_Onetime: 		TimerEdit_Timer.reservationType = RESERVE_TYPE_WeekDay; break;
							case RESERVE_TYPE_Everyday: 	TimerEdit_Timer.reservationType = RESERVE_TYPE_Onetime; break;
							case RESERVE_TYPE_EveryWeekend: TimerEdit_Timer.reservationType = RESERVE_TYPE_Everyday; break;
							case RESERVE_TYPE_Weekly: 		TimerEdit_Timer.reservationType = RESERVE_TYPE_EveryWeekend; break;
							case RESERVE_TYPE_WeekDay: 		TimerEdit_Timer.reservationType = RESERVE_TYPE_Weekly; break;
							default : break;
					}
					break;
					default :
						break;
				}
				break;
			case TIMEREDIT_DATE:
				TimerEdit_AdjustDate( param1 );
				break;
		}
		TimerEdit_DrawItem( TimerEdit_MemRGN, TimerEdit_Selected, TimerEdit_Selected );
		TAP_Osd_Copy( TimerEdit_MemRGN, TimerEdit_RGN, TimerEdit_X+TIMEREDIT_PADDING, TimerEdit_Y+TIMEREDIT_GAP+((TimerEdit_Option_H+TIMEREDIT_GAP)*TimerEdit_Selected), TimerEdit_Option_W, TimerEdit_Option_H, TimerEdit_X+TIMEREDIT_PADDING, TimerEdit_Y+TIMEREDIT_GAP+((TimerEdit_Option_H+TIMEREDIT_GAP)*TimerEdit_Selected), FALSE);
	}
	if( param1 == RKEY_Ok ){
		switch( TimerEdit_Selected ){
			case TIMEREDIT_TIMERNAME:
				Keyboard_Init( TimerEdit_RGN, TimerEdit_MemRGN, TimerEdit_X, TimerEdit_Y, TimerEdit_W, TimerEdit_H, "Timer Name" );	// Initalise the keyboard
				Keyboard_SetColours( DISPLAY_KEYBOARDITEM, DISPLAY_KEYBOARDITEMTEXT, DISPLAY_KEYBOARDITEMSELECTED, DISPLAY_KEYBOARDITEMSELECTEDTEXT, DISPLAY_KEYBOARDFIELDTEXT, DISPLAY_KEYBOARDFIELD, DISPLAY_KEYBOARDBACKGROUND );
				if( Keyboard_Activate(TimerEdit_Timer.fileName,&TimerEdit_KeyboardClose) ){
				}
				break;
			case TIMEREDIT_TIMERSELECTION:
				if( TimerEdit_OptionSelected == TIMEREDIT_OK ){
					TimerEdit_Deactivate(TimerEdit_Modify(),FALSE);
				} else {
					TimerEdit_Deactivate(0,TRUE);
				}
				break;
		}
	}
	return 0;
}

void TimerEdit_AdjustDate( dword param1 ){
	word 	mjd;
	int 	startTime; // , duration
	startTime = TimerEdit_Timer.startTime&0xffff;		// extract the time
	mjd = TimerEdit_Timer.startTime>>16;
	switch ( param1 ){
		case RKEY_VolUp:	mjd++;	break;			// Date up
		case RKEY_VolDown:	mjd--;	break;			// Date down
		default :			break;
	}
	TimerEdit_Timer.startTime = (mjd<<16) | startTime;
}

void TimerEdit_AlterDuration( dword param1 ){
	switch( param1 ){
		case RKEY_VolUp:							// Time up
			TimerEdit_Timer.duration++;
			break;
		case RKEY_VolDown:							// Time down
			if ( TimerEdit_Timer.duration > 0 ) TimerEdit_Timer.duration--;
			break;
		case RKEY_Forward:							// Time up
			TimerEdit_Timer.duration+=60;
			break;
		case RKEY_Rewind:							// Time down
			if ( TimerEdit_Timer.duration > 60 ) TimerEdit_Timer.duration-=60;
			break;
		default :
			break;
	}
}

void TimerEdit_AlterStartTime( dword param1 ){
	word 	mjd;
	int 	min, hour;
	hour = (TimerEdit_Timer.startTime&0xff00)>>8;			// extract the time
	min = (TimerEdit_Timer.startTime&0xff);
	mjd = TimerEdit_Timer.startTime>>16;
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
	TimerEdit_Timer.startTime = (mjd<<16) | ((hour&0xff)<<8) | (min&0xff);
}

// Function to set the colours of the Timer window
void TimerEdit_SetColours( dword fc, dword bc, dword fhc, dword bhc, dword base ){
	TimerEdit_Fc = fc;
	TimerEdit_HghBc = bhc;
	TimerEdit_HghFc = fhc;
	TimerEdit_Bc = bc;
	TimerEdit_BaseColour = base;
}


// Function to get and set the variable associated with editing the timer
// TO DO - Find the bug that results in a non match being found but timereditsmall works
bool TimerEdit_GetTimer( int row ){
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
					TimerEdit_Timer = currentTimerDefault;
					TimerEdit_TimerNum = i;
					return TRUE;
				}
				break;
			case RESERVE_TYPE_Everyday:
				// Initial check to determine against currentTimer start
				if( (result=Timer_TimerFind( currentChInfo.svcId, Events_GetEventSvcId(row), currentTimer.startTime, AddTime(currentTimer.startTime, currentTimer.duration), event.startTime, event.endTime )) != TIMER_NONE ){
					TimerEdit_Timer = currentTimerDefault;
					TimerEdit_TimerNum = i;
					return TRUE;
				}
				// Not found on today so we need to go and increment for the next 7 days and check on weekdays
				for( x = 0; x < TIMERS_MAXDAYSAHEAD; x++ ){
					currentTimer.startTime = AddTime( currentTimer.startTime, 1440 );
					if( (result=Timer_TimerFind( currentChInfo.svcId, Events_GetEventSvcId(row), currentTimer.startTime, AddTime(currentTimer.startTime, currentTimer.duration), event.startTime, event.endTime )) != TIMER_NONE ){
						TimerEdit_Timer = currentTimerDefault;
						TimerEdit_TimerNum = i;
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
							TimerEdit_Timer = currentTimerDefault;
							TimerEdit_TimerNum = i;
							return TRUE;
						}
					}
					// Not found on today so we need to go and increment for the next 7 days and check on weekdays
					for( x = 0; x < TIMERS_MAXDAYSAHEAD; x++ ){
						currentTimer.startTime = AddTime( currentTimer.startTime, 1440 );
						TAP_ExtractMjd( currentTimer.startTime>>16, &year, &month, &day, &weekDay);
						if( weekDay == SAT || weekDay == SUN  ){	// Sanity check to make sure timer is on a weekday
							if( (result=Timer_TimerFind( currentChInfo.svcId, Events_GetEventSvcId(row), currentTimer.startTime, AddTime(currentTimer.startTime, currentTimer.duration), event.startTime, event.endTime )) != TIMER_NONE ){
								TimerEdit_Timer = currentTimerDefault;
								TimerEdit_TimerNum = i;
								return TRUE;
							}
						}
					}
				}
				break;
			case RESERVE_TYPE_Weekly:
				if( (result=Timer_TimerFind( currentChInfo.svcId, Events_GetEventSvcId(row), currentTimer.startTime, AddTime(currentTimer.startTime, currentTimer.duration), event.startTime, event.endTime )) != TIMER_NONE ){
					TimerEdit_Timer = currentTimerDefault;
					TimerEdit_TimerNum = i;
					return TRUE;
				}
				for( x = 0; x < 7; x++ ){	// AddTime only handle 1440 max so loop the adding of the time
					currentTimer.startTime = AddTime(currentTimer.startTime, 1440 );
				}
				// Only search a week ahead atm as data shouldn't be anymore than that
				if( (result=Timer_TimerFind( currentChInfo.svcId, Events_GetEventSvcId(row), currentTimer.startTime, AddTime(currentTimer.startTime, currentTimer.duration), event.startTime, event.endTime )) != TIMER_NONE ){
					TimerEdit_Timer = currentTimerDefault;
					TimerEdit_TimerNum = i;
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
							TimerEdit_Timer = currentTimerDefault;
							TimerEdit_TimerNum = i;
							return TRUE;
						}
					}
					// Not found on today so we need to go and increment for the next 7 days and check on weekdays
					for( x = 0; x < TIMERS_MAXDAYSAHEAD; x++ ){
						currentTimer.startTime = AddTime( currentTimer.startTime, 1440 );
						TAP_ExtractMjd( currentTimer.startTime>>16, &year, &month, &day, &weekDay);
						if( weekDay != SAT && weekDay != SUN  ){	// Sanity check to make sure timer is on a weekday
							if( (result=Timer_TimerFind( currentChInfo.svcId, Events_GetEventSvcId(row), currentTimer.startTime, AddTime(currentTimer.startTime, currentTimer.duration), event.startTime, event.endTime )) != TIMER_NONE ){
								TimerEdit_Timer = currentTimerDefault;
								TimerEdit_TimerNum = i;
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


