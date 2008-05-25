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

#include "yesnobox.h"
#include "fontman.h"
#include "tools.h"

bool YesNo_IsActive( void ){
	return YesNo_Active;
}

// Function to initalize the keyboard - region and location to render
void YesNo_Init( word rgn, word memrgn, int x, int y, int w, int h ){
	YesNo_RGN = rgn;	// Store the region to render on
	YesNo_MemRGN = memrgn;
	YesNo_X = x;
	YesNo_Y = y;
	YesNo_W = w;
	YesNo_H = h;
	YesNo_Initalised = TRUE;
	YesNo_Option_H = (YesNo_H/YESNOOPTIONS)-YesNo_GAP;
	YesNo_Option_W = YesNo_W - (YesNo_PADDING*2);
	YesNo_CreateGradients();
}

void YesNo_MemFree( void ){
	if (YesNo_ItemBase) TAP_MemFree(YesNo_ItemBase);
	if (YesNo_ItemHigh) TAP_MemFree(YesNo_ItemHigh);
	YesNo_ItemBase = NULL;
	YesNo_ItemHigh = NULL;
}

void YesNo_CreateGradients( void ){
	word rgn;
	YesNo_MemFree();
	if( Settings_GradientFactor > 0 ){
		rgn	= TAP_Osd_Create(0, 0, YesNo_Option_W, YesNo_Option_H, 0, OSD_Flag_MemRgn);
		// Main default item
		createGradient(rgn, YesNo_Option_W, YesNo_Option_H, Settings_GradientFactor, DISPLAY_ITEM);
		YesNo_ItemBase = TAP_Osd_SaveBox(rgn, 0, 0, YesNo_Option_W, YesNo_Option_H);
		createGradient(rgn, YesNo_Option_W, YesNo_Option_H, Settings_GradientFactor, DISPLAY_ITEMSELECTED);
		YesNo_ItemHigh = TAP_Osd_SaveBox(rgn, 0, 0, YesNo_Option_W, YesNo_Option_H);
		TAP_Osd_Delete(rgn);
	}
}

bool YesNo_Activate( char * question, void (*ReturningProcedure)( bool ) ){
	if( !YesNo_Initalised ) return FALSE;	// We haven't initalised so gracefully fail
	YesNo_Callback = ReturningProcedure;
	// Get the details of the event and create the timer structure
	YesNo_Selected = YES;
	YesNo_PrevSelected = YES;
	memset( YesNo_Question, '\0', sizeof(YesNo_Question) );
	strcpy(YesNo_Question, question);
	// Now lets render the display
	YesNo_DrawBackground(YesNo_MemRGN);
	YesNo_DrawDetails(YesNo_MemRGN);
	// Coppy rendered to foreground
	TAP_Osd_Copy( YesNo_MemRGN, YesNo_RGN, YesNo_X, YesNo_Y, YesNo_W, YesNo_H, YesNo_X, YesNo_Y, FALSE);
	YesNo_Active = TRUE;
	return TRUE;
}

// Routine to handle the closing of the timer window
void YesNo_Deactivate( int yes ){
	YesNo_Active = FALSE;
	// Restore what we had drawn over
	TAP_Osd_RestoreBox(YesNo_RGN, YesNo_X, YesNo_Y, YesNo_W, YesNo_H, YesNo_Base);
	TAP_MemFree(YesNo_Base);
	YesNo_Base = NULL;
	YesNo_MemFree();
	if( yes == YES ){
		YesNo_Callback(TRUE);
	} else {
		YesNo_Callback(FALSE);
	}
}

void YesNo_DrawItem( word rgn, int item, int selected ){
	int fntSize = 0;
	int pad_y;
	dword fc = YesNo_Fc;
	dword bc = YesNo_Bc;
	// Work out the font size allowed for the height of the option
	if( ( YesNo_Option_H - 12 ) > 19  ){
//		if( ( YesNo_Option_H - 12 ) > 22 ){
//			// Use the big font
//			fntSize = FNT_Size_1926;
//			pad_y = (YesNo_Option_H/2)-(26/2);
//		} else {
			fntSize = FNT_Size_1622;
			pad_y = (YesNo_Option_H/2)-(22/2);
//		}
	} else {
		pad_y = (YesNo_Option_H/2)-(19/2);
	}
	// If selected change the colors of the elements
	if( item == selected ){
		fc = YesNo_HghFc;
		bc = YesNo_HghBc;
	}
	switch( item ){
		case QUESTION:
			TAP_Osd_FillBox(rgn, YesNo_X+YesNo_PADDING, YesNo_Y+YesNo_GAP+((YesNo_Option_H+YesNo_GAP)*item), YesNo_Option_W, YesNo_Option_H, YesNo_BaseColour);
			WrapPutStr(rgn, YesNo_Question, YesNo_X+YesNo_PADDING+YesNo_GAP, YesNo_Y+YesNo_GAP+((YesNo_Option_H+YesNo_GAP)*item)+pad_y, YesNo_Option_W, fc, COLOR_None, 4, fntSize, 2);
			break;
		case YES:
			if( item == selected ){
				if( YesNo_ItemHigh != NULL ){
					TAP_Osd_RestoreBox( rgn, YesNo_X+YesNo_PADDING, YesNo_Y+YesNo_GAP+((YesNo_Option_H+YesNo_GAP)*item)-YesNo_PADDING, YesNo_Option_W, YesNo_Option_H, YesNo_ItemHigh );
				} else {
					TAP_Osd_FillBox(rgn, YesNo_X+YesNo_PADDING, YesNo_Y+YesNo_GAP+((YesNo_Option_H+YesNo_GAP)*item)-YesNo_PADDING, YesNo_Option_W, YesNo_Option_H, bc);
				}
			} else {
				if( YesNo_ItemBase != NULL ){
					TAP_Osd_RestoreBox( rgn, YesNo_X+YesNo_PADDING, YesNo_Y+YesNo_GAP+((YesNo_Option_H+YesNo_GAP)*item)-YesNo_PADDING, YesNo_Option_W, YesNo_Option_H, YesNo_ItemBase );
				} else {
					TAP_Osd_FillBox(rgn, YesNo_X+YesNo_PADDING, YesNo_Y+YesNo_GAP+((YesNo_Option_H+YesNo_GAP)*item)-YesNo_PADDING, YesNo_Option_W, YesNo_Option_H, bc);
				}
			}
//			TAP_Osd_FillBox(rgn, YesNo_X+YesNo_PADDING, YesNo_Y+YesNo_GAP+((YesNo_Option_H+YesNo_GAP)*item)-YesNo_PADDING, YesNo_Option_W, YesNo_Option_H, bc);
			switch( fntSize ){
				case FNT_Size_1419:
					Font_Osd_PutString1419( rgn, YesNo_X+YesNo_PADDING+YesNo_GAP, YesNo_Y-YesNo_PADDING+YesNo_GAP+((YesNo_Option_H+YesNo_GAP)*item)+pad_y, YesNo_X+YesNo_PADDING+YesNo_Option_W-YesNo_GAP, "Yes", fc, COLOR_None);
					break;
				case FNT_Size_1622:
					Font_Osd_PutString1622( rgn, YesNo_X+YesNo_PADDING+YesNo_GAP, YesNo_Y-YesNo_PADDING+YesNo_GAP+((YesNo_Option_H+YesNo_GAP)*item)+pad_y, YesNo_X+YesNo_PADDING+YesNo_Option_W-YesNo_GAP, "Yes", fc, COLOR_None);
					break;
				case FNT_Size_1926:
					Font_Osd_PutString1926( rgn, YesNo_X+YesNo_PADDING+YesNo_GAP, YesNo_Y-YesNo_PADDING+YesNo_GAP+((YesNo_Option_H+YesNo_GAP)*item)+pad_y, YesNo_X+YesNo_PADDING+YesNo_Option_W-YesNo_GAP, "Yes", fc, COLOR_None);
					break;
			}
			break;
		case NO:
			if( item == selected ){
				if( YesNo_ItemHigh != NULL ){
					TAP_Osd_RestoreBox( rgn, YesNo_X+YesNo_PADDING, YesNo_Y+YesNo_GAP+((YesNo_Option_H+YesNo_GAP)*item)-YesNo_PADDING, YesNo_Option_W, YesNo_Option_H, YesNo_ItemHigh );
				} else {
					TAP_Osd_FillBox(rgn, YesNo_X+YesNo_PADDING, YesNo_Y+YesNo_GAP+((YesNo_Option_H+YesNo_GAP)*item)-YesNo_PADDING, YesNo_Option_W, YesNo_Option_H, bc);
				}
			} else {
				if( YesNo_ItemBase != NULL ){
					TAP_Osd_RestoreBox( rgn, YesNo_X+YesNo_PADDING, YesNo_Y+YesNo_GAP+((YesNo_Option_H+YesNo_GAP)*item)-YesNo_PADDING, YesNo_Option_W, YesNo_Option_H, YesNo_ItemBase );
				} else {
					TAP_Osd_FillBox(rgn, YesNo_X+YesNo_PADDING, YesNo_Y+YesNo_GAP+((YesNo_Option_H+YesNo_GAP)*item)-YesNo_PADDING, YesNo_Option_W, YesNo_Option_H, bc);
				}
			}
//			TAP_Osd_FillBox(rgn, YesNo_X+YesNo_PADDING, YesNo_Y+YesNo_GAP+((YesNo_Option_H+YesNo_GAP)*item)-YesNo_PADDING, YesNo_Option_W, YesNo_Option_H, bc);
			switch( fntSize ){
				case FNT_Size_1419:
					Font_Osd_PutString1419( rgn, YesNo_X+YesNo_PADDING+YesNo_GAP, YesNo_Y-YesNo_PADDING+YesNo_GAP+((YesNo_Option_H+YesNo_GAP)*item)+pad_y, YesNo_X+YesNo_PADDING+YesNo_Option_W-YesNo_GAP, "No", fc, COLOR_None);
					break;
				case FNT_Size_1622:
					Font_Osd_PutString1622( rgn, YesNo_X+YesNo_PADDING+YesNo_GAP, YesNo_Y-YesNo_PADDING+YesNo_GAP+((YesNo_Option_H+YesNo_GAP)*item)+pad_y, YesNo_X+YesNo_PADDING+YesNo_Option_W-YesNo_GAP, "No", fc, COLOR_None);
					break;
				case FNT_Size_1926:
					Font_Osd_PutString1926( rgn, YesNo_X+YesNo_PADDING+YesNo_GAP, YesNo_Y-YesNo_PADDING+YesNo_GAP+((YesNo_Option_H+YesNo_GAP)*item)+pad_y, YesNo_X+YesNo_PADDING+YesNo_Option_W-YesNo_GAP, "No", fc, COLOR_None);
					break;
			}
			break;
	}
}

// Function to draw the yes no options to the region
void YesNo_DrawDetails( word rgn ){
	int i;
	for( i = 0; i < YESNOOPTIONS; i++ ){
		YesNo_DrawItem( rgn, i, YesNo_Selected );
	}
}

// Function to set the colours of the Timer window
void YesNo_SetColours( dword fc, dword bc, dword fhc, dword bhc, dword base ){
	YesNo_Fc = fc;
	YesNo_HghBc = bhc;
	YesNo_HghFc = fhc;
	YesNo_Bc = bc;
	YesNo_BaseColour = base;
}

void YesNo_DrawBackground( word rgn ){
	if( YesNo_Base != NULL ) TAP_MemFree(YesNo_Base);
	YesNo_Base = TAP_Osd_SaveBox(YesNo_RGN, YesNo_X, YesNo_Y, YesNo_W, YesNo_H);	// Save what is under the Timer screen
	TAP_Osd_FillBox(rgn, YesNo_X, YesNo_Y, YesNo_W, YesNo_H, YesNo_BaseColour);
}


dword YesNo_KeyHandler( dword param1 ){
	if( param1 == RKEY_Exit ){
		YesNo_Deactivate(NO);
		return 0;
	}
	// Do the navigation stuff now
	if( param1 == RKEY_ChUp ){
		YesNo_PrevSelected = YesNo_Selected;
		YesNo_Selected--;
		if( YesNo_Selected < YES ){
			YesNo_Selected = YESNOOPTIONS-1;
		}
		YesNo_DrawItem( YesNo_MemRGN, YesNo_PrevSelected, YesNo_Selected );
		YesNo_DrawItem( YesNo_MemRGN, YesNo_Selected, YesNo_Selected );
		TAP_Osd_Copy( YesNo_MemRGN, YesNo_RGN, YesNo_X, YesNo_Y, YesNo_W, YesNo_H, YesNo_X, YesNo_Y, FALSE);
	}
	// Do the navigation stuff now
	if( param1 == RKEY_ChDown ){
		YesNo_PrevSelected = YesNo_Selected;
		YesNo_Selected++;
		if( YesNo_Selected > YESNOOPTIONS-1 ){
			YesNo_Selected = YES;
		}
		YesNo_DrawItem( YesNo_MemRGN, YesNo_PrevSelected, YesNo_Selected );
		YesNo_DrawItem( YesNo_MemRGN, YesNo_Selected, YesNo_Selected );
		TAP_Osd_Copy( YesNo_MemRGN, YesNo_RGN, YesNo_X, YesNo_Y, YesNo_W, YesNo_H, YesNo_X, YesNo_Y, FALSE);
	}
	if( param1 == RKEY_Ok ){
		YesNo_Deactivate(YesNo_Selected);
	}
	return 0;
}

