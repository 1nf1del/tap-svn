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

#include "favsortorder.h"
#include "fontman.h"
#include "favmanage.h"
#include "tools.h"

bool FavSortOrder_IsActive( void ){
	return FavSortOrder_Active;
}

// Function to initalize the keyboard - region and location to render
void FavSortOrder_Init( word rgn, word memrgn, int x, int y, int w, int h ){
	FavSortOrder_RGN = rgn;	// Store the region to render on
	FavSortOrder_MemRGN = memrgn;
	FavSortOrder_X = x;
	FavSortOrder_Y = y;
	FavSortOrder_W = w;
	FavSortOrder_H = h;
	FavSortOrder_Initalised = TRUE;
	FavSortOrder_Option_H = (FavSortOrder_H/FavSortOrderOPTIONS)-FavSortOrder_GAP;
	FavSortOrder_Option_W = FavSortOrder_W - (FavSortOrder_PADDING*2);
	FavSortOrder_CreateGradients();
}

void FavSortOrder_MemFree( void ){
	if (FavSortOrder_ItemBase) TAP_MemFree(FavSortOrder_ItemBase);
	if (FavSortOrder_ItemHigh) TAP_MemFree(FavSortOrder_ItemHigh);
	FavSortOrder_ItemBase = NULL;
	FavSortOrder_ItemHigh = NULL;
}

void FavSortOrder_CreateGradients( void ){
	word rgn;
	FavSortOrder_MemFree();
	if( Settings_GradientFactor > 0 ){
		rgn	= TAP_Osd_Create(0, 0, FavSortOrder_Option_W, FavSortOrder_Option_H, 0, OSD_Flag_MemRgn);
		// Main default item
		createGradient(rgn, FavSortOrder_Option_W, FavSortOrder_Option_H, Settings_GradientFactor, DISPLAY_ITEM);
		FavSortOrder_ItemBase = TAP_Osd_SaveBox(rgn, 0, 0, FavSortOrder_Option_W, FavSortOrder_Option_H);
		createGradient(rgn, FavSortOrder_Option_W, FavSortOrder_Option_H, Settings_GradientFactor, DISPLAY_ITEMSELECTED);
		FavSortOrder_ItemHigh = TAP_Osd_SaveBox(rgn, 0, 0, FavSortOrder_Option_W, FavSortOrder_Option_H);
		TAP_Osd_Delete(rgn);
	}
}


bool FavSortOrder_Activate( char * question, void (*ReturningProcedure)( bool, int ) ){
	if( !FavSortOrder_Initalised ) return FALSE;	// We haven't initalised so gracefully fail
	FavSortOrder_Callback = ReturningProcedure;
	// Get the details of the event and create the timer structure
	FavSortOrder_Selected = FavManage_SortOrder;
	FavSortOrder_PrevSelected = FavManage_SortOrder;
	memset( FavSortOrder_Question, '\0', sizeof(FavSortOrder_Question) );
	strcpy(FavSortOrder_Question, question);
	// Now lets render the display
	FavSortOrder_DrawBackground(FavSortOrder_MemRGN);
	FavSortOrder_DrawDetails(FavSortOrder_MemRGN);
	// Coppy rendered to foreground
	TAP_Osd_Copy( FavSortOrder_MemRGN, FavSortOrder_RGN, FavSortOrder_X, FavSortOrder_Y, FavSortOrder_W, FavSortOrder_H, FavSortOrder_X, FavSortOrder_Y, FALSE);
	FavSortOrder_Active = TRUE;
	return TRUE;
}

// Routine to handle the closing of the timer window
void FavSortOrder_Deactivate( bool yes ){
	FavSortOrder_Active = FALSE;
	// Restore what we had drawn over
	TAP_Osd_RestoreBox(FavSortOrder_RGN, FavSortOrder_X, FavSortOrder_Y, FavSortOrder_W, FavSortOrder_H, FavSortOrder_Base);
	TAP_MemFree(FavSortOrder_Base);
	FavSortOrder_Base = NULL;
	FavSortOrder_MemFree();
	if( yes ){
		FavSortOrder_Callback(TRUE, FavSortOrder_Selected);
	} else {
		FavSortOrder_Callback(FALSE, FavSortOrder_Selected);
	}
}

void FavSortOrder_DrawItem( word rgn, int item, int selected ){
	int fntSize = 0;
	int pad_y;
	dword fc = FavSortOrder_Fc;
	dword bc = FavSortOrder_Bc;
	// Work out the font size allowed for the height of the option
	if( ( FavSortOrder_Option_H - 12 ) > 19  ){
//		if( ( FavSortOrder_Option_H - 12 ) > 22 ){
//			// Use the big font
//			fntSize = FNT_Size_1926;
//			pad_y = (FavSortOrder_Option_H/2)-(26/2);
//		} else {
			fntSize = FNT_Size_1622;
			pad_y = (FavSortOrder_Option_H/2)-(22/2);
//		}
	} else {
		pad_y = (FavSortOrder_Option_H/2)-(19/2);
	}
	// If selected change the colors of the elements
	if( item == selected ){
		fc = FavSortOrder_HghFc;
		bc = FavSortOrder_HghBc;
	}
	switch( item ){
		case QUESTION:
			TAP_Osd_FillBox(rgn, FavSortOrder_X+FavSortOrder_PADDING, FavSortOrder_Y+FavSortOrder_GAP+((FavSortOrder_Option_H+FavSortOrder_GAP)*item), FavSortOrder_Option_W, FavSortOrder_Option_H, FavSortOrder_BaseColour);
			WrapPutStr(rgn, FavSortOrder_Question, FavSortOrder_X+FavSortOrder_PADDING+FavSortOrder_GAP, FavSortOrder_Y+FavSortOrder_GAP+((FavSortOrder_Option_H+FavSortOrder_GAP)*item)+pad_y, FavSortOrder_Option_W, fc, COLOR_None, 4, fntSize, 2);
			break;
		case SORTFAVPRIORITY:
			if( item == selected ){
				if( FavSortOrder_ItemHigh != NULL ){
					TAP_Osd_RestoreBox( rgn, FavSortOrder_X+FavSortOrder_PADDING, FavSortOrder_Y+FavSortOrder_GAP+((FavSortOrder_Option_H+FavSortOrder_GAP)*item)-FavSortOrder_PADDING, FavSortOrder_Option_W, FavSortOrder_Option_H, FavSortOrder_ItemHigh );
				} else {
					TAP_Osd_FillBox(rgn, FavSortOrder_X+FavSortOrder_PADDING, FavSortOrder_Y+FavSortOrder_GAP+((FavSortOrder_Option_H+FavSortOrder_GAP)*item)-FavSortOrder_PADDING, FavSortOrder_Option_W, FavSortOrder_Option_H, bc);
				}
			} else {
				if( FavSortOrder_ItemBase != NULL ){
					TAP_Osd_RestoreBox( rgn, FavSortOrder_X+FavSortOrder_PADDING, FavSortOrder_Y+FavSortOrder_GAP+((FavSortOrder_Option_H+FavSortOrder_GAP)*item)-FavSortOrder_PADDING, FavSortOrder_Option_W, FavSortOrder_Option_H, FavSortOrder_ItemBase );
				} else {
					TAP_Osd_FillBox(rgn, FavSortOrder_X+FavSortOrder_PADDING, FavSortOrder_Y+FavSortOrder_GAP+((FavSortOrder_Option_H+FavSortOrder_GAP)*item)-FavSortOrder_PADDING, FavSortOrder_Option_W, FavSortOrder_Option_H, bc);
				}
			}
//			TAP_Osd_FillBox(rgn, FavSortOrder_X+FavSortOrder_PADDING, FavSortOrder_Y+FavSortOrder_GAP+((FavSortOrder_Option_H+FavSortOrder_GAP)*item)-FavSortOrder_PADDING, FavSortOrder_Option_W, FavSortOrder_Option_H, bc);
			switch( fntSize ){
				case FNT_Size_1419:
					Font_Osd_PutString1419( rgn, FavSortOrder_X+FavSortOrder_PADDING+FavSortOrder_GAP, FavSortOrder_Y-FavSortOrder_PADDING+FavSortOrder_GAP+((FavSortOrder_Option_H+FavSortOrder_GAP)*item)+pad_y, FavSortOrder_X+FavSortOrder_PADDING+FavSortOrder_Option_W-FavSortOrder_GAP, "Priority", fc, COLOR_None);
					break;
				case FNT_Size_1622:
					Font_Osd_PutString1622( rgn, FavSortOrder_X+FavSortOrder_PADDING+FavSortOrder_GAP, FavSortOrder_Y-FavSortOrder_PADDING+FavSortOrder_GAP+((FavSortOrder_Option_H+FavSortOrder_GAP)*item)+pad_y, FavSortOrder_X+FavSortOrder_PADDING+FavSortOrder_Option_W-FavSortOrder_GAP, "Priority", fc, COLOR_None);
					break;
				case FNT_Size_1926:
					Font_Osd_PutString1926( rgn, FavSortOrder_X+FavSortOrder_PADDING+FavSortOrder_GAP, FavSortOrder_Y-FavSortOrder_PADDING+FavSortOrder_GAP+((FavSortOrder_Option_H+FavSortOrder_GAP)*item)+pad_y, FavSortOrder_X+FavSortOrder_PADDING+FavSortOrder_Option_W-FavSortOrder_GAP, "Priority", fc, COLOR_None);
					break;
			}
			break;
		case SORTFAVNAME:
			if( item == selected ){
				if( FavSortOrder_ItemHigh != NULL ){
					TAP_Osd_RestoreBox( rgn, FavSortOrder_X+FavSortOrder_PADDING, FavSortOrder_Y+FavSortOrder_GAP+((FavSortOrder_Option_H+FavSortOrder_GAP)*item)-FavSortOrder_PADDING, FavSortOrder_Option_W, FavSortOrder_Option_H, FavSortOrder_ItemHigh );
				} else {
					TAP_Osd_FillBox(rgn, FavSortOrder_X+FavSortOrder_PADDING, FavSortOrder_Y+FavSortOrder_GAP+((FavSortOrder_Option_H+FavSortOrder_GAP)*item)-FavSortOrder_PADDING, FavSortOrder_Option_W, FavSortOrder_Option_H, bc);
				}
			} else {
				if( FavSortOrder_ItemBase != NULL ){
					TAP_Osd_RestoreBox( rgn, FavSortOrder_X+FavSortOrder_PADDING, FavSortOrder_Y+FavSortOrder_GAP+((FavSortOrder_Option_H+FavSortOrder_GAP)*item)-FavSortOrder_PADDING, FavSortOrder_Option_W, FavSortOrder_Option_H, FavSortOrder_ItemBase );
				} else {
					TAP_Osd_FillBox(rgn, FavSortOrder_X+FavSortOrder_PADDING, FavSortOrder_Y+FavSortOrder_GAP+((FavSortOrder_Option_H+FavSortOrder_GAP)*item)-FavSortOrder_PADDING, FavSortOrder_Option_W, FavSortOrder_Option_H, bc);
				}
			}
//			TAP_Osd_FillBox(rgn, FavSortOrder_X+FavSortOrder_PADDING, FavSortOrder_Y+FavSortOrder_GAP+((FavSortOrder_Option_H+FavSortOrder_GAP)*item)-FavSortOrder_PADDING, FavSortOrder_Option_W, FavSortOrder_Option_H, bc);
			switch( fntSize ){
				case FNT_Size_1419:
					Font_Osd_PutString1419( rgn, FavSortOrder_X+FavSortOrder_PADDING+FavSortOrder_GAP, FavSortOrder_Y-FavSortOrder_PADDING+FavSortOrder_GAP+((FavSortOrder_Option_H+FavSortOrder_GAP)*item)+pad_y, FavSortOrder_X+FavSortOrder_PADDING+FavSortOrder_Option_W-FavSortOrder_GAP, "Name", fc, COLOR_None);
					break;
				case FNT_Size_1622:
					Font_Osd_PutString1622( rgn, FavSortOrder_X+FavSortOrder_PADDING+FavSortOrder_GAP, FavSortOrder_Y-FavSortOrder_PADDING+FavSortOrder_GAP+((FavSortOrder_Option_H+FavSortOrder_GAP)*item)+pad_y, FavSortOrder_X+FavSortOrder_PADDING+FavSortOrder_Option_W-FavSortOrder_GAP, "Name", fc, COLOR_None);
					break;
				case FNT_Size_1926:
					Font_Osd_PutString1926( rgn, FavSortOrder_X+FavSortOrder_PADDING+FavSortOrder_GAP, FavSortOrder_Y-FavSortOrder_PADDING+FavSortOrder_GAP+((FavSortOrder_Option_H+FavSortOrder_GAP)*item)+pad_y, FavSortOrder_X+FavSortOrder_PADDING+FavSortOrder_Option_W-FavSortOrder_GAP, "Name", fc, COLOR_None);
					break;
			}
			break;
	}
}

// Function to draw the yes no options to the region
void FavSortOrder_DrawDetails( word rgn ){
	int i;
	for( i = 0; i < FavSortOrderOPTIONS; i++ ){
		FavSortOrder_DrawItem( rgn, i, FavSortOrder_Selected );
	}
}

// Function to set the colours of the Timer window
void FavSortOrder_SetColours( dword fc, dword bc, dword fhc, dword bhc, dword base ){
	FavSortOrder_Fc = fc;
	FavSortOrder_HghBc = bhc;
	FavSortOrder_HghFc = fhc;
	FavSortOrder_Bc = bc;
	FavSortOrder_BaseColour = base;
}

void FavSortOrder_DrawBackground( word rgn ){
	if( FavSortOrder_Base != NULL ) TAP_MemFree(FavSortOrder_Base);
	FavSortOrder_Base = TAP_Osd_SaveBox(FavSortOrder_RGN, FavSortOrder_X, FavSortOrder_Y, FavSortOrder_W, FavSortOrder_H);	// Save what is under the Timer screen
	TAP_Osd_FillBox(rgn, FavSortOrder_X, FavSortOrder_Y, FavSortOrder_W, FavSortOrder_H, FavSortOrder_BaseColour);
}


dword FavSortOrder_KeyHandler( dword param1 ){
	if( param1 == RKEY_Exit ){
		FavSortOrder_Deactivate(FALSE);
		return 0;
	}
	// Do the navigation stuff now
	if( param1 == RKEY_ChUp ){
		FavSortOrder_PrevSelected = FavSortOrder_Selected;
		FavSortOrder_Selected--;
		if( FavSortOrder_Selected < SORTFAVPRIORITY ){
			FavSortOrder_Selected = FavSortOrderOPTIONS-1;
		}
		FavSortOrder_DrawItem( FavSortOrder_MemRGN, FavSortOrder_PrevSelected, FavSortOrder_Selected );
		FavSortOrder_DrawItem( FavSortOrder_MemRGN, FavSortOrder_Selected, FavSortOrder_Selected );
		TAP_Osd_Copy( FavSortOrder_MemRGN, FavSortOrder_RGN, FavSortOrder_X, FavSortOrder_Y, FavSortOrder_W, FavSortOrder_H, FavSortOrder_X, FavSortOrder_Y, FALSE);
	}
	// Do the navigation stuff now
	if( param1 == RKEY_ChDown ){
		FavSortOrder_PrevSelected = FavSortOrder_Selected;
		FavSortOrder_Selected++;
		if( FavSortOrder_Selected > FavSortOrderOPTIONS-1 ){
			FavSortOrder_Selected = SORTFAVPRIORITY;
		}
		FavSortOrder_DrawItem( FavSortOrder_MemRGN, FavSortOrder_PrevSelected, FavSortOrder_Selected );
		FavSortOrder_DrawItem( FavSortOrder_MemRGN, FavSortOrder_Selected, FavSortOrder_Selected );
		TAP_Osd_Copy( FavSortOrder_MemRGN, FavSortOrder_RGN, FavSortOrder_X, FavSortOrder_Y, FavSortOrder_W, FavSortOrder_H, FavSortOrder_X, FavSortOrder_Y, FALSE);		
	}
	if( param1 == RKEY_Ok ){
		FavSortOrder_Deactivate(TRUE);
	}
	return 0;
}
