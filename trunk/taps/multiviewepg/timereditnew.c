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

#include "timereditnew.h"
#include "fontman.h"
#include "skin.h"
#include "keyboard.h"

bool TimerEditNew_IsActive( void ){
	return TimerEditNew_Active;
}

// Function to initalize the keyboard - region and location to render
void TimerEditNew_Init( word rgn, word memrgn, int x, int y, int w, int h ){
	TimerEditNew_RGN = rgn;	// Store the region to render on
	TimerEditNew_MemRGN = memrgn;
	TimerEditNew_X = x;
	TimerEditNew_Y = y;
	TimerEditNew_W = w;
	TimerEditNew_H = h;
	TimerEditNew_Initalised = TRUE;
	TimerEditNew_Option_H = (TimerEditNew_H/TIMEREDITNEW_OPTIONS)-TIMEREDITNEW_GAP;
	TimerEditNew_Option_W = TimerEditNew_W - (TIMEREDITNEW_PADDING*2);
	TimerEditNew_CreateGradients();
}

void TimerEditNew_MemFree( void ){
	if (TimerEditNew_ItemBase) TAP_MemFree(TimerEditNew_ItemBase);
	if (TimerEditNew_ItemHigh) TAP_MemFree(TimerEditNew_ItemHigh);
	if (TimerEditNew_OkItemHigh) TAP_MemFree(TimerEditNew_OkItemHigh);
	if (TimerEditNew_CancelItemHigh) TAP_MemFree(TimerEditNew_CancelItemHigh);
	TimerEditNew_ItemBase = NULL;
	TimerEditNew_ItemHigh = NULL;
	TimerEditNew_OkItemHigh = NULL;
	TimerEditNew_CancelItemHigh = NULL;
}

void TimerEditNew_CreateGradients( void ){
	word rgn;
	TimerEditNew_MemFree();
	if( Settings_GradientFactor > 0 ){
		rgn	= TAP_Osd_Create(0, 0, TimerEditNew_Option_W, TimerEditNew_Option_H, 0, OSD_Flag_MemRgn);
		// Main default item
		createGradient(rgn, TimerEditNew_Option_W, TimerEditNew_Option_H, Settings_GradientFactor, DISPLAY_ITEM);
		TimerEditNew_ItemBase = TAP_Osd_SaveBox(rgn, 0, 0, TimerEditNew_Option_W, TimerEditNew_Option_H);
		createGradient(rgn, TimerEditNew_Option_W, TimerEditNew_Option_H, Settings_GradientFactor, DISPLAY_ITEMSELECTED);
		TimerEditNew_ItemHigh = TAP_Osd_SaveBox(rgn, 0, 0, TimerEditNew_Option_W, TimerEditNew_Option_H);
		TimerEditNew_CancelItemHigh = TAP_Osd_SaveBox(rgn, 0, 0, (TimerEditNew_Option_W/2)-(TIMEREDITNEW_GAP/2), TimerEditNew_Option_H);
		TimerEditNew_OkItemHigh = TAP_Osd_SaveBox(rgn, 0, 0, (TimerEditNew_Option_W/2)-(TIMEREDITNEW_GAP/2)-1, TimerEditNew_Option_H);
		TAP_Osd_Delete(rgn);
	}
}

bool TimerEditNew_Activate( TYPE_TimerInfo editTimer, void (*ReturningProcedure)( int, bool, TYPE_TimerInfo ) ){
	if( !TimerEditNew_Initalised ) return FALSE;	// We haven't initalised so gracefully fail
	TimerEditNew_Callback = ReturningProcedure;
	// Get the details of the event and create the timer structure
	TimerEditNew_Timer = editTimer;
	StripFileName(TimerEditNew_Timer.fileName);
	TimerEditNew_Selected = 1;
	TimerEditNew_PrevSelected = 1;
	TimerEditNew_OptionSelected = 0;
	// Now lets render the display
	TimerEditNew_DrawBackground(TimerEditNew_MemRGN);
	TimerEditNew_DrawDetails(TimerEditNew_MemRGN);
	// Coppy rendered to foreground
	TAP_Osd_Copy( TimerEditNew_MemRGN, TimerEditNew_RGN, TimerEditNew_X, TimerEditNew_Y, TimerEditNew_W, TimerEditNew_H, TimerEditNew_X, TimerEditNew_Y, FALSE);
	TimerEditNew_Active = TRUE;
	return TRUE;
}

// Routine to handle the closing of the timer window
void TimerEditNew_Deactivate( int retval, bool cancel, TYPE_TimerInfo timer ){
	TimerEditNew_Active = FALSE;
	// Restore what we had drawn over
	TAP_Osd_RestoreBox(TimerEditNew_RGN, TimerEditNew_X, TimerEditNew_Y, TimerEditNew_W, TimerEditNew_H, TimerEditNew_Base);
	TAP_MemFree(TimerEditNew_Base);
	TimerEditNew_Base = NULL;
	TimerEditNew_MemFree();
	TimerEditNew_Callback(retval, cancel, timer);
}


// Here we will just draw the basic layout of the keyboard
void TimerEditNew_DrawBackground( word rgn ){
	if( TimerEditNew_Base != NULL ) TAP_MemFree(TimerEditNew_Base);
	TimerEditNew_Base = TAP_Osd_SaveBox(TimerEditNew_RGN, TimerEditNew_X, TimerEditNew_Y, TimerEditNew_W, TimerEditNew_H);	// Save what is under the Timer screen
	TAP_Osd_FillBox(rgn, TimerEditNew_X, TimerEditNew_Y, TimerEditNew_W, TimerEditNew_H, TimerEditNew_BaseColour);
}

void TimerEditNew_DrawItem( word rgn, int item, int selected ){
	int fntSize = 0;
	int pad_y;
	dword fc = TimerEditNew_Fc;
	dword bc = TimerEditNew_Bc;
	// Work out the font size allowed for the height of the option
	if( ( TimerEditNew_Option_H - 8 ) > 19  ){
		if( ( TimerEditNew_Option_H - 12 ) > 22 ){
			// Use the big font
			fntSize = FNT_Size_1926;
			pad_y = (TimerEditNew_Option_H/2)-(26/2);
		} else {
			fntSize = FNT_Size_1622;
			pad_y = (TimerEditNew_Option_H/2)-(22/2);
		}
	} else {
		pad_y = (TimerEditNew_Option_H/2)-(19/2);
	}
	// If selected change the colors of the elements
	if( item == selected ){
		fc = TimerEditNew_HghFc;
		bc = TimerEditNew_HghBc;
	}
	switch( item ){
		case TIMEREDITNEW_SECTIONTITLE:
			TAP_Osd_FillBox(rgn, TimerEditNew_X+TIMEREDITNEW_PADDING, TimerEditNew_Y+TIMEREDITNEW_GAP+((TimerEditNew_Option_H+TIMEREDITNEW_GAP)*item), TimerEditNew_Option_W, TimerEditNew_Option_H, TimerEditNew_BaseColour);
			switch( fntSize ){
				case FNT_Size_1419:
					Font_Osd_PutString1419( rgn, TimerEditNew_X+TIMEREDITNEW_PADDING+TIMEREDITNEW_GAP, TimerEditNew_Y+TIMEREDITNEW_GAP+((TimerEditNew_Option_H+TIMEREDITNEW_GAP)*item)+pad_y, TimerEditNew_X+TIMEREDITNEW_PADDING+TimerEditNew_Option_W-TIMEREDITNEW_GAP, "Timer Details", fc, COLOR_None);
					break;
				case FNT_Size_1622:
					Font_Osd_PutString1622( rgn, TimerEditNew_X+TIMEREDITNEW_PADDING+TIMEREDITNEW_GAP, TimerEditNew_Y+TIMEREDITNEW_GAP+((TimerEditNew_Option_H+TIMEREDITNEW_GAP)*item)+pad_y, TimerEditNew_X+TIMEREDITNEW_PADDING+TimerEditNew_Option_W-TIMEREDITNEW_GAP, "Timer Details", fc, COLOR_None);
					break;
				case FNT_Size_1926:
					Font_Osd_PutString1926( rgn, TimerEditNew_X+TIMEREDITNEW_PADDING+TIMEREDITNEW_GAP, TimerEditNew_Y+TIMEREDITNEW_GAP+((TimerEditNew_Option_H+TIMEREDITNEW_GAP)*item)+pad_y, TimerEditNew_X+TIMEREDITNEW_PADDING+TimerEditNew_Option_W-TIMEREDITNEW_GAP, "Timer Details", fc, COLOR_None);
					break;
			}
			break;
		case TIMEREDITNEW_TIMERNAME:
			if( item == selected ){
				if( TimerEditNew_ItemHigh != NULL ){
					TAP_Osd_RestoreBox( rgn, TimerEditNew_X+TIMEREDITNEW_PADDING, TimerEditNew_Y+TIMEREDITNEW_GAP+((TimerEditNew_Option_H+TIMEREDITNEW_GAP)*item), TimerEditNew_Option_W, TimerEditNew_Option_H, TimerEditNew_ItemHigh );
				} else {
					TAP_Osd_FillBox(rgn, TimerEditNew_X+TIMEREDITNEW_PADDING, TimerEditNew_Y+TIMEREDITNEW_GAP+((TimerEditNew_Option_H+TIMEREDITNEW_GAP)*item), TimerEditNew_Option_W, TimerEditNew_Option_H, bc);
				}
			} else {
				if( TimerEditNew_ItemBase != NULL ){
					TAP_Osd_RestoreBox( rgn, TimerEditNew_X+TIMEREDITNEW_PADDING, TimerEditNew_Y+TIMEREDITNEW_GAP+((TimerEditNew_Option_H+TIMEREDITNEW_GAP)*item), TimerEditNew_Option_W, TimerEditNew_Option_H, TimerEditNew_ItemBase );
				} else {
					TAP_Osd_FillBox(rgn, TimerEditNew_X+TIMEREDITNEW_PADDING, TimerEditNew_Y+TIMEREDITNEW_GAP+((TimerEditNew_Option_H+TIMEREDITNEW_GAP)*item), TimerEditNew_Option_W, TimerEditNew_Option_H, bc);
				}
			}
//			TAP_Osd_FillBox(rgn, TimerEditNew_X+TIMEREDITNEW_PADDING, TimerEditNew_Y+TIMEREDITNEW_GAP+((TimerEditNew_Option_H+TIMEREDITNEW_GAP)*item), TimerEditNew_Option_W, TimerEditNew_Option_H, bc);
			switch( fntSize ){
				case FNT_Size_1419:
					Font_Osd_PutString1419( rgn, TimerEditNew_X+TIMEREDITNEW_PADDING+TIMEREDITNEW_GAP, TimerEditNew_Y+TIMEREDITNEW_GAP+((TimerEditNew_Option_H+TIMEREDITNEW_GAP)*item)+pad_y, TimerEditNew_X+TIMEREDITNEW_PADDING+TimerEditNew_Option_W-TIMEREDITNEW_GAP, TimerEditNew_Timer.fileName, fc, COLOR_None);
					break;
				case FNT_Size_1622:
					Font_Osd_PutString1622( rgn, TimerEditNew_X+TIMEREDITNEW_PADDING+TIMEREDITNEW_GAP, TimerEditNew_Y+TIMEREDITNEW_GAP+((TimerEditNew_Option_H+TIMEREDITNEW_GAP)*item)+pad_y, TimerEditNew_X+TIMEREDITNEW_PADDING+TimerEditNew_Option_W-TIMEREDITNEW_GAP, TimerEditNew_Timer.fileName, fc, COLOR_None);
					break;
				case FNT_Size_1926:
					Font_Osd_PutString1926( rgn, TimerEditNew_X+TIMEREDITNEW_PADDING+TIMEREDITNEW_GAP, TimerEditNew_Y+TIMEREDITNEW_GAP+((TimerEditNew_Option_H+TIMEREDITNEW_GAP)*item)+pad_y, TimerEditNew_X+TIMEREDITNEW_PADDING+TimerEditNew_Option_W-TIMEREDITNEW_GAP, TimerEditNew_Timer.fileName, fc, COLOR_None);
					break;
			}
			break;
		case TIMEREDITNEW_TIMERSTART:
			if( item == selected ){
				if( TimerEditNew_ItemHigh != NULL ){
					TAP_Osd_RestoreBox( rgn, TimerEditNew_X+TIMEREDITNEW_PADDING, TimerEditNew_Y+TIMEREDITNEW_GAP+((TimerEditNew_Option_H+TIMEREDITNEW_GAP)*item), TimerEditNew_Option_W, TimerEditNew_Option_H, TimerEditNew_ItemHigh );
				} else {
					TAP_Osd_FillBox(rgn, TimerEditNew_X+TIMEREDITNEW_PADDING, TimerEditNew_Y+TIMEREDITNEW_GAP+((TimerEditNew_Option_H+TIMEREDITNEW_GAP)*item), TimerEditNew_Option_W, TimerEditNew_Option_H, bc);
				}
			} else {
				if( TimerEditNew_ItemBase != NULL ){
					TAP_Osd_RestoreBox( rgn, TimerEditNew_X+TIMEREDITNEW_PADDING, TimerEditNew_Y+TIMEREDITNEW_GAP+((TimerEditNew_Option_H+TIMEREDITNEW_GAP)*item), TimerEditNew_Option_W, TimerEditNew_Option_H, TimerEditNew_ItemBase );
				} else {
					TAP_Osd_FillBox(rgn, TimerEditNew_X+TIMEREDITNEW_PADDING, TimerEditNew_Y+TIMEREDITNEW_GAP+((TimerEditNew_Option_H+TIMEREDITNEW_GAP)*item), TimerEditNew_Option_W, TimerEditNew_Option_H, bc);
				}
			}
//			TAP_Osd_FillBox(rgn, TimerEditNew_X+TIMEREDITNEW_PADDING, TimerEditNew_Y+TIMEREDITNEW_GAP+((TimerEditNew_Option_H+TIMEREDITNEW_GAP)*item), TimerEditNew_Option_W, TimerEditNew_Option_H, bc);
			switch( fntSize ){
				case FNT_Size_1419:
					Font_Osd_PutString1419( rgn, TimerEditNew_X+TIMEREDITNEW_PADDING+TIMEREDITNEW_GAP, TimerEditNew_Y+TIMEREDITNEW_GAP+((TimerEditNew_Option_H+TIMEREDITNEW_GAP)*item)+pad_y, TimerEditNew_X+TIMEREDITNEW_PADDING+TimerEditNew_Option_W-TIMEREDITNEW_GAP, Time_HHMM(TimerEditNew_Timer.startTime), fc, COLOR_None);
					break;
				case FNT_Size_1622:
					Font_Osd_PutString1622( rgn, TimerEditNew_X+TIMEREDITNEW_PADDING+TIMEREDITNEW_GAP, TimerEditNew_Y+TIMEREDITNEW_GAP+((TimerEditNew_Option_H+TIMEREDITNEW_GAP)*item)+pad_y, TimerEditNew_X+TIMEREDITNEW_PADDING+TimerEditNew_Option_W-TIMEREDITNEW_GAP, Time_HHMM(TimerEditNew_Timer.startTime), fc, COLOR_None);
					break;
				case FNT_Size_1926:
					Font_Osd_PutString1926( rgn, TimerEditNew_X+TIMEREDITNEW_PADDING+TIMEREDITNEW_GAP, TimerEditNew_Y+TIMEREDITNEW_GAP+((TimerEditNew_Option_H+TIMEREDITNEW_GAP)*item)+pad_y, TimerEditNew_X+TIMEREDITNEW_PADDING+TimerEditNew_Option_W-TIMEREDITNEW_GAP, Time_HHMM(TimerEditNew_Timer.startTime), fc, COLOR_None);
					break;
			}
			break;
		case TIMEREDITNEW_TIMEREND:
			if( item == selected ){
				if( TimerEditNew_ItemHigh != NULL ){
					TAP_Osd_RestoreBox( rgn, TimerEditNew_X+TIMEREDITNEW_PADDING, TimerEditNew_Y+TIMEREDITNEW_GAP+((TimerEditNew_Option_H+TIMEREDITNEW_GAP)*item), TimerEditNew_Option_W, TimerEditNew_Option_H, TimerEditNew_ItemHigh );
				} else {
					TAP_Osd_FillBox(rgn, TimerEditNew_X+TIMEREDITNEW_PADDING, TimerEditNew_Y+TIMEREDITNEW_GAP+((TimerEditNew_Option_H+TIMEREDITNEW_GAP)*item), TimerEditNew_Option_W, TimerEditNew_Option_H, bc);
				}
			} else {
				if( TimerEditNew_ItemBase != NULL ){
					TAP_Osd_RestoreBox( rgn, TimerEditNew_X+TIMEREDITNEW_PADDING, TimerEditNew_Y+TIMEREDITNEW_GAP+((TimerEditNew_Option_H+TIMEREDITNEW_GAP)*item), TimerEditNew_Option_W, TimerEditNew_Option_H, TimerEditNew_ItemBase );
				} else {
					TAP_Osd_FillBox(rgn, TimerEditNew_X+TIMEREDITNEW_PADDING, TimerEditNew_Y+TIMEREDITNEW_GAP+((TimerEditNew_Option_H+TIMEREDITNEW_GAP)*item), TimerEditNew_Option_W, TimerEditNew_Option_H, bc);
				}
			}
//			TAP_Osd_FillBox(rgn, TimerEditNew_X+TIMEREDITNEW_PADDING, TimerEditNew_Y+TIMEREDITNEW_GAP+((TimerEditNew_Option_H+TIMEREDITNEW_GAP)*item), TimerEditNew_Option_W, TimerEditNew_Option_H, bc);
			switch( fntSize ){
				case FNT_Size_1419:
					Font_Osd_PutString1419( rgn, TimerEditNew_X+TIMEREDITNEW_PADDING+TIMEREDITNEW_GAP, TimerEditNew_Y+TIMEREDITNEW_GAP+((TimerEditNew_Option_H+TIMEREDITNEW_GAP)*item)+pad_y, TimerEditNew_X+TIMEREDITNEW_PADDING+TimerEditNew_Option_W-TIMEREDITNEW_GAP, Endtime_HHMM(TimerEditNew_Timer.startTime,TimerEditNew_Timer.duration), fc, COLOR_None);
					break;
				case FNT_Size_1622:
					Font_Osd_PutString1622( rgn, TimerEditNew_X+TIMEREDITNEW_PADDING+TIMEREDITNEW_GAP, TimerEditNew_Y+TIMEREDITNEW_GAP+((TimerEditNew_Option_H+TIMEREDITNEW_GAP)*item)+pad_y, TimerEditNew_X+TIMEREDITNEW_PADDING+TimerEditNew_Option_W-TIMEREDITNEW_GAP, Endtime_HHMM(TimerEditNew_Timer.startTime,TimerEditNew_Timer.duration), fc, COLOR_None);
					break;
				case FNT_Size_1926:
					Font_Osd_PutString1926( rgn, TimerEditNew_X+TIMEREDITNEW_PADDING+TIMEREDITNEW_GAP, TimerEditNew_Y+TIMEREDITNEW_GAP+((TimerEditNew_Option_H+TIMEREDITNEW_GAP)*item)+pad_y, TimerEditNew_X+TIMEREDITNEW_PADDING+TimerEditNew_Option_W-TIMEREDITNEW_GAP, Endtime_HHMM(TimerEditNew_Timer.startTime,TimerEditNew_Timer.duration), fc, COLOR_None);
					break;
			}
			break;
		case TIMEREDITNEW_RESERVETYPE:
			if( item == selected ){
				if( TimerEditNew_ItemHigh != NULL ){
					TAP_Osd_RestoreBox( rgn, TimerEditNew_X+TIMEREDITNEW_PADDING, TimerEditNew_Y+TIMEREDITNEW_GAP+((TimerEditNew_Option_H+TIMEREDITNEW_GAP)*item), TimerEditNew_Option_W, TimerEditNew_Option_H, TimerEditNew_ItemHigh );
				} else {
					TAP_Osd_FillBox(rgn, TimerEditNew_X+TIMEREDITNEW_PADDING, TimerEditNew_Y+TIMEREDITNEW_GAP+((TimerEditNew_Option_H+TIMEREDITNEW_GAP)*item), TimerEditNew_Option_W, TimerEditNew_Option_H, bc);
				}
			} else {
				if( TimerEditNew_ItemBase != NULL ){
					TAP_Osd_RestoreBox( rgn, TimerEditNew_X+TIMEREDITNEW_PADDING, TimerEditNew_Y+TIMEREDITNEW_GAP+((TimerEditNew_Option_H+TIMEREDITNEW_GAP)*item), TimerEditNew_Option_W, TimerEditNew_Option_H, TimerEditNew_ItemBase );
				} else {
					TAP_Osd_FillBox(rgn, TimerEditNew_X+TIMEREDITNEW_PADDING, TimerEditNew_Y+TIMEREDITNEW_GAP+((TimerEditNew_Option_H+TIMEREDITNEW_GAP)*item), TimerEditNew_Option_W, TimerEditNew_Option_H, bc);
				}
			}
//			TAP_Osd_FillBox(rgn, TimerEditNew_X+TIMEREDITNEW_PADDING, TimerEditNew_Y+TIMEREDITNEW_GAP+((TimerEditNew_Option_H+TIMEREDITNEW_GAP)*item), TimerEditNew_Option_W, TimerEditNew_Option_H, bc);
			switch( fntSize ){
				case FNT_Size_1419:
					Font_Osd_PutString1419( rgn, TimerEditNew_X+TIMEREDITNEW_PADDING+TIMEREDITNEW_GAP, TimerEditNew_Y+TIMEREDITNEW_GAP+((TimerEditNew_Option_H+TIMEREDITNEW_GAP)*item)+pad_y, TimerEditNew_X+TIMEREDITNEW_PADDING+TimerEditNew_Option_W-TIMEREDITNEW_GAP, TimerEditNew_ReserveType(TimerEditNew_Timer.reservationType), fc, COLOR_None);
					break;
				case FNT_Size_1622:
					Font_Osd_PutString1622( rgn, TimerEditNew_X+TIMEREDITNEW_PADDING+TIMEREDITNEW_GAP, TimerEditNew_Y+TIMEREDITNEW_GAP+((TimerEditNew_Option_H+TIMEREDITNEW_GAP)*item)+pad_y, TimerEditNew_X+TIMEREDITNEW_PADDING+TimerEditNew_Option_W-TIMEREDITNEW_GAP, TimerEditNew_ReserveType(TimerEditNew_Timer.reservationType), fc, COLOR_None);
					break;
				case FNT_Size_1926:
					Font_Osd_PutString1926( rgn, TimerEditNew_X+TIMEREDITNEW_PADDING+TIMEREDITNEW_GAP, TimerEditNew_Y+TIMEREDITNEW_GAP+((TimerEditNew_Option_H+TIMEREDITNEW_GAP)*item)+pad_y, TimerEditNew_X+TIMEREDITNEW_PADDING+TimerEditNew_Option_W-TIMEREDITNEW_GAP, TimerEditNew_ReserveType(TimerEditNew_Timer.reservationType), fc, COLOR_None);
					break;
			}
			break;		
		case TIMEREDITNEW_DATE:
			if( item == selected ){
				if( TimerEditNew_ItemHigh != NULL ){
					TAP_Osd_RestoreBox( rgn, TimerEditNew_X+TIMEREDITNEW_PADDING, TimerEditNew_Y+TIMEREDITNEW_GAP+((TimerEditNew_Option_H+TIMEREDITNEW_GAP)*item), TimerEditNew_Option_W, TimerEditNew_Option_H, TimerEditNew_ItemHigh );
				} else {
					TAP_Osd_FillBox(rgn, TimerEditNew_X+TIMEREDITNEW_PADDING, TimerEditNew_Y+TIMEREDITNEW_GAP+((TimerEditNew_Option_H+TIMEREDITNEW_GAP)*item), TimerEditNew_Option_W, TimerEditNew_Option_H, bc);
				}
			} else {
				if( TimerEditNew_ItemBase != NULL ){
					TAP_Osd_RestoreBox( rgn, TimerEditNew_X+TIMEREDITNEW_PADDING, TimerEditNew_Y+TIMEREDITNEW_GAP+((TimerEditNew_Option_H+TIMEREDITNEW_GAP)*item), TimerEditNew_Option_W, TimerEditNew_Option_H, TimerEditNew_ItemBase );
				} else {
					TAP_Osd_FillBox(rgn, TimerEditNew_X+TIMEREDITNEW_PADDING, TimerEditNew_Y+TIMEREDITNEW_GAP+((TimerEditNew_Option_H+TIMEREDITNEW_GAP)*item), TimerEditNew_Option_W, TimerEditNew_Option_H, bc);
				}
			}
//			TAP_Osd_FillBox(rgn, TimerEditNew_X+TIMEREDITNEW_PADDING, TimerEditNew_Y+TIMEREDITNEW_GAP+((TimerEditNew_Option_H+TIMEREDITNEW_GAP)*item), TimerEditNew_Option_W, TimerEditNew_Option_H, bc);
			switch( fntSize ){
				case FNT_Size_1419:
					Font_Osd_PutString1419( rgn, TimerEditNew_X+TIMEREDITNEW_PADDING+TIMEREDITNEW_GAP, TimerEditNew_Y+TIMEREDITNEW_GAP+((TimerEditNew_Option_H+TIMEREDITNEW_GAP)*item)+pad_y, TimerEditNew_X+TIMEREDITNEW_PADDING+TimerEditNew_Option_W-TIMEREDITNEW_GAP, Time_DOWDDMMSpec(TimerEditNew_Timer.startTime), fc, COLOR_None);
					break;
				case FNT_Size_1622:
					Font_Osd_PutString1622( rgn, TimerEditNew_X+TIMEREDITNEW_PADDING+TIMEREDITNEW_GAP, TimerEditNew_Y+TIMEREDITNEW_GAP+((TimerEditNew_Option_H+TIMEREDITNEW_GAP)*item)+pad_y, TimerEditNew_X+TIMEREDITNEW_PADDING+TimerEditNew_Option_W-TIMEREDITNEW_GAP, Time_DOWDDMMSpec(TimerEditNew_Timer.startTime), fc, COLOR_None);
					break;
				case FNT_Size_1926:
					Font_Osd_PutString1926( rgn, TimerEditNew_X+TIMEREDITNEW_PADDING+TIMEREDITNEW_GAP, TimerEditNew_Y+TIMEREDITNEW_GAP+((TimerEditNew_Option_H+TIMEREDITNEW_GAP)*item)+pad_y, TimerEditNew_X+TIMEREDITNEW_PADDING+TimerEditNew_Option_W-TIMEREDITNEW_GAP, Time_DOWDDMMSpec(TimerEditNew_Timer.startTime), fc, COLOR_None);
					break;
			}
			break;	
			break;
		case TIMEREDITNEW_TIMERSELECTION:
			if( TimerEditNew_ItemBase != NULL ){
				TAP_Osd_RestoreBox( rgn, TimerEditNew_X+TIMEREDITNEW_PADDING, TimerEditNew_Y+TIMEREDITNEW_GAP+((TimerEditNew_Option_H+TIMEREDITNEW_GAP)*item), TimerEditNew_Option_W, TimerEditNew_Option_H, TimerEditNew_ItemBase );
			} else {
				TAP_Osd_FillBox(rgn, TimerEditNew_X+TIMEREDITNEW_PADDING, TimerEditNew_Y+TIMEREDITNEW_GAP+((TimerEditNew_Option_H+TIMEREDITNEW_GAP)*item), TimerEditNew_Option_W, TimerEditNew_Option_H, bc);
			}
			TAP_Osd_FillBox(rgn, TimerEditNew_X+TIMEREDITNEW_PADDING+(TimerEditNew_Option_W/2)-(TIMEREDITNEW_GAP/2), TimerEditNew_Y+TIMEREDITNEW_GAP+((TimerEditNew_Option_H+TIMEREDITNEW_GAP)*item), TIMEREDITNEW_GAP-1, TimerEditNew_Option_H, TimerEditNew_BaseColour);
			if( item == selected ){
				if( TimerEditNew_OptionSelected == TIMEREDITNEW_OK ){
					if( TimerEditNew_OkItemHigh != NULL ){
						TAP_Osd_RestoreBox( rgn, TimerEditNew_X+TIMEREDITNEW_PADDING, TimerEditNew_Y+TIMEREDITNEW_GAP+((TimerEditNew_Option_H+TIMEREDITNEW_GAP)*item), (TimerEditNew_Option_W/2)-(TIMEREDITNEW_GAP/2)-1, TimerEditNew_Option_H, TimerEditNew_OkItemHigh );
					} else {
						TAP_Osd_FillBox(rgn, TimerEditNew_X+TIMEREDITNEW_PADDING, TimerEditNew_Y+TIMEREDITNEW_GAP+((TimerEditNew_Option_H+TIMEREDITNEW_GAP)*item), (TimerEditNew_Option_W/2)-(TIMEREDITNEW_GAP/2), TimerEditNew_Option_H, TimerEditNew_HghBc);
					}
				} else {
					if( TimerEditNew_OkItemHigh != NULL ){
						TAP_Osd_RestoreBox( rgn, TimerEditNew_X+TIMEREDITNEW_PADDING+(TimerEditNew_Option_W/2)+(TIMEREDITNEW_GAP/2), TimerEditNew_Y+TIMEREDITNEW_GAP+((TimerEditNew_Option_H+TIMEREDITNEW_GAP)*item), (TimerEditNew_Option_W/2)-(TIMEREDITNEW_GAP/2)-1, TimerEditNew_Option_H, TimerEditNew_CancelItemHigh );
					} else {
						TAP_Osd_FillBox(rgn, TimerEditNew_X+TIMEREDITNEW_PADDING+(TimerEditNew_Option_W/2)+(TIMEREDITNEW_GAP/2), TimerEditNew_Y+TIMEREDITNEW_GAP+((TimerEditNew_Option_H+TIMEREDITNEW_GAP)*item), (TimerEditNew_Option_W/2)-(TIMEREDITNEW_GAP/2)-1, TimerEditNew_Option_H, TimerEditNew_HghBc);
					}
				}
				switch( fntSize ){
					case FNT_Size_1419:
						Font_Osd_PutString1419( rgn, TimerEditNew_X+TIMEREDITNEW_PADDING+TIMEREDITNEW_GAP, TimerEditNew_Y+TIMEREDITNEW_GAP+((TimerEditNew_Option_H+TIMEREDITNEW_GAP)*item)+pad_y, TimerEditNew_X+TIMEREDITNEW_PADDING+TimerEditNew_Option_W-TIMEREDITNEW_GAP, "OK", fc, COLOR_None);
						break;
					case FNT_Size_1622:
						Font_Osd_PutString1622( rgn, TimerEditNew_X+TIMEREDITNEW_PADDING+TIMEREDITNEW_GAP, TimerEditNew_Y+TIMEREDITNEW_GAP+((TimerEditNew_Option_H+TIMEREDITNEW_GAP)*item)+pad_y, TimerEditNew_X+TIMEREDITNEW_PADDING+TimerEditNew_Option_W-TIMEREDITNEW_GAP, "OK", fc, COLOR_None);
						break;
					case FNT_Size_1926:
						Font_Osd_PutString1926( rgn, TimerEditNew_X+TIMEREDITNEW_PADDING+TIMEREDITNEW_GAP, TimerEditNew_Y+TIMEREDITNEW_GAP+((TimerEditNew_Option_H+TIMEREDITNEW_GAP)*item)+pad_y, TimerEditNew_X+TIMEREDITNEW_PADDING+TimerEditNew_Option_W-TIMEREDITNEW_GAP, "OK", fc, COLOR_None);
						break;
				}
				switch( fntSize ){
					case FNT_Size_1419:
						Font_Osd_PutString1419( rgn, TimerEditNew_X+TIMEREDITNEW_PADDING+TIMEREDITNEW_GAP+(TimerEditNew_Option_W/2)+(TIMEREDITNEW_GAP/2), TimerEditNew_Y+TIMEREDITNEW_GAP+((TimerEditNew_Option_H+TIMEREDITNEW_GAP)*item)+pad_y, TimerEditNew_X+TIMEREDITNEW_PADDING+TimerEditNew_Option_W-TIMEREDITNEW_GAP, "Cancel", fc, COLOR_None);
						break;
					case FNT_Size_1622:
						Font_Osd_PutString1622( rgn, TimerEditNew_X+TIMEREDITNEW_PADDING+TIMEREDITNEW_GAP+(TimerEditNew_Option_W/2)+(TIMEREDITNEW_GAP/2), TimerEditNew_Y+TIMEREDITNEW_GAP+((TimerEditNew_Option_H+TIMEREDITNEW_GAP)*item)+pad_y, TimerEditNew_X+TIMEREDITNEW_PADDING+TimerEditNew_Option_W-TIMEREDITNEW_GAP, "Cancel", fc, COLOR_None);
						break;
					case FNT_Size_1926:
						Font_Osd_PutString1926( rgn, TimerEditNew_X+TIMEREDITNEW_PADDING+TIMEREDITNEW_GAP+(TimerEditNew_Option_W/2)+(TIMEREDITNEW_GAP/2), TimerEditNew_Y+TIMEREDITNEW_GAP+((TimerEditNew_Option_H+TIMEREDITNEW_GAP)*item)+pad_y, TimerEditNew_X+TIMEREDITNEW_PADDING+TimerEditNew_Option_W-TIMEREDITNEW_GAP, "Cancel", fc, COLOR_None);
						break;
				}	
			} else {
//				TAP_Osd_FillBox(rgn, TimerEditNew_X+TIMEREDITNEW_PADDING, TimerEditNew_Y+TIMEREDITNEW_GAP+((TimerEditNew_Option_H+TIMEREDITNEW_GAP)*item), (TimerEditNew_Option_W/2)-(TIMEREDITNEW_GAP/2)-1, TimerEditNew_Option_H, TimerEditNew_Bc);
//				TAP_Osd_FillBox(rgn, TimerEditNew_X+TIMEREDITNEW_PADDING+(TimerEditNew_Option_W/2)+(TIMEREDITNEW_GAP/2), TimerEditNew_Y+TIMEREDITNEW_GAP+((TimerEditNew_Option_H+TIMEREDITNEW_GAP)*item), (TimerEditNew_Option_W/2)-(TIMEREDITNEW_GAP/2)-1, TimerEditNew_Option_H, TimerEditNew_Bc);
				switch( fntSize ){
					case FNT_Size_1419:
						Font_Osd_PutString1419( rgn, TimerEditNew_X+TIMEREDITNEW_PADDING+TIMEREDITNEW_GAP, TimerEditNew_Y+TIMEREDITNEW_GAP+((TimerEditNew_Option_H+TIMEREDITNEW_GAP)*item)+pad_y, TimerEditNew_X+TIMEREDITNEW_PADDING+TimerEditNew_Option_W-TIMEREDITNEW_GAP, "OK", fc, COLOR_None);
						break;
					case FNT_Size_1622:
						Font_Osd_PutString1622( rgn, TimerEditNew_X+TIMEREDITNEW_PADDING+TIMEREDITNEW_GAP, TimerEditNew_Y+TIMEREDITNEW_GAP+((TimerEditNew_Option_H+TIMEREDITNEW_GAP)*item)+pad_y, TimerEditNew_X+TIMEREDITNEW_PADDING+TimerEditNew_Option_W-TIMEREDITNEW_GAP, "OK", fc, COLOR_None);
						break;
					case FNT_Size_1926:
						Font_Osd_PutString1926( rgn, TimerEditNew_X+TIMEREDITNEW_PADDING+TIMEREDITNEW_GAP, TimerEditNew_Y+TIMEREDITNEW_GAP+((TimerEditNew_Option_H+TIMEREDITNEW_GAP)*item)+pad_y, TimerEditNew_X+TIMEREDITNEW_PADDING+TimerEditNew_Option_W-TIMEREDITNEW_GAP, "OK", fc, COLOR_None);
						break;
				}
				switch( fntSize ){
					case FNT_Size_1419:
						Font_Osd_PutString1419( rgn, TimerEditNew_X+TIMEREDITNEW_PADDING+TIMEREDITNEW_GAP+(TimerEditNew_Option_W/2)+(TIMEREDITNEW_GAP/2), TimerEditNew_Y+TIMEREDITNEW_GAP+((TimerEditNew_Option_H+TIMEREDITNEW_GAP)*item)+pad_y, TimerEditNew_X+TIMEREDITNEW_PADDING+TimerEditNew_Option_W-TIMEREDITNEW_GAP, "Cancel", fc, COLOR_None);
						break;
					case FNT_Size_1622:
						Font_Osd_PutString1622( rgn, TimerEditNew_X+TIMEREDITNEW_PADDING+TIMEREDITNEW_GAP+(TimerEditNew_Option_W/2)+(TIMEREDITNEW_GAP/2), TimerEditNew_Y+TIMEREDITNEW_GAP+((TimerEditNew_Option_H+TIMEREDITNEW_GAP)*item)+pad_y, TimerEditNew_X+TIMEREDITNEW_PADDING+TimerEditNew_Option_W-TIMEREDITNEW_GAP, "Cancel", fc, COLOR_None);
						break;
					case FNT_Size_1926:
						Font_Osd_PutString1926( rgn, TimerEditNew_X+TIMEREDITNEW_PADDING+TIMEREDITNEW_GAP+(TimerEditNew_Option_W/2)+(TIMEREDITNEW_GAP/2), TimerEditNew_Y+TIMEREDITNEW_GAP+((TimerEditNew_Option_H+TIMEREDITNEW_GAP)*item)+pad_y, TimerEditNew_X+TIMEREDITNEW_PADDING+TimerEditNew_Option_W-TIMEREDITNEW_GAP, "Cancel", fc, COLOR_None);
						break;
				}	
			}
		
			break;
	}
}

char * TimerEditNew_ReserveType( int type ){
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

void TimerEditNew_KeyboardClose( char * text, bool changed ){
	// Do nothing atm until it is implement
	if( changed ){
		// We have change the text of the timer so modify the fileName field
		memset( TimerEditNew_Timer.fileName, '\0', sizeof(TimerEditNew_Timer.fileName) ); // Blank the filename field
		strncpy(TimerEditNew_Timer.fileName, text, sizeof(TimerEditNew_Timer.fileName));
	}
}

// Function to draw the timer details to the region
void TimerEditNew_DrawDetails( word rgn ){
	int i;
	for( i = 0; i < TIMEREDITNEW_OPTIONS; i++ ){
		TimerEditNew_DrawItem( rgn, i, TimerEditNew_Selected );
	}
}

int TimerEditNew_Modify( void ){
	strcat(TimerEditNew_Timer.fileName,".rec");
	return TAP_Timer_Add(&TimerEditNew_Timer);
}

dword TimerEditNew_KeyHandler( dword param1 ){
	if( Keyboard_IsActive() ){	// Handle the keyboard
		return Keyboard_Keyhandler(param1);
	}
	if( param1 == RKEY_Exit ){
		TimerEditNew_Deactivate(0,FALSE,TimerEditNew_Timer);
		return 0;
	}
	// Do the navigation stuff now
	if( param1 == RKEY_ChUp ){
		TimerEditNew_PrevSelected = TimerEditNew_Selected;
		TimerEditNew_Selected--;
		if( TimerEditNew_Selected < 1 ){
			TimerEditNew_Selected = TIMEREDITNEW_OPTIONS-1;
		}
		TimerEditNew_DrawItem( TimerEditNew_MemRGN, TimerEditNew_PrevSelected, TimerEditNew_Selected );
		TimerEditNew_DrawItem( TimerEditNew_MemRGN, TimerEditNew_Selected, TimerEditNew_Selected );
		TAP_Osd_Copy( TimerEditNew_MemRGN, TimerEditNew_RGN, TimerEditNew_X+TIMEREDITNEW_PADDING, TimerEditNew_Y+TIMEREDITNEW_GAP+((TimerEditNew_Option_H+TIMEREDITNEW_GAP)*TimerEditNew_PrevSelected), TimerEditNew_Option_W, TimerEditNew_Option_H, TimerEditNew_X+TIMEREDITNEW_PADDING, TimerEditNew_Y+TIMEREDITNEW_GAP+((TimerEditNew_Option_H+TIMEREDITNEW_GAP)*TimerEditNew_PrevSelected), FALSE);
		TAP_Osd_Copy( TimerEditNew_MemRGN, TimerEditNew_RGN, TimerEditNew_X+TIMEREDITNEW_PADDING, TimerEditNew_Y+TIMEREDITNEW_GAP+((TimerEditNew_Option_H+TIMEREDITNEW_GAP)*TimerEditNew_Selected), TimerEditNew_Option_W, TimerEditNew_Option_H, TimerEditNew_X+TIMEREDITNEW_PADDING, TimerEditNew_Y+TIMEREDITNEW_GAP+((TimerEditNew_Option_H+TIMEREDITNEW_GAP)*TimerEditNew_Selected), FALSE);
	}
	// Do the navigation stuff now
	if( param1 == RKEY_ChDown ){
		TimerEditNew_PrevSelected = TimerEditNew_Selected;
		TimerEditNew_Selected++;
		if( TimerEditNew_Selected > TIMEREDITNEW_OPTIONS-1 ){
			TimerEditNew_Selected = 1;
		}
		TimerEditNew_DrawItem( TimerEditNew_MemRGN, TimerEditNew_PrevSelected, TimerEditNew_Selected );
		TimerEditNew_DrawItem( TimerEditNew_MemRGN, TimerEditNew_Selected, TimerEditNew_Selected );
		TAP_Osd_Copy( TimerEditNew_MemRGN, TimerEditNew_RGN, TimerEditNew_X+TIMEREDITNEW_PADDING, TimerEditNew_Y+TIMEREDITNEW_GAP+((TimerEditNew_Option_H+TIMEREDITNEW_GAP)*TimerEditNew_PrevSelected), TimerEditNew_Option_W, TimerEditNew_Option_H, TimerEditNew_X+TIMEREDITNEW_PADDING, TimerEditNew_Y+TIMEREDITNEW_GAP+((TimerEditNew_Option_H+TIMEREDITNEW_GAP)*TimerEditNew_PrevSelected), FALSE);
		TAP_Osd_Copy( TimerEditNew_MemRGN, TimerEditNew_RGN, TimerEditNew_X+TIMEREDITNEW_PADDING, TimerEditNew_Y+TIMEREDITNEW_GAP+((TimerEditNew_Option_H+TIMEREDITNEW_GAP)*TimerEditNew_Selected), TimerEditNew_Option_W, TimerEditNew_Option_H, TimerEditNew_X+TIMEREDITNEW_PADDING, TimerEditNew_Y+TIMEREDITNEW_GAP+((TimerEditNew_Option_H+TIMEREDITNEW_GAP)*TimerEditNew_Selected), FALSE);
	}
	// Do the navigation stuff now
	if( param1 == RKEY_VolDown || param1 == RKEY_VolUp || param1 == RKEY_Forward || param1 == RKEY_Rewind ){
		switch( TimerEditNew_Selected ){
			case TIMEREDITNEW_TIMERSTART:
				TimerEditNew_AlterStartTime(param1);
				TimerEditNew_DrawItem( TimerEditNew_MemRGN, TIMEREDITNEW_TIMEREND, TimerEditNew_Selected );
				TimerEditNew_DrawItem( TimerEditNew_MemRGN, TIMEREDITNEW_TIMERSTART, TimerEditNew_Selected );
				TAP_Osd_Copy( TimerEditNew_MemRGN, TimerEditNew_RGN, TimerEditNew_X+TIMEREDITNEW_PADDING, TimerEditNew_Y+TIMEREDITNEW_GAP+((TimerEditNew_Option_H+TIMEREDITNEW_GAP)*TIMEREDITNEW_TIMEREND), TimerEditNew_Option_W, TimerEditNew_Option_H, TimerEditNew_X+TIMEREDITNEW_PADDING, TimerEditNew_Y+TIMEREDITNEW_GAP+((TimerEditNew_Option_H+TIMEREDITNEW_GAP)*TIMEREDITNEW_TIMEREND), FALSE);
				TAP_Osd_Copy( TimerEditNew_MemRGN, TimerEditNew_RGN, TimerEditNew_X+TIMEREDITNEW_PADDING, TimerEditNew_Y+TIMEREDITNEW_GAP+((TimerEditNew_Option_H+TIMEREDITNEW_GAP)*TIMEREDITNEW_TIMERSTART), TimerEditNew_Option_W, TimerEditNew_Option_H, TimerEditNew_X+TIMEREDITNEW_PADDING, TimerEditNew_Y+TIMEREDITNEW_GAP+((TimerEditNew_Option_H+TIMEREDITNEW_GAP)*TIMEREDITNEW_TIMERSTART), FALSE);
				return 0;
				break;
			case TIMEREDITNEW_TIMEREND:
				TimerEditNew_AlterDuration(param1);
				TimerEditNew_DrawItem( TimerEditNew_MemRGN, TIMEREDITNEW_TIMEREND, TimerEditNew_Selected );
				TAP_Osd_Copy( TimerEditNew_MemRGN, TimerEditNew_RGN, TimerEditNew_X+TIMEREDITNEW_PADDING, TimerEditNew_Y+TIMEREDITNEW_GAP+((TimerEditNew_Option_H+TIMEREDITNEW_GAP)*TIMEREDITNEW_TIMEREND), TimerEditNew_Option_W, TimerEditNew_Option_H, TimerEditNew_X+TIMEREDITNEW_PADDING, TimerEditNew_Y+TIMEREDITNEW_GAP+((TimerEditNew_Option_H+TIMEREDITNEW_GAP)*TIMEREDITNEW_TIMEREND), FALSE);
				return 0;
				break;
			case TIMEREDITNEW_TIMERSELECTION:
				TimerEditNew_OptionSelected++;
				if( TimerEditNew_OptionSelected > 1 ){
					TimerEditNew_OptionSelected  = 0;
				}
				break;
			case TIMEREDITNEW_RESERVETYPE:
				switch ( param1 ){
					case RKEY_VolUp:
						switch ( TimerEditNew_Timer.reservationType ){
							case RESERVE_TYPE_Onetime: 		TimerEditNew_Timer.reservationType = RESERVE_TYPE_Everyday; break;
							case RESERVE_TYPE_Everyday: 	TimerEditNew_Timer.reservationType = RESERVE_TYPE_EveryWeekend; break;
							case RESERVE_TYPE_EveryWeekend: TimerEditNew_Timer.reservationType = RESERVE_TYPE_Weekly; break;
							case RESERVE_TYPE_Weekly: 		TimerEditNew_Timer.reservationType = RESERVE_TYPE_WeekDay; break;
							case RESERVE_TYPE_WeekDay: 		TimerEditNew_Timer.reservationType = RESERVE_TYPE_Onetime; break;
							default : break;
						}
						break;
					case RKEY_VolDown:
						switch ( TimerEditNew_Timer.reservationType ){
							case RESERVE_TYPE_Onetime: 		TimerEditNew_Timer.reservationType = RESERVE_TYPE_WeekDay; break;
							case RESERVE_TYPE_Everyday: 	TimerEditNew_Timer.reservationType = RESERVE_TYPE_Onetime; break;
							case RESERVE_TYPE_EveryWeekend: TimerEditNew_Timer.reservationType = RESERVE_TYPE_Everyday; break;
							case RESERVE_TYPE_Weekly: 		TimerEditNew_Timer.reservationType = RESERVE_TYPE_EveryWeekend; break;
							case RESERVE_TYPE_WeekDay: 		TimerEditNew_Timer.reservationType = RESERVE_TYPE_Weekly; break;
							default : break;
					}
					break;
					default :
						break;
				}
				break;
			case TIMEREDITNEW_DATE:
				TimerEditNew_AdjustDate( param1 );
				break;
		}
		TimerEditNew_DrawItem( TimerEditNew_RGN, TimerEditNew_Selected, TimerEditNew_Selected );
		TAP_Osd_Copy( TimerEditNew_MemRGN, TimerEditNew_RGN, TimerEditNew_X+TIMEREDITNEW_PADDING, TimerEditNew_Y+TIMEREDITNEW_GAP+((TimerEditNew_Option_H+TIMEREDITNEW_GAP)*TimerEditNew_Selected), TimerEditNew_Option_W, TimerEditNew_Option_H, TimerEditNew_X+TIMEREDITNEW_PADDING, TimerEditNew_Y+TIMEREDITNEW_GAP+((TimerEditNew_Option_H+TIMEREDITNEW_GAP)*TimerEditNew_Selected), FALSE);
	}
	if( param1 == RKEY_Ok ){
		switch( TimerEditNew_Selected ){
			case TIMEREDITNEW_TIMERNAME:
				Keyboard_Init( TimerEditNew_RGN, TimerEditNew_MemRGN,  TimerEditNew_X, TimerEditNew_Y, TimerEditNew_W, TimerEditNew_H, "Timer Name" );	// Initalise the keyboard
				Keyboard_SetColours( DISPLAY_KEYBOARDITEM, DISPLAY_KEYBOARDITEMTEXT, DISPLAY_KEYBOARDITEMSELECTED, DISPLAY_KEYBOARDITEMSELECTEDTEXT, DISPLAY_KEYBOARDFIELDTEXT, DISPLAY_KEYBOARDFIELD, DISPLAY_KEYBOARDBACKGROUND );
				if( Keyboard_Activate(TimerEditNew_Timer.fileName,&TimerEditNew_KeyboardClose) ){
				}
				break;
			case TIMEREDITNEW_TIMERSELECTION:
				if( TimerEditNew_OptionSelected == TIMEREDITNEW_OK ){
					TimerEditNew_Deactivate(TimerEditNew_Modify(),TRUE, TimerEditNew_Timer);
				} else {
					TimerEditNew_Deactivate(0,FALSE,TimerEditNew_Timer);
				}
				break;
		}
	}
	return 0;
}

void TimerEditNew_AdjustDate( dword param1 ){
	word 	mjd;
	int 	startTime; // , duration
	startTime = TimerEditNew_Timer.startTime&0xffff;		// extract the time
	mjd = TimerEditNew_Timer.startTime>>16;
	switch ( param1 ){
		case RKEY_VolUp:	mjd++;	break;			// Date up
		case RKEY_VolDown:	mjd--;	break;			// Date down
		default :			break;
	}
	TimerEditNew_Timer.startTime = (mjd<<16) | startTime;
}

void TimerEditNew_AlterDuration( dword param1 ){
	switch( param1 ){
		case RKEY_VolUp:							// Time up
			TimerEditNew_Timer.duration++;
			break;
		case RKEY_VolDown:							// Time down
			if ( TimerEditNew_Timer.duration > 0 ) TimerEditNew_Timer.duration--;
			break;
		case RKEY_Forward:							// Time up
			TimerEditNew_Timer.duration+=60;
			break;
		case RKEY_Rewind:							// Time down
			if ( TimerEditNew_Timer.duration > 60 ) TimerEditNew_Timer.duration-=60;
			break;
		default :
			break;
	}
}

void TimerEditNew_AlterStartTime( dword param1 ){
	word 	mjd;
	int 	min, hour;
	hour = (TimerEditNew_Timer.startTime&0xff00)>>8;			// extract the time
	min = (TimerEditNew_Timer.startTime&0xff);
	mjd = TimerEditNew_Timer.startTime>>16;
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
	TimerEditNew_Timer.startTime = (mjd<<16) | ((hour&0xff)<<8) | (min&0xff);
}

// Function to set the colours of the Timer window
void TimerEditNew_SetColours( dword fc, dword bc, dword fhc, dword bhc, dword base ){
	TimerEditNew_Fc = fc;
	TimerEditNew_HghBc = bhc;
	TimerEditNew_HghFc = fhc;
	TimerEditNew_Bc = bc;
	TimerEditNew_BaseColour = base;
}

