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

#include "favedit.h"
#include "fontman.h"
#include "skin.h"
#include "settings.h"
#include "favmanage.h"
#include "keyboard.h"

// Desc: Routine to return whether the favourite editing dialog is active
// OUT:
//      bool - Activate status

bool FavEdit_IsActive( void ){
	return FavEdit_Active;
}

// Desc: Routine to initalise the favourite edit dialog
// IN:
//      rgn - Visible region to render to
//      memrgn - Memory region to render to
//      x - Start position for dialog
//      y - Start position for dialog
//      w - Width of dialog
//      h - Height og dialog


void FavEdit_Init( word rgn, word memrgn, int x, int y, int w, int h ){
	FavEdit_RGN = rgn;	// Store the region to render on
	FavEdit_MemRGN = memrgn;
	FavEdit_X = x;
	FavEdit_Y = y;
	FavEdit_W = w;
	FavEdit_H = h;
	FavEdit_Initalised = TRUE;
	FavEdit_Option_H = (FavEdit_H/FAVEDIT_OPTIONS)-FAVEDIT_GAP;
	FavEdit_H += 5;
	FavEdit_Option_W = FavEdit_W - (FAVEDIT_PADDING*2);
	FavEdit_CreateGradients();	// Create the gradient for the display
}

void FavEdit_MemFree( void ){
	if (FavEdit_ItemBase) TAP_MemFree(FavEdit_ItemBase);
	if (FavEdit_ItemHigh) TAP_MemFree(FavEdit_ItemHigh);
	if (FavEdit_OkItemHigh) TAP_MemFree(FavEdit_OkItemHigh);
	if (FavEdit_CancelItemHigh) TAP_MemFree(FavEdit_CancelItemHigh);
	FavEdit_ItemBase = NULL;
	FavEdit_ItemHigh = NULL;
	FavEdit_OkItemHigh = NULL;
	FavEdit_CancelItemHigh = NULL;
}

void FavEdit_CreateGradients( void ){
	word rgn;
	FavEdit_MemFree();
	if( Settings_GradientFactor > 0 ){
		rgn	= TAP_Osd_Create(0, 0, FavEdit_Option_W, FavEdit_Option_H, 0, OSD_Flag_MemRgn);
		// Main default item
		createGradient(rgn, FavEdit_Option_W, FavEdit_Option_H, Settings_GradientFactor, DISPLAY_ITEM);
		FavEdit_ItemBase = TAP_Osd_SaveBox(rgn, 0, 0, FavEdit_Option_W, FavEdit_Option_H);
		createGradient(rgn, FavEdit_Option_W, FavEdit_Option_H, Settings_GradientFactor, DISPLAY_ITEMSELECTED);
		FavEdit_ItemHigh = TAP_Osd_SaveBox(rgn, 0, 0, FavEdit_Option_W, FavEdit_Option_H);
		FavEdit_CancelItemHigh = TAP_Osd_SaveBox(rgn, 0, 0, (FavEdit_Option_W/2)-(FAVEDIT_GAP/2), FavEdit_Option_H);
		FavEdit_OkItemHigh = TAP_Osd_SaveBox(rgn, 0, 0, (FavEdit_Option_W/2)-(FAVEDIT_GAP/2)-1, FavEdit_Option_H);
		TAP_Osd_Delete(rgn);
	}
}

bool FavEdit_Activate( int row, void (*ReturningProcedure)( favitem, bool ) ){
	if( !FavEdit_Initalised ) return FALSE;	// We haven't initalised so gracefully fail
	FavEdit_Callback = ReturningProcedure;
	FavEdit_Item = *favourites[row];
	FavEdit_Selected = 1;
	FavEdit_PrevSelected = 1;
	FavEdit_OptionSelected = 0;
	// Now lets render the display
	FavEdit_DrawBackground(FavEdit_MemRGN);
	FavEdit_DrawDetails(FavEdit_MemRGN);
	// Copy rendered to foreground
	TAP_Osd_Copy( FavEdit_MemRGN, FavEdit_RGN, FavEdit_X, FavEdit_Y, FavEdit_W, FavEdit_H, FavEdit_X, FavEdit_Y, FALSE);
	FavEdit_Active = TRUE;
	return TRUE;
}

// Routine to handle the closing of the timer window
void FavEdit_Deactivate( bool cancel ){
	FavEdit_Active = FALSE;
	// Restore what we had drawn over
	TAP_Osd_RestoreBox(FavEdit_RGN, FavEdit_X, FavEdit_Y, FavEdit_W, FavEdit_H, FavEdit_Base);
	TAP_MemFree(FavEdit_Base);
	FavEdit_Base = NULL;
	FavEdit_MemFree();
	FavEdit_Callback(FavEdit_Item, cancel);
}


// Here we will just draw the basic layout of the keyboard
void FavEdit_DrawBackground( word rgn ){
	if( FavEdit_Base != NULL ) TAP_MemFree(FavEdit_Base);
	FavEdit_Base = TAP_Osd_SaveBox(FavEdit_RGN, FavEdit_X, FavEdit_Y, FavEdit_W, FavEdit_H);	// Save what is under the Timer screen
	TAP_Osd_FillBox(rgn, FavEdit_X, FavEdit_Y, FavEdit_W, FavEdit_H, FavEdit_BaseColour);
}

void FavEdit_DrawItem( word rgn, int item, int selected ){
	int fntSize = 0;
	int pad_y;
	dword fc = FavEdit_Fc;
	dword bc = FavEdit_Bc;
	// Work out the font size allowed for the height of the option
	if( ( FavEdit_Option_H - 8 ) > 19  ){
		if( ( FavEdit_Option_H - 12 ) > 22 ){
			// Use the big font
			fntSize = FNT_Size_1926;
			pad_y = (FavEdit_Option_H/2)-(26/2);
		} else {
			fntSize = FNT_Size_1622;
			pad_y = (FavEdit_Option_H/2)-(22/2);
		}
	} else {
		pad_y = (FavEdit_Option_H/2)-(19/2);
	}
	// If selected change the colors of the elements
	if( item == selected ){
		fc = FavEdit_HghFc;
		bc = FavEdit_HghBc;
	}
	switch( item ){
		case FAVEDIT_SECTIONTITLE:
			TAP_Osd_FillBox(rgn, FavEdit_X+FAVEDIT_PADDING, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item), FavEdit_Option_W, FavEdit_Option_H, FavEdit_BaseColour);
			switch( fntSize ){
				case FNT_Size_1419:
					Font_Osd_PutString1419( rgn, FavEdit_X+FAVEDIT_PADDING+FAVEDIT_GAP, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item)+pad_y, FavEdit_X+FAVEDIT_PADDING+FavEdit_Option_W-FAVEDIT_GAP, "Edit Favourite Details", fc, COLOR_None);
					break;
				case FNT_Size_1622:
					Font_Osd_PutString1622( rgn, FavEdit_X+FAVEDIT_PADDING+FAVEDIT_GAP, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item)+pad_y, FavEdit_X+FAVEDIT_PADDING+FavEdit_Option_W-FAVEDIT_GAP, "Edit Favourite Details", fc, COLOR_None);
					break;
				case FNT_Size_1926:
					Font_Osd_PutString1926( rgn, FavEdit_X+FAVEDIT_PADDING+FAVEDIT_GAP, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item)+pad_y, FavEdit_X+FAVEDIT_PADDING+FavEdit_Option_W-FAVEDIT_GAP, "Edit Favourite Details", fc, COLOR_None);
					break;
			}
			break;
		case FAVEDIT_TERM:
			if( item == selected ){
				if( FavEdit_ItemHigh != NULL ){
					TAP_Osd_RestoreBox( rgn, FavEdit_X+FAVEDIT_PADDING, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item), FavEdit_Option_W, FavEdit_Option_H, FavEdit_ItemHigh );
				} else {
					TAP_Osd_FillBox(rgn, FavEdit_X+FAVEDIT_PADDING, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item), FavEdit_Option_W, FavEdit_Option_H, bc);
				}
			} else {
				if( FavEdit_ItemBase != NULL ){
					TAP_Osd_RestoreBox( rgn, FavEdit_X+FAVEDIT_PADDING, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item), FavEdit_Option_W, FavEdit_Option_H, FavEdit_ItemBase );
				} else {
					TAP_Osd_FillBox(rgn, FavEdit_X+FAVEDIT_PADDING, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item), FavEdit_Option_W, FavEdit_Option_H, bc);
				}
			}
			// Draw black divider
			TAP_Osd_FillBox(rgn, FavEdit_X+FAVEDIT_PADDING+FAVEDIT_GAP+90, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item), FAVEDIT_GAP, FavEdit_Option_H, FavEdit_BaseColour);
			switch( fntSize ){
				case FNT_Size_1419:
					Font_Osd_PutString1419( rgn, FavEdit_X+FAVEDIT_PADDING+FAVEDIT_GAP, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item)+pad_y, FavEdit_X+FAVEDIT_PADDING+FAVEDIT_GAP+90, "Term", fc, COLOR_None);
					Font_Osd_PutString1419( rgn, FavEdit_X+FAVEDIT_PADDING+FAVEDIT_GAP+100, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item)+pad_y, FavEdit_X+FAVEDIT_PADDING+FavEdit_Option_W-FAVEDIT_GAP, FavEdit_Item.term, fc, COLOR_None);
					break;
				case FNT_Size_1622:
					Font_Osd_PutString1622( rgn, FavEdit_X+FAVEDIT_PADDING+FAVEDIT_GAP, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item)+pad_y, FavEdit_X+FAVEDIT_PADDING+FAVEDIT_GAP+90, "Term", fc, COLOR_None);
					Font_Osd_PutString1622( rgn, FavEdit_X+FAVEDIT_PADDING+FAVEDIT_GAP+100, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item)+pad_y, FavEdit_X+FAVEDIT_PADDING+FavEdit_Option_W-FAVEDIT_GAP, FavEdit_Item.term, fc, COLOR_None);
					break;
				case FNT_Size_1926:
					Font_Osd_PutString1926( rgn, FavEdit_X+FAVEDIT_PADDING+FAVEDIT_GAP, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item)+pad_y, FavEdit_X+FAVEDIT_PADDING+FAVEDIT_GAP+90, "Term", fc, COLOR_None);
					Font_Osd_PutString1926( rgn, FavEdit_X+FAVEDIT_PADDING+FAVEDIT_GAP+100, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item)+pad_y, FavEdit_X+FAVEDIT_PADDING+FavEdit_Option_W-FAVEDIT_GAP, FavEdit_Item.term, fc, COLOR_None);
					break;
			}
			break;
		case FAVEDIT_CHANNEL:
			if( item == selected ){
				if( FavEdit_ItemHigh != NULL ){
					TAP_Osd_RestoreBox( rgn, FavEdit_X+FAVEDIT_PADDING, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item), FavEdit_Option_W, FavEdit_Option_H, FavEdit_ItemHigh );
				} else {
					TAP_Osd_FillBox(rgn, FavEdit_X+FAVEDIT_PADDING, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item), FavEdit_Option_W, FavEdit_Option_H, bc);
				}
			} else {
				if( FavEdit_ItemBase != NULL ){
					TAP_Osd_RestoreBox( rgn, FavEdit_X+FAVEDIT_PADDING, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item), FavEdit_Option_W, FavEdit_Option_H, FavEdit_ItemBase );
				} else {
					TAP_Osd_FillBox(rgn, FavEdit_X+FAVEDIT_PADDING, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item), FavEdit_Option_W, FavEdit_Option_H, bc);
				}
			}
//			TAP_Osd_FillBox(rgn, FavEdit_X+FAVEDIT_PADDING, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item), FavEdit_Option_W, FavEdit_Option_H, bc);
			TAP_Osd_FillBox(rgn, FavEdit_X+FAVEDIT_PADDING+FAVEDIT_GAP+90, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item), FAVEDIT_GAP, FavEdit_Option_H, FavEdit_BaseColour);
			switch( fntSize ){
				case FNT_Size_1419:
					Font_Osd_PutString1419( rgn, FavEdit_X+FAVEDIT_PADDING+FAVEDIT_GAP, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item)+pad_y, FavEdit_X+FAVEDIT_PADDING+FAVEDIT_GAP+90, "Channel", fc, COLOR_None);
					Font_Osd_PutString1419( rgn, FavEdit_X+FAVEDIT_PADDING+FAVEDIT_GAP+100, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item)+pad_y, FavEdit_X+FAVEDIT_PADDING+FavEdit_Option_W-FAVEDIT_GAP, FavEdit_Item.channel, fc, COLOR_None);
					break;
				case FNT_Size_1622:
					Font_Osd_PutString1622( rgn, FavEdit_X+FAVEDIT_PADDING+FAVEDIT_GAP, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item)+pad_y, FavEdit_X+FAVEDIT_PADDING+FAVEDIT_GAP+90, "Channel", fc, COLOR_None);
					Font_Osd_PutString1622( rgn, FavEdit_X+FAVEDIT_PADDING+FAVEDIT_GAP+100, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item)+pad_y, FavEdit_X+FAVEDIT_PADDING+FavEdit_Option_W-FAVEDIT_GAP, FavEdit_Item.channel, fc, COLOR_None);
					break;
				case FNT_Size_1926:
					Font_Osd_PutString1926( rgn, FavEdit_X+FAVEDIT_PADDING+FAVEDIT_GAP, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item)+pad_y, FavEdit_X+FAVEDIT_PADDING+FAVEDIT_GAP+90, "Channel", fc, COLOR_None);
					Font_Osd_PutString1926( rgn, FavEdit_X+FAVEDIT_PADDING+FAVEDIT_GAP+100, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item)+pad_y, FavEdit_X+FAVEDIT_PADDING+FavEdit_Option_W-FAVEDIT_GAP, FavEdit_Item.channel, fc, COLOR_None);
					break;
			}
			break;
		case FAVEDIT_DAY:
			if( item == selected ){
				if( FavEdit_ItemHigh != NULL ){
					TAP_Osd_RestoreBox( rgn, FavEdit_X+FAVEDIT_PADDING, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item), FavEdit_Option_W, FavEdit_Option_H, FavEdit_ItemHigh );
				} else {
					TAP_Osd_FillBox(rgn, FavEdit_X+FAVEDIT_PADDING, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item), FavEdit_Option_W, FavEdit_Option_H, bc);
				}
			} else {
				if( FavEdit_ItemBase != NULL ){
					TAP_Osd_RestoreBox( rgn, FavEdit_X+FAVEDIT_PADDING, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item), FavEdit_Option_W, FavEdit_Option_H, FavEdit_ItemBase );
				} else {
					TAP_Osd_FillBox(rgn, FavEdit_X+FAVEDIT_PADDING, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item), FavEdit_Option_W, FavEdit_Option_H, bc);
				}
			}
//			TAP_Osd_FillBox(rgn, FavEdit_X+FAVEDIT_PADDING, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item), FavEdit_Option_W, FavEdit_Option_H, bc);
			TAP_Osd_FillBox(rgn, FavEdit_X+FAVEDIT_PADDING+FAVEDIT_GAP+90, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item), FAVEDIT_GAP, FavEdit_Option_H, FavEdit_BaseColour);
			switch( fntSize ){
				case FNT_Size_1419:
					Font_Osd_PutString1419( rgn, FavEdit_X+FAVEDIT_PADDING+FAVEDIT_GAP, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item)+pad_y, FavEdit_X+FAVEDIT_PADDING+FAVEDIT_GAP+90, "Day", fc, COLOR_None);
					Font_Osd_PutString1419( rgn, FavEdit_X+FAVEDIT_PADDING+FAVEDIT_GAP+100, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item)+pad_y, FavEdit_X+FAVEDIT_PADDING+FavEdit_Option_W-FAVEDIT_GAP, FavEdit_Item.day, fc, COLOR_None);
					break;
				case FNT_Size_1622:
					Font_Osd_PutString1622( rgn, FavEdit_X+FAVEDIT_PADDING+FAVEDIT_GAP, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item)+pad_y, FavEdit_X+FAVEDIT_PADDING+FAVEDIT_GAP+90, "Day", fc, COLOR_None);
					Font_Osd_PutString1622( rgn, FavEdit_X+FAVEDIT_PADDING+FAVEDIT_GAP+100, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item)+pad_y, FavEdit_X+FAVEDIT_PADDING+FavEdit_Option_W-FAVEDIT_GAP, FavEdit_Item.day, fc, COLOR_None);
					break;
				case FNT_Size_1926:
					Font_Osd_PutString1926( rgn, FavEdit_X+FAVEDIT_PADDING+FAVEDIT_GAP, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item)+pad_y, FavEdit_X+FAVEDIT_PADDING+FAVEDIT_GAP+90, "Day", fc, COLOR_None);
					Font_Osd_PutString1926( rgn, FavEdit_X+FAVEDIT_PADDING+FAVEDIT_GAP+100, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item)+pad_y, FavEdit_X+FAVEDIT_PADDING+FavEdit_Option_W-FAVEDIT_GAP, FavEdit_Item.day, fc, COLOR_None);
					break;
			}
			break;
		case FAVEDIT_LOCATION:
			if( item == selected ){
				if( FavEdit_ItemHigh != NULL ){
					TAP_Osd_RestoreBox( rgn, FavEdit_X+FAVEDIT_PADDING, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item), FavEdit_Option_W, FavEdit_Option_H, FavEdit_ItemHigh );
				} else {
					TAP_Osd_FillBox(rgn, FavEdit_X+FAVEDIT_PADDING, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item), FavEdit_Option_W, FavEdit_Option_H, bc);
				}
			} else {
				if( FavEdit_ItemBase != NULL ){
					TAP_Osd_RestoreBox( rgn, FavEdit_X+FAVEDIT_PADDING, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item), FavEdit_Option_W, FavEdit_Option_H, FavEdit_ItemBase );
				} else {
					TAP_Osd_FillBox(rgn, FavEdit_X+FAVEDIT_PADDING, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item), FavEdit_Option_W, FavEdit_Option_H, bc);
				}
			}
//			TAP_Osd_FillBox(rgn, FavEdit_X+FAVEDIT_PADDING, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item), FavEdit_Option_W, FavEdit_Option_H, bc);
			TAP_Osd_FillBox(rgn, FavEdit_X+FAVEDIT_PADDING+FAVEDIT_GAP+90, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item), FAVEDIT_GAP, FavEdit_Option_H, FavEdit_BaseColour);
			switch( fntSize ){
				case FNT_Size_1419:
					Font_Osd_PutString1419( rgn, FavEdit_X+FAVEDIT_PADDING+FAVEDIT_GAP, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item)+pad_y, FavEdit_X+FAVEDIT_PADDING+FAVEDIT_GAP+90, "Location", fc, COLOR_None);
					Font_Osd_PutString1419( rgn, FavEdit_X+FAVEDIT_PADDING+FAVEDIT_GAP+100, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item)+pad_y, FavEdit_X+FAVEDIT_PADDING+FavEdit_Option_W-FAVEDIT_GAP, FavManage_LocationText( FavEdit_Item.location ), fc, COLOR_None);
					break;
				case FNT_Size_1622:
					Font_Osd_PutString1622( rgn, FavEdit_X+FAVEDIT_PADDING+FAVEDIT_GAP, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item)+pad_y, FavEdit_X+FAVEDIT_PADDING+FAVEDIT_GAP+90, "Location", fc, COLOR_None);
					Font_Osd_PutString1622( rgn, FavEdit_X+FAVEDIT_PADDING+FAVEDIT_GAP+100, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item)+pad_y, FavEdit_X+FAVEDIT_PADDING+FavEdit_Option_W-FAVEDIT_GAP, FavManage_LocationText( FavEdit_Item.location ), fc, COLOR_None);
					break;
				case FNT_Size_1926:
					Font_Osd_PutString1926( rgn, FavEdit_X+FAVEDIT_PADDING+FAVEDIT_GAP, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item)+pad_y, FavEdit_X+FAVEDIT_PADDING+FAVEDIT_GAP+90, "Location", fc, COLOR_None);
					Font_Osd_PutString1926( rgn, FavEdit_X+FAVEDIT_PADDING+FAVEDIT_GAP+100, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item)+pad_y, FavEdit_X+FAVEDIT_PADDING+FavEdit_Option_W-FAVEDIT_GAP, FavManage_LocationText( FavEdit_Item.location ), fc, COLOR_None);
					break;
			}
			break;
		case FAVEDIT_START:
			if( item == selected ){
				if( FavEdit_ItemHigh != NULL ){
					TAP_Osd_RestoreBox( rgn, FavEdit_X+FAVEDIT_PADDING, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item), FavEdit_Option_W, FavEdit_Option_H, FavEdit_ItemHigh );
				} else {
					TAP_Osd_FillBox(rgn, FavEdit_X+FAVEDIT_PADDING, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item), FavEdit_Option_W, FavEdit_Option_H, bc);
				}
			} else {
				if( FavEdit_ItemBase != NULL ){
					TAP_Osd_RestoreBox( rgn, FavEdit_X+FAVEDIT_PADDING, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item), FavEdit_Option_W, FavEdit_Option_H, FavEdit_ItemBase );
				} else {
					TAP_Osd_FillBox(rgn, FavEdit_X+FAVEDIT_PADDING, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item), FavEdit_Option_W, FavEdit_Option_H, bc);
				}
			}
//			TAP_Osd_FillBox(rgn, FavEdit_X+FAVEDIT_PADDING, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item), FavEdit_Option_W, FavEdit_Option_H, bc);
			TAP_Osd_FillBox(rgn, FavEdit_X+FAVEDIT_PADDING+FAVEDIT_GAP+90, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item), FAVEDIT_GAP, FavEdit_Option_H, FavEdit_BaseColour);			
			switch( fntSize ){
				case FNT_Size_1419:
					Font_Osd_PutString1419( rgn, FavEdit_X+FAVEDIT_PADDING+FAVEDIT_GAP, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item)+pad_y, FavEdit_X+FAVEDIT_PADDING+FAVEDIT_GAP+90, "Min Start", fc, COLOR_None);
					Font_Osd_PutString1419( rgn, FavEdit_X+FAVEDIT_PADDING+FAVEDIT_GAP+100, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item)+pad_y, FavEdit_X+FAVEDIT_PADDING+FavEdit_Option_W-FAVEDIT_GAP, MinsToHHMM(FavEdit_Item.start), fc, COLOR_None);
					break;
				case FNT_Size_1622:
					Font_Osd_PutString1622( rgn, FavEdit_X+FAVEDIT_PADDING+FAVEDIT_GAP, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item)+pad_y, FavEdit_X+FAVEDIT_PADDING+FAVEDIT_GAP+90, "Min Start", fc, COLOR_None);
					Font_Osd_PutString1622( rgn, FavEdit_X+FAVEDIT_PADDING+FAVEDIT_GAP+100, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item)+pad_y, FavEdit_X+FAVEDIT_PADDING+FavEdit_Option_W-FAVEDIT_GAP, MinsToHHMM(FavEdit_Item.start), fc, COLOR_None);
					break;
				case FNT_Size_1926:
					Font_Osd_PutString1926( rgn, FavEdit_X+FAVEDIT_PADDING+FAVEDIT_GAP, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item)+pad_y, FavEdit_X+FAVEDIT_PADDING+FAVEDIT_GAP+90, "Min Start", fc, COLOR_None);
					Font_Osd_PutString1926( rgn, FavEdit_X+FAVEDIT_PADDING+FAVEDIT_GAP+100, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item)+pad_y, FavEdit_X+FAVEDIT_PADDING+FavEdit_Option_W-FAVEDIT_GAP, MinsToHHMM(FavEdit_Item.start), fc, COLOR_None);
					break;
			}
			break;
		case FAVEDIT_END:
			if( item == selected ){
				if( FavEdit_ItemHigh != NULL ){
					TAP_Osd_RestoreBox( rgn, FavEdit_X+FAVEDIT_PADDING, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item), FavEdit_Option_W, FavEdit_Option_H, FavEdit_ItemHigh );
				} else {
					TAP_Osd_FillBox(rgn, FavEdit_X+FAVEDIT_PADDING, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item), FavEdit_Option_W, FavEdit_Option_H, bc);
				}
			} else {
				if( FavEdit_ItemBase != NULL ){
					TAP_Osd_RestoreBox( rgn, FavEdit_X+FAVEDIT_PADDING, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item), FavEdit_Option_W, FavEdit_Option_H, FavEdit_ItemBase );
				} else {
					TAP_Osd_FillBox(rgn, FavEdit_X+FAVEDIT_PADDING, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item), FavEdit_Option_W, FavEdit_Option_H, bc);
				}
			}
//			TAP_Osd_FillBox(rgn, FavEdit_X+FAVEDIT_PADDING, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item), FavEdit_Option_W, FavEdit_Option_H, bc);
			TAP_Osd_FillBox(rgn, FavEdit_X+FAVEDIT_PADDING+FAVEDIT_GAP+90, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item), FAVEDIT_GAP, FavEdit_Option_H, FavEdit_BaseColour);			
			switch( fntSize ){
				case FNT_Size_1419:
					Font_Osd_PutString1419( rgn, FavEdit_X+FAVEDIT_PADDING+FAVEDIT_GAP, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item)+pad_y, FavEdit_X+FAVEDIT_PADDING+FAVEDIT_GAP+90, "Max Start", fc, COLOR_None);
					Font_Osd_PutString1419( rgn, FavEdit_X+FAVEDIT_PADDING+FAVEDIT_GAP+100, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item)+pad_y, FavEdit_X+FAVEDIT_PADDING+FavEdit_Option_W-FAVEDIT_GAP, MinsToHHMM(FavEdit_Item.end), fc, COLOR_None);
					break;
				case FNT_Size_1622:
					Font_Osd_PutString1622( rgn, FavEdit_X+FAVEDIT_PADDING+FAVEDIT_GAP, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item)+pad_y, FavEdit_X+FAVEDIT_PADDING+FAVEDIT_GAP+90, "Max Start", fc, COLOR_None);
					Font_Osd_PutString1622( rgn, FavEdit_X+FAVEDIT_PADDING+FAVEDIT_GAP+100, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item)+pad_y, FavEdit_X+FAVEDIT_PADDING+FavEdit_Option_W-FAVEDIT_GAP, MinsToHHMM(FavEdit_Item.end), fc, COLOR_None);
					break;
				case FNT_Size_1926:
					Font_Osd_PutString1926( rgn, FavEdit_X+FAVEDIT_PADDING+FAVEDIT_GAP, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item)+pad_y, FavEdit_X+FAVEDIT_PADDING+FAVEDIT_GAP+90, "Max Start", fc, COLOR_None);
					Font_Osd_PutString1926( rgn, FavEdit_X+FAVEDIT_PADDING+FAVEDIT_GAP+100, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item)+pad_y, FavEdit_X+FAVEDIT_PADDING+FavEdit_Option_W-FAVEDIT_GAP, MinsToHHMM(FavEdit_Item.end), fc, COLOR_None);
					break;
			}
			break;
		case FAVEDIT_PRIORITY:
			if( item == selected ){
				if( FavEdit_ItemHigh != NULL ){
					TAP_Osd_RestoreBox( rgn, FavEdit_X+FAVEDIT_PADDING, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item), FavEdit_Option_W, FavEdit_Option_H, FavEdit_ItemHigh );
				} else {
					TAP_Osd_FillBox(rgn, FavEdit_X+FAVEDIT_PADDING, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item), FavEdit_Option_W, FavEdit_Option_H, bc);
				}
			} else {
				if( FavEdit_ItemBase != NULL ){
					TAP_Osd_RestoreBox( rgn, FavEdit_X+FAVEDIT_PADDING, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item), FavEdit_Option_W, FavEdit_Option_H, FavEdit_ItemBase );
				} else {
					TAP_Osd_FillBox(rgn, FavEdit_X+FAVEDIT_PADDING, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item), FavEdit_Option_W, FavEdit_Option_H, bc);
				}
			}
//			TAP_Osd_FillBox(rgn, FavEdit_X+FAVEDIT_PADDING, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item), FavEdit_Option_W, FavEdit_Option_H, bc);
			TAP_Osd_FillBox(rgn, FavEdit_X+FAVEDIT_PADDING+FAVEDIT_GAP+90, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item), FAVEDIT_GAP, FavEdit_Option_H, FavEdit_BaseColour);			
			switch( fntSize ){
				case FNT_Size_1419:
					Font_Osd_PutString1419( rgn, FavEdit_X+FAVEDIT_PADDING+FAVEDIT_GAP, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item)+pad_y, FavEdit_X+FAVEDIT_PADDING+FAVEDIT_GAP+90, "Priority", fc, COLOR_None);
					Font_Osd_PutString1419( rgn, FavEdit_X+FAVEDIT_PADDING+FAVEDIT_GAP+100, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item)+pad_y, FavEdit_X+FAVEDIT_PADDING+FavEdit_Option_W-FAVEDIT_GAP, NumberToString(FavEdit_Item.priority), fc, COLOR_None);
					break;
				case FNT_Size_1622:
					Font_Osd_PutString1622( rgn, FavEdit_X+FAVEDIT_PADDING+FAVEDIT_GAP, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item)+pad_y, FavEdit_X+FAVEDIT_PADDING+FAVEDIT_GAP+90, "Priority", fc, COLOR_None);
					Font_Osd_PutString1622( rgn, FavEdit_X+FAVEDIT_PADDING+FAVEDIT_GAP+100, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item)+pad_y, FavEdit_X+FAVEDIT_PADDING+FavEdit_Option_W-FAVEDIT_GAP, NumberToString(FavEdit_Item.priority), fc, COLOR_None);
					break;
				case FNT_Size_1926:
					Font_Osd_PutString1926( rgn, FavEdit_X+FAVEDIT_PADDING+FAVEDIT_GAP, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item)+pad_y, FavEdit_X+FAVEDIT_PADDING+FAVEDIT_GAP+90, "Priority", fc, COLOR_None);
					Font_Osd_PutString1926( rgn, FavEdit_X+FAVEDIT_PADDING+FAVEDIT_GAP+100, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item)+pad_y, FavEdit_X+FAVEDIT_PADDING+FavEdit_Option_W-FAVEDIT_GAP, NumberToString(FavEdit_Item.priority), fc, COLOR_None);
					break;
			}
			break;		
		case FAVEDIT_RPT:
			if( item == selected ){
				if( FavEdit_ItemHigh != NULL ){
					TAP_Osd_RestoreBox( rgn, FavEdit_X+FAVEDIT_PADDING, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item), FavEdit_Option_W, FavEdit_Option_H, FavEdit_ItemHigh );
				} else {
					TAP_Osd_FillBox(rgn, FavEdit_X+FAVEDIT_PADDING, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item), FavEdit_Option_W, FavEdit_Option_H, bc);
				}
			} else {
				if( FavEdit_ItemBase != NULL ){
					TAP_Osd_RestoreBox( rgn, FavEdit_X+FAVEDIT_PADDING, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item), FavEdit_Option_W, FavEdit_Option_H, FavEdit_ItemBase );
				} else {
					TAP_Osd_FillBox(rgn, FavEdit_X+FAVEDIT_PADDING, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item), FavEdit_Option_W, FavEdit_Option_H, bc);
				}
			}
//			TAP_Osd_FillBox(rgn, FavEdit_X+FAVEDIT_PADDING, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item), FavEdit_Option_W, FavEdit_Option_H, bc);
			TAP_Osd_FillBox(rgn, FavEdit_X+FAVEDIT_PADDING+FAVEDIT_GAP+90, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item), FAVEDIT_GAP, FavEdit_Option_H, FavEdit_BaseColour);			
			switch( fntSize ){
				case FNT_Size_1419:
					Font_Osd_PutString1419( rgn, FavEdit_X+FAVEDIT_PADDING+FAVEDIT_GAP, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item)+pad_y, FavEdit_X+FAVEDIT_PADDING+FAVEDIT_GAP+90, "Repeats", fc, COLOR_None);
					Font_Osd_PutString1419( rgn, FavEdit_X+FAVEDIT_PADDING+FAVEDIT_GAP+100, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item)+pad_y, FavEdit_X+FAVEDIT_PADDING+FavEdit_Option_W-FAVEDIT_GAP, FavEdit_Item.rpt==0?"Yes":"No", fc, COLOR_None);
					break;
				case FNT_Size_1622:
					Font_Osd_PutString1622( rgn, FavEdit_X+FAVEDIT_PADDING+FAVEDIT_GAP, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item)+pad_y, FavEdit_X+FAVEDIT_PADDING+FAVEDIT_GAP+90, "Repeats", fc, COLOR_None);
					Font_Osd_PutString1622( rgn, FavEdit_X+FAVEDIT_PADDING+FAVEDIT_GAP+100, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item)+pad_y, FavEdit_X+FAVEDIT_PADDING+FavEdit_Option_W-FAVEDIT_GAP, FavEdit_Item.rpt==0?"Yes":"No", fc, COLOR_None);
					break;
				case FNT_Size_1926:
					Font_Osd_PutString1926( rgn, FavEdit_X+FAVEDIT_PADDING+FAVEDIT_GAP, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item)+pad_y, FavEdit_X+FAVEDIT_PADDING+FAVEDIT_GAP+90, "Repeats", fc, COLOR_None);
					Font_Osd_PutString1926( rgn, FavEdit_X+FAVEDIT_PADDING+FAVEDIT_GAP+100, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item)+pad_y, FavEdit_X+FAVEDIT_PADDING+FavEdit_Option_W-FAVEDIT_GAP, FavEdit_Item.rpt==0?"Yes":"No", fc, COLOR_None);
					break;
			}
			break;
		case FAVEDIT_ADJ:
			if( item == selected ){
				if( FavEdit_ItemHigh != NULL ){
					TAP_Osd_RestoreBox( rgn, FavEdit_X+FAVEDIT_PADDING, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item), FavEdit_Option_W, FavEdit_Option_H, FavEdit_ItemHigh );
				} else {
					TAP_Osd_FillBox(rgn, FavEdit_X+FAVEDIT_PADDING, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item), FavEdit_Option_W, FavEdit_Option_H, bc);
				}
			} else {
				if( FavEdit_ItemBase != NULL ){
					TAP_Osd_RestoreBox( rgn, FavEdit_X+FAVEDIT_PADDING, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item), FavEdit_Option_W, FavEdit_Option_H, FavEdit_ItemBase );
				} else {
					TAP_Osd_FillBox(rgn, FavEdit_X+FAVEDIT_PADDING, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item), FavEdit_Option_W, FavEdit_Option_H, bc);
				}
			}
//			TAP_Osd_FillBox(rgn, FavEdit_X+FAVEDIT_PADDING, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item), FavEdit_Option_W, FavEdit_Option_H, bc);
			TAP_Osd_FillBox(rgn, FavEdit_X+FAVEDIT_PADDING+FAVEDIT_GAP+90, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item), FAVEDIT_GAP, FavEdit_Option_H, FavEdit_BaseColour);			
			switch( fntSize ){
				case FNT_Size_1419:
					Font_Osd_PutString1419( rgn, FavEdit_X+FAVEDIT_PADDING+FAVEDIT_GAP, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item)+pad_y, FavEdit_X+FAVEDIT_PADDING+FAVEDIT_GAP+90, "Adjust", fc, COLOR_None);
					Font_Osd_PutString1419( rgn, FavEdit_X+FAVEDIT_PADDING+FAVEDIT_GAP+100, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item)+pad_y, FavEdit_X+FAVEDIT_PADDING+FavEdit_Option_W-FAVEDIT_GAP, FavEdit_Item.adj==1?"Yes":"No", fc, COLOR_None);
					break;
				case FNT_Size_1622:
					Font_Osd_PutString1622( rgn, FavEdit_X+FAVEDIT_PADDING+FAVEDIT_GAP, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item)+pad_y, FavEdit_X+FAVEDIT_PADDING+FAVEDIT_GAP+90, "Adjust", fc, COLOR_None);
					Font_Osd_PutString1622( rgn, FavEdit_X+FAVEDIT_PADDING+FAVEDIT_GAP+100, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item)+pad_y, FavEdit_X+FAVEDIT_PADDING+FavEdit_Option_W-FAVEDIT_GAP, FavEdit_Item.adj==1?"Yes":"No", fc, COLOR_None);
					break;
				case FNT_Size_1926:
					Font_Osd_PutString1926( rgn, FavEdit_X+FAVEDIT_PADDING+FAVEDIT_GAP, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item)+pad_y, FavEdit_X+FAVEDIT_PADDING+FAVEDIT_GAP+90, "Adjust", fc, COLOR_None);
					Font_Osd_PutString1926( rgn, FavEdit_X+FAVEDIT_PADDING+FAVEDIT_GAP+100, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item)+pad_y, FavEdit_X+FAVEDIT_PADDING+FavEdit_Option_W-FAVEDIT_GAP, FavEdit_Item.adj==1?"Yes":"No", fc, COLOR_None);
					break;
			}
			break;
		case FAVEDIT_SELECTION:
			// Render the base
			if( FavEdit_ItemBase != NULL ){
				TAP_Osd_RestoreBox( rgn, FavEdit_X+FAVEDIT_PADDING, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item), FavEdit_Option_W, FavEdit_Option_H, FavEdit_ItemBase );
			} else {
				TAP_Osd_FillBox(rgn, FavEdit_X+FAVEDIT_PADDING, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item), FavEdit_Option_W, FavEdit_Option_H, bc);
			}
			TAP_Osd_FillBox(rgn, FavEdit_X+FAVEDIT_PADDING+(FavEdit_Option_W/2)-(FAVEDIT_GAP/2), FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item), FAVEDIT_GAP-1, FavEdit_Option_H, FavEdit_BaseColour);
			if( item == selected ){
				if( FavEdit_OptionSelected == FAVEDIT_OK ){
					if( FavEdit_OkItemHigh != NULL ){
						TAP_Osd_RestoreBox( rgn, FavEdit_X+FAVEDIT_PADDING, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item), (FavEdit_Option_W/2)-(FAVEDIT_GAP/2)-1, FavEdit_Option_H, FavEdit_OkItemHigh );
					} else {
						TAP_Osd_FillBox(rgn, FavEdit_X+FAVEDIT_PADDING, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item), (FavEdit_Option_W/2)-(FAVEDIT_GAP/2), FavEdit_Option_H, FavEdit_HghBc);
					}
				} else {
					if( FavEdit_OkItemHigh != NULL ){
						TAP_Osd_RestoreBox( rgn, FavEdit_X+FAVEDIT_PADDING+(FavEdit_Option_W/2)+(FAVEDIT_GAP/2), FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item), (FavEdit_Option_W/2)-(FAVEDIT_GAP/2)-1, FavEdit_Option_H, FavEdit_CancelItemHigh );
					} else {
						TAP_Osd_FillBox(rgn, FavEdit_X+FAVEDIT_PADDING+(FavEdit_Option_W/2)+(FAVEDIT_GAP/2), FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item), (FavEdit_Option_W/2)-(FAVEDIT_GAP/2)-1, FavEdit_Option_H, FavEdit_HghBc);
					}
				}
				switch( fntSize ){
					case FNT_Size_1419:
						Font_Osd_PutString1419( rgn, FavEdit_X+FAVEDIT_PADDING+FAVEDIT_GAP, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item)+pad_y, FavEdit_X+FAVEDIT_PADDING+FavEdit_Option_W-FAVEDIT_GAP, "OK", fc, COLOR_None);
						break;
					case FNT_Size_1622:
						Font_Osd_PutString1622( rgn, FavEdit_X+FAVEDIT_PADDING+FAVEDIT_GAP, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item)+pad_y, FavEdit_X+FAVEDIT_PADDING+FavEdit_Option_W-FAVEDIT_GAP, "OK", fc, COLOR_None);
						break;
					case FNT_Size_1926:
						Font_Osd_PutString1926( rgn, FavEdit_X+FAVEDIT_PADDING+FAVEDIT_GAP, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item)+pad_y, FavEdit_X+FAVEDIT_PADDING+FavEdit_Option_W-FAVEDIT_GAP, "OK", fc, COLOR_None);
						break;
				}
				switch( fntSize ){
					case FNT_Size_1419:
						Font_Osd_PutString1419( rgn, FavEdit_X+FAVEDIT_PADDING+FAVEDIT_GAP+(FavEdit_Option_W/2)+(FAVEDIT_GAP/2), FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item)+pad_y, FavEdit_X+FAVEDIT_PADDING+FavEdit_Option_W-FAVEDIT_GAP, "Cancel", fc, COLOR_None);
						break;
					case FNT_Size_1622:
						Font_Osd_PutString1622( rgn, FavEdit_X+FAVEDIT_PADDING+FAVEDIT_GAP+(FavEdit_Option_W/2)+(FAVEDIT_GAP/2), FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item)+pad_y, FavEdit_X+FAVEDIT_PADDING+FavEdit_Option_W-FAVEDIT_GAP, "Cancel", fc, COLOR_None);
						break;
					case FNT_Size_1926:
						Font_Osd_PutString1926( rgn, FavEdit_X+FAVEDIT_PADDING+FAVEDIT_GAP+(FavEdit_Option_W/2)+(FAVEDIT_GAP/2), FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item)+pad_y, FavEdit_X+FAVEDIT_PADDING+FavEdit_Option_W-FAVEDIT_GAP, "Cancel", fc, COLOR_None);
						break;
				}	
			} else {
//				TAP_Osd_FillBox(rgn, FavEdit_X+FAVEDIT_PADDING, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item), (FavEdit_Option_W/2)-(FAVEDIT_GAP/2)-1, FavEdit_Option_H, FavEdit_Bc);
//				TAP_Osd_FillBox(rgn, FavEdit_X+FAVEDIT_PADDING+(FavEdit_Option_W/2)+(FAVEDIT_GAP/2), FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item), (FavEdit_Option_W/2)-(FAVEDIT_GAP/2)-1, FavEdit_Option_H, FavEdit_Bc);
				switch( fntSize ){
					case FNT_Size_1419:
						Font_Osd_PutString1419( rgn, FavEdit_X+FAVEDIT_PADDING+FAVEDIT_GAP, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item)+pad_y, FavEdit_X+FAVEDIT_PADDING+FavEdit_Option_W-FAVEDIT_GAP, "OK", fc, COLOR_None);
						break;
					case FNT_Size_1622:
						Font_Osd_PutString1622( rgn, FavEdit_X+FAVEDIT_PADDING+FAVEDIT_GAP, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item)+pad_y, FavEdit_X+FAVEDIT_PADDING+FavEdit_Option_W-FAVEDIT_GAP, "OK", fc, COLOR_None);
						break;
					case FNT_Size_1926:
						Font_Osd_PutString1926( rgn, FavEdit_X+FAVEDIT_PADDING+FAVEDIT_GAP, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item)+pad_y, FavEdit_X+FAVEDIT_PADDING+FavEdit_Option_W-FAVEDIT_GAP, "OK", fc, COLOR_None);
						break;
				}
				switch( fntSize ){
					case FNT_Size_1419:
						Font_Osd_PutString1419( rgn, FavEdit_X+FAVEDIT_PADDING+FAVEDIT_GAP+(FavEdit_Option_W/2)+(FAVEDIT_GAP/2), FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item)+pad_y, FavEdit_X+FAVEDIT_PADDING+FavEdit_Option_W-FAVEDIT_GAP, "Cancel", fc, COLOR_None);
						break;
					case FNT_Size_1622:
						Font_Osd_PutString1622( rgn, FavEdit_X+FAVEDIT_PADDING+FAVEDIT_GAP+(FavEdit_Option_W/2)+(FAVEDIT_GAP/2), FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item)+pad_y, FavEdit_X+FAVEDIT_PADDING+FavEdit_Option_W-FAVEDIT_GAP, "Cancel", fc, COLOR_None);
						break;
					case FNT_Size_1926:
						Font_Osd_PutString1926( rgn, FavEdit_X+FAVEDIT_PADDING+FAVEDIT_GAP+(FavEdit_Option_W/2)+(FAVEDIT_GAP/2), FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item)+pad_y, FavEdit_X+FAVEDIT_PADDING+FavEdit_Option_W-FAVEDIT_GAP, "Cancel", fc, COLOR_None);
						break;
				}	
			}
			break;
		case FAVEDIT_HELP:
			if( FavEdit_Option_W > 250 ){
				TAP_Osd_FillBox(rgn, FavEdit_X+FAVEDIT_PADDING, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item), FavEdit_Option_W, FavEdit_Option_H, FavEdit_BaseColour);
				TAP_Osd_PutGd( rgn, FavEdit_X+FAVEDIT_PADDING+FAVEDIT_GAP, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item)+pad_y, &_okGd, TRUE );	// Draw highlighted
				Font_Osd_PutString1419( rgn, FavEdit_X+FAVEDIT_PADDING+FAVEDIT_GAP+30, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item)+pad_y, FavEdit_X+FAVEDIT_PADDING+FAVEDIT_GAP+90, "Edit Text", fc, COLOR_None);
				TAP_Osd_PutGd( rgn, FavEdit_X+FAVEDIT_PADDING+FAVEDIT_GAP+100, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item)+pad_y, &_rewindGd, TRUE );	// Draw highlighted
				TAP_Osd_PutGd( rgn, FavEdit_X+FAVEDIT_PADDING+FAVEDIT_GAP+100+23, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item)+pad_y, &_forwardGd, TRUE );	// Draw highlighted
				Font_Osd_PutString1419( rgn, FavEdit_X+FAVEDIT_PADDING+FAVEDIT_GAP+100+50, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item)+pad_y, FavEdit_X+FAVEDIT_PADDING+FAVEDIT_GAP+130+90, "+/- Hour", fc, COLOR_None);
				TAP_Osd_PutGd( rgn, FavEdit_X+FAVEDIT_PADDING+FAVEDIT_GAP+215, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item)+pad_y, &_voldownGd, TRUE );	// Draw highlighted
				TAP_Osd_PutGd( rgn, FavEdit_X+FAVEDIT_PADDING+FAVEDIT_GAP+215+23, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item)+pad_y, &_volupGd, TRUE );	// Draw highlighted
				Font_Osd_PutString1419( rgn, FavEdit_X+FAVEDIT_PADDING+FAVEDIT_GAP+215+50, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*item)+pad_y, FavEdit_X+FAVEDIT_PADDING+FAVEDIT_GAP+225+90, "Adjust", fc, COLOR_None);
			}
			break;
	}
}

void FavEdit_KeyboardClose( char * text, bool changed ){
	TAP_Osd_Copy( FavEdit_RGN, FavEdit_MemRGN, 0, 0, 720, 576, 0, 0, FALSE);	// Copy the main region to memory to avoid corruption from the previous OSD
	if( changed ){
		switch( FavEdit_Selected ){
			case FAVEDIT_TERM:
				memset( FavEdit_Item.term, '\0', sizeof(FavEdit_Item.term) ); // Blank the filename field
				strncpy(FavEdit_Item.term, text, sizeof(FavEdit_Item.term));
				break;
			case FAVEDIT_CHANNEL:
				memset( FavEdit_Item.channel, '\0', sizeof(FavEdit_Item.channel) ); // Blank the filename field
				strncpy(FavEdit_Item.channel, text, sizeof(FavEdit_Item.channel));
				break;
			case FAVEDIT_DAY:
				memset( FavEdit_Item.day, '\0', sizeof(FavEdit_Item.day) ); // Blank the filename field
				strncpy(FavEdit_Item.day, text, sizeof(FavEdit_Item.day));
				break;
		}
		// We have change the text of the timer so modify the fileName field
		FavEdit_DrawItem( FavEdit_RGN, FavEdit_Selected, FavEdit_Selected );
	}
}

// Function to draw the timer details to the region
void FavEdit_DrawDetails( word rgn ){
	int i;
	for( i = 0; i < FAVEDIT_OPTIONS; i++ ){
		FavEdit_DrawItem( rgn, i, FavEdit_Selected );
	}
}

dword FavEdit_KeyHandler( dword param1 ){
	if( Keyboard_IsActive() ){	// Handle the keyboard
		return Keyboard_Keyhandler(param1);
	}
	if( param1 == RKEY_Exit ){
		FavEdit_Deactivate(TRUE);
		return 0;
	}
	// Do the navigation stuff now
	if( param1 == RKEY_ChUp ){
		FavEdit_PrevSelected = FavEdit_Selected;
		FavEdit_Selected--;
		if( FavEdit_Selected < 1 ){
			FavEdit_Selected = FAVEDIT_HELP-1;
		}
		FavEdit_DrawItem( FavEdit_MemRGN, FavEdit_PrevSelected, FavEdit_Selected );
		FavEdit_DrawItem( FavEdit_MemRGN, FavEdit_Selected, FavEdit_Selected );
		TAP_Osd_Copy( FavEdit_MemRGN, FavEdit_RGN, FavEdit_X+FAVEDIT_PADDING, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*FavEdit_PrevSelected), FavEdit_Option_W, FavEdit_Option_H, FavEdit_X+FAVEDIT_PADDING, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*FavEdit_PrevSelected), FALSE);
		TAP_Osd_Copy( FavEdit_MemRGN, FavEdit_RGN, FavEdit_X+FAVEDIT_PADDING, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*FavEdit_Selected), FavEdit_Option_W, FavEdit_Option_H, FavEdit_X+FAVEDIT_PADDING, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*FavEdit_Selected), FALSE);
	}
	// Do the navigation stuff now
	if( param1 == RKEY_ChDown ){
		FavEdit_PrevSelected = FavEdit_Selected;
		FavEdit_Selected++;
		if( FavEdit_Selected > FAVEDIT_HELP-1 ){
			FavEdit_Selected = 1;
		}
		FavEdit_DrawItem( FavEdit_MemRGN, FavEdit_PrevSelected, FavEdit_Selected );
		FavEdit_DrawItem( FavEdit_MemRGN, FavEdit_Selected, FavEdit_Selected );
		TAP_Osd_Copy( FavEdit_MemRGN, FavEdit_RGN, FavEdit_X+FAVEDIT_PADDING, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*FavEdit_PrevSelected), FavEdit_Option_W, FavEdit_Option_H, FavEdit_X+FAVEDIT_PADDING, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*FavEdit_PrevSelected), FALSE);
		TAP_Osd_Copy( FavEdit_MemRGN, FavEdit_RGN, FavEdit_X+FAVEDIT_PADDING, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*FavEdit_Selected), FavEdit_Option_W, FavEdit_Option_H, FavEdit_X+FAVEDIT_PADDING, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*FavEdit_Selected), FALSE);
	}
	// Do the navigation stuff now
	if( param1 == RKEY_VolDown || param1 == RKEY_VolUp ){
		switch( FavEdit_Selected ){
			case FAVEDIT_START:
				if( param1 == RKEY_VolUp ){
					FavEdit_Item.start++;
				} else {
					FavEdit_Item.start--;
				}
				if( FavEdit_Item.start < -1 ){
					FavEdit_Item.start = -1;
				}
				if( FavEdit_Item.start > 1439 ){
					FavEdit_Item.start = 1439;
				}
				break;
			case FAVEDIT_END:
				if( param1 == RKEY_VolUp ){
					FavEdit_Item.end++;
				} else {
					FavEdit_Item.end--;
				}
				if( FavEdit_Item.end < -1 ){
					FavEdit_Item.end = -1;
				}
				if( FavEdit_Item.end > 1439 ){
					FavEdit_Item.end = 1439;
				}
				break;
			case FAVEDIT_PRIORITY:
				if( param1 == RKEY_VolUp ){
					FavEdit_Item.priority++;
				} else {
					FavEdit_Item.priority--;
				}
				if( FavEdit_Item.priority < 1 ){
					FavEdit_Item.priority = 1;
				}
				if( FavEdit_Item.priority > 10 ){
					FavEdit_Item.priority = 10;
				}
				break;
			case FAVEDIT_RPT:
				if( param1 == RKEY_VolUp ){
					FavEdit_Item.rpt++;
				} else {
					FavEdit_Item.rpt--;
				}
				if( FavEdit_Item.rpt < 0 ){
					FavEdit_Item.rpt = 1;
				}
				if( FavEdit_Item.rpt > 1 ){
					FavEdit_Item.rpt = 0;
				}
				break;
			case FAVEDIT_ADJ:
				if( param1 == RKEY_VolUp ){
					FavEdit_Item.adj++;
				} else {
					FavEdit_Item.adj--;
				}
				if( FavEdit_Item.adj < 0 ){
					FavEdit_Item.adj = 1;
				}
				if( FavEdit_Item.adj > 1 ){
					FavEdit_Item.adj = 0;
				}
				break;
			case FAVEDIT_SELECTION:
				FavEdit_OptionSelected++;
				if( FavEdit_OptionSelected > 1 ){
					FavEdit_OptionSelected  = 0;
				}
				break;
			case FAVEDIT_LOCATION:
				if( param1 == RKEY_VolUp ){
					FavEdit_Item.location++;
				} else {
					FavEdit_Item.location--;
				}
				if( FavEdit_Item.location < TITLEEXACT ){
					FavEdit_Item.location = ALL;
				}
				if( FavEdit_Item.location > ALL ){
					FavEdit_Item.location = TITLEEXACT;
				}
				break;
		}
		FavEdit_DrawItem( FavEdit_MemRGN, FavEdit_Selected, FavEdit_Selected );
		TAP_Osd_Copy( FavEdit_MemRGN, FavEdit_RGN, FavEdit_X+FAVEDIT_PADDING, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*FavEdit_Selected), FavEdit_Option_W, FavEdit_Option_H, FavEdit_X+FAVEDIT_PADDING, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*FavEdit_Selected), FALSE);
	}
	if( param1 == RKEY_Ok ){
		switch( FavEdit_Selected ){
			case FAVEDIT_TERM:
				Keyboard_Init( FavEdit_RGN, FavEdit_MemRGN, FavEdit_X, FavEdit_Y, FavEdit_W, FavEdit_H, "Favourites Term" );	// Initalise the keyboard
				Keyboard_SetColours( DISPLAY_KEYBOARDITEM, DISPLAY_KEYBOARDITEMTEXT, DISPLAY_KEYBOARDITEMSELECTED, DISPLAY_KEYBOARDITEMSELECTEDTEXT, DISPLAY_KEYBOARDFIELDTEXT, DISPLAY_KEYBOARDFIELD, DISPLAY_KEYBOARDBACKGROUND );
				if( Keyboard_Activate(FavEdit_Item.term,&FavEdit_KeyboardClose) ){
				}
				break;
			case FAVEDIT_CHANNEL:
				Keyboard_Init( FavEdit_RGN, FavEdit_MemRGN, FavEdit_X, FavEdit_Y, FavEdit_W, FavEdit_H, "Favourites Channel" );	// Initalise the keyboard
				Keyboard_SetColours( DISPLAY_KEYBOARDITEM, DISPLAY_KEYBOARDITEMTEXT, DISPLAY_KEYBOARDITEMSELECTED, DISPLAY_KEYBOARDITEMSELECTEDTEXT, DISPLAY_KEYBOARDFIELDTEXT, DISPLAY_KEYBOARDFIELD, DISPLAY_KEYBOARDBACKGROUND );
				if( Keyboard_Activate(FavEdit_Item.channel,&FavEdit_KeyboardClose) ){
				}
				break;
			case FAVEDIT_DAY:
				Keyboard_Init( FavEdit_RGN, FavEdit_MemRGN, FavEdit_X, FavEdit_Y, FavEdit_W, FavEdit_H, "Favourites Day" );	// Initalise the keyboard
				Keyboard_SetColours( DISPLAY_KEYBOARDITEM, DISPLAY_KEYBOARDITEMTEXT, DISPLAY_KEYBOARDITEMSELECTED, DISPLAY_KEYBOARDITEMSELECTEDTEXT, DISPLAY_KEYBOARDFIELDTEXT, DISPLAY_KEYBOARDFIELD, DISPLAY_KEYBOARDBACKGROUND );
				if( Keyboard_Activate(FavEdit_Item.day,&FavEdit_KeyboardClose) ){
				}
				break;
			case FAVEDIT_SELECTION:
				if( FavEdit_OptionSelected == FAVEDIT_OK ){
					FavEdit_Deactivate(FALSE);
				} else {
					FavEdit_Deactivate(TRUE);
				}
				break;
		}
	}
	if( param1 == RKEY_0 ){
		switch( FavEdit_Selected ){
			case FAVEDIT_START:
				FavEdit_Item.start = -1;
				break;
			case FAVEDIT_END:
				FavEdit_Item.end = -1;
				break;
			case FAVEDIT_PRIORITY:
				FavEdit_Item.priority = 1;
				break;
		}
		FavEdit_DrawItem( FavEdit_MemRGN, FavEdit_Selected, FavEdit_Selected );
		TAP_Osd_Copy( FavEdit_MemRGN, FavEdit_RGN, FavEdit_X+FAVEDIT_PADDING, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*FavEdit_Selected), FavEdit_Option_W, FavEdit_Option_H, FavEdit_X+FAVEDIT_PADDING, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*FavEdit_Selected), FALSE);
		return 0;
	}
	if( param1 == RKEY_Forward || param1 == RKEY_Rewind ){
		switch( FavEdit_Selected ){
			case FAVEDIT_START:
				if( param1 == RKEY_Forward ){
					FavEdit_Item.start+=60;
				} else {
					FavEdit_Item.start-=60;
				}
				if( FavEdit_Item.start < -1 ){
					FavEdit_Item.start = -1;
				}
				if( FavEdit_Item.start > 1439 ){
					FavEdit_Item.start = 1439;
				}
				break;
			case FAVEDIT_END:
				if( param1 == RKEY_Forward ){
					FavEdit_Item.end+=60;
				} else {
					FavEdit_Item.end-=60;
				}
				if( FavEdit_Item.end < -1 ){
					FavEdit_Item.end = -1;
				}
				if( FavEdit_Item.end > 1439 ){
					FavEdit_Item.end = 1439;
				}
				break;
		}
		FavEdit_DrawItem( FavEdit_MemRGN, FavEdit_Selected, FavEdit_Selected );
		TAP_Osd_Copy( FavEdit_MemRGN, FavEdit_RGN, FavEdit_X+FAVEDIT_PADDING, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*FavEdit_Selected), FavEdit_Option_W, FavEdit_Option_H, FavEdit_X+FAVEDIT_PADDING, FavEdit_Y+FAVEDIT_GAP+((FavEdit_Option_H+FAVEDIT_GAP)*FavEdit_Selected), FALSE);
		return 0;
	}
	return 0;
}

// Function to set the colours of the Timer window
void FavEdit_SetColours( dword fc, dword bc, dword fhc, dword bhc, dword base ){
	FavEdit_Fc = fc;
	FavEdit_HghBc = bhc;
	FavEdit_HghFc = fhc;
	FavEdit_Bc = bc;
	FavEdit_BaseColour = base;
}

