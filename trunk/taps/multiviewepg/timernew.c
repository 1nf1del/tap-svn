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

#include "timernew.h"
#include "fontman.h"
#include "skin.h"
#include "settings.h"
#include "keyboard.h"

bool TimerNew_IsActive( void ){
	return TimerNew_Active;
}

// Function to initalize the keyboard - region and location to render
void TimerNew_Init( word rgn, word memrgn, int x, int y, int w, int h ){
	TimerNew_RGN = rgn;	// Store the region to render on
	TimerNew_MemRGN = memrgn;
	TimerNew_X = x;
	TimerNew_Y = y;
	TimerNew_W = w;
	TimerNew_H = h;
	TimerNew_Initalised = TRUE;
	TimerNew_Option_H = (TimerNew_H/TIMERNEW_OPTIONS)-TIMERNEW_GAP;
	TimerNew_Option_W = TimerNew_W - (TIMERNEW_PADDING*2);
	TimerNew_CreateGradients();
}

void TimerNew_MemFree( void ){
	if (TimerNew_ItemBase) TAP_MemFree(TimerNew_ItemBase);
	if (TimerNew_ItemHigh) TAP_MemFree(TimerNew_ItemHigh);
	if (TimerNew_OkItemHigh) TAP_MemFree(TimerNew_OkItemHigh);
	if (TimerNew_CancelItemHigh) TAP_MemFree(TimerNew_CancelItemHigh);
	TimerNew_ItemBase = NULL;
	TimerNew_ItemHigh = NULL;
	TimerNew_OkItemHigh = NULL;
	TimerNew_CancelItemHigh = NULL;
}

void TimerNew_CreateGradients( void ){
	word rgn;
	TimerNew_MemFree();
	if( Settings_GradientFactor > 0 ){
		rgn	= TAP_Osd_Create(0, 0, TimerNew_Option_W, TimerNew_Option_H, 0, OSD_Flag_MemRgn);
		// Main default item
		createGradient(rgn, TimerNew_Option_W, TimerNew_Option_H, Settings_GradientFactor, DISPLAY_ITEM);
		TimerNew_ItemBase = TAP_Osd_SaveBox(rgn, 0, 0, TimerNew_Option_W, TimerNew_Option_H);
		createGradient(rgn, TimerNew_Option_W, TimerNew_Option_H, Settings_GradientFactor, DISPLAY_ITEMSELECTED);
		TimerNew_ItemHigh = TAP_Osd_SaveBox(rgn, 0, 0, TimerNew_Option_W, TimerNew_Option_H);
		TimerNew_CancelItemHigh = TAP_Osd_SaveBox(rgn, 0, 0, (TimerNew_Option_W/2)-(TIMERNEW_GAP/2), TimerNew_Option_H);
		TimerNew_OkItemHigh = TAP_Osd_SaveBox(rgn, 0, 0, (TimerNew_Option_W/2)-(TIMERNEW_GAP/2)-1, TimerNew_Option_H);
		TAP_Osd_Delete(rgn);
	}
}
// Activate by sending the timer and the number. Used when not using events ala conflict calling
bool TimerNew_Activate( void (*ReturningProcedure)( int, bool, TYPE_TimerInfo ) ){
	if( !TimerNew_Initalised ) return FALSE;	// We haven't initalised so gracefully fail
	TimerNew_Callback = ReturningProcedure;
	// Get the details of the event and create the timer structure
	
	TimerNew_Timer.isRec = TRUE;
	TimerNew_Timer.nameFix = 0;
	TimerNew_Timer.svcType = SVC_TYPE_Tv;
	TimerNew_Timer.svcNum = 0;
	TimerNew_Timer.startTime = Now(0);
	TimerNew_Timer.duration = 120;
	TimerNew_Timer.reservationType = RESERVE_TYPE_Onetime;
	memset(TimerNew_Timer.fileName, 0, sizeof( TimerNew_Timer.fileName) );
	
	
	TimerNew_Selected = 1;
	TimerNew_PrevSelected = 1;
	TimerNew_OptionSelected = 0;
	// Now lets render the display
	TimerNew_DrawBackground(TimerNew_MemRGN);
	TimerNew_DrawDetails(TimerNew_MemRGN);
	// Coppy rendered to foreground
	TAP_Osd_Copy( TimerNew_MemRGN, TimerNew_RGN, TimerNew_X, TimerNew_Y, TimerNew_W, TimerNew_H, TimerNew_X, TimerNew_Y, FALSE);
	TimerNew_Active = TRUE;
	return TRUE;
}

// Routine to handle the closing of the timer window
void TimerNew_Deactivate( int retval, bool cancel ){
	TimerNew_Active = FALSE;
	// Restore what we had drawn over
	TAP_Osd_RestoreBox(TimerNew_RGN, TimerNew_X, TimerNew_Y, TimerNew_W, TimerNew_H, TimerNew_Base);
	TAP_MemFree(TimerNew_Base);
	TimerNew_Base = NULL;
	TimerNew_MemFree();
	TimerNew_Callback(retval, cancel, TimerNew_Timer);
}

// Here we will just draw the basic layout of the keyboard
void TimerNew_DrawBackground( word rgn ){
	if( TimerNew_Base != NULL ) TAP_MemFree(TimerNew_Base);
	TimerNew_Base = TAP_Osd_SaveBox(TimerNew_RGN, TimerNew_X, TimerNew_Y, TimerNew_W, TimerNew_H);	// Save what is under the Timer screen
	TAP_Osd_FillBox(rgn, TimerNew_X, TimerNew_Y, TimerNew_W, TimerNew_H, TimerNew_BaseColour);
}

void TimerNew_DrawItem( word rgn, int item, int selected ){
	int fntSize = 0;
	int pad_y;
	dword fc = TimerNew_Fc;
	dword bc = TimerNew_Bc;
	// Work out the font size allowed for the height of the option
	if( ( TimerNew_Option_H - 8 ) > 19  ){
		if( ( TimerNew_Option_H - 12 ) > 22 ){
			// Use the big font
			fntSize = FNT_Size_1926;
			pad_y = (TimerNew_Option_H/2)-(26/2);
		} else {
			fntSize = FNT_Size_1622;
			pad_y = (TimerNew_Option_H/2)-(22/2);
		}
	} else {
		pad_y = (TimerNew_Option_H/2)-(19/2);
	}
	// If selected change the colors of the elements
	if( item == selected ){
		fc = TimerNew_HghFc;
		bc = TimerNew_HghBc;
	}
	switch( item ){
		case TIMERNEW_SECTIONTITLE:
			TAP_Osd_FillBox(rgn, TimerNew_X+TIMERNEW_PADDING, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*item), TimerNew_Option_W, TimerNew_Option_H, TimerNew_BaseColour);
			switch( fntSize ){
				case FNT_Size_1419:
					Font_Osd_PutString1419( rgn, TimerNew_X+TIMERNEW_PADDING+TIMERNEW_GAP, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*item)+pad_y, TimerNew_X+TIMERNEW_PADDING+TimerNew_Option_W-TIMERNEW_GAP, "New Timer Details", fc, COLOR_None);
					break;
				case FNT_Size_1622:
					Font_Osd_PutString1622( rgn, TimerNew_X+TIMERNEW_PADDING+TIMERNEW_GAP, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*item)+pad_y, TimerNew_X+TIMERNEW_PADDING+TimerNew_Option_W-TIMERNEW_GAP, "New Timer Details", fc, COLOR_None);
					break;
				case FNT_Size_1926:
					Font_Osd_PutString1926( rgn, TimerNew_X+TIMERNEW_PADDING+TIMERNEW_GAP, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*item)+pad_y, TimerNew_X+TIMERNEW_PADDING+TimerNew_Option_W-TIMERNEW_GAP, "New Timer Details", fc, COLOR_None);
					break;
			}
			break;
		case TIMERNEW_TIMERNAME:
			if( item == selected ){
				if( TimerNew_ItemHigh != NULL ){
					TAP_Osd_RestoreBox( rgn, TimerNew_X+TIMERNEW_PADDING, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*item), TimerNew_Option_W, TimerNew_Option_H, TimerNew_ItemHigh );
				} else {
					TAP_Osd_FillBox(rgn, TimerNew_X+TIMERNEW_PADDING, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*item), TimerNew_Option_W, TimerNew_Option_H, bc);
				}
			} else {
				if( TimerNew_ItemBase != NULL ){
					TAP_Osd_RestoreBox( rgn, TimerNew_X+TIMERNEW_PADDING, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*item), TimerNew_Option_W, TimerNew_Option_H, TimerNew_ItemBase );
				} else {
					TAP_Osd_FillBox(rgn, TimerNew_X+TIMERNEW_PADDING, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*item), TimerNew_Option_W, TimerNew_Option_H, bc);
				}
			}
			TAP_Osd_FillBox(rgn, TimerNew_X+TIMERNEW_PADDING+TIMERNEW_GAP+120, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*item), TIMERNEW_GAP, TimerNew_Option_H, TimerNew_BaseColour);
			switch( fntSize ){
				case FNT_Size_1419:
					Font_Osd_PutString1419( rgn, TimerNew_X+TIMERNEW_PADDING+TIMERNEW_GAP, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*item)+pad_y, TimerNew_X+TIMERNEW_PADDING+TIMERNEW_GAP+120, "File Name", fc, COLOR_None);
					Font_Osd_PutString1419( rgn, TimerNew_X+TIMERNEW_PADDING+TIMERNEW_GAP+130, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*item)+pad_y, TimerNew_X+TIMERNEW_PADDING+TimerNew_Option_W-TIMERNEW_GAP, TimerNew_Timer.fileName, fc, COLOR_None);
					break;
				case FNT_Size_1622:
					Font_Osd_PutString1622( rgn, TimerNew_X+TIMERNEW_PADDING+TIMERNEW_GAP, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*item)+pad_y, TimerNew_X+TIMERNEW_PADDING+TIMERNEW_GAP+120, "File Name", fc, COLOR_None);
					Font_Osd_PutString1622( rgn, TimerNew_X+TIMERNEW_PADDING+TIMERNEW_GAP+130, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*item)+pad_y, TimerNew_X+TIMERNEW_PADDING+TimerNew_Option_W-TIMERNEW_GAP, TimerNew_Timer.fileName, fc, COLOR_None);
					break;
				case FNT_Size_1926:
					Font_Osd_PutString1926( rgn, TimerNew_X+TIMERNEW_PADDING+TIMERNEW_GAP, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*item)+pad_y, TimerNew_X+TIMERNEW_PADDING+TIMERNEW_GAP+120, "File Name", fc, COLOR_None);
					Font_Osd_PutString1926( rgn, TimerNew_X+TIMERNEW_PADDING+TIMERNEW_GAP+130, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*item)+pad_y, TimerNew_X+TIMERNEW_PADDING+TimerNew_Option_W-TIMERNEW_GAP, TimerNew_Timer.fileName, fc, COLOR_None);
					break;
			}
			break;
		case TIMERNEW_TIMERSTART:
			if( item == selected ){
				if( TimerNew_ItemHigh != NULL ){
					TAP_Osd_RestoreBox( rgn, TimerNew_X+TIMERNEW_PADDING, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*item), TimerNew_Option_W, TimerNew_Option_H, TimerNew_ItemHigh );
				} else {
					TAP_Osd_FillBox(rgn, TimerNew_X+TIMERNEW_PADDING, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*item), TimerNew_Option_W, TimerNew_Option_H, bc);
				}
			} else {
				if( TimerNew_ItemBase != NULL ){
					TAP_Osd_RestoreBox( rgn, TimerNew_X+TIMERNEW_PADDING, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*item), TimerNew_Option_W, TimerNew_Option_H, TimerNew_ItemBase );
				} else {
					TAP_Osd_FillBox(rgn, TimerNew_X+TIMERNEW_PADDING, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*item), TimerNew_Option_W, TimerNew_Option_H, bc);
				}
			}
			TAP_Osd_FillBox(rgn, TimerNew_X+TIMERNEW_PADDING+TIMERNEW_GAP+120, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*item), TIMERNEW_GAP, TimerNew_Option_H, TimerNew_BaseColour);
			switch( fntSize ){
				case FNT_Size_1419:
					Font_Osd_PutString1419( rgn, TimerNew_X+TIMERNEW_PADDING+TIMERNEW_GAP, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*item)+pad_y, TimerNew_X+TIMERNEW_PADDING+TIMERNEW_GAP+120, "Start Time", fc, COLOR_None);
					Font_Osd_PutString1419( rgn, TimerNew_X+TIMERNEW_PADDING+TIMERNEW_GAP+130, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*item)+pad_y, TimerNew_X+TIMERNEW_PADDING+TimerNew_Option_W-TIMERNEW_GAP, Time_HHMM(TimerNew_Timer.startTime), fc, COLOR_None);
					break;
				case FNT_Size_1622:
					Font_Osd_PutString1622( rgn, TimerNew_X+TIMERNEW_PADDING+TIMERNEW_GAP, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*item)+pad_y, TimerNew_X+TIMERNEW_PADDING+TIMERNEW_GAP+120, "Start Time", fc, COLOR_None);
					Font_Osd_PutString1622( rgn, TimerNew_X+TIMERNEW_PADDING+TIMERNEW_GAP+130, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*item)+pad_y, TimerNew_X+TIMERNEW_PADDING+TimerNew_Option_W-TIMERNEW_GAP, Time_HHMM(TimerNew_Timer.startTime), fc, COLOR_None);
					break;
				case FNT_Size_1926:
					Font_Osd_PutString1926( rgn, TimerNew_X+TIMERNEW_PADDING+TIMERNEW_GAP, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*item)+pad_y, TimerNew_X+TIMERNEW_PADDING+TIMERNEW_GAP+120, "Start Time", fc, COLOR_None);
					Font_Osd_PutString1926( rgn, TimerNew_X+TIMERNEW_PADDING+TIMERNEW_GAP+130, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*item)+pad_y, TimerNew_X+TIMERNEW_PADDING+TimerNew_Option_W-TIMERNEW_GAP, Time_HHMM(TimerNew_Timer.startTime), fc, COLOR_None);
					break;
			}
			break;
		case TIMERNEW_TIMEREND:
			if( item == selected ){
				if( TimerNew_ItemHigh != NULL ){
					TAP_Osd_RestoreBox( rgn, TimerNew_X+TIMERNEW_PADDING, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*item), TimerNew_Option_W, TimerNew_Option_H, TimerNew_ItemHigh );
				} else {
					TAP_Osd_FillBox(rgn, TimerNew_X+TIMERNEW_PADDING, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*item), TimerNew_Option_W, TimerNew_Option_H, bc);
				}
			} else {
				if( TimerNew_ItemBase != NULL ){
					TAP_Osd_RestoreBox( rgn, TimerNew_X+TIMERNEW_PADDING, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*item), TimerNew_Option_W, TimerNew_Option_H, TimerNew_ItemBase );
				} else {
					TAP_Osd_FillBox(rgn, TimerNew_X+TIMERNEW_PADDING, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*item), TimerNew_Option_W, TimerNew_Option_H, bc);
				}
			}
			TAP_Osd_FillBox(rgn, TimerNew_X+TIMERNEW_PADDING+TIMERNEW_GAP+120, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*item), TIMERNEW_GAP, TimerNew_Option_H, TimerNew_BaseColour);
			switch( fntSize ){
				case FNT_Size_1419:
					Font_Osd_PutString1419( rgn, TimerNew_X+TIMERNEW_PADDING+TIMERNEW_GAP, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*item)+pad_y, TimerNew_X+TIMERNEW_PADDING+TIMERNEW_GAP+120, "End Time", fc, COLOR_None);
					Font_Osd_PutString1419( rgn, TimerNew_X+TIMERNEW_PADDING+TIMERNEW_GAP+130, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*item)+pad_y, TimerNew_X+TIMERNEW_PADDING+TimerNew_Option_W-TIMERNEW_GAP, Endtime_HHMM(TimerNew_Timer.startTime,TimerNew_Timer.duration), fc, COLOR_None);
					break;
				case FNT_Size_1622:
					Font_Osd_PutString1622( rgn, TimerNew_X+TIMERNEW_PADDING+TIMERNEW_GAP, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*item)+pad_y, TimerNew_X+TIMERNEW_PADDING+TIMERNEW_GAP+120, "End Time", fc, COLOR_None);
					Font_Osd_PutString1622( rgn, TimerNew_X+TIMERNEW_PADDING+TIMERNEW_GAP+130, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*item)+pad_y, TimerNew_X+TIMERNEW_PADDING+TimerNew_Option_W-TIMERNEW_GAP, Endtime_HHMM(TimerNew_Timer.startTime,TimerNew_Timer.duration), fc, COLOR_None);
					break;
				case FNT_Size_1926:
					Font_Osd_PutString1926( rgn, TimerNew_X+TIMERNEW_PADDING+TIMERNEW_GAP, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*item)+pad_y, TimerNew_X+TIMERNEW_PADDING+TIMERNEW_GAP+120, "End Time", fc, COLOR_None);
					Font_Osd_PutString1926( rgn, TimerNew_X+TIMERNEW_PADDING+TIMERNEW_GAP+130, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*item)+pad_y, TimerNew_X+TIMERNEW_PADDING+TimerNew_Option_W-TIMERNEW_GAP, Endtime_HHMM(TimerNew_Timer.startTime,TimerNew_Timer.duration), fc, COLOR_None);
					break;
			}
			break;
		case TIMERNEW_RESERVETYPE:
			if( item == selected ){
				if( TimerNew_ItemHigh != NULL ){
					TAP_Osd_RestoreBox( rgn, TimerNew_X+TIMERNEW_PADDING, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*item), TimerNew_Option_W, TimerNew_Option_H, TimerNew_ItemHigh );
				} else {
					TAP_Osd_FillBox(rgn, TimerNew_X+TIMERNEW_PADDING, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*item), TimerNew_Option_W, TimerNew_Option_H, bc);
				}
			} else {
				if( TimerNew_ItemBase != NULL ){
					TAP_Osd_RestoreBox( rgn, TimerNew_X+TIMERNEW_PADDING, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*item), TimerNew_Option_W, TimerNew_Option_H, TimerNew_ItemBase );
				} else {
					TAP_Osd_FillBox(rgn, TimerNew_X+TIMERNEW_PADDING, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*item), TimerNew_Option_W, TimerNew_Option_H, bc);
				}
			}
			TAP_Osd_FillBox(rgn, TimerNew_X+TIMERNEW_PADDING+TIMERNEW_GAP+120, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*item), TIMERNEW_GAP, TimerNew_Option_H, TimerNew_BaseColour);
			switch( fntSize ){
				case FNT_Size_1419:
					Font_Osd_PutString1419( rgn, TimerNew_X+TIMERNEW_PADDING+TIMERNEW_GAP, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*item)+pad_y, TimerNew_X+TIMERNEW_PADDING+TIMERNEW_GAP+120, "Repeat Type", fc, COLOR_None);
					Font_Osd_PutString1419( rgn, TimerNew_X+TIMERNEW_PADDING+TIMERNEW_GAP+130, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*item)+pad_y, TimerNew_X+TIMERNEW_PADDING+TimerNew_Option_W-TIMERNEW_GAP, TimerNew_ReserveType(TimerNew_Timer.reservationType), fc, COLOR_None);
					break;
				case FNT_Size_1622:
					Font_Osd_PutString1622( rgn, TimerNew_X+TIMERNEW_PADDING+TIMERNEW_GAP, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*item)+pad_y, TimerNew_X+TIMERNEW_PADDING+TIMERNEW_GAP+120, "Repeat Type", fc, COLOR_None);
					Font_Osd_PutString1622( rgn, TimerNew_X+TIMERNEW_PADDING+TIMERNEW_GAP+130, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*item)+pad_y, TimerNew_X+TIMERNEW_PADDING+TimerNew_Option_W-TIMERNEW_GAP, TimerNew_ReserveType(TimerNew_Timer.reservationType), fc, COLOR_None);
					break;
				case FNT_Size_1926:
					Font_Osd_PutString1926( rgn, TimerNew_X+TIMERNEW_PADDING+TIMERNEW_GAP, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*item)+pad_y, TimerNew_X+TIMERNEW_PADDING+TIMERNEW_GAP+120, "Repeat Type", fc, COLOR_None);
					Font_Osd_PutString1926( rgn, TimerNew_X+TIMERNEW_PADDING+TIMERNEW_GAP+130, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*item)+pad_y, TimerNew_X+TIMERNEW_PADDING+TimerNew_Option_W-TIMERNEW_GAP, TimerNew_ReserveType(TimerNew_Timer.reservationType), fc, COLOR_None);
					break;
			}
			break;		
		case TIMERNEW_DATE:
			if( item == selected ){
				if( TimerNew_ItemHigh != NULL ){
					TAP_Osd_RestoreBox( rgn, TimerNew_X+TIMERNEW_PADDING, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*item), TimerNew_Option_W, TimerNew_Option_H, TimerNew_ItemHigh );
				} else {
					TAP_Osd_FillBox(rgn, TimerNew_X+TIMERNEW_PADDING, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*item), TimerNew_Option_W, TimerNew_Option_H, bc);
				}
			} else {
				if( TimerNew_ItemBase != NULL ){
					TAP_Osd_RestoreBox( rgn, TimerNew_X+TIMERNEW_PADDING, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*item), TimerNew_Option_W, TimerNew_Option_H, TimerNew_ItemBase );
				} else {
					TAP_Osd_FillBox(rgn, TimerNew_X+TIMERNEW_PADDING, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*item), TimerNew_Option_W, TimerNew_Option_H, bc);
				}
			}
			TAP_Osd_FillBox(rgn, TimerNew_X+TIMERNEW_PADDING+TIMERNEW_GAP+120, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*item), TIMERNEW_GAP, TimerNew_Option_H, TimerNew_BaseColour);
			switch( fntSize ){
				case FNT_Size_1419:
					Font_Osd_PutString1419( rgn, TimerNew_X+TIMERNEW_PADDING+TIMERNEW_GAP, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*item)+pad_y, TimerNew_X+TIMERNEW_PADDING+TIMERNEW_GAP+120, "Date", fc, COLOR_None);
					Font_Osd_PutString1419( rgn, TimerNew_X+TIMERNEW_PADDING+TIMERNEW_GAP+130, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*item)+pad_y, TimerNew_X+TIMERNEW_PADDING+TimerNew_Option_W-TIMERNEW_GAP, Time_DOWDDMMSpec(TimerNew_Timer.startTime), fc, COLOR_None);
					break;
				case FNT_Size_1622:
					Font_Osd_PutString1622( rgn, TimerNew_X+TIMERNEW_PADDING+TIMERNEW_GAP, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*item)+pad_y, TimerNew_X+TIMERNEW_PADDING+TIMERNEW_GAP+120, "Date", fc, COLOR_None);
					Font_Osd_PutString1622( rgn, TimerNew_X+TIMERNEW_PADDING+TIMERNEW_GAP+130, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*item)+pad_y, TimerNew_X+TIMERNEW_PADDING+TimerNew_Option_W-TIMERNEW_GAP, Time_DOWDDMMSpec(TimerNew_Timer.startTime), fc, COLOR_None);
					break;
				case FNT_Size_1926:
					Font_Osd_PutString1926( rgn, TimerNew_X+TIMERNEW_PADDING+TIMERNEW_GAP, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*item)+pad_y, TimerNew_X+TIMERNEW_PADDING+TIMERNEW_GAP+120, "Date", fc, COLOR_None);
					Font_Osd_PutString1926( rgn, TimerNew_X+TIMERNEW_PADDING+TIMERNEW_GAP+130, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*item)+pad_y, TimerNew_X+TIMERNEW_PADDING+TimerNew_Option_W-TIMERNEW_GAP, Time_DOWDDMMSpec(TimerNew_Timer.startTime), fc, COLOR_None);
					break;
			}
			break;	
			
		case TIMERNEW_RECORD:
			if( item == selected ){
				if( TimerNew_ItemHigh != NULL ){
					TAP_Osd_RestoreBox( rgn, TimerNew_X+TIMERNEW_PADDING, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*item), TimerNew_Option_W, TimerNew_Option_H, TimerNew_ItemHigh );
				} else {
					TAP_Osd_FillBox(rgn, TimerNew_X+TIMERNEW_PADDING, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*item), TimerNew_Option_W, TimerNew_Option_H, bc);
				}
			} else {
				if( TimerNew_ItemBase != NULL ){
					TAP_Osd_RestoreBox( rgn, TimerNew_X+TIMERNEW_PADDING, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*item), TimerNew_Option_W, TimerNew_Option_H, TimerNew_ItemBase );
				} else {
					TAP_Osd_FillBox(rgn, TimerNew_X+TIMERNEW_PADDING, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*item), TimerNew_Option_W, TimerNew_Option_H, bc);
				}
			}
			TAP_Osd_FillBox(rgn, TimerNew_X+TIMERNEW_PADDING+TIMERNEW_GAP+120, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*item), TIMERNEW_GAP, TimerNew_Option_H, TimerNew_BaseColour);
			switch( fntSize ){
				case FNT_Size_1419:
					Font_Osd_PutString1419( rgn, TimerNew_X+TIMERNEW_PADDING+TIMERNEW_GAP, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*item)+pad_y, TimerNew_X+TIMERNEW_PADDING+TIMERNEW_GAP+120, "Record Mode", fc, COLOR_None);
					Font_Osd_PutString1419( rgn, TimerNew_X+TIMERNEW_PADDING+TIMERNEW_GAP+130, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*item)+pad_y, TimerNew_X+TIMERNEW_PADDING+TimerNew_Option_W-TIMERNEW_GAP, TimerNew_Timer.isRec == 0?"Watch":"Record", fc, COLOR_None);
					break;
				case FNT_Size_1622:
					Font_Osd_PutString1622( rgn, TimerNew_X+TIMERNEW_PADDING+TIMERNEW_GAP, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*item)+pad_y, TimerNew_X+TIMERNEW_PADDING+TIMERNEW_GAP+120, "Record Mode", fc, COLOR_None);
					Font_Osd_PutString1622( rgn, TimerNew_X+TIMERNEW_PADDING+TIMERNEW_GAP+130, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*item)+pad_y, TimerNew_X+TIMERNEW_PADDING+TimerNew_Option_W-TIMERNEW_GAP, TimerNew_Timer.isRec == 0?"Watch":"Record", fc, COLOR_None);
					break;
				case FNT_Size_1926:
					Font_Osd_PutString1926( rgn, TimerNew_X+TIMERNEW_PADDING+TIMERNEW_GAP, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*item)+pad_y, TimerNew_X+TIMERNEW_PADDING+TIMERNEW_GAP+120, "Record Mode", fc, COLOR_None);
					Font_Osd_PutString1926( rgn, TimerNew_X+TIMERNEW_PADDING+TIMERNEW_GAP+130, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*item)+pad_y, TimerNew_X+TIMERNEW_PADDING+TimerNew_Option_W-TIMERNEW_GAP, TimerNew_Timer.isRec == 0?"Watch":"Record", fc, COLOR_None);
					break;
			}
			break;	
			
		case TIMERNEW_TYPE:
			if( item == selected ){
				if( TimerNew_ItemHigh != NULL ){
					TAP_Osd_RestoreBox( rgn, TimerNew_X+TIMERNEW_PADDING, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*item), TimerNew_Option_W, TimerNew_Option_H, TimerNew_ItemHigh );
				} else {
					TAP_Osd_FillBox(rgn, TimerNew_X+TIMERNEW_PADDING, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*item), TimerNew_Option_W, TimerNew_Option_H, bc);
				}
			} else {
				if( TimerNew_ItemBase != NULL ){
					TAP_Osd_RestoreBox( rgn, TimerNew_X+TIMERNEW_PADDING, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*item), TimerNew_Option_W, TimerNew_Option_H, TimerNew_ItemBase );
				} else {
					TAP_Osd_FillBox(rgn, TimerNew_X+TIMERNEW_PADDING, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*item), TimerNew_Option_W, TimerNew_Option_H, bc);
				}
			}
			// Draw black divider
			TAP_Osd_FillBox(rgn, TimerNew_X+TIMERNEW_PADDING+TIMERNEW_GAP+120, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*item), TIMERNEW_GAP, TimerNew_Option_H, TimerNew_BaseColour);
			switch( fntSize ){
				case FNT_Size_1419:
					Font_Osd_PutString1419( rgn, TimerNew_X+TIMERNEW_PADDING+TIMERNEW_GAP, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*item)+pad_y, TimerNew_X+TIMERNEW_PADDING+TIMERNEW_GAP+120, "Service", fc, COLOR_None);
					Font_Osd_PutString1419( rgn, TimerNew_X+TIMERNEW_PADDING+TIMERNEW_GAP+130, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*item)+pad_y, TimerNew_X+TIMERNEW_PADDING+TimerNew_Option_W-TIMERNEW_GAP, TimerNew_Timer.svcType == SVC_TYPE_Tv?"TV":"Radio", fc, COLOR_None);
					break;
				case FNT_Size_1622:
					Font_Osd_PutString1622( rgn, TimerNew_X+TIMERNEW_PADDING+TIMERNEW_GAP, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*item)+pad_y, TimerNew_X+TIMERNEW_PADDING+TIMERNEW_GAP+120, "Service", fc, COLOR_None);
					Font_Osd_PutString1622( rgn, TimerNew_X+TIMERNEW_PADDING+TIMERNEW_GAP+130, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*item)+pad_y, TimerNew_X+TIMERNEW_PADDING+TimerNew_Option_W-TIMERNEW_GAP, TimerNew_Timer.svcType == SVC_TYPE_Tv?"TV":"Radio", fc, COLOR_None);
					break;
				case FNT_Size_1926:
					Font_Osd_PutString1926( rgn, TimerNew_X+TIMERNEW_PADDING+TIMERNEW_GAP, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*item)+pad_y, TimerNew_X+TIMERNEW_PADDING+TIMERNEW_GAP+120, "Service", fc, COLOR_None);
					Font_Osd_PutString1926( rgn, TimerNew_X+TIMERNEW_PADDING+TIMERNEW_GAP+130, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*item)+pad_y, TimerNew_X+TIMERNEW_PADDING+TimerNew_Option_W-TIMERNEW_GAP, TimerNew_Timer.svcType == SVC_TYPE_Tv?"TV":"Radio", fc, COLOR_None);
					break;
			}
			break;
			
		case TIMERNEW_CHANNEL:
			if( item == selected ){
				if( TimerNew_ItemHigh != NULL ){
					TAP_Osd_RestoreBox( rgn, TimerNew_X+TIMERNEW_PADDING, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*item), TimerNew_Option_W, TimerNew_Option_H, TimerNew_ItemHigh );
				} else {
					TAP_Osd_FillBox(rgn, TimerNew_X+TIMERNEW_PADDING, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*item), TimerNew_Option_W, TimerNew_Option_H, bc);
				}
			} else {
				if( TimerNew_ItemBase != NULL ){
					TAP_Osd_RestoreBox( rgn, TimerNew_X+TIMERNEW_PADDING, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*item), TimerNew_Option_W, TimerNew_Option_H, TimerNew_ItemBase );
				} else {
					TAP_Osd_FillBox(rgn, TimerNew_X+TIMERNEW_PADDING, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*item), TimerNew_Option_W, TimerNew_Option_H, bc);
				}
			}
			TAP_Osd_FillBox(rgn, TimerNew_X+TIMERNEW_PADDING+TIMERNEW_GAP+120, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*item), TIMERNEW_GAP, TimerNew_Option_H, TimerNew_BaseColour);
			switch( fntSize ){
				case FNT_Size_1419:
					Font_Osd_PutString1419( rgn, TimerNew_X+TIMERNEW_PADDING+TIMERNEW_GAP, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*item)+pad_y, TimerNew_X+TIMERNEW_PADDING+TIMERNEW_GAP+120, "Channel", fc, COLOR_None);
					Font_Osd_PutString1419( rgn, TimerNew_X+TIMERNEW_PADDING+TIMERNEW_GAP+130, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*item)+pad_y, TimerNew_X+TIMERNEW_PADDING+TimerNew_Option_W-TIMERNEW_GAP, Channel_Name( TimerNew_Timer.svcType, TimerNew_Timer.svcNum ), fc, COLOR_None);
					break;
				case FNT_Size_1622:
					Font_Osd_PutString1622( rgn, TimerNew_X+TIMERNEW_PADDING+TIMERNEW_GAP, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*item)+pad_y, TimerNew_X+TIMERNEW_PADDING+TIMERNEW_GAP+120, "Channel", fc, COLOR_None);
					Font_Osd_PutString1622( rgn, TimerNew_X+TIMERNEW_PADDING+TIMERNEW_GAP+130, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*item)+pad_y, TimerNew_X+TIMERNEW_PADDING+TimerNew_Option_W-TIMERNEW_GAP, Channel_Name( TimerNew_Timer.svcType, TimerNew_Timer.svcNum ), fc, COLOR_None);
					break;
				case FNT_Size_1926:
					Font_Osd_PutString1926( rgn, TimerNew_X+TIMERNEW_PADDING+TIMERNEW_GAP, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*item)+pad_y, TimerNew_X+TIMERNEW_PADDING+TIMERNEW_GAP+120, "Channel", fc, COLOR_None);
					Font_Osd_PutString1926( rgn, TimerNew_X+TIMERNEW_PADDING+TIMERNEW_GAP+130, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*item)+pad_y, TimerNew_X+TIMERNEW_PADDING+TimerNew_Option_W-TIMERNEW_GAP, Channel_Name( TimerNew_Timer.svcType, TimerNew_Timer.svcNum ), fc, COLOR_None);
					break;
			}
			break;
			
		case TIMERNEW_DURATION:
			if( item == selected ){
				if( TimerNew_ItemHigh != NULL ){
					TAP_Osd_RestoreBox( rgn, TimerNew_X+TIMERNEW_PADDING, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*item), TimerNew_Option_W, TimerNew_Option_H, TimerNew_ItemHigh );
				} else {
					TAP_Osd_FillBox(rgn, TimerNew_X+TIMERNEW_PADDING, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*item), TimerNew_Option_W, TimerNew_Option_H, bc);
				}
			} else {
				if( TimerNew_ItemBase != NULL ){
					TAP_Osd_RestoreBox( rgn, TimerNew_X+TIMERNEW_PADDING, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*item), TimerNew_Option_W, TimerNew_Option_H, TimerNew_ItemBase );
				} else {
					TAP_Osd_FillBox(rgn, TimerNew_X+TIMERNEW_PADDING, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*item), TimerNew_Option_W, TimerNew_Option_H, bc);
				}
			}
			TAP_Osd_FillBox(rgn, TimerNew_X+TIMERNEW_PADDING+TIMERNEW_GAP+120, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*item), TIMERNEW_GAP, TimerNew_Option_H, TimerNew_BaseColour);
			switch( fntSize ){
				case FNT_Size_1419:
					Font_Osd_PutString1419( rgn, TimerNew_X+TIMERNEW_PADDING+TIMERNEW_GAP, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*item)+pad_y, TimerNew_X+TIMERNEW_PADDING+TIMERNEW_GAP+120, "Duration", fc, COLOR_None);
					Font_Osd_PutString1419( rgn, TimerNew_X+TIMERNEW_PADDING+TIMERNEW_GAP+130, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*item)+pad_y, TimerNew_X+TIMERNEW_PADDING+TimerNew_Option_W-TIMERNEW_GAP, MinsToString(TimerNew_Timer.duration), fc, COLOR_None);
					break;
				case FNT_Size_1622:
					Font_Osd_PutString1622( rgn, TimerNew_X+TIMERNEW_PADDING+TIMERNEW_GAP, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*item)+pad_y, TimerNew_X+TIMERNEW_PADDING+TIMERNEW_GAP+120, "Duration", fc, COLOR_None);
					Font_Osd_PutString1622( rgn, TimerNew_X+TIMERNEW_PADDING+TIMERNEW_GAP+130, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*item)+pad_y, TimerNew_X+TIMERNEW_PADDING+TimerNew_Option_W-TIMERNEW_GAP, MinsToString(TimerNew_Timer.duration), fc, COLOR_None);
					break;
				case FNT_Size_1926:
					Font_Osd_PutString1926( rgn, TimerNew_X+TIMERNEW_PADDING+TIMERNEW_GAP, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*item)+pad_y, TimerNew_X+TIMERNEW_PADDING+TIMERNEW_GAP+120, "Duration", fc, COLOR_None);
					Font_Osd_PutString1926( rgn, TimerNew_X+TIMERNEW_PADDING+TIMERNEW_GAP+130, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*item)+pad_y, TimerNew_X+TIMERNEW_PADDING+TimerNew_Option_W-TIMERNEW_GAP, MinsToString(TimerNew_Timer.duration), fc, COLOR_None);
					break;
			}
			break;
			
		case TIMERNEW_TIMERSELECTION:
			if( TimerNew_ItemBase != NULL ){
				TAP_Osd_RestoreBox( rgn, TimerNew_X+TIMERNEW_PADDING, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*item), TimerNew_Option_W, TimerNew_Option_H, TimerNew_ItemBase );
			} else {
				TAP_Osd_FillBox(rgn, TimerNew_X+TIMERNEW_PADDING, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*item), TimerNew_Option_W, TimerNew_Option_H, bc);
			}
			TAP_Osd_FillBox(rgn, TimerNew_X+TIMERNEW_PADDING+(TimerNew_Option_W/2)-(TIMERNEW_GAP/2), TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*item), TIMERNEW_GAP-1, TimerNew_Option_H, TimerNew_BaseColour);
			if( item == selected ){
				if( TimerNew_OptionSelected == TIMERNEW_OK ){
					if( TimerNew_OkItemHigh != NULL ){
						TAP_Osd_RestoreBox( rgn, TimerNew_X+TIMERNEW_PADDING, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*item), (TimerNew_Option_W/2)-(TIMERNEW_GAP/2)-1, TimerNew_Option_H, TimerNew_OkItemHigh );
					} else {
						TAP_Osd_FillBox(rgn, TimerNew_X+TIMERNEW_PADDING, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*item), (TimerNew_Option_W/2)-(TIMERNEW_GAP/2), TimerNew_Option_H, TimerNew_HghBc);
					}
				} else {
					if( TimerNew_OkItemHigh != NULL ){
						TAP_Osd_RestoreBox( rgn, TimerNew_X+TIMERNEW_PADDING+(TimerNew_Option_W/2)+(TIMERNEW_GAP/2), TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*item), (TimerNew_Option_W/2)-(TIMERNEW_GAP/2)-1, TimerNew_Option_H, TimerNew_CancelItemHigh );
					} else {
						TAP_Osd_FillBox(rgn, TimerNew_X+TIMERNEW_PADDING+(TimerNew_Option_W/2)+(TIMERNEW_GAP/2), TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*item), (TimerNew_Option_W/2)-(TIMERNEW_GAP/2)-1, TimerNew_Option_H, TimerNew_HghBc);
					}
				}
				switch( fntSize ){
					case FNT_Size_1419:
						Font_Osd_PutString1419( rgn, TimerNew_X+TIMERNEW_PADDING+TIMERNEW_GAP, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*item)+pad_y, TimerNew_X+TIMERNEW_PADDING+TimerNew_Option_W-TIMERNEW_GAP, "OK", fc, COLOR_None);
						break;
					case FNT_Size_1622:
						Font_Osd_PutString1622( rgn, TimerNew_X+TIMERNEW_PADDING+TIMERNEW_GAP, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*item)+pad_y, TimerNew_X+TIMERNEW_PADDING+TimerNew_Option_W-TIMERNEW_GAP, "OK", fc, COLOR_None);
						break;
					case FNT_Size_1926:
						Font_Osd_PutString1926( rgn, TimerNew_X+TIMERNEW_PADDING+TIMERNEW_GAP, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*item)+pad_y, TimerNew_X+TIMERNEW_PADDING+TimerNew_Option_W-TIMERNEW_GAP, "OK", fc, COLOR_None);
						break;
				}
				switch( fntSize ){
					case FNT_Size_1419:
						Font_Osd_PutString1419( rgn, TimerNew_X+TIMERNEW_PADDING+TIMERNEW_GAP+(TimerNew_Option_W/2)+(TIMERNEW_GAP/2), TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*item)+pad_y, TimerNew_X+TIMERNEW_PADDING+TimerNew_Option_W-TIMERNEW_GAP, "Cancel", fc, COLOR_None);
						break;
					case FNT_Size_1622:
						Font_Osd_PutString1622( rgn, TimerNew_X+TIMERNEW_PADDING+TIMERNEW_GAP+(TimerNew_Option_W/2)+(TIMERNEW_GAP/2), TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*item)+pad_y, TimerNew_X+TIMERNEW_PADDING+TimerNew_Option_W-TIMERNEW_GAP, "Cancel", fc, COLOR_None);
						break;
					case FNT_Size_1926:
						Font_Osd_PutString1926( rgn, TimerNew_X+TIMERNEW_PADDING+TIMERNEW_GAP+(TimerNew_Option_W/2)+(TIMERNEW_GAP/2), TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*item)+pad_y, TimerNew_X+TIMERNEW_PADDING+TimerNew_Option_W-TIMERNEW_GAP, "Cancel", fc, COLOR_None);
						break;
				}	
			} else {
//				TAP_Osd_FillBox(rgn, TimerNew_X+TIMERNEW_PADDING, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*item), (TimerNew_Option_W/2)-(TIMERNEW_GAP/2)-1, TimerNew_Option_H, TimerNew_Bc);
//				TAP_Osd_FillBox(rgn, TimerNew_X+TIMERNEW_PADDING+(TimerNew_Option_W/2)+(TIMERNEW_GAP/2), TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*item), (TimerNew_Option_W/2)-(TIMERNEW_GAP/2)-1, TimerNew_Option_H, TimerNew_Bc);
				switch( fntSize ){
					case FNT_Size_1419:
						Font_Osd_PutString1419( rgn, TimerNew_X+TIMERNEW_PADDING+TIMERNEW_GAP, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*item)+pad_y, TimerNew_X+TIMERNEW_PADDING+TimerNew_Option_W-TIMERNEW_GAP, "OK", fc, COLOR_None);
						break;
					case FNT_Size_1622:
						Font_Osd_PutString1622( rgn, TimerNew_X+TIMERNEW_PADDING+TIMERNEW_GAP, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*item)+pad_y, TimerNew_X+TIMERNEW_PADDING+TimerNew_Option_W-TIMERNEW_GAP, "OK", fc, COLOR_None);
						break;
					case FNT_Size_1926:
						Font_Osd_PutString1926( rgn, TimerNew_X+TIMERNEW_PADDING+TIMERNEW_GAP, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*item)+pad_y, TimerNew_X+TIMERNEW_PADDING+TimerNew_Option_W-TIMERNEW_GAP, "OK", fc, COLOR_None);
						break;
				}
				switch( fntSize ){
					case FNT_Size_1419:
						Font_Osd_PutString1419( rgn, TimerNew_X+TIMERNEW_PADDING+TIMERNEW_GAP+(TimerNew_Option_W/2)+(TIMERNEW_GAP/2), TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*item)+pad_y, TimerNew_X+TIMERNEW_PADDING+TimerNew_Option_W-TIMERNEW_GAP, "Cancel", fc, COLOR_None);
						break;
					case FNT_Size_1622:
						Font_Osd_PutString1622( rgn, TimerNew_X+TIMERNEW_PADDING+TIMERNEW_GAP+(TimerNew_Option_W/2)+(TIMERNEW_GAP/2), TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*item)+pad_y, TimerNew_X+TIMERNEW_PADDING+TimerNew_Option_W-TIMERNEW_GAP, "Cancel", fc, COLOR_None);
						break;
					case FNT_Size_1926:
						Font_Osd_PutString1926( rgn, TimerNew_X+TIMERNEW_PADDING+TIMERNEW_GAP+(TimerNew_Option_W/2)+(TIMERNEW_GAP/2), TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*item)+pad_y, TimerNew_X+TIMERNEW_PADDING+TimerNew_Option_W-TIMERNEW_GAP, "Cancel", fc, COLOR_None);
						break;
				}	
			}
		
			break;
	}
}

char * TimerNew_ReserveType( int type ){
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

void TimerNew_KeyboardClose( char * text, bool changed ){
	// Do nothing atm until it is implement
	if( changed ){
		// We have change the text of the timer so modify the fileName field
		memset( TimerNew_Timer.fileName, '\0', sizeof(TimerNew_Timer.fileName) ); // Blank the filename field
		strncpy(TimerNew_Timer.fileName, text, sizeof(TimerNew_Timer.fileName));
		TimerNew_DrawItem( TimerNew_RGN, TimerNew_Selected, TimerNew_Selected );
		if( sizeof(TimerNew_Timer.fileName) > 0 ){
			TimerNew_Timer.nameFix = 1;	// If user has entered a name then fix it otherwise let the unit decide what to call it.
		} else {
			TimerNew_Timer.nameFix = 0;
		}
	}
}

// Function to draw the timer details to the region
void TimerNew_DrawDetails( word rgn ){
	int i;
	for( i = 0; i < TIMERNEW_OPTIONS; i++ ){
		TimerNew_DrawItem( rgn, i, TimerNew_Selected );
	}
}

int TimerNew_Add( void ){
	strcat(TimerNew_Timer.fileName,".rec");
	return TAP_Timer_Add(&TimerNew_Timer);
}

dword TimerNew_KeyHandler( dword param1 ){
	if( Keyboard_IsActive() ){	// Handle the keyboard
		return Keyboard_Keyhandler(param1);
	}
	if( param1 == RKEY_Exit ){
		TimerNew_Deactivate(0,TRUE);
		return 0;
	}
	// Do the navigation stuff now
	if( param1 == RKEY_ChUp ){
		TimerNew_PrevSelected = TimerNew_Selected;
		TimerNew_Selected--;
		if( TimerNew_Selected < 1 ){
			TimerNew_Selected = TIMERNEW_OPTIONS-1;
		}
		TimerNew_DrawItem( TimerNew_MemRGN, TimerNew_PrevSelected, TimerNew_Selected );
		TimerNew_DrawItem( TimerNew_MemRGN, TimerNew_Selected, TimerNew_Selected );
		TAP_Osd_Copy( TimerNew_MemRGN, TimerNew_RGN, TimerNew_X+TIMERNEW_PADDING, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*TimerNew_PrevSelected), TimerNew_Option_W, TimerNew_Option_H, TimerNew_X+TIMERNEW_PADDING, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*TimerNew_PrevSelected), FALSE);
		TAP_Osd_Copy( TimerNew_MemRGN, TimerNew_RGN, TimerNew_X+TIMERNEW_PADDING, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*TimerNew_Selected), TimerNew_Option_W, TimerNew_Option_H, TimerNew_X+TIMERNEW_PADDING, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*TimerNew_Selected), FALSE);
	}
	// Do the navigation stuff now
	if( param1 == RKEY_ChDown ){
		TimerNew_PrevSelected = TimerNew_Selected;
		TimerNew_Selected++;
		if( TimerNew_Selected > TIMERNEW_OPTIONS-1 ){
			TimerNew_Selected = 1;
		}
		TimerNew_DrawItem( TimerNew_MemRGN, TimerNew_PrevSelected, TimerNew_Selected );
		TimerNew_DrawItem( TimerNew_MemRGN, TimerNew_Selected, TimerNew_Selected );
		TAP_Osd_Copy( TimerNew_MemRGN, TimerNew_RGN, TimerNew_X+TIMERNEW_PADDING, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*TimerNew_PrevSelected), TimerNew_Option_W, TimerNew_Option_H, TimerNew_X+TIMERNEW_PADDING, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*TimerNew_PrevSelected), FALSE);
		TAP_Osd_Copy( TimerNew_MemRGN, TimerNew_RGN, TimerNew_X+TIMERNEW_PADDING, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*TimerNew_Selected), TimerNew_Option_W, TimerNew_Option_H, TimerNew_X+TIMERNEW_PADDING, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*TimerNew_Selected), FALSE);
	}
	// Do the navigation stuff now
	if( param1 == RKEY_VolDown || param1 == RKEY_VolUp || param1 == RKEY_Forward || param1 == RKEY_Rewind ){
		switch( TimerNew_Selected ){
			case TIMERNEW_TIMERSTART:
				TimerNew_AlterStartTime(param1);
				TimerNew_DrawItem( TimerNew_MemRGN, TIMERNEW_TIMEREND, TimerNew_Selected );
				TAP_Osd_Copy( TimerNew_MemRGN, TimerNew_RGN, TimerNew_X+TIMERNEW_PADDING, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*TIMERNEW_TIMEREND), TimerNew_Option_W, TimerNew_Option_H, TimerNew_X+TIMERNEW_PADDING, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*TIMERNEW_TIMEREND), FALSE);
				break;
			case TIMERNEW_TIMEREND:
			case TIMERNEW_DURATION:
				TimerNew_AlterDuration(param1);
				TimerNew_DrawItem( TimerNew_MemRGN, TIMERNEW_TIMEREND, TimerNew_Selected );
				TimerNew_DrawItem( TimerNew_MemRGN, TIMERNEW_DURATION, TimerNew_Selected );
				TAP_Osd_Copy( TimerNew_MemRGN, TimerNew_RGN, TimerNew_X+TIMERNEW_PADDING, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*TIMERNEW_TIMEREND), TimerNew_Option_W, TimerNew_Option_H, TimerNew_X+TIMERNEW_PADDING, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*TIMERNEW_TIMEREND), FALSE);
				TAP_Osd_Copy( TimerNew_MemRGN, TimerNew_RGN, TimerNew_X+TIMERNEW_PADDING, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*TIMERNEW_DURATION), TimerNew_Option_W, TimerNew_Option_H, TimerNew_X+TIMERNEW_PADDING, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*TIMERNEW_DURATION), FALSE);
				return 0;
				break;
			case TIMERNEW_TIMERSELECTION:
				TimerNew_OptionSelected++;
				if( TimerNew_OptionSelected > 1 ){
					TimerNew_OptionSelected  = 0;
				}
				break;
			case TIMERNEW_RESERVETYPE:
				switch ( param1 ){
					case RKEY_VolUp:
						switch ( TimerNew_Timer.reservationType ){
							case RESERVE_TYPE_Onetime: 		TimerNew_Timer.reservationType = RESERVE_TYPE_Everyday; break;
							case RESERVE_TYPE_Everyday: 	TimerNew_Timer.reservationType = RESERVE_TYPE_EveryWeekend; break;
							case RESERVE_TYPE_EveryWeekend: TimerNew_Timer.reservationType = RESERVE_TYPE_Weekly; break;
							case RESERVE_TYPE_Weekly: 		TimerNew_Timer.reservationType = RESERVE_TYPE_WeekDay; break;
							case RESERVE_TYPE_WeekDay: 		TimerNew_Timer.reservationType = RESERVE_TYPE_Onetime; break;
							default : break;
						}
						break;
					case RKEY_VolDown:
						switch ( TimerNew_Timer.reservationType ){
							case RESERVE_TYPE_Onetime: 		TimerNew_Timer.reservationType = RESERVE_TYPE_WeekDay; break;
							case RESERVE_TYPE_Everyday: 	TimerNew_Timer.reservationType = RESERVE_TYPE_Onetime; break;
							case RESERVE_TYPE_EveryWeekend: TimerNew_Timer.reservationType = RESERVE_TYPE_Everyday; break;
							case RESERVE_TYPE_Weekly: 		TimerNew_Timer.reservationType = RESERVE_TYPE_EveryWeekend; break;
							case RESERVE_TYPE_WeekDay: 		TimerNew_Timer.reservationType = RESERVE_TYPE_Weekly; break;
							default : break;
					}
					break;
					default :
						break;
				}
				break;
			case TIMERNEW_DATE:
				TimerNew_AdjustDate( param1 );
				break;
			case TIMERNEW_RECORD:
				TimerNew_ToggleRec();
				break;
			case TIMERNEW_TYPE:
				TimerNew_ToggleType();
				break;
			case TIMERNEW_CHANNEL:
				TimerNew_AlterChannel(param1);
				break;
			
		}
		TimerNew_DrawItem( TimerNew_MemRGN, TimerNew_Selected, TimerNew_Selected );
		TAP_Osd_Copy( TimerNew_MemRGN, TimerNew_RGN, TimerNew_X+TIMERNEW_PADDING, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*TimerNew_Selected), TimerNew_Option_W, TimerNew_Option_H, TimerNew_X+TIMERNEW_PADDING, TimerNew_Y+TIMERNEW_GAP+((TimerNew_Option_H+TIMERNEW_GAP)*TimerNew_Selected), FALSE);
	}
	if( param1 == RKEY_Ok ){
		switch( TimerNew_Selected ){
			case TIMERNEW_TIMERNAME:
				Keyboard_Init( TimerNew_RGN, TimerNew_MemRGN, TimerNew_X, TimerNew_Y, TimerNew_W, TimerNew_H, "Timer Name" );	// Initalise the keyboard
				Keyboard_SetColours( DISPLAY_KEYBOARDITEM, DISPLAY_KEYBOARDITEMTEXT, DISPLAY_KEYBOARDITEMSELECTED, DISPLAY_KEYBOARDITEMSELECTEDTEXT, DISPLAY_KEYBOARDFIELDTEXT, DISPLAY_KEYBOARDFIELD, DISPLAY_KEYBOARDBACKGROUND );
				if( Keyboard_Activate(TimerNew_Timer.fileName,&TimerNew_KeyboardClose) ){
				}
				break;
			case TIMERNEW_TIMERSELECTION:
				if( TimerNew_OptionSelected == TIMERNEW_OK ){
					TimerNew_Deactivate(TimerNew_Add(),FALSE);
				} else {
					TimerNew_Deactivate(0,TRUE);
				}
				break;
		}
	}
	return 0;
}

void TimerNew_AlterChannel( dword key ){
	int count = 0;
	int newSvcNum;
	int max, result;
	int maxTvChannels, maxRadioChannels;
	TYPE_TapChInfo	currentChInfo;
	newSvcNum = TimerNew_Timer.svcNum;
	TAP_Channel_GetTotalNum( &maxTvChannels, &maxRadioChannels );
	if ( TimerNew_Timer.svcType == SVC_TYPE_Tv) max = maxTvChannels;
	else max = maxRadioChannels;
	count = 0;
	switch ( key ){
		case RKEY_VolUp:								// Channel up
			do {
			    if ( newSvcNum < max ) newSvcNum++;		// skip to the next channel
				else newSvcNum = 1;
				count++;								// if the channel is not selectable, go round again
				result = TAP_Channel_GetInfo( TimerNew_Timer.svcType, newSvcNum, &currentChInfo);
			} while ((result == 0) && (count < 3000));	// until either we find a channel that is ok,
			if (result == 0) break;
			TimerNew_Timer.svcNum = newSvcNum;
			break;
		case RKEY_VolDown:								// Channel down
			do {
			    if ( newSvcNum > 0 ) newSvcNum--;
				else newSvcNum = max;
				count++;
				result = TAP_Channel_GetInfo( TimerNew_Timer.svcType, newSvcNum, &currentChInfo);
			} while ((result == 0) && (count < 3000));
			if (result == 0) break;
			TimerNew_Timer.svcNum = newSvcNum;
			break;
	}
}

void TimerNew_ToggleType( void ){
	int maxTvChannels, maxRadioChannels;
	TAP_Channel_GetTotalNum( &maxTvChannels, &maxRadioChannels );
	if ( TimerNew_Timer.svcType == SVC_TYPE_Tv ){
		TimerNew_Timer.svcType = SVC_TYPE_Radio;
		if( maxRadioChannels < 1 ){
			TimerNew_Timer.svcType = SVC_TYPE_Tv;
		} else {
			if ( TimerNew_Timer.svcNum >= maxRadioChannels ) TimerNew_Timer.svcNum = 0;
		}
	} else {
	    TimerNew_Timer.svcType = SVC_TYPE_Tv;
		if( maxTvChannels < 1 ){
			TimerNew_Timer.svcType = SVC_TYPE_Radio;
		} else {
			if ( TimerNew_Timer.svcNum >= maxTvChannels ) TimerNew_Timer.svcNum = 0;
		}
	}
}

void TimerNew_ToggleRec( void ){
	if(TimerNew_Timer.isRec == 1) TimerNew_Timer.isRec = 0;
	else TimerNew_Timer.isRec = 1;
}

void TimerNew_AdjustDate( dword param1 ){
	word 	mjd;
	int 	startTime; // , duration
	startTime = TimerNew_Timer.startTime&0xffff;		// extract the time
	mjd = TimerNew_Timer.startTime>>16;
	switch ( param1 ){
		case RKEY_VolUp:	mjd++;	break;			// Date up
		case RKEY_VolDown:	mjd--;	break;			// Date down
		default :			break;
	}
	TimerNew_Timer.startTime = (mjd<<16) | startTime;
}

void TimerNew_AlterDuration( dword param1 ){
	switch( param1 ){
		case RKEY_VolUp:							// Time up
			TimerNew_Timer.duration++;
			break;
		case RKEY_VolDown:							// Time down
			if ( TimerNew_Timer.duration > 0 ) TimerNew_Timer.duration--;
			break;
		case RKEY_Forward:							// Time up
			TimerNew_Timer.duration+=60;
			break;
		case RKEY_Rewind:							// Time down
			if ( TimerNew_Timer.duration > 60 ) TimerNew_Timer.duration-=60;
			break;
		default :
			break;
	}
}

void TimerNew_AlterStartTime( dword param1 ){
	word 	mjd;
	int 	min, hour;
	hour = (TimerNew_Timer.startTime&0xff00)>>8;			// extract the time
	min = (TimerNew_Timer.startTime&0xff);
	mjd = TimerNew_Timer.startTime>>16;
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
	TimerNew_Timer.startTime = (mjd<<16) | ((hour&0xff)<<8) | (min&0xff);
}

// Function to set the colours of the Timer window
void TimerNew_SetColours( dword fc, dword bc, dword fhc, dword bhc, dword base ){
	TimerNew_Fc = fc;
	TimerNew_HghBc = bhc;
	TimerNew_HghFc = fhc;
	TimerNew_Bc = bc;
	TimerNew_BaseColour = base;
}

