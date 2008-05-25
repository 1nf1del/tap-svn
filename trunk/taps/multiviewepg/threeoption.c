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

#include "threeoption.h"
#include "fontman.h"
#include "tools.h"

bool ThreeOption_IsActive( void ){
	return ThreeOption_Active;
}

// Function to initalize the keyboard - region and location to render
void ThreeOption_Init( word rgn, word memrgn, int x, int y, int w, int h ){
	ThreeOption_RGN = rgn;	// Store the region to render on
	ThreeOption_MemRGN = memrgn;
	ThreeOption_X = x;
	ThreeOption_Y = y;
	ThreeOption_W = w;
	ThreeOption_H = h;
	ThreeOption_Initalised = TRUE;
	ThreeOption_Option_H = (ThreeOption_H/ThreeOptionOPTIONS)-ThreeOption_GAP;
	ThreeOption_Option_W = ThreeOption_W - (ThreeOption_PADDING*2);
	ThreeOption_CreateGradients();
}

void ThreeOption_MemFree( void ){
	if (ThreeOption_ItemBase) TAP_MemFree(ThreeOption_ItemBase);
	if (ThreeOption_ItemHigh) TAP_MemFree(ThreeOption_ItemHigh);
	ThreeOption_ItemBase = NULL;
	ThreeOption_ItemHigh = NULL;
}

void ThreeOption_CreateGradients( void ){
	word rgn;
	ThreeOption_MemFree();
	if( Settings_GradientFactor > 0 ){
		rgn	= TAP_Osd_Create(0, 0, ThreeOption_Option_W, ThreeOption_Option_H, 0, OSD_Flag_MemRgn);
		// Main default item
		createGradient(rgn, ThreeOption_Option_W, ThreeOption_Option_H, Settings_GradientFactor, DISPLAY_ITEM);
		ThreeOption_ItemBase = TAP_Osd_SaveBox(rgn, 0, 0, ThreeOption_Option_W, ThreeOption_Option_H);
		createGradient(rgn, ThreeOption_Option_W, ThreeOption_Option_H, Settings_GradientFactor, DISPLAY_ITEMSELECTED);
		ThreeOption_ItemHigh = TAP_Osd_SaveBox(rgn, 0, 0, ThreeOption_Option_W, ThreeOption_Option_H);
		TAP_Osd_Delete(rgn);
	}
}

bool ThreeOption_Activate( char * question, char * option1, char * option2, int selected, void (*ReturningProcedure)( int option ) ){
	if( !ThreeOption_Initalised ) return FALSE;	// We haven't initalised so gracefully fail
	ThreeOption_Callback = ReturningProcedure;
	// Get the details of the event and create the timer structure
	ThreeOption_Selected = YES;
	ThreeOption_PrevSelected = YES;
	memset( ThreeOption_Question, '\0', sizeof(ThreeOption_Question) );
	memset( ThreeOption_Option1, '\0', sizeof(ThreeOption_Option1) );
	memset( ThreeOption_Option2, '\0', sizeof(ThreeOption_Option2) );
	strcpy(ThreeOption_Question, question);
	strcpy(ThreeOption_Option1, option1);
	strcpy(ThreeOption_Option2, option2);
	ThreeOption_Selected = selected;
	// Now lets render the display
	ThreeOption_DrawBackground(ThreeOption_MemRGN);
	ThreeOption_DrawDetails(ThreeOption_MemRGN);
	// Coppy rendered to foreground
	TAP_Osd_Copy( ThreeOption_MemRGN, ThreeOption_RGN, ThreeOption_X, ThreeOption_Y, ThreeOption_W, ThreeOption_H, ThreeOption_X, ThreeOption_Y, FALSE);
	ThreeOption_Active = TRUE;
	return TRUE;
}

// Routine to handle the closing of the timer window
void ThreeOption_Deactivate( int option ){
	ThreeOption_Active = FALSE;
	// Restore what we had drawn over
	TAP_Osd_RestoreBox(ThreeOption_RGN, ThreeOption_X, ThreeOption_Y, ThreeOption_W, ThreeOption_H, ThreeOption_Base);
	TAP_MemFree(ThreeOption_Base);
	ThreeOption_Base = NULL;
	ThreeOption_MemFree();
	ThreeOption_Callback(option);
}

void ThreeOption_DrawItem( word rgn, int item, int selected ){
	int fntSize = 0;
	int pad_y;
	dword fc = ThreeOption_Fc;
	dword bc = ThreeOption_Bc;
	// Work out the font size allowed for the height of the option
	if( ( ThreeOption_Option_H - 12 ) > 19  ){
		fntSize = FNT_Size_1622;
		pad_y = (ThreeOption_Option_H/2)-(22/2);
	} else {
		pad_y = (ThreeOption_Option_H/2)-(19/2);
	}
	// If selected change the colors of the elements
	if( item == selected ){
		fc = ThreeOption_HghFc;
		bc = ThreeOption_HghBc;
	}
	switch( item ){
		case THREEOPTION_QUESTION:
			TAP_Osd_FillBox(rgn, ThreeOption_X+ThreeOption_PADDING, ThreeOption_Y+ThreeOption_GAP+((ThreeOption_Option_H+ThreeOption_GAP)*item), ThreeOption_Option_W, ThreeOption_Option_H, ThreeOption_BaseColour);
			WrapPutStr(rgn, ThreeOption_Question, ThreeOption_X+ThreeOption_PADDING+ThreeOption_GAP, ThreeOption_Y+ThreeOption_GAP+((ThreeOption_Option_H+ThreeOption_GAP)*item)+pad_y, ThreeOption_Option_W, fc, COLOR_None, 4, fntSize, 2);
			break;
		case THREEOPTION_OPTION1:
			if( item == selected ){
				if( ThreeOption_ItemHigh != NULL ){
					TAP_Osd_RestoreBox( rgn, ThreeOption_X+ThreeOption_PADDING, ThreeOption_Y+ThreeOption_GAP+((ThreeOption_Option_H+ThreeOption_GAP)*item)-ThreeOption_PADDING, ThreeOption_Option_W, ThreeOption_Option_H, ThreeOption_ItemHigh );
				} else {
					TAP_Osd_FillBox(rgn, ThreeOption_X+ThreeOption_PADDING, ThreeOption_Y+ThreeOption_GAP+((ThreeOption_Option_H+ThreeOption_GAP)*item)-ThreeOption_PADDING, ThreeOption_Option_W, ThreeOption_Option_H, bc);
				}
			} else {
				if( ThreeOption_ItemBase != NULL ){
					TAP_Osd_RestoreBox( rgn, ThreeOption_X+ThreeOption_PADDING, ThreeOption_Y+ThreeOption_GAP+((ThreeOption_Option_H+ThreeOption_GAP)*item)-ThreeOption_PADDING, ThreeOption_Option_W, ThreeOption_Option_H, ThreeOption_ItemBase );
				} else {
					TAP_Osd_FillBox(rgn, ThreeOption_X+ThreeOption_PADDING, ThreeOption_Y+ThreeOption_GAP+((ThreeOption_Option_H+ThreeOption_GAP)*item)-ThreeOption_PADDING, ThreeOption_Option_W, ThreeOption_Option_H, bc);
				}
			}
			switch( fntSize ){
				case FNT_Size_1419:
					Font_Osd_PutString1419( rgn, ThreeOption_X+ThreeOption_PADDING+ThreeOption_GAP, ThreeOption_Y-ThreeOption_PADDING+ThreeOption_GAP+((ThreeOption_Option_H+ThreeOption_GAP)*item)+pad_y, ThreeOption_X+ThreeOption_PADDING+ThreeOption_Option_W-ThreeOption_GAP, ThreeOption_Option1, fc, COLOR_None);
					break;
				case FNT_Size_1622:
					Font_Osd_PutString1622( rgn, ThreeOption_X+ThreeOption_PADDING+ThreeOption_GAP, ThreeOption_Y-ThreeOption_PADDING+ThreeOption_GAP+((ThreeOption_Option_H+ThreeOption_GAP)*item)+pad_y, ThreeOption_X+ThreeOption_PADDING+ThreeOption_Option_W-ThreeOption_GAP, ThreeOption_Option1, fc, COLOR_None);
					break;
				case FNT_Size_1926:
					Font_Osd_PutString1926( rgn, ThreeOption_X+ThreeOption_PADDING+ThreeOption_GAP, ThreeOption_Y-ThreeOption_PADDING+ThreeOption_GAP+((ThreeOption_Option_H+ThreeOption_GAP)*item)+pad_y, ThreeOption_X+ThreeOption_PADDING+ThreeOption_Option_W-ThreeOption_GAP, ThreeOption_Option1, fc, COLOR_None);
					break;
			}
			break;
		case THREEOPTION_OPTION2:
			if( item == selected ){
				if( ThreeOption_ItemHigh != NULL ){
					TAP_Osd_RestoreBox( rgn, ThreeOption_X+ThreeOption_PADDING, ThreeOption_Y+ThreeOption_GAP+((ThreeOption_Option_H+ThreeOption_GAP)*item)-ThreeOption_PADDING, ThreeOption_Option_W, ThreeOption_Option_H, ThreeOption_ItemHigh );
				} else {
					TAP_Osd_FillBox(rgn, ThreeOption_X+ThreeOption_PADDING, ThreeOption_Y+ThreeOption_GAP+((ThreeOption_Option_H+ThreeOption_GAP)*item)-ThreeOption_PADDING, ThreeOption_Option_W, ThreeOption_Option_H, bc);
				}
			} else {
				if( ThreeOption_ItemBase != NULL ){
					TAP_Osd_RestoreBox( rgn, ThreeOption_X+ThreeOption_PADDING, ThreeOption_Y+ThreeOption_GAP+((ThreeOption_Option_H+ThreeOption_GAP)*item)-ThreeOption_PADDING, ThreeOption_Option_W, ThreeOption_Option_H, ThreeOption_ItemBase );
				} else {
					TAP_Osd_FillBox(rgn, ThreeOption_X+ThreeOption_PADDING, ThreeOption_Y+ThreeOption_GAP+((ThreeOption_Option_H+ThreeOption_GAP)*item)-ThreeOption_PADDING, ThreeOption_Option_W, ThreeOption_Option_H, bc);
				}
			}
			switch( fntSize ){
				case FNT_Size_1419:
					Font_Osd_PutString1419( rgn, ThreeOption_X+ThreeOption_PADDING+ThreeOption_GAP, ThreeOption_Y-ThreeOption_PADDING+ThreeOption_GAP+((ThreeOption_Option_H+ThreeOption_GAP)*item)+pad_y, ThreeOption_X+ThreeOption_PADDING+ThreeOption_Option_W-ThreeOption_GAP, ThreeOption_Option2, fc, COLOR_None);
					break;
				case FNT_Size_1622:
					Font_Osd_PutString1622( rgn, ThreeOption_X+ThreeOption_PADDING+ThreeOption_GAP, ThreeOption_Y-ThreeOption_PADDING+ThreeOption_GAP+((ThreeOption_Option_H+ThreeOption_GAP)*item)+pad_y, ThreeOption_X+ThreeOption_PADDING+ThreeOption_Option_W-ThreeOption_GAP, ThreeOption_Option2, fc, COLOR_None);
					break;
				case FNT_Size_1926:
					Font_Osd_PutString1926( rgn, ThreeOption_X+ThreeOption_PADDING+ThreeOption_GAP, ThreeOption_Y-ThreeOption_PADDING+ThreeOption_GAP+((ThreeOption_Option_H+ThreeOption_GAP)*item)+pad_y, ThreeOption_X+ThreeOption_PADDING+ThreeOption_Option_W-ThreeOption_GAP, ThreeOption_Option2, fc, COLOR_None);
					break;
			}
			break;
		case THREEOPTION_NO:
			if( item == selected ){
				if( ThreeOption_ItemHigh != NULL ){
					TAP_Osd_RestoreBox( rgn, ThreeOption_X+ThreeOption_PADDING, ThreeOption_Y+ThreeOption_GAP+((ThreeOption_Option_H+ThreeOption_GAP)*item)-ThreeOption_PADDING, ThreeOption_Option_W, ThreeOption_Option_H, ThreeOption_ItemHigh );
				} else {
					TAP_Osd_FillBox(rgn, ThreeOption_X+ThreeOption_PADDING, ThreeOption_Y+ThreeOption_GAP+((ThreeOption_Option_H+ThreeOption_GAP)*item)-ThreeOption_PADDING, ThreeOption_Option_W, ThreeOption_Option_H, bc);
				}
			} else {
				if( ThreeOption_ItemBase != NULL ){
					TAP_Osd_RestoreBox( rgn, ThreeOption_X+ThreeOption_PADDING, ThreeOption_Y+ThreeOption_GAP+((ThreeOption_Option_H+ThreeOption_GAP)*item)-ThreeOption_PADDING, ThreeOption_Option_W, ThreeOption_Option_H, ThreeOption_ItemBase );
				} else {
					TAP_Osd_FillBox(rgn, ThreeOption_X+ThreeOption_PADDING, ThreeOption_Y+ThreeOption_GAP+((ThreeOption_Option_H+ThreeOption_GAP)*item)-ThreeOption_PADDING, ThreeOption_Option_W, ThreeOption_Option_H, bc);
				}
			}
			switch( fntSize ){
				case FNT_Size_1419:
					Font_Osd_PutString1419( rgn, ThreeOption_X+ThreeOption_PADDING+ThreeOption_GAP, ThreeOption_Y-ThreeOption_PADDING+ThreeOption_GAP+((ThreeOption_Option_H+ThreeOption_GAP)*item)+pad_y, ThreeOption_X+ThreeOption_PADDING+ThreeOption_Option_W-ThreeOption_GAP, "Cancel", fc, COLOR_None);
					break;
				case FNT_Size_1622:
					Font_Osd_PutString1622( rgn, ThreeOption_X+ThreeOption_PADDING+ThreeOption_GAP, ThreeOption_Y-ThreeOption_PADDING+ThreeOption_GAP+((ThreeOption_Option_H+ThreeOption_GAP)*item)+pad_y, ThreeOption_X+ThreeOption_PADDING+ThreeOption_Option_W-ThreeOption_GAP, "Cancel", fc, COLOR_None);
					break;
				case FNT_Size_1926:
					Font_Osd_PutString1926( rgn, ThreeOption_X+ThreeOption_PADDING+ThreeOption_GAP, ThreeOption_Y-ThreeOption_PADDING+ThreeOption_GAP+((ThreeOption_Option_H+ThreeOption_GAP)*item)+pad_y, ThreeOption_X+ThreeOption_PADDING+ThreeOption_Option_W-ThreeOption_GAP, "Cancel", fc, COLOR_None);
					break;
			}
			break;
	}
}

// Function to draw the yes no options to the region
void ThreeOption_DrawDetails( word rgn ){
	int i;
	for( i = 0; i < ThreeOptionOPTIONS; i++ ){
		ThreeOption_DrawItem( rgn, i, ThreeOption_Selected );
	}
}

// Function to set the colours of the Timer window
void ThreeOption_SetColours( dword fc, dword bc, dword fhc, dword bhc, dword base ){
	ThreeOption_Fc = fc;
	ThreeOption_HghBc = bhc;
	ThreeOption_HghFc = fhc;
	ThreeOption_Bc = bc;
	ThreeOption_BaseColour = base;
}

void ThreeOption_DrawBackground( word rgn ){
	if( ThreeOption_Base != NULL ) TAP_MemFree(ThreeOption_Base);
	ThreeOption_Base = TAP_Osd_SaveBox(ThreeOption_RGN, ThreeOption_X, ThreeOption_Y, ThreeOption_W, ThreeOption_H);	// Save what is under the Timer screen
	TAP_Osd_FillBox(rgn, ThreeOption_X, ThreeOption_Y, ThreeOption_W, ThreeOption_H, ThreeOption_BaseColour);
}


dword ThreeOption_KeyHandler( dword param1 ){
	if( param1 == RKEY_Exit ){
		ThreeOption_Deactivate(THREEOPTION_NO);
		return 0;
	}
	// Do the navigation stuff now
	if( param1 == RKEY_ChUp ){
		ThreeOption_PrevSelected = ThreeOption_Selected;
		ThreeOption_Selected--;
		if( ThreeOption_Selected < THREEOPTION_OPTION1 ){
			ThreeOption_Selected = ThreeOptionOPTIONS-1;
		}
		ThreeOption_DrawItem( ThreeOption_MemRGN, ThreeOption_PrevSelected, ThreeOption_Selected );
		ThreeOption_DrawItem( ThreeOption_MemRGN, ThreeOption_Selected, ThreeOption_Selected );
		TAP_Osd_Copy( ThreeOption_MemRGN, ThreeOption_RGN, ThreeOption_X, ThreeOption_Y, ThreeOption_W, ThreeOption_H, ThreeOption_X, ThreeOption_Y, FALSE);
	}
	// Do the navigation stuff now
	if( param1 == RKEY_ChDown ){
		ThreeOption_PrevSelected = ThreeOption_Selected;
		ThreeOption_Selected++;
		if( ThreeOption_Selected > ThreeOptionOPTIONS-1 ){
			ThreeOption_Selected = THREEOPTION_OPTION1;
		}
		ThreeOption_DrawItem( ThreeOption_MemRGN, ThreeOption_PrevSelected, ThreeOption_Selected );
		ThreeOption_DrawItem( ThreeOption_MemRGN, ThreeOption_Selected, ThreeOption_Selected );
		TAP_Osd_Copy( ThreeOption_MemRGN, ThreeOption_RGN, ThreeOption_X, ThreeOption_Y, ThreeOption_W, ThreeOption_H, ThreeOption_X, ThreeOption_Y, FALSE);
	}
	if( param1 == RKEY_Ok ){
		ThreeOption_Deactivate(ThreeOption_Selected);
	}
	return 0;
}


