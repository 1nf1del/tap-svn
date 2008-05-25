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

#include "favnew.h"
#include "fontman.h"
#include "skin.h"
#include "settings.h"
#include "favmanage.h"
#include "keyboard.h"

bool FavNew_IsActive( void ){
	return FavNew_Active;
}

// Function to initalize the keyboard - region and location to render
void FavNew_Init( word rgn, word memrgn, int x, int y, int w, int h ){
	FavNew_RGN = rgn;	// Store the region to render on
	FavNew_MemRGN = memrgn;
	FavNew_X = x;
	FavNew_Y = y;
	FavNew_W = w;
	FavNew_H = h;
	FavNew_Initalised = TRUE;
	FavNew_Option_H = (FavNew_H/FAVNEW_OPTIONS)-FAVNEW_GAP;
	FavNew_H += 5;
	FavNew_Option_W = FavNew_W - (FAVNEW_PADDING*2);
	FavNew_CreateGradients();	// Create the gradient for the display
}

void FavNew_MemFree( void ){
	if (FavNew_ItemBase) TAP_MemFree(FavNew_ItemBase);
	if (FavNew_ItemHigh) TAP_MemFree(FavNew_ItemHigh);
	if (FavNew_OkItemHigh) TAP_MemFree(FavNew_OkItemHigh);
	if (FavNew_CancelItemHigh) TAP_MemFree(FavNew_CancelItemHigh);
	FavNew_ItemBase = NULL;
	FavNew_ItemHigh = NULL;
	FavNew_OkItemHigh = NULL;
	FavNew_CancelItemHigh = NULL;
}

void FavNew_CreateGradients( void ){
	word rgn;
	FavNew_MemFree();
	if( Settings_GradientFactor > 0 ){
		rgn	= TAP_Osd_Create(0, 0, FavNew_Option_W, FavNew_Option_H, 0, OSD_Flag_MemRgn);
		// Main default item
		createGradient(rgn, FavNew_Option_W, FavNew_Option_H, Settings_GradientFactor, DISPLAY_ITEM);
		FavNew_ItemBase = TAP_Osd_SaveBox(rgn, 0, 0, FavNew_Option_W, FavNew_Option_H);
		createGradient(rgn, FavNew_Option_W, FavNew_Option_H, Settings_GradientFactor, DISPLAY_ITEMSELECTED);
		FavNew_ItemHigh = TAP_Osd_SaveBox(rgn, 0, 0, FavNew_Option_W, FavNew_Option_H);
		FavNew_CancelItemHigh = TAP_Osd_SaveBox(rgn, 0, 0, (FavNew_Option_W/2)-(FAVNEW_GAP/2), FavNew_Option_H);
		FavNew_OkItemHigh = TAP_Osd_SaveBox(rgn, 0, 0, (FavNew_Option_W/2)-(FAVNEW_GAP/2)-1, FavNew_Option_H);
		TAP_Osd_Delete(rgn);
	}
}

bool FavNew_ActivateWithItem( favitem item, void (*ReturningProcedure)( favitem, bool ) ){
	if( !FavNew_Initalised ) return FALSE;	// We haven't initalised so gracefully fail
	FavNew_Callback = ReturningProcedure;
	FavNew_Item = item;
	FavNew_Selected = 1;
	FavNew_PrevSelected = 1;
	FavNew_OptionSelected = 0;
	// Now lets render the display
	FavNew_DrawBackground(FavNew_MemRGN);
	FavNew_DrawDetails(FavNew_MemRGN);
	// Copy rendered to foreground
	TAP_Osd_Copy( FavNew_MemRGN, FavNew_RGN, FavNew_X, FavNew_Y, FavNew_W, FavNew_H, FavNew_X, FavNew_Y, FALSE);
	FavNew_Active = TRUE;
	return TRUE;
}

bool FavNew_Activate( void (*ReturningProcedure)( favitem, bool ) ){
	if( !FavNew_Initalised ) return FALSE;	// We haven't initalised so gracefully fail
	FavNew_Callback = ReturningProcedure;
	// Clear our favourite item
	memset(FavNew_Item.term,0x0,sizeof(FavNew_Item.term));
	memset(FavNew_Item.channel,0x0,sizeof(FavNew_Item.channel));
	memset(FavNew_Item.day,0x0,sizeof(FavNew_Item.day));
	FavNew_Item.location = 0;
	FavNew_Item.start = -1;
	FavNew_Item.end = -1;
	FavNew_Item.priority = 0;
	FavNew_Item.adj = 0;
	FavNew_Item.rpt = 0;
	FavNew_Selected = 1;
	FavNew_PrevSelected = 1;
	FavNew_OptionSelected = 0;
	// Now lets render the display
	FavNew_DrawBackground(FavNew_MemRGN);
	FavNew_DrawDetails(FavNew_MemRGN);
	// Coppy rendered to foreground
	TAP_Osd_Copy( FavNew_MemRGN, FavNew_RGN, FavNew_X, FavNew_Y, FavNew_W, FavNew_H, FavNew_X, FavNew_Y, FALSE);
	FavNew_Active = TRUE;
	return TRUE;
}

// Routine to handle the closing of the timer window
void FavNew_Deactivate( bool cancel ){
	FavNew_Active = FALSE;
	// Restore what we had drawn over
	TAP_Osd_RestoreBox(FavNew_RGN, FavNew_X, FavNew_Y, FavNew_W, FavNew_H, FavNew_Base);
	TAP_MemFree(FavNew_Base);
	FavNew_Base = NULL;
	FavNew_Callback(FavNew_Item, cancel);
}


// Here we will just draw the basic layout of the keyboard
void FavNew_DrawBackground( word rgn ){
	if( FavNew_Base != NULL ) TAP_MemFree(FavNew_Base);
	FavNew_Base = TAP_Osd_SaveBox(FavNew_RGN, FavNew_X, FavNew_Y, FavNew_W, FavNew_H);	// Save what is under the Timer screen
	TAP_Osd_FillBox(rgn, FavNew_X, FavNew_Y, FavNew_W, FavNew_H, FavNew_BaseColour);
}

void FavNew_DrawItem( word rgn, int item, int selected ){
	int fntSize = 0;
	int pad_y;
	dword fc = FavNew_Fc;
	dword bc = FavNew_Bc;
	// Work out the font size allowed for the height of the option
	if( ( FavNew_Option_H - 8 ) > 19  ){
		if( ( FavNew_Option_H - 12 ) > 22 ){
			// Use the big font
			fntSize = FNT_Size_1926;
			pad_y = (FavNew_Option_H/2)-(26/2);
		} else {
			fntSize = FNT_Size_1622;
			pad_y = (FavNew_Option_H/2)-(22/2);
		}
	} else {
		pad_y = (FavNew_Option_H/2)-(19/2);
	}
	// If selected change the colors of the elements
	if( item == selected ){
		fc = FavNew_HghFc;
		bc = FavNew_HghBc;
	}
	switch( item ){
		case FAVNEW_SECTIONTITLE:
			TAP_Osd_FillBox(rgn, FavNew_X+FAVNEW_PADDING, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item), FavNew_Option_W, FavNew_Option_H, FavNew_BaseColour);
			switch( fntSize ){
				case FNT_Size_1419:
					Font_Osd_PutString1419( rgn, FavNew_X+FAVNEW_PADDING+FAVNEW_GAP, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item)+pad_y, FavNew_X+FAVNEW_PADDING+FavNew_Option_W-FAVNEW_GAP, "New Favourite Details", fc, COLOR_None);
					break;
				case FNT_Size_1622:
					Font_Osd_PutString1622( rgn, FavNew_X+FAVNEW_PADDING+FAVNEW_GAP, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item)+pad_y, FavNew_X+FAVNEW_PADDING+FavNew_Option_W-FAVNEW_GAP, "New Favourite Details", fc, COLOR_None);
					break;
				case FNT_Size_1926:
					Font_Osd_PutString1926( rgn, FavNew_X+FAVNEW_PADDING+FAVNEW_GAP, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item)+pad_y, FavNew_X+FAVNEW_PADDING+FavNew_Option_W-FAVNEW_GAP, "New Favourite Details", fc, COLOR_None);
					break;
			}
			break;
		case FAVNEW_TERM:
			if( item == selected ){
				if( FavNew_ItemHigh != NULL ){
					TAP_Osd_RestoreBox( rgn, FavNew_X+FAVNEW_PADDING, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item), FavNew_Option_W, FavNew_Option_H, FavNew_ItemHigh );
				} else {
					TAP_Osd_FillBox(rgn, FavNew_X+FAVNEW_PADDING, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item), FavNew_Option_W, FavNew_Option_H, bc);
				}
			} else {
				if( FavNew_ItemBase != NULL ){
					TAP_Osd_RestoreBox( rgn, FavNew_X+FAVNEW_PADDING, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item), FavNew_Option_W, FavNew_Option_H, FavNew_ItemBase );
				} else {
					TAP_Osd_FillBox(rgn, FavNew_X+FAVNEW_PADDING, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item), FavNew_Option_W, FavNew_Option_H, bc);
				}
			}
//			TAP_Osd_FillBox(rgn, FavNew_X+FAVNEW_PADDING, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item), FavNew_Option_W, FavNew_Option_H, bc);
			TAP_Osd_FillBox(rgn, FavNew_X+FAVNEW_PADDING+FAVNEW_GAP+90, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item), FAVNEW_GAP, FavNew_Option_H, FavNew_BaseColour);
			switch( fntSize ){
				case FNT_Size_1419:
					Font_Osd_PutString1419( rgn, FavNew_X+FAVNEW_PADDING+FAVNEW_GAP, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item)+pad_y, FavNew_X+FAVNEW_PADDING+FAVNEW_GAP+90, "Term", fc, COLOR_None);
					Font_Osd_PutString1419( rgn, FavNew_X+FAVNEW_PADDING+FAVNEW_GAP+100, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item)+pad_y, FavNew_X+FAVNEW_PADDING+FavNew_Option_W-FAVNEW_GAP, FavNew_Item.term, fc, COLOR_None);
					break;
				case FNT_Size_1622:
					Font_Osd_PutString1622( rgn, FavNew_X+FAVNEW_PADDING+FAVNEW_GAP, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item)+pad_y, FavNew_X+FAVNEW_PADDING+FAVNEW_GAP+90, "Term", fc, COLOR_None);
					Font_Osd_PutString1622( rgn, FavNew_X+FAVNEW_PADDING+FAVNEW_GAP+100, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item)+pad_y, FavNew_X+FAVNEW_PADDING+FavNew_Option_W-FAVNEW_GAP, FavNew_Item.term, fc, COLOR_None);
					break;
				case FNT_Size_1926:
					Font_Osd_PutString1926( rgn, FavNew_X+FAVNEW_PADDING+FAVNEW_GAP, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item)+pad_y, FavNew_X+FAVNEW_PADDING+FAVNEW_GAP+90, "Term", fc, COLOR_None);
					Font_Osd_PutString1926( rgn, FavNew_X+FAVNEW_PADDING+FAVNEW_GAP+100, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item)+pad_y, FavNew_X+FAVNEW_PADDING+FavNew_Option_W-FAVNEW_GAP, FavNew_Item.term, fc, COLOR_None);
					break;
			}
			break;
		case FAVNEW_CHANNEL:
			if( item == selected ){
				if( FavNew_ItemHigh != NULL ){
					TAP_Osd_RestoreBox( rgn, FavNew_X+FAVNEW_PADDING, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item), FavNew_Option_W, FavNew_Option_H, FavNew_ItemHigh );
				} else {
					TAP_Osd_FillBox(rgn, FavNew_X+FAVNEW_PADDING, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item), FavNew_Option_W, FavNew_Option_H, bc);
				}
			} else {
				if( FavNew_ItemBase != NULL ){
					TAP_Osd_RestoreBox( rgn, FavNew_X+FAVNEW_PADDING, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item), FavNew_Option_W, FavNew_Option_H, FavNew_ItemBase );
				} else {
					TAP_Osd_FillBox(rgn, FavNew_X+FAVNEW_PADDING, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item), FavNew_Option_W, FavNew_Option_H, bc);
				}
			}
//			TAP_Osd_FillBox(rgn, FavNew_X+FAVNEW_PADDING, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item), FavNew_Option_W, FavNew_Option_H, bc);
			TAP_Osd_FillBox(rgn, FavNew_X+FAVNEW_PADDING+FAVNEW_GAP+90, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item), FAVNEW_GAP, FavNew_Option_H, FavNew_BaseColour);
			switch( fntSize ){
				case FNT_Size_1419:
					Font_Osd_PutString1419( rgn, FavNew_X+FAVNEW_PADDING+FAVNEW_GAP, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item)+pad_y, FavNew_X+FAVNEW_PADDING+FAVNEW_GAP+90, "Channel", fc, COLOR_None);
					Font_Osd_PutString1419( rgn, FavNew_X+FAVNEW_PADDING+FAVNEW_GAP+100, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item)+pad_y, FavNew_X+FAVNEW_PADDING+FavNew_Option_W-FAVNEW_GAP, FavNew_Item.channel, fc, COLOR_None);
					break;
				case FNT_Size_1622:
					Font_Osd_PutString1622( rgn, FavNew_X+FAVNEW_PADDING+FAVNEW_GAP, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item)+pad_y, FavNew_X+FAVNEW_PADDING+FAVNEW_GAP+90, "Channel", fc, COLOR_None);
					Font_Osd_PutString1622( rgn, FavNew_X+FAVNEW_PADDING+FAVNEW_GAP+100, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item)+pad_y, FavNew_X+FAVNEW_PADDING+FavNew_Option_W-FAVNEW_GAP, FavNew_Item.channel, fc, COLOR_None);
					break;
				case FNT_Size_1926:
					Font_Osd_PutString1926( rgn, FavNew_X+FAVNEW_PADDING+FAVNEW_GAP, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item)+pad_y, FavNew_X+FAVNEW_PADDING+FAVNEW_GAP+90, "Channel", fc, COLOR_None);
					Font_Osd_PutString1926( rgn, FavNew_X+FAVNEW_PADDING+FAVNEW_GAP+100, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item)+pad_y, FavNew_X+FAVNEW_PADDING+FavNew_Option_W-FAVNEW_GAP, FavNew_Item.channel, fc, COLOR_None);
					break;
			}
			break;
		case FAVNEW_DAY:
			if( item == selected ){
				if( FavNew_ItemHigh != NULL ){
					TAP_Osd_RestoreBox( rgn, FavNew_X+FAVNEW_PADDING, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item), FavNew_Option_W, FavNew_Option_H, FavNew_ItemHigh );
				} else {
					TAP_Osd_FillBox(rgn, FavNew_X+FAVNEW_PADDING, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item), FavNew_Option_W, FavNew_Option_H, bc);
				}
			} else {
				if( FavNew_ItemBase != NULL ){
					TAP_Osd_RestoreBox( rgn, FavNew_X+FAVNEW_PADDING, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item), FavNew_Option_W, FavNew_Option_H, FavNew_ItemBase );
				} else {
					TAP_Osd_FillBox(rgn, FavNew_X+FAVNEW_PADDING, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item), FavNew_Option_W, FavNew_Option_H, bc);
				}
			}
//			TAP_Osd_FillBox(rgn, FavNew_X+FAVNEW_PADDING, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item), FavNew_Option_W, FavNew_Option_H, bc);
			TAP_Osd_FillBox(rgn, FavNew_X+FAVNEW_PADDING+FAVNEW_GAP+90, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item), FAVNEW_GAP, FavNew_Option_H, FavNew_BaseColour);
			switch( fntSize ){
				case FNT_Size_1419:
					Font_Osd_PutString1419( rgn, FavNew_X+FAVNEW_PADDING+FAVNEW_GAP, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item)+pad_y, FavNew_X+FAVNEW_PADDING+FAVNEW_GAP+90, "Day", fc, COLOR_None);
					Font_Osd_PutString1419( rgn, FavNew_X+FAVNEW_PADDING+FAVNEW_GAP+100, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item)+pad_y, FavNew_X+FAVNEW_PADDING+FavNew_Option_W-FAVNEW_GAP, FavNew_Item.day, fc, COLOR_None);
					break;
				case FNT_Size_1622:
					Font_Osd_PutString1622( rgn, FavNew_X+FAVNEW_PADDING+FAVNEW_GAP, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item)+pad_y, FavNew_X+FAVNEW_PADDING+FAVNEW_GAP+90, "Day", fc, COLOR_None);
					Font_Osd_PutString1622( rgn, FavNew_X+FAVNEW_PADDING+FAVNEW_GAP+100, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item)+pad_y, FavNew_X+FAVNEW_PADDING+FavNew_Option_W-FAVNEW_GAP, FavNew_Item.day, fc, COLOR_None);
					break;
				case FNT_Size_1926:
					Font_Osd_PutString1926( rgn, FavNew_X+FAVNEW_PADDING+FAVNEW_GAP, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item)+pad_y, FavNew_X+FAVNEW_PADDING+FAVNEW_GAP+90, "Day", fc, COLOR_None);
					Font_Osd_PutString1926( rgn, FavNew_X+FAVNEW_PADDING+FAVNEW_GAP+100, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item)+pad_y, FavNew_X+FAVNEW_PADDING+FavNew_Option_W-FAVNEW_GAP, FavNew_Item.day, fc, COLOR_None);
					break;
			}
			break;
		case FAVNEW_LOCATION:
			if( item == selected ){
				if( FavNew_ItemHigh != NULL ){
					TAP_Osd_RestoreBox( rgn, FavNew_X+FAVNEW_PADDING, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item), FavNew_Option_W, FavNew_Option_H, FavNew_ItemHigh );
				} else {
					TAP_Osd_FillBox(rgn, FavNew_X+FAVNEW_PADDING, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item), FavNew_Option_W, FavNew_Option_H, bc);
				}
			} else {
				if( FavNew_ItemBase != NULL ){
					TAP_Osd_RestoreBox( rgn, FavNew_X+FAVNEW_PADDING, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item), FavNew_Option_W, FavNew_Option_H, FavNew_ItemBase );
				} else {
					TAP_Osd_FillBox(rgn, FavNew_X+FAVNEW_PADDING, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item), FavNew_Option_W, FavNew_Option_H, bc);
				}
			}
//			TAP_Osd_FillBox(rgn, FavNew_X+FAVNEW_PADDING, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item), FavNew_Option_W, FavNew_Option_H, bc);
			TAP_Osd_FillBox(rgn, FavNew_X+FAVNEW_PADDING+FAVNEW_GAP+90, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item), FAVNEW_GAP, FavNew_Option_H, FavNew_BaseColour);
			switch( fntSize ){
				case FNT_Size_1419:
					Font_Osd_PutString1419( rgn, FavNew_X+FAVNEW_PADDING+FAVNEW_GAP, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item)+pad_y, FavNew_X+FAVNEW_PADDING+FAVNEW_GAP+90, "Location", fc, COLOR_None);
					Font_Osd_PutString1419( rgn, FavNew_X+FAVNEW_PADDING+FAVNEW_GAP+100, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item)+pad_y, FavNew_X+FAVNEW_PADDING+FavNew_Option_W-FAVNEW_GAP, FavManage_LocationText( FavNew_Item.location ), fc, COLOR_None);
					break;
				case FNT_Size_1622:
					Font_Osd_PutString1622( rgn, FavNew_X+FAVNEW_PADDING+FAVNEW_GAP, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item)+pad_y, FavNew_X+FAVNEW_PADDING+FAVNEW_GAP+90, "Location", fc, COLOR_None);
					Font_Osd_PutString1622( rgn, FavNew_X+FAVNEW_PADDING+FAVNEW_GAP+100, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item)+pad_y, FavNew_X+FAVNEW_PADDING+FavNew_Option_W-FAVNEW_GAP, FavManage_LocationText( FavNew_Item.location ), fc, COLOR_None);
					break;
				case FNT_Size_1926:
					Font_Osd_PutString1926( rgn, FavNew_X+FAVNEW_PADDING+FAVNEW_GAP, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item)+pad_y, FavNew_X+FAVNEW_PADDING+FAVNEW_GAP+90, "Location", fc, COLOR_None);
					Font_Osd_PutString1926( rgn, FavNew_X+FAVNEW_PADDING+FAVNEW_GAP+100, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item)+pad_y, FavNew_X+FAVNEW_PADDING+FavNew_Option_W-FAVNEW_GAP, FavManage_LocationText( FavNew_Item.location ), fc, COLOR_None);
					break;
			}
			break;
		case FAVNEW_START:
			if( item == selected ){
				if( FavNew_ItemHigh != NULL ){
					TAP_Osd_RestoreBox( rgn, FavNew_X+FAVNEW_PADDING, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item), FavNew_Option_W, FavNew_Option_H, FavNew_ItemHigh );
				} else {
					TAP_Osd_FillBox(rgn, FavNew_X+FAVNEW_PADDING, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item), FavNew_Option_W, FavNew_Option_H, bc);
				}
			} else {
				if( FavNew_ItemBase != NULL ){
					TAP_Osd_RestoreBox( rgn, FavNew_X+FAVNEW_PADDING, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item), FavNew_Option_W, FavNew_Option_H, FavNew_ItemBase );
				} else {
					TAP_Osd_FillBox(rgn, FavNew_X+FAVNEW_PADDING, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item), FavNew_Option_W, FavNew_Option_H, bc);
				}
			}
//			TAP_Osd_FillBox(rgn, FavNew_X+FAVNEW_PADDING, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item), FavNew_Option_W, FavNew_Option_H, bc);
			TAP_Osd_FillBox(rgn, FavNew_X+FAVNEW_PADDING+FAVNEW_GAP+90, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item), FAVNEW_GAP, FavNew_Option_H, FavNew_BaseColour);			
			switch( fntSize ){
				case FNT_Size_1419:
					Font_Osd_PutString1419( rgn, FavNew_X+FAVNEW_PADDING+FAVNEW_GAP, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item)+pad_y, FavNew_X+FAVNEW_PADDING+FAVNEW_GAP+90, "Min Start", fc, COLOR_None);
					Font_Osd_PutString1419( rgn, FavNew_X+FAVNEW_PADDING+FAVNEW_GAP+100, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item)+pad_y, FavNew_X+FAVNEW_PADDING+FavNew_Option_W-FAVNEW_GAP, MinsToHHMM(FavNew_Item.start), fc, COLOR_None);
					break;
				case FNT_Size_1622:
					Font_Osd_PutString1622( rgn, FavNew_X+FAVNEW_PADDING+FAVNEW_GAP, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item)+pad_y, FavNew_X+FAVNEW_PADDING+FAVNEW_GAP+90, "Min Start", fc, COLOR_None);
					Font_Osd_PutString1622( rgn, FavNew_X+FAVNEW_PADDING+FAVNEW_GAP+100, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item)+pad_y, FavNew_X+FAVNEW_PADDING+FavNew_Option_W-FAVNEW_GAP, MinsToHHMM(FavNew_Item.start), fc, COLOR_None);
					break;
				case FNT_Size_1926:
					Font_Osd_PutString1926( rgn, FavNew_X+FAVNEW_PADDING+FAVNEW_GAP, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item)+pad_y, FavNew_X+FAVNEW_PADDING+FAVNEW_GAP+90, "Min Start", fc, COLOR_None);
					Font_Osd_PutString1926( rgn, FavNew_X+FAVNEW_PADDING+FAVNEW_GAP+100, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item)+pad_y, FavNew_X+FAVNEW_PADDING+FavNew_Option_W-FAVNEW_GAP, MinsToHHMM(FavNew_Item.start), fc, COLOR_None);
					break;
			}
			break;
		case FAVNEW_END:
			if( item == selected ){
				if( FavNew_ItemHigh != NULL ){
					TAP_Osd_RestoreBox( rgn, FavNew_X+FAVNEW_PADDING, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item), FavNew_Option_W, FavNew_Option_H, FavNew_ItemHigh );
				} else {
					TAP_Osd_FillBox(rgn, FavNew_X+FAVNEW_PADDING, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item), FavNew_Option_W, FavNew_Option_H, bc);
				}
			} else {
				if( FavNew_ItemBase != NULL ){
					TAP_Osd_RestoreBox( rgn, FavNew_X+FAVNEW_PADDING, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item), FavNew_Option_W, FavNew_Option_H, FavNew_ItemBase );
				} else {
					TAP_Osd_FillBox(rgn, FavNew_X+FAVNEW_PADDING, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item), FavNew_Option_W, FavNew_Option_H, bc);
				}
			}
//			TAP_Osd_FillBox(rgn, FavNew_X+FAVNEW_PADDING, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item), FavNew_Option_W, FavNew_Option_H, bc);
			TAP_Osd_FillBox(rgn, FavNew_X+FAVNEW_PADDING+FAVNEW_GAP+90, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item), FAVNEW_GAP, FavNew_Option_H, FavNew_BaseColour);			
			switch( fntSize ){
				case FNT_Size_1419:
					Font_Osd_PutString1419( rgn, FavNew_X+FAVNEW_PADDING+FAVNEW_GAP, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item)+pad_y, FavNew_X+FAVNEW_PADDING+FAVNEW_GAP+90, "Max Start", fc, COLOR_None);
					Font_Osd_PutString1419( rgn, FavNew_X+FAVNEW_PADDING+FAVNEW_GAP+100, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item)+pad_y, FavNew_X+FAVNEW_PADDING+FavNew_Option_W-FAVNEW_GAP, MinsToHHMM(FavNew_Item.end), fc, COLOR_None);
					break;
				case FNT_Size_1622:
					Font_Osd_PutString1622( rgn, FavNew_X+FAVNEW_PADDING+FAVNEW_GAP, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item)+pad_y, FavNew_X+FAVNEW_PADDING+FAVNEW_GAP+90, "Max Start", fc, COLOR_None);
					Font_Osd_PutString1622( rgn, FavNew_X+FAVNEW_PADDING+FAVNEW_GAP+100, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item)+pad_y, FavNew_X+FAVNEW_PADDING+FavNew_Option_W-FAVNEW_GAP, MinsToHHMM(FavNew_Item.end), fc, COLOR_None);
					break;
				case FNT_Size_1926:
					Font_Osd_PutString1926( rgn, FavNew_X+FAVNEW_PADDING+FAVNEW_GAP, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item)+pad_y, FavNew_X+FAVNEW_PADDING+FAVNEW_GAP+90, "Max Start", fc, COLOR_None);
					Font_Osd_PutString1926( rgn, FavNew_X+FAVNEW_PADDING+FAVNEW_GAP+100, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item)+pad_y, FavNew_X+FAVNEW_PADDING+FavNew_Option_W-FAVNEW_GAP, MinsToHHMM(FavNew_Item.end), fc, COLOR_None);
					break;
			}
			break;
		case FAVNEW_PRIORITY:
			if( item == selected ){
				if( FavNew_ItemHigh != NULL ){
					TAP_Osd_RestoreBox( rgn, FavNew_X+FAVNEW_PADDING, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item), FavNew_Option_W, FavNew_Option_H, FavNew_ItemHigh );
				} else {
					TAP_Osd_FillBox(rgn, FavNew_X+FAVNEW_PADDING, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item), FavNew_Option_W, FavNew_Option_H, bc);
				}
			} else {
				if( FavNew_ItemBase != NULL ){
					TAP_Osd_RestoreBox( rgn, FavNew_X+FAVNEW_PADDING, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item), FavNew_Option_W, FavNew_Option_H, FavNew_ItemBase );
				} else {
					TAP_Osd_FillBox(rgn, FavNew_X+FAVNEW_PADDING, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item), FavNew_Option_W, FavNew_Option_H, bc);
				}
			}
//			TAP_Osd_FillBox(rgn, FavNew_X+FAVNEW_PADDING, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item), FavNew_Option_W, FavNew_Option_H, bc);
			TAP_Osd_FillBox(rgn, FavNew_X+FAVNEW_PADDING+FAVNEW_GAP+90, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item), FAVNEW_GAP, FavNew_Option_H, FavNew_BaseColour);			
			switch( fntSize ){
				case FNT_Size_1419:
					Font_Osd_PutString1419( rgn, FavNew_X+FAVNEW_PADDING+FAVNEW_GAP, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item)+pad_y, FavNew_X+FAVNEW_PADDING+FAVNEW_GAP+90, "Priority", fc, COLOR_None);
					Font_Osd_PutString1419( rgn, FavNew_X+FAVNEW_PADDING+FAVNEW_GAP+100, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item)+pad_y, FavNew_X+FAVNEW_PADDING+FavNew_Option_W-FAVNEW_GAP, NumberToString(FavNew_Item.priority), fc, COLOR_None);
					break;
				case FNT_Size_1622:
					Font_Osd_PutString1622( rgn, FavNew_X+FAVNEW_PADDING+FAVNEW_GAP, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item)+pad_y, FavNew_X+FAVNEW_PADDING+FAVNEW_GAP+90, "Priority", fc, COLOR_None);
					Font_Osd_PutString1622( rgn, FavNew_X+FAVNEW_PADDING+FAVNEW_GAP+100, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item)+pad_y, FavNew_X+FAVNEW_PADDING+FavNew_Option_W-FAVNEW_GAP, NumberToString(FavNew_Item.priority), fc, COLOR_None);
					break;
				case FNT_Size_1926:
					Font_Osd_PutString1926( rgn, FavNew_X+FAVNEW_PADDING+FAVNEW_GAP, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item)+pad_y, FavNew_X+FAVNEW_PADDING+FAVNEW_GAP+90, "Priority", fc, COLOR_None);
					Font_Osd_PutString1926( rgn, FavNew_X+FAVNEW_PADDING+FAVNEW_GAP+100, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item)+pad_y, FavNew_X+FAVNEW_PADDING+FavNew_Option_W-FAVNEW_GAP, NumberToString(FavNew_Item.priority), fc, COLOR_None);
					break;
			}
			break;		
		case FAVNEW_RPT:
			if( item == selected ){
				if( FavNew_ItemHigh != NULL ){
					TAP_Osd_RestoreBox( rgn, FavNew_X+FAVNEW_PADDING, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item), FavNew_Option_W, FavNew_Option_H, FavNew_ItemHigh );
				} else {
					TAP_Osd_FillBox(rgn, FavNew_X+FAVNEW_PADDING, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item), FavNew_Option_W, FavNew_Option_H, bc);
				}
			} else {
				if( FavNew_ItemBase != NULL ){
					TAP_Osd_RestoreBox( rgn, FavNew_X+FAVNEW_PADDING, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item), FavNew_Option_W, FavNew_Option_H, FavNew_ItemBase );
				} else {
					TAP_Osd_FillBox(rgn, FavNew_X+FAVNEW_PADDING, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item), FavNew_Option_W, FavNew_Option_H, bc);
				}
			}
//			TAP_Osd_FillBox(rgn, FavNew_X+FAVNEW_PADDING, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item), FavNew_Option_W, FavNew_Option_H, bc);
			TAP_Osd_FillBox(rgn, FavNew_X+FAVNEW_PADDING+FAVNEW_GAP+90, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item), FAVNEW_GAP, FavNew_Option_H, FavNew_BaseColour);			
			switch( fntSize ){
				case FNT_Size_1419:
					Font_Osd_PutString1419( rgn, FavNew_X+FAVNEW_PADDING+FAVNEW_GAP, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item)+pad_y, FavNew_X+FAVNEW_PADDING+FAVNEW_GAP+90, "Repeats", fc, COLOR_None);
					Font_Osd_PutString1419( rgn, FavNew_X+FAVNEW_PADDING+FAVNEW_GAP+100, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item)+pad_y, FavNew_X+FAVNEW_PADDING+FavNew_Option_W-FAVNEW_GAP, FavNew_Item.rpt==0?"Yes":"No", fc, COLOR_None);
					break;
				case FNT_Size_1622:
					Font_Osd_PutString1622( rgn, FavNew_X+FAVNEW_PADDING+FAVNEW_GAP, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item)+pad_y, FavNew_X+FAVNEW_PADDING+FAVNEW_GAP+90, "Repeats", fc, COLOR_None);
					Font_Osd_PutString1622( rgn, FavNew_X+FAVNEW_PADDING+FAVNEW_GAP+100, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item)+pad_y, FavNew_X+FAVNEW_PADDING+FavNew_Option_W-FAVNEW_GAP, FavNew_Item.rpt==0?"Yes":"No", fc, COLOR_None);
					break;
				case FNT_Size_1926:
					Font_Osd_PutString1926( rgn, FavNew_X+FAVNEW_PADDING+FAVNEW_GAP, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item)+pad_y, FavNew_X+FAVNEW_PADDING+FAVNEW_GAP+90, "Repeats", fc, COLOR_None);
					Font_Osd_PutString1926( rgn, FavNew_X+FAVNEW_PADDING+FAVNEW_GAP+100, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item)+pad_y, FavNew_X+FAVNEW_PADDING+FavNew_Option_W-FAVNEW_GAP, FavNew_Item.rpt==0?"Yes":"No", fc, COLOR_None);
					break;
			}
			break;
		case FAVNEW_ADJ:
			if( item == selected ){
				if( FavNew_ItemHigh != NULL ){
					TAP_Osd_RestoreBox( rgn, FavNew_X+FAVNEW_PADDING, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item), FavNew_Option_W, FavNew_Option_H, FavNew_ItemHigh );
				} else {
					TAP_Osd_FillBox(rgn, FavNew_X+FAVNEW_PADDING, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item), FavNew_Option_W, FavNew_Option_H, bc);
				}
			} else {
				if( FavNew_ItemBase != NULL ){
					TAP_Osd_RestoreBox( rgn, FavNew_X+FAVNEW_PADDING, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item), FavNew_Option_W, FavNew_Option_H, FavNew_ItemBase );
				} else {
					TAP_Osd_FillBox(rgn, FavNew_X+FAVNEW_PADDING, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item), FavNew_Option_W, FavNew_Option_H, bc);
				}
			}
//			TAP_Osd_FillBox(rgn, FavNew_X+FAVNEW_PADDING, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item), FavNew_Option_W, FavNew_Option_H, bc);
			TAP_Osd_FillBox(rgn, FavNew_X+FAVNEW_PADDING+FAVNEW_GAP+90, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item), FAVNEW_GAP, FavNew_Option_H, FavNew_BaseColour);			
			switch( fntSize ){
				case FNT_Size_1419:
					Font_Osd_PutString1419( rgn, FavNew_X+FAVNEW_PADDING+FAVNEW_GAP, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item)+pad_y, FavNew_X+FAVNEW_PADDING+FAVNEW_GAP+90, "Adjust", fc, COLOR_None);
					Font_Osd_PutString1419( rgn, FavNew_X+FAVNEW_PADDING+FAVNEW_GAP+100, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item)+pad_y, FavNew_X+FAVNEW_PADDING+FavNew_Option_W-FAVNEW_GAP, FavNew_Item.adj==1?"Yes":"No", fc, COLOR_None);
					break;
				case FNT_Size_1622:
					Font_Osd_PutString1622( rgn, FavNew_X+FAVNEW_PADDING+FAVNEW_GAP, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item)+pad_y, FavNew_X+FAVNEW_PADDING+FAVNEW_GAP+90, "Adjust", fc, COLOR_None);
					Font_Osd_PutString1622( rgn, FavNew_X+FAVNEW_PADDING+FAVNEW_GAP+100, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item)+pad_y, FavNew_X+FAVNEW_PADDING+FavNew_Option_W-FAVNEW_GAP, FavNew_Item.adj==1?"Yes":"No", fc, COLOR_None);
					break;
				case FNT_Size_1926:
					Font_Osd_PutString1926( rgn, FavNew_X+FAVNEW_PADDING+FAVNEW_GAP, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item)+pad_y, FavNew_X+FAVNEW_PADDING+FAVNEW_GAP+90, "Adjust", fc, COLOR_None);
					Font_Osd_PutString1926( rgn, FavNew_X+FAVNEW_PADDING+FAVNEW_GAP+100, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item)+pad_y, FavNew_X+FAVNEW_PADDING+FavNew_Option_W-FAVNEW_GAP, FavNew_Item.adj==1?"Yes":"No", fc, COLOR_None);
					break;
			}
			break;
		case FAVNEW_SELECTION:
			if( FavNew_ItemBase != NULL ){
				TAP_Osd_RestoreBox( rgn, FavNew_X+FAVNEW_PADDING, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item), FavNew_Option_W, FavNew_Option_H, FavNew_ItemBase );
			} else {
				TAP_Osd_FillBox(rgn, FavNew_X+FAVNEW_PADDING, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item), FavNew_Option_W, FavNew_Option_H, bc);
			}
			TAP_Osd_FillBox(rgn, FavNew_X+FAVNEW_PADDING+(FavNew_Option_W/2)-(FAVNEW_GAP/2), FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item), FAVNEW_GAP-1, FavNew_Option_H, FavNew_BaseColour);
			if( item == selected ){
				if( FavNew_OptionSelected == FAVNEW_OK ){
					if( FavNew_OkItemHigh != NULL ){
						TAP_Osd_RestoreBox( rgn, FavNew_X+FAVNEW_PADDING, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item), (FavNew_Option_W/2)-(FAVNEW_GAP/2)-1, FavNew_Option_H, FavNew_OkItemHigh );
					} else {
						TAP_Osd_FillBox(rgn, FavNew_X+FAVNEW_PADDING, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item), (FavNew_Option_W/2)-(FAVNEW_GAP/2), FavNew_Option_H, FavNew_HghBc);
					}
				} else {
					if( FavNew_OkItemHigh != NULL ){
						TAP_Osd_RestoreBox( rgn, FavNew_X+FAVNEW_PADDING+(FavNew_Option_W/2)+(FAVNEW_GAP/2), FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item), (FavNew_Option_W/2)-(FAVNEW_GAP/2)-1, FavNew_Option_H, FavNew_CancelItemHigh );
					} else {
						TAP_Osd_FillBox(rgn, FavNew_X+FAVNEW_PADDING+(FavNew_Option_W/2)+(FAVNEW_GAP/2), FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item), (FavNew_Option_W/2)-(FAVNEW_GAP/2)-1, FavNew_Option_H, FavNew_HghBc);
					}
				}
				switch( fntSize ){
					case FNT_Size_1419:
						Font_Osd_PutString1419( rgn, FavNew_X+FAVNEW_PADDING+FAVNEW_GAP, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item)+pad_y, FavNew_X+FAVNEW_PADDING+FavNew_Option_W-FAVNEW_GAP, "OK", fc, COLOR_None);
						break;
					case FNT_Size_1622:
						Font_Osd_PutString1622( rgn, FavNew_X+FAVNEW_PADDING+FAVNEW_GAP, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item)+pad_y, FavNew_X+FAVNEW_PADDING+FavNew_Option_W-FAVNEW_GAP, "OK", fc, COLOR_None);
						break;
					case FNT_Size_1926:
						Font_Osd_PutString1926( rgn, FavNew_X+FAVNEW_PADDING+FAVNEW_GAP, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item)+pad_y, FavNew_X+FAVNEW_PADDING+FavNew_Option_W-FAVNEW_GAP, "OK", fc, COLOR_None);
						break;
				}
				switch( fntSize ){
					case FNT_Size_1419:
						Font_Osd_PutString1419( rgn, FavNew_X+FAVNEW_PADDING+FAVNEW_GAP+(FavNew_Option_W/2)+(FAVNEW_GAP/2), FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item)+pad_y, FavNew_X+FAVNEW_PADDING+FavNew_Option_W-FAVNEW_GAP, "Cancel", fc, COLOR_None);
						break;
					case FNT_Size_1622:
						Font_Osd_PutString1622( rgn, FavNew_X+FAVNEW_PADDING+FAVNEW_GAP+(FavNew_Option_W/2)+(FAVNEW_GAP/2), FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item)+pad_y, FavNew_X+FAVNEW_PADDING+FavNew_Option_W-FAVNEW_GAP, "Cancel", fc, COLOR_None);
						break;
					case FNT_Size_1926:
						Font_Osd_PutString1926( rgn, FavNew_X+FAVNEW_PADDING+FAVNEW_GAP+(FavNew_Option_W/2)+(FAVNEW_GAP/2), FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item)+pad_y, FavNew_X+FAVNEW_PADDING+FavNew_Option_W-FAVNEW_GAP, "Cancel", fc, COLOR_None);
						break;
				}	
			} else {
//				TAP_Osd_FillBox(rgn, FavNew_X+FAVNEW_PADDING, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item), (FavNew_Option_W/2)-(FAVNEW_GAP/2)-1, FavNew_Option_H, FavNew_Bc);
//				TAP_Osd_FillBox(rgn, FavNew_X+FAVNEW_PADDING+(FavNew_Option_W/2)+(FAVNEW_GAP/2), FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item), (FavNew_Option_W/2)-(FAVNEW_GAP/2)-1, FavNew_Option_H, FavNew_Bc);
				switch( fntSize ){
					case FNT_Size_1419:
						Font_Osd_PutString1419( rgn, FavNew_X+FAVNEW_PADDING+FAVNEW_GAP, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item)+pad_y, FavNew_X+FAVNEW_PADDING+FavNew_Option_W-FAVNEW_GAP, "OK", fc, COLOR_None);
						break;
					case FNT_Size_1622:
						Font_Osd_PutString1622( rgn, FavNew_X+FAVNEW_PADDING+FAVNEW_GAP, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item)+pad_y, FavNew_X+FAVNEW_PADDING+FavNew_Option_W-FAVNEW_GAP, "OK", fc, COLOR_None);
						break;
					case FNT_Size_1926:
						Font_Osd_PutString1926( rgn, FavNew_X+FAVNEW_PADDING+FAVNEW_GAP, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item)+pad_y, FavNew_X+FAVNEW_PADDING+FavNew_Option_W-FAVNEW_GAP, "OK", fc, COLOR_None);
						break;
				}
				switch( fntSize ){
					case FNT_Size_1419:
						Font_Osd_PutString1419( rgn, FavNew_X+FAVNEW_PADDING+FAVNEW_GAP+(FavNew_Option_W/2)+(FAVNEW_GAP/2), FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item)+pad_y, FavNew_X+FAVNEW_PADDING+FavNew_Option_W-FAVNEW_GAP, "Cancel", fc, COLOR_None);
						break;
					case FNT_Size_1622:
						Font_Osd_PutString1622( rgn, FavNew_X+FAVNEW_PADDING+FAVNEW_GAP+(FavNew_Option_W/2)+(FAVNEW_GAP/2), FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item)+pad_y, FavNew_X+FAVNEW_PADDING+FavNew_Option_W-FAVNEW_GAP, "Cancel", fc, COLOR_None);
						break;
					case FNT_Size_1926:
						Font_Osd_PutString1926( rgn, FavNew_X+FAVNEW_PADDING+FAVNEW_GAP+(FavNew_Option_W/2)+(FAVNEW_GAP/2), FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item)+pad_y, FavNew_X+FAVNEW_PADDING+FavNew_Option_W-FAVNEW_GAP, "Cancel", fc, COLOR_None);
						break;
				}	
			}
			break;
		case FAVNEW_HELP:
			if( FavNew_Option_W > 250 ){
				TAP_Osd_FillBox(rgn, FavNew_X+FAVNEW_PADDING, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item), FavNew_Option_W, FavNew_Option_H, FavNew_BaseColour);
				TAP_Osd_PutGd( rgn, FavNew_X+FAVNEW_PADDING+FAVNEW_GAP, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item)+pad_y, &_okGd, TRUE );	// Draw highlighted
				Font_Osd_PutString1419( rgn, FavNew_X+FAVNEW_PADDING+FAVNEW_GAP+30, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item)+pad_y, FavNew_X+FAVNEW_PADDING+FAVNEW_GAP+90, "Edit Text", fc, COLOR_None);
				TAP_Osd_PutGd( rgn, FavNew_X+FAVNEW_PADDING+FAVNEW_GAP+100, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item)+pad_y, &_rewindGd, TRUE );	// Draw highlighted
				TAP_Osd_PutGd( rgn, FavNew_X+FAVNEW_PADDING+FAVNEW_GAP+100+23, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item)+pad_y, &_forwardGd, TRUE );	// Draw highlighted
				Font_Osd_PutString1419( rgn, FavNew_X+FAVNEW_PADDING+FAVNEW_GAP+100+50, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item)+pad_y, FavNew_X+FAVNEW_PADDING+FAVNEW_GAP+130+90, "+/- Hour", fc, COLOR_None);
				TAP_Osd_PutGd( rgn, FavNew_X+FAVNEW_PADDING+FAVNEW_GAP+215, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item)+pad_y, &_voldownGd, TRUE );	// Draw highlighted
				TAP_Osd_PutGd( rgn, FavNew_X+FAVNEW_PADDING+FAVNEW_GAP+215+23, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item)+pad_y, &_volupGd, TRUE );	// Draw highlighted
				Font_Osd_PutString1419( rgn, FavNew_X+FAVNEW_PADDING+FAVNEW_GAP+215+50, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*item)+pad_y, FavNew_X+FAVNEW_PADDING+FAVNEW_GAP+225+90, "Adjust", fc, COLOR_None);
			}
			break;
	}
}

void FavNew_KeyboardClose( char * text, bool changed ){
	TAP_Osd_Copy( FavNew_RGN, FavNew_MemRGN, 0, 0, 720, 576, 0, 0, FALSE);	// Copy the main region to memory to avoid corruption from the previous OSD
	if( changed ){
		switch( FavNew_Selected ){
			case FAVNEW_TERM:
				memset( FavNew_Item.term, '\0', sizeof(FavNew_Item.term) ); // Blank the filename field
				strncpy(FavNew_Item.term, text, sizeof(FavNew_Item.term));
				break;
			case FAVNEW_CHANNEL:
				memset( FavNew_Item.channel, '\0', sizeof(FavNew_Item.channel) ); // Blank the filename field
				strncpy(FavNew_Item.channel, text, sizeof(FavNew_Item.channel));
				break;
			case FAVNEW_DAY:
				memset( FavNew_Item.day, '\0', sizeof(FavNew_Item.day) ); // Blank the filename field
				strncpy(FavNew_Item.day, text, sizeof(FavNew_Item.day));
				break;
		}
		// We have change the text of the timer so modify the fileName field
		FavNew_DrawItem( FavNew_RGN, FavNew_Selected, FavNew_Selected );
	}
}

// Function to draw the timer details to the region
void FavNew_DrawDetails( word rgn ){
	int i;
	for( i = 0; i < FAVNEW_OPTIONS; i++ ){
		FavNew_DrawItem( rgn, i, FavNew_Selected );
	}
}

dword FavNew_KeyHandler( dword param1 ){
	if( Keyboard_IsActive() ){	// Handle the keyboard
		return Keyboard_Keyhandler(param1);
	}
	if( param1 == RKEY_Exit ){
		FavNew_Deactivate(TRUE);
		return 0;
	}
	// Do the navigation stuff now
	if( param1 == RKEY_ChUp ){
		FavNew_PrevSelected = FavNew_Selected;
		FavNew_Selected--;
		if( FavNew_Selected < 1 ){
			FavNew_Selected = FAVNEW_HELP-1;
		}
		FavNew_DrawItem( FavNew_MemRGN, FavNew_PrevSelected, FavNew_Selected );
		FavNew_DrawItem( FavNew_MemRGN, FavNew_Selected, FavNew_Selected );
		TAP_Osd_Copy( FavNew_MemRGN, FavNew_RGN, FavNew_X+FAVNEW_PADDING, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*FavNew_PrevSelected), FavNew_Option_W, FavNew_Option_H, FavNew_X+FAVNEW_PADDING, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*FavNew_PrevSelected), FALSE);
		TAP_Osd_Copy( FavNew_MemRGN, FavNew_RGN, FavNew_X+FAVNEW_PADDING, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*FavNew_Selected), FavNew_Option_W, FavNew_Option_H, FavNew_X+FAVNEW_PADDING, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*FavNew_Selected), FALSE);
	}
	// Do the navigation stuff now
	if( param1 == RKEY_ChDown ){
		FavNew_PrevSelected = FavNew_Selected;
		FavNew_Selected++;
		if( FavNew_Selected > FAVNEW_HELP-1 ){
			FavNew_Selected = 1;
		}
		FavNew_DrawItem( FavNew_MemRGN, FavNew_PrevSelected, FavNew_Selected );
		FavNew_DrawItem( FavNew_MemRGN, FavNew_Selected, FavNew_Selected );
		TAP_Osd_Copy( FavNew_MemRGN, FavNew_RGN, FavNew_X+FAVNEW_PADDING, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*FavNew_PrevSelected), FavNew_Option_W, FavNew_Option_H, FavNew_X+FAVNEW_PADDING, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*FavNew_PrevSelected), FALSE);
		TAP_Osd_Copy( FavNew_MemRGN, FavNew_RGN, FavNew_X+FAVNEW_PADDING, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*FavNew_Selected), FavNew_Option_W, FavNew_Option_H, FavNew_X+FAVNEW_PADDING, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*FavNew_Selected), FALSE);
	}
	// Do the navigation stuff now
	if( param1 == RKEY_VolDown || param1 == RKEY_VolUp ){
		switch( FavNew_Selected ){
			case FAVNEW_START:
				if( param1 == RKEY_VolUp ){
					FavNew_Item.start++;
				} else {
					FavNew_Item.start--;
				}
				if( FavNew_Item.start < -1 ){
					FavNew_Item.start = -1;
				}
				if( FavNew_Item.start > 1439 ){
					FavNew_Item.start = 1439;
				}
				break;
			case FAVNEW_END:
				if( param1 == RKEY_VolUp ){
					FavNew_Item.end++;
				} else {
					FavNew_Item.end--;
				}
				if( FavNew_Item.end < -1 ){
					FavNew_Item.end = -1;
				}
				if( FavNew_Item.end > 1439 ){
					FavNew_Item.end = 1439;
				}
				break;
			case FAVNEW_PRIORITY:
				if( param1 == RKEY_VolUp ){
					FavNew_Item.priority++;
				} else {
					FavNew_Item.priority--;
				}
				if( FavNew_Item.priority < 1 ){
					FavNew_Item.priority = 1;
				}
				if( FavNew_Item.priority > 10 ){
					FavNew_Item.priority = 10;
				}
				break;
			case FAVNEW_RPT:
				if( param1 == RKEY_VolUp ){
					FavNew_Item.rpt++;
				} else {
					FavNew_Item.rpt--;
				}
				if( FavNew_Item.rpt < 0 ){
					FavNew_Item.rpt = 1;
				}
				if( FavNew_Item.rpt > 1 ){
					FavNew_Item.rpt = 0;
				}
				break;
			case FAVNEW_ADJ:
				if( param1 == RKEY_VolUp ){
					FavNew_Item.adj++;
				} else {
					FavNew_Item.adj--;
				}
				if( FavNew_Item.adj < 0 ){
					FavNew_Item.adj = 1;
				}
				if( FavNew_Item.adj > 1 ){
					FavNew_Item.adj = 0;
				}
				break;
			case FAVNEW_SELECTION:
				FavNew_OptionSelected++;
				if( FavNew_OptionSelected > 1 ){
					FavNew_OptionSelected  = 0;
				}
				break;
			case FAVNEW_LOCATION:
				if( param1 == RKEY_VolUp ){
					FavNew_Item.location++;
				} else {
					FavNew_Item.location--;
				}
				if( FavNew_Item.location < TITLEEXACT ){
					FavNew_Item.location = ALL;
				}
				if( FavNew_Item.location > ALL ){
					FavNew_Item.location = TITLEEXACT;
				}
				break;
		}
		FavNew_DrawItem( FavNew_MemRGN, FavNew_Selected, FavNew_Selected );
		TAP_Osd_Copy( FavNew_MemRGN, FavNew_RGN, FavNew_X+FAVNEW_PADDING, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*FavNew_Selected), FavNew_Option_W, FavNew_Option_H, FavNew_X+FAVNEW_PADDING, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*FavNew_Selected), FALSE);
	}
	if( param1 == RKEY_Ok ){
		switch( FavNew_Selected ){
			case FAVNEW_TERM:
				Keyboard_Init( FavNew_RGN, FavNew_MemRGN,  FavNew_X, FavNew_Y, FavNew_W, FavNew_H, "Favourites Term" );	// Initalise the keyboard
				Keyboard_SetColours( DISPLAY_KEYBOARDITEM, DISPLAY_KEYBOARDITEMTEXT, DISPLAY_KEYBOARDITEMSELECTED, DISPLAY_KEYBOARDITEMSELECTEDTEXT, DISPLAY_KEYBOARDFIELDTEXT, DISPLAY_KEYBOARDFIELD, DISPLAY_KEYBOARDBACKGROUND );
				if( Keyboard_Activate(FavNew_Item.term,&FavNew_KeyboardClose) ){
				}
				break;
			case FAVNEW_CHANNEL:
				Keyboard_Init( FavNew_RGN, FavNew_MemRGN,  FavNew_X, FavNew_Y, FavNew_W, FavNew_H, "Favourites Channel" );	// Initalise the keyboard
				Keyboard_SetColours( DISPLAY_KEYBOARDITEM, DISPLAY_KEYBOARDITEMTEXT, DISPLAY_KEYBOARDITEMSELECTED, DISPLAY_KEYBOARDITEMSELECTEDTEXT, DISPLAY_KEYBOARDFIELDTEXT, DISPLAY_KEYBOARDFIELD, DISPLAY_KEYBOARDBACKGROUND );
				if( Keyboard_Activate(FavNew_Item.channel,&FavNew_KeyboardClose) ){
				}
				break;
			case FAVNEW_DAY:
				Keyboard_Init( FavNew_RGN, FavNew_MemRGN,  FavNew_X, FavNew_Y, FavNew_W, FavNew_H, "Favourites Day" );	// Initalise the keyboard
				Keyboard_SetColours( DISPLAY_KEYBOARDITEM, DISPLAY_KEYBOARDITEMTEXT, DISPLAY_KEYBOARDITEMSELECTED, DISPLAY_KEYBOARDITEMSELECTEDTEXT, DISPLAY_KEYBOARDFIELDTEXT, DISPLAY_KEYBOARDFIELD, DISPLAY_KEYBOARDBACKGROUND );
				if( Keyboard_Activate(FavNew_Item.day,&FavNew_KeyboardClose) ){
				}
				break;
			case FAVNEW_SELECTION:
				if( FavNew_OptionSelected == FAVNEW_OK ){
					FavNew_Deactivate(FALSE);
				} else {
					FavNew_Deactivate(TRUE);
				}
				break;
		}
	}
	if( param1 == RKEY_0 ){
		switch( FavNew_Selected ){
			case FAVNEW_START:
				FavNew_Item.start = -1;
				break;
			case FAVNEW_END:
				FavNew_Item.end = -1;
				break;
			case FAVNEW_PRIORITY:
				FavNew_Item.priority = 1;
				break;
		}
		FavNew_DrawItem( FavNew_MemRGN, FavNew_Selected, FavNew_Selected );
		TAP_Osd_Copy( FavNew_MemRGN, FavNew_RGN, FavNew_X+FAVNEW_PADDING, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*FavNew_Selected), FavNew_Option_W, FavNew_Option_H, FavNew_X+FAVNEW_PADDING, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*FavNew_Selected), FALSE);
	}
	if( param1 == RKEY_Forward || param1 == RKEY_Rewind ){
		switch( FavNew_Selected ){
			case FAVNEW_START:
				if( param1 == RKEY_Forward ){
					FavNew_Item.start+=60;
				} else {
					FavNew_Item.start-=60;
				}
				if( FavNew_Item.start < -1 ){
					FavNew_Item.start = -1;
				}
				if( FavNew_Item.start > 1439 ){
					FavNew_Item.start = 1439;
				}
				break;
			case FAVNEW_END:
				if( param1 == RKEY_Forward ){
					FavNew_Item.end+=60;
				} else {
					FavNew_Item.end-=60;
				}
				if( FavNew_Item.end < -1 ){
					FavNew_Item.end = -1;
				}
				if( FavNew_Item.end > 1439 ){
					FavNew_Item.end = 1439;
				}
				break;
		}
		FavNew_DrawItem( FavNew_MemRGN, FavNew_Selected, FavNew_Selected );
		TAP_Osd_Copy( FavNew_MemRGN, FavNew_RGN, FavNew_X+FAVNEW_PADDING, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*FavNew_Selected), FavNew_Option_W, FavNew_Option_H, FavNew_X+FAVNEW_PADDING, FavNew_Y+FAVNEW_GAP+((FavNew_Option_H+FAVNEW_GAP)*FavNew_Selected), FALSE);
		return 0;
	}
	return 0;
}

// Function to set the colours of the Timer window
void FavNew_SetColours( dword fc, dword bc, dword fhc, dword bhc, dword base ){
	FavNew_Fc = fc;
	FavNew_HghBc = bhc;
	FavNew_HghFc = fhc;
	FavNew_Bc = bc;
	FavNew_BaseColour = base;
}

