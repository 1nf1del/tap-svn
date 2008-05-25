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

#include "recordingedit.h"
#include "fontman.h"
#include "skin.h"
#include "settings.h"
#include "keyboard.h"

bool RecordingEdit_IsActive( void ){
	return RecordingEdit_Active;
}

// Function to initalize the keyboard - region and location to render
void RecordingEdit_Init( word rgn, word memrgn, int x, int y, int w, int h ){
	RecordingEdit_RGN = rgn;	// Store the region to render on
	RecordingEdit_MemRGN = memrgn;
	RecordingEdit_X = x;
	RecordingEdit_Y = y;
	RecordingEdit_W = w;
	RecordingEdit_H = h;
	RecordingEdit_Initalised = TRUE;
	RecordingEdit_Option_H = (RecordingEdit_H/RECORDINGEDIT_OPTIONS)-RECORDINGEDIT_GAP-2;
	RecordingEdit_Option_W = RecordingEdit_W - (RECORDINGEDIT_PADDING*2);
	RecordingEdit_CreateGradients();
}

void RecordingEdit_MemFree( void ){
	if (RecordingEdit_ItemBase) TAP_MemFree(RecordingEdit_ItemBase);
	if (RecordingEdit_ItemHigh) TAP_MemFree(RecordingEdit_ItemHigh);
	RecordingEdit_ItemBase = NULL;
	RecordingEdit_ItemHigh = NULL;
}

void RecordingEdit_CreateGradients( void ){
	word rgn;
	RecordingEdit_MemFree();
	if( Settings_GradientFactor > 0 ){
		rgn	= TAP_Osd_Create(0, 0, RecordingEdit_Option_W, RecordingEdit_Option_H, 0, OSD_Flag_MemRgn);
		// Main default item
		createGradient(rgn, RecordingEdit_Option_W, RecordingEdit_Option_H, Settings_GradientFactor, DISPLAY_ITEM);
		RecordingEdit_ItemBase = TAP_Osd_SaveBox(rgn, 0, 0, RecordingEdit_Option_W, RecordingEdit_Option_H);
		createGradient(rgn, RecordingEdit_Option_W, RecordingEdit_Option_H, Settings_GradientFactor, DISPLAY_ITEMSELECTED);
		RecordingEdit_ItemHigh = TAP_Osd_SaveBox(rgn, 0, 0, RecordingEdit_Option_W, RecordingEdit_Option_H);
		TAP_Osd_Delete(rgn);
	}
}
// Activate by sending the timer and the number. Used when not using events ala conflict calling
bool RecordingEdit_Activate( int slot, void (*ReturningProcedure)( void ) ){
	if( !RecordingEdit_Initalised ) return FALSE;	// We haven't initalised so gracefully fail
	RecordingEdit_Callback = ReturningProcedure;
	// Get the details of the event and create the timer structure
	RecordingEdit_Slot = slot;
	RecordingEdit_Selected = RECORDINGEDIT_TIMEREND;
	RecordingEdit_PrevSelected = RECORDINGEDIT_TIMEREND;
	RecordingEdit_DrawBackground(RecordingEdit_MemRGN);
	RecordingEdit_DrawDetails(RecordingEdit_MemRGN);
	// Coppy rendered to foreground
	TAP_Osd_Copy( RecordingEdit_MemRGN, RecordingEdit_RGN, RecordingEdit_X, RecordingEdit_Y, RecordingEdit_W, RecordingEdit_H, RecordingEdit_X, RecordingEdit_Y, FALSE);
	RecordingEdit_Active = TRUE;
	return TRUE;
}

// Routine to handle the closing of the timer window
void RecordingEdit_Deactivate( int retval, bool cancel ){
	RecordingEdit_Active = FALSE;
	// Restore what we had drawn over
	TAP_Osd_RestoreBox(RecordingEdit_RGN, RecordingEdit_X, RecordingEdit_Y, RecordingEdit_W, RecordingEdit_H, RecordingEdit_Base);
	TAP_MemFree(RecordingEdit_Base);
	RecordingEdit_Base = NULL;
	RecordingEdit_MemFree();
	RecordingEdit_Callback();
}

// Here we will just draw the basic layout of the keyboard
void RecordingEdit_DrawBackground( word rgn ){
	if( RecordingEdit_Base != NULL ) TAP_MemFree(RecordingEdit_Base);
	RecordingEdit_Base = TAP_Osd_SaveBox(RecordingEdit_RGN, RecordingEdit_X, RecordingEdit_Y, RecordingEdit_W, RecordingEdit_H);	// Save what is under the Timer screen
	TAP_Osd_FillBox(rgn, RecordingEdit_X, RecordingEdit_Y, RecordingEdit_W, RecordingEdit_H, RecordingEdit_BaseColour);
}

void RecordingEdit_DrawItem( word rgn, int item, int selected ){
	int fntSize = 0;
	int pad_y;
	dword fc = RecordingEdit_Fc;
	dword bc = RecordingEdit_Bc;
	// Work out the font size allowed for the height of the option
	if( ( RecordingEdit_Option_H - 8 ) > 19  ){
		if( ( RecordingEdit_Option_H - 12 ) > 22 ){
			// Use the big font
			fntSize = FNT_Size_1926;
			pad_y = (RecordingEdit_Option_H/2)-(26/2);
		} else {
			fntSize = FNT_Size_1622;
			pad_y = (RecordingEdit_Option_H/2)-(22/2);
		}
	} else {
		pad_y = (RecordingEdit_Option_H/2)-(19/2);
	}
	// If selected change the colors of the elements
	if( item == selected ){
		fc = RecordingEdit_HghFc;
		bc = RecordingEdit_HghBc;
	}
	Update_RecordingInfo(RecordingEdit_Slot);
	switch( item ){
		case RECORDINGEDIT_SECTIONTITLE:
			TAP_Osd_FillBox(rgn, RecordingEdit_X+RECORDINGEDIT_PADDING, RecordingEdit_Y+RECORDINGEDIT_GAP+((RecordingEdit_Option_H+RECORDINGEDIT_GAP)*item), RecordingEdit_Option_W, RecordingEdit_Option_H, RecordingEdit_BaseColour);
			switch( fntSize ){
				case FNT_Size_1419:
					Font_Osd_PutString1419( rgn, RecordingEdit_X+RECORDINGEDIT_PADDING+RECORDINGEDIT_GAP, RecordingEdit_Y+RECORDINGEDIT_GAP+((RecordingEdit_Option_H+RECORDINGEDIT_GAP)*item)+pad_y, RecordingEdit_X+RECORDINGEDIT_PADDING+RecordingEdit_Option_W-RECORDINGEDIT_GAP, "Adjust Selected Recording", fc, COLOR_None);
					break;
				case FNT_Size_1622:
					Font_Osd_PutString1622( rgn, RecordingEdit_X+RECORDINGEDIT_PADDING+RECORDINGEDIT_GAP, RecordingEdit_Y+RECORDINGEDIT_GAP+((RecordingEdit_Option_H+RECORDINGEDIT_GAP)*item)+pad_y, RecordingEdit_X+RECORDINGEDIT_PADDING+RecordingEdit_Option_W-RECORDINGEDIT_GAP, "Adjust Selected Recording", fc, COLOR_None);
					break;
				case FNT_Size_1926:
					Font_Osd_PutString1926( rgn, RecordingEdit_X+RECORDINGEDIT_PADDING+RECORDINGEDIT_GAP, RecordingEdit_Y+RECORDINGEDIT_GAP+((RecordingEdit_Option_H+RECORDINGEDIT_GAP)*item)+pad_y, RecordingEdit_X+RECORDINGEDIT_PADDING+RecordingEdit_Option_W-RECORDINGEDIT_GAP, "Adjust Selected Recording", fc, COLOR_None);
					break;
			}
			break;
		case RECORDINGEDIT_TIMERSTART:
			if( item == selected ){
				if( RecordingEdit_ItemHigh != NULL ){
					TAP_Osd_RestoreBox( rgn, RecordingEdit_X+RECORDINGEDIT_PADDING, RecordingEdit_Y+RECORDINGEDIT_GAP+((RecordingEdit_Option_H+RECORDINGEDIT_GAP)*item), RecordingEdit_Option_W, RecordingEdit_Option_H, RecordingEdit_ItemHigh );
				} else {
					TAP_Osd_FillBox(rgn, RecordingEdit_X+RECORDINGEDIT_PADDING, RecordingEdit_Y+RECORDINGEDIT_GAP+((RecordingEdit_Option_H+RECORDINGEDIT_GAP)*item), RecordingEdit_Option_W, RecordingEdit_Option_H, bc);
				}
			} else {
				if( RecordingEdit_ItemBase != NULL ){
					TAP_Osd_RestoreBox( rgn, RecordingEdit_X+RECORDINGEDIT_PADDING, RecordingEdit_Y+RECORDINGEDIT_GAP+((RecordingEdit_Option_H+RECORDINGEDIT_GAP)*item), RecordingEdit_Option_W, RecordingEdit_Option_H, RecordingEdit_ItemBase );
				} else {
					TAP_Osd_FillBox(rgn, RecordingEdit_X+RECORDINGEDIT_PADDING, RecordingEdit_Y+RECORDINGEDIT_GAP+((RecordingEdit_Option_H+RECORDINGEDIT_GAP)*item), RecordingEdit_Option_W, RecordingEdit_Option_H, bc);
				}
			}
			TAP_Osd_FillBox(rgn, RecordingEdit_X+RECORDINGEDIT_PADDING+RECORDINGEDIT_GAP+120, RecordingEdit_Y+RECORDINGEDIT_GAP+((RecordingEdit_Option_H+RECORDINGEDIT_GAP)*item), RECORDINGEDIT_GAP, RecordingEdit_Option_H, RecordingEdit_BaseColour);
			switch( fntSize ){
				case FNT_Size_1419:
					Font_Osd_PutString1419( rgn, RecordingEdit_X+RECORDINGEDIT_PADDING+RECORDINGEDIT_GAP, RecordingEdit_Y+RECORDINGEDIT_GAP+((RecordingEdit_Option_H+RECORDINGEDIT_GAP)*item)+pad_y, RecordingEdit_X+RECORDINGEDIT_PADDING+RECORDINGEDIT_GAP+120, "Start Time", fc, COLOR_None);
					Font_Osd_PutString1419( rgn, RecordingEdit_X+RECORDINGEDIT_PADDING+RECORDINGEDIT_GAP+130, RecordingEdit_Y+RECORDINGEDIT_GAP+((RecordingEdit_Option_H+RECORDINGEDIT_GAP)*item)+pad_y, RecordingEdit_X+RECORDINGEDIT_PADDING+RecordingEdit_Option_W-RECORDINGEDIT_GAP, Time_HHMM(Recording_GetRecInfo(RecordingEdit_Slot).startTime), fc, COLOR_None);
					break;
				case FNT_Size_1622:
					Font_Osd_PutString1622( rgn, RecordingEdit_X+RECORDINGEDIT_PADDING+RECORDINGEDIT_GAP, RecordingEdit_Y+RECORDINGEDIT_GAP+((RecordingEdit_Option_H+RECORDINGEDIT_GAP)*item)+pad_y, RecordingEdit_X+RECORDINGEDIT_PADDING+RECORDINGEDIT_GAP+120, "Start Time", fc, COLOR_None);
					Font_Osd_PutString1622( rgn, RecordingEdit_X+RECORDINGEDIT_PADDING+RECORDINGEDIT_GAP+130, RecordingEdit_Y+RECORDINGEDIT_GAP+((RecordingEdit_Option_H+RECORDINGEDIT_GAP)*item)+pad_y, RecordingEdit_X+RECORDINGEDIT_PADDING+RecordingEdit_Option_W-RECORDINGEDIT_GAP, Time_HHMM(Recording_GetRecInfo(RecordingEdit_Slot).startTime), fc, COLOR_None);
					break;
				case FNT_Size_1926:
					Font_Osd_PutString1926( rgn, RecordingEdit_X+RECORDINGEDIT_PADDING+RECORDINGEDIT_GAP, RecordingEdit_Y+RECORDINGEDIT_GAP+((RecordingEdit_Option_H+RECORDINGEDIT_GAP)*item)+pad_y, RecordingEdit_X+RECORDINGEDIT_PADDING+RECORDINGEDIT_GAP+120, "Start Time", fc, COLOR_None);
					Font_Osd_PutString1926( rgn, RecordingEdit_X+RECORDINGEDIT_PADDING+RECORDINGEDIT_GAP+130, RecordingEdit_Y+RECORDINGEDIT_GAP+((RecordingEdit_Option_H+RECORDINGEDIT_GAP)*item)+pad_y, RecordingEdit_X+RECORDINGEDIT_PADDING+RecordingEdit_Option_W-RECORDINGEDIT_GAP, Time_HHMM(Recording_GetRecInfo(RecordingEdit_Slot).startTime), fc, COLOR_None);
					break;
			}
			break;
		case RECORDINGEDIT_TIMEREND:
			if( item == selected ){
				if( RecordingEdit_ItemHigh != NULL ){
					TAP_Osd_RestoreBox( rgn, RecordingEdit_X+RECORDINGEDIT_PADDING, RecordingEdit_Y+RECORDINGEDIT_GAP+((RecordingEdit_Option_H+RECORDINGEDIT_GAP)*item), RecordingEdit_Option_W, RecordingEdit_Option_H, RecordingEdit_ItemHigh );
				} else {
					TAP_Osd_FillBox(rgn, RecordingEdit_X+RECORDINGEDIT_PADDING, RecordingEdit_Y+RECORDINGEDIT_GAP+((RecordingEdit_Option_H+RECORDINGEDIT_GAP)*item), RecordingEdit_Option_W, RecordingEdit_Option_H, bc);
				}
			} else {
				if( RecordingEdit_ItemBase != NULL ){
					TAP_Osd_RestoreBox( rgn, RecordingEdit_X+RECORDINGEDIT_PADDING, RecordingEdit_Y+RECORDINGEDIT_GAP+((RecordingEdit_Option_H+RECORDINGEDIT_GAP)*item), RecordingEdit_Option_W, RecordingEdit_Option_H, RecordingEdit_ItemBase );
				} else {
					TAP_Osd_FillBox(rgn, RecordingEdit_X+RECORDINGEDIT_PADDING, RecordingEdit_Y+RECORDINGEDIT_GAP+((RecordingEdit_Option_H+RECORDINGEDIT_GAP)*item), RecordingEdit_Option_W, RecordingEdit_Option_H, bc);
				}
			}
			TAP_Osd_FillBox(rgn, RecordingEdit_X+RECORDINGEDIT_PADDING+RECORDINGEDIT_GAP+120, RecordingEdit_Y+RECORDINGEDIT_GAP+((RecordingEdit_Option_H+RECORDINGEDIT_GAP)*item), RECORDINGEDIT_GAP, RecordingEdit_Option_H, RecordingEdit_BaseColour);
			switch( fntSize ){
				case FNT_Size_1419:
					Font_Osd_PutString1419( rgn, RecordingEdit_X+RECORDINGEDIT_PADDING+RECORDINGEDIT_GAP, RecordingEdit_Y+RECORDINGEDIT_GAP+((RecordingEdit_Option_H+RECORDINGEDIT_GAP)*item)+pad_y, RecordingEdit_X+RECORDINGEDIT_PADDING+RECORDINGEDIT_GAP+120, "End Time", fc, COLOR_None);
					Font_Osd_PutString1419( rgn, RecordingEdit_X+RECORDINGEDIT_PADDING+RECORDINGEDIT_GAP+130, RecordingEdit_Y+RECORDINGEDIT_GAP+((RecordingEdit_Option_H+RECORDINGEDIT_GAP)*item)+pad_y, RecordingEdit_X+RECORDINGEDIT_PADDING+RecordingEdit_Option_W-RECORDINGEDIT_GAP, Endtime_HHMM(Recording_GetRecInfo(RecordingEdit_Slot).startTime,Recording_GetRecInfo(RecordingEdit_Slot).duration), fc, COLOR_None);
					break;
				case FNT_Size_1622:
					Font_Osd_PutString1622( rgn, RecordingEdit_X+RECORDINGEDIT_PADDING+RECORDINGEDIT_GAP, RecordingEdit_Y+RECORDINGEDIT_GAP+((RecordingEdit_Option_H+RECORDINGEDIT_GAP)*item)+pad_y, RecordingEdit_X+RECORDINGEDIT_PADDING+RECORDINGEDIT_GAP+120, "End Time", fc, COLOR_None);
					Font_Osd_PutString1622( rgn, RecordingEdit_X+RECORDINGEDIT_PADDING+RECORDINGEDIT_GAP+130, RecordingEdit_Y+RECORDINGEDIT_GAP+((RecordingEdit_Option_H+RECORDINGEDIT_GAP)*item)+pad_y, RecordingEdit_X+RECORDINGEDIT_PADDING+RecordingEdit_Option_W-RECORDINGEDIT_GAP, Endtime_HHMM(Recording_GetRecInfo(RecordingEdit_Slot).startTime,Recording_GetRecInfo(RecordingEdit_Slot).duration), fc, COLOR_None);
					break;
				case FNT_Size_1926:
					Font_Osd_PutString1926( rgn, RecordingEdit_X+RECORDINGEDIT_PADDING+RECORDINGEDIT_GAP, RecordingEdit_Y+RECORDINGEDIT_GAP+((RecordingEdit_Option_H+RECORDINGEDIT_GAP)*item)+pad_y, RecordingEdit_X+RECORDINGEDIT_PADDING+RECORDINGEDIT_GAP+120, "End Time", fc, COLOR_None);
					Font_Osd_PutString1926( rgn, RecordingEdit_X+RECORDINGEDIT_PADDING+RECORDINGEDIT_GAP+130, RecordingEdit_Y+RECORDINGEDIT_GAP+((RecordingEdit_Option_H+RECORDINGEDIT_GAP)*item)+pad_y, RecordingEdit_X+RECORDINGEDIT_PADDING+RecordingEdit_Option_W-RECORDINGEDIT_GAP, Endtime_HHMM(Recording_GetRecInfo(RecordingEdit_Slot).startTime,Recording_GetRecInfo(RecordingEdit_Slot).duration), fc, COLOR_None);
					break;
			}
			break;
	}
}

// Function to draw the timer details to the region
void RecordingEdit_DrawDetails( word rgn ){
	int i;
	for( i = 0; i < RECORDINGEDIT_OPTIONS; i++ ){
		RecordingEdit_DrawItem( rgn, i, RecordingEdit_Selected );
	}
}

dword RecordingEdit_KeyHandler( dword param1 ){
	if( param1 == RKEY_Exit ){
		RecordingEdit_Deactivate(0,TRUE);
		return 0;
	}
	// Do the navigation stuff now
	if( param1 == RKEY_VolDown || param1 == RKEY_VolUp || param1 == RKEY_Forward || param1 == RKEY_Rewind ){
		switch( RecordingEdit_Selected ){
			case RECORDINGEDIT_TIMEREND:
				RecordingEdit_AlterDuration(param1);
				break;
		}
		RecordingEdit_DrawItem( RecordingEdit_MemRGN, RecordingEdit_Selected, RecordingEdit_Selected );
		TAP_Osd_Copy( RecordingEdit_MemRGN, RecordingEdit_RGN, RecordingEdit_X+RECORDINGEDIT_PADDING, RecordingEdit_Y+RECORDINGEDIT_GAP+((RecordingEdit_Option_H+RECORDINGEDIT_GAP)*RecordingEdit_PrevSelected), RecordingEdit_Option_W, RecordingEdit_Option_H, RecordingEdit_X+RECORDINGEDIT_PADDING, RecordingEdit_Y+RECORDINGEDIT_GAP+((RecordingEdit_Option_H+RECORDINGEDIT_GAP)*RecordingEdit_PrevSelected), FALSE);
	}
	return 0;
}

void RecordingEdit_AlterDuration( dword param1 ){
	int addtime = 0;
	Update_RecordingInfo(RecordingEdit_Slot);
	switch( param1 ){
		case RKEY_VolUp:							// Time up
			addtime = 1;
			break;
		case RKEY_VolDown:							// Time down
			addtime = -1;
			break;
		case RKEY_Forward:							// Time up
			addtime = 60;
			break;
		case RKEY_Rewind:							// Time down
			addtime = -60;
			break;
		default :
			break;
	}
	if( AddTime( Recording_GetRecInfo(RecordingEdit_Slot).startTime, (Recording_GetRecInfo(RecordingEdit_Slot).duration+addtime) ) < Now(0) ) return;	// Can be less than now
	HDD_RECSlotSetDuration(RecordingEdit_Slot, (Recording_GetRecInfo(RecordingEdit_Slot).duration+addtime));
}

// Function to set the colours of the Timer window
void RecordingEdit_SetColours( dword fc, dword bc, dword fhc, dword bhc, dword base ){
	RecordingEdit_Fc = fc;
	RecordingEdit_HghBc = bhc;
	RecordingEdit_HghFc = fhc;
	RecordingEdit_Bc = bc;
	RecordingEdit_BaseColour = base;
}

