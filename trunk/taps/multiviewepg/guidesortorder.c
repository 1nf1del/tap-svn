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

#include "guidesortorder.h"
#include "fontman.h"
#include "favmanage.h"
#include "tools.h"

bool GuideSortOrder_IsActive( void ){
	return GuideSortOrder_Active;
}

// Function to initalize the keyboard - region and location to render
void GuideSortOrder_Init( word rgn, word memrgn, int x, int y, int w, int h ){
	GuideSortOrder_RGN = rgn;	// Store the region to render on
	GuideSortOrder_MemRGN = memrgn;
	GuideSortOrder_X = x;
	GuideSortOrder_Y = y;
	GuideSortOrder_W = w;
	GuideSortOrder_H = h;
	GuideSortOrder_Initalised = TRUE;
	GuideSortOrder_Option_H = (GuideSortOrder_H/GUIDE_SORTTYPES)-GuideSortOrder_GAP;
	GuideSortOrder_Option_W = GuideSortOrder_W - (GuideSortOrder_PADDING*2);
	GuideSortOrder_CreateGradients();
}

void GuideSortOrder_MemFree( void ){
	if (GuideSortOrder_ItemBase) TAP_MemFree(GuideSortOrder_ItemBase);
	if (GuideSortOrder_ItemHigh) TAP_MemFree(GuideSortOrder_ItemHigh);
	GuideSortOrder_ItemBase = NULL;
	GuideSortOrder_ItemHigh = NULL;
}

void GuideSortOrder_CreateGradients( void ){
	word rgn;
	GuideSortOrder_MemFree();
	if( Settings_GradientFactor > 0 ){
		rgn	= TAP_Osd_Create(0, 0, GuideSortOrder_Option_W, GuideSortOrder_Option_H, 0, OSD_Flag_MemRgn);
		// Main default item
		createGradient(rgn, GuideSortOrder_Option_W, GuideSortOrder_Option_H, Settings_GradientFactor, DISPLAY_ITEM);
		GuideSortOrder_ItemBase = TAP_Osd_SaveBox(rgn, 0, 0, GuideSortOrder_Option_W, GuideSortOrder_Option_H);
		createGradient(rgn, GuideSortOrder_Option_W, GuideSortOrder_Option_H, Settings_GradientFactor, DISPLAY_ITEMSELECTED);
		GuideSortOrder_ItemHigh = TAP_Osd_SaveBox(rgn, 0, 0, GuideSortOrder_Option_W, GuideSortOrder_Option_H);
		TAP_Osd_Delete(rgn);
	}
}


bool GuideSortOrder_Activate( char * question, void (*ReturningProcedure)( bool, int ) ){
	if( !GuideSortOrder_Initalised ) return FALSE;	// We haven't initalised so gracefully fail
	GuideSortOrder_Callback = ReturningProcedure;
	// Get the details of the event and create the timer structure
	GuideSortOrder_Selected = FavManage_SortOrder;
	GuideSortOrder_PrevSelected = FavManage_SortOrder;
	memset( GuideSortOrder_Question, '\0', sizeof(GuideSortOrder_Question) );
	strcpy(GuideSortOrder_Question, question);
	// Now lets render the display
	GuideSortOrder_DrawBackground(GuideSortOrder_MemRGN);
	GuideSortOrder_DrawDetails(GuideSortOrder_MemRGN);
	// Coppy rendered to foreground
	TAP_Osd_Copy( GuideSortOrder_MemRGN, GuideSortOrder_RGN, GuideSortOrder_X, GuideSortOrder_Y, GuideSortOrder_W, GuideSortOrder_H, GuideSortOrder_X, GuideSortOrder_Y, FALSE);
	GuideSortOrder_Active = TRUE;
	return TRUE;
}

// Routine to handle the closing of the timer window
void GuideSortOrder_Deactivate( bool yes ){
	GuideSortOrder_Active = FALSE;
	// Restore what we had drawn over
	TAP_Osd_RestoreBox(GuideSortOrder_RGN, GuideSortOrder_X, GuideSortOrder_Y, GuideSortOrder_W, GuideSortOrder_H, GuideSortOrder_Base);
	TAP_MemFree(GuideSortOrder_Base);
	GuideSortOrder_Base = NULL;
	GuideSortOrder_MemFree();
	if( yes ){
		GuideSortOrder_Callback(TRUE, GuideSortOrder_Selected);
	} else {
		GuideSortOrder_Callback(FALSE, GuideSortOrder_Selected);
	}
}

void GuideSortOrder_DrawItem( word rgn, int item, int selected ){
	int fntSize = 0;
	int pad_y;
	dword fc = GuideSortOrder_Fc;
	dword bc = GuideSortOrder_Bc;
	// Work out the font size allowed for the height of the option
	if( ( GuideSortOrder_Option_H - 12 ) > 19  ){
//		if( ( GuideSortOrder_Option_H - 12 ) > 22 ){
//			// Use the big font
//			fntSize = FNT_Size_1926;
//			pad_y = (GuideSortOrder_Option_H/2)-(26/2);
//		} else {
			fntSize = FNT_Size_1622;
			pad_y = (GuideSortOrder_Option_H/2)-(22/2);
//		}
	} else {
		pad_y = (GuideSortOrder_Option_H/2)-(19/2);
	}
	// If selected change the colors of the elements
	if( item == selected ){
		fc = GuideSortOrder_HghFc;
		bc = GuideSortOrder_HghBc;
	}
	switch( item ){
		case GUIDE_SORTTITLE:
			TAP_Osd_FillBox(rgn, GuideSortOrder_X+GuideSortOrder_PADDING, GuideSortOrder_Y+GuideSortOrder_GAP+((GuideSortOrder_Option_H+GuideSortOrder_GAP)*item), GuideSortOrder_Option_W, GuideSortOrder_Option_H, GuideSortOrder_BaseColour);
			WrapPutStr(rgn, GuideSortOrder_Question, GuideSortOrder_X+GuideSortOrder_PADDING+GuideSortOrder_GAP, GuideSortOrder_Y+GuideSortOrder_GAP+((GuideSortOrder_Option_H+GuideSortOrder_GAP)*item)+pad_y, GuideSortOrder_Option_W, fc, COLOR_None, 4, fntSize, 2);
			break;
		case GUIDE_SORTNAME:
			if( item == selected ){
				if( GuideSortOrder_ItemHigh != NULL ){
					TAP_Osd_RestoreBox( rgn, GuideSortOrder_X+GuideSortOrder_PADDING, GuideSortOrder_Y+GuideSortOrder_GAP+((GuideSortOrder_Option_H+GuideSortOrder_GAP)*item)-GuideSortOrder_PADDING, GuideSortOrder_Option_W, GuideSortOrder_Option_H, GuideSortOrder_ItemHigh );
				} else {
					TAP_Osd_FillBox(rgn, GuideSortOrder_X+GuideSortOrder_PADDING, GuideSortOrder_Y+GuideSortOrder_GAP+((GuideSortOrder_Option_H+GuideSortOrder_GAP)*item)-GuideSortOrder_PADDING, GuideSortOrder_Option_W, GuideSortOrder_Option_H, bc);
				}
			} else {
				if( GuideSortOrder_ItemBase != NULL ){
					TAP_Osd_RestoreBox( rgn, GuideSortOrder_X+GuideSortOrder_PADDING, GuideSortOrder_Y+GuideSortOrder_GAP+((GuideSortOrder_Option_H+GuideSortOrder_GAP)*item)-GuideSortOrder_PADDING, GuideSortOrder_Option_W, GuideSortOrder_Option_H, GuideSortOrder_ItemBase );
				} else {
					TAP_Osd_FillBox(rgn, GuideSortOrder_X+GuideSortOrder_PADDING, GuideSortOrder_Y+GuideSortOrder_GAP+((GuideSortOrder_Option_H+GuideSortOrder_GAP)*item)-GuideSortOrder_PADDING, GuideSortOrder_Option_W, GuideSortOrder_Option_H, bc);
				}
			}
//			TAP_Osd_FillBox(rgn, GuideSortOrder_X+GuideSortOrder_PADDING, GuideSortOrder_Y+GuideSortOrder_GAP+((GuideSortOrder_Option_H+GuideSortOrder_GAP)*item)-GuideSortOrder_PADDING, GuideSortOrder_Option_W, GuideSortOrder_Option_H, bc);
			switch( fntSize ){
				case FNT_Size_1419:
					Font_Osd_PutString1419( rgn, GuideSortOrder_X+GuideSortOrder_PADDING+GuideSortOrder_GAP, GuideSortOrder_Y-GuideSortOrder_PADDING+GuideSortOrder_GAP+((GuideSortOrder_Option_H+GuideSortOrder_GAP)*item)+pad_y, GuideSortOrder_X+GuideSortOrder_PADDING+GuideSortOrder_Option_W-GuideSortOrder_GAP, "Name", fc, COLOR_None);
					break;
				case FNT_Size_1622:
					Font_Osd_PutString1622( rgn, GuideSortOrder_X+GuideSortOrder_PADDING+GuideSortOrder_GAP, GuideSortOrder_Y-GuideSortOrder_PADDING+GuideSortOrder_GAP+((GuideSortOrder_Option_H+GuideSortOrder_GAP)*item)+pad_y, GuideSortOrder_X+GuideSortOrder_PADDING+GuideSortOrder_Option_W-GuideSortOrder_GAP, "Name", fc, COLOR_None);
					break;
				case FNT_Size_1926:
					Font_Osd_PutString1926( rgn, GuideSortOrder_X+GuideSortOrder_PADDING+GuideSortOrder_GAP, GuideSortOrder_Y-GuideSortOrder_PADDING+GuideSortOrder_GAP+((GuideSortOrder_Option_H+GuideSortOrder_GAP)*item)+pad_y, GuideSortOrder_X+GuideSortOrder_PADDING+GuideSortOrder_Option_W-GuideSortOrder_GAP, "Name", fc, COLOR_None);
					break;
			}
			break;
		case GUIDE_SORTCHANNEL:
			if( item == selected ){
				if( GuideSortOrder_ItemHigh != NULL ){
					TAP_Osd_RestoreBox( rgn, GuideSortOrder_X+GuideSortOrder_PADDING, GuideSortOrder_Y+GuideSortOrder_GAP+((GuideSortOrder_Option_H+GuideSortOrder_GAP)*item)-GuideSortOrder_PADDING, GuideSortOrder_Option_W, GuideSortOrder_Option_H, GuideSortOrder_ItemHigh );
				} else {
					TAP_Osd_FillBox(rgn, GuideSortOrder_X+GuideSortOrder_PADDING, GuideSortOrder_Y+GuideSortOrder_GAP+((GuideSortOrder_Option_H+GuideSortOrder_GAP)*item)-GuideSortOrder_PADDING, GuideSortOrder_Option_W, GuideSortOrder_Option_H, bc);
				}
			} else {
				if( GuideSortOrder_ItemBase != NULL ){
					TAP_Osd_RestoreBox( rgn, GuideSortOrder_X+GuideSortOrder_PADDING, GuideSortOrder_Y+GuideSortOrder_GAP+((GuideSortOrder_Option_H+GuideSortOrder_GAP)*item)-GuideSortOrder_PADDING, GuideSortOrder_Option_W, GuideSortOrder_Option_H, GuideSortOrder_ItemBase );
				} else {
					TAP_Osd_FillBox(rgn, GuideSortOrder_X+GuideSortOrder_PADDING, GuideSortOrder_Y+GuideSortOrder_GAP+((GuideSortOrder_Option_H+GuideSortOrder_GAP)*item)-GuideSortOrder_PADDING, GuideSortOrder_Option_W, GuideSortOrder_Option_H, bc);
				}
			}
//			TAP_Osd_FillBox(rgn, GuideSortOrder_X+GuideSortOrder_PADDING, GuideSortOrder_Y+GuideSortOrder_GAP+((GuideSortOrder_Option_H+GuideSortOrder_GAP)*item)-GuideSortOrder_PADDING, GuideSortOrder_Option_W, GuideSortOrder_Option_H, bc);
			switch( fntSize ){
				case FNT_Size_1419:
					Font_Osd_PutString1419( rgn, GuideSortOrder_X+GuideSortOrder_PADDING+GuideSortOrder_GAP, GuideSortOrder_Y-GuideSortOrder_PADDING+GuideSortOrder_GAP+((GuideSortOrder_Option_H+GuideSortOrder_GAP)*item)+pad_y, GuideSortOrder_X+GuideSortOrder_PADDING+GuideSortOrder_Option_W-GuideSortOrder_GAP, "Channel", fc, COLOR_None);
					break;
				case FNT_Size_1622:
					Font_Osd_PutString1622( rgn, GuideSortOrder_X+GuideSortOrder_PADDING+GuideSortOrder_GAP, GuideSortOrder_Y-GuideSortOrder_PADDING+GuideSortOrder_GAP+((GuideSortOrder_Option_H+GuideSortOrder_GAP)*item)+pad_y, GuideSortOrder_X+GuideSortOrder_PADDING+GuideSortOrder_Option_W-GuideSortOrder_GAP, "Channel", fc, COLOR_None);
					break;
				case FNT_Size_1926:
					Font_Osd_PutString1926( rgn, GuideSortOrder_X+GuideSortOrder_PADDING+GuideSortOrder_GAP, GuideSortOrder_Y-GuideSortOrder_PADDING+GuideSortOrder_GAP+((GuideSortOrder_Option_H+GuideSortOrder_GAP)*item)+pad_y, GuideSortOrder_X+GuideSortOrder_PADDING+GuideSortOrder_Option_W-GuideSortOrder_GAP, "Channel", fc, COLOR_None);
					break;
			}
			break;
		case GUIDE_SORTSTART:
			if( item == selected ){
				if( GuideSortOrder_ItemHigh != NULL ){
					TAP_Osd_RestoreBox( rgn, GuideSortOrder_X+GuideSortOrder_PADDING, GuideSortOrder_Y+GuideSortOrder_GAP+((GuideSortOrder_Option_H+GuideSortOrder_GAP)*item)-GuideSortOrder_PADDING, GuideSortOrder_Option_W, GuideSortOrder_Option_H, GuideSortOrder_ItemHigh );
				} else {
					TAP_Osd_FillBox(rgn, GuideSortOrder_X+GuideSortOrder_PADDING, GuideSortOrder_Y+GuideSortOrder_GAP+((GuideSortOrder_Option_H+GuideSortOrder_GAP)*item)-GuideSortOrder_PADDING, GuideSortOrder_Option_W, GuideSortOrder_Option_H, bc);
				}
			} else {
				if( GuideSortOrder_ItemBase != NULL ){
					TAP_Osd_RestoreBox( rgn, GuideSortOrder_X+GuideSortOrder_PADDING, GuideSortOrder_Y+GuideSortOrder_GAP+((GuideSortOrder_Option_H+GuideSortOrder_GAP)*item)-GuideSortOrder_PADDING, GuideSortOrder_Option_W, GuideSortOrder_Option_H, GuideSortOrder_ItemBase );
				} else {
					TAP_Osd_FillBox(rgn, GuideSortOrder_X+GuideSortOrder_PADDING, GuideSortOrder_Y+GuideSortOrder_GAP+((GuideSortOrder_Option_H+GuideSortOrder_GAP)*item)-GuideSortOrder_PADDING, GuideSortOrder_Option_W, GuideSortOrder_Option_H, bc);
				}
			}
//			TAP_Osd_FillBox(rgn, GuideSortOrder_X+GuideSortOrder_PADDING, GuideSortOrder_Y+GuideSortOrder_GAP+((GuideSortOrder_Option_H+GuideSortOrder_GAP)*item)-GuideSortOrder_PADDING, GuideSortOrder_Option_W, GuideSortOrder_Option_H, bc);
			switch( fntSize ){
				case FNT_Size_1419:
					Font_Osd_PutString1419( rgn, GuideSortOrder_X+GuideSortOrder_PADDING+GuideSortOrder_GAP, GuideSortOrder_Y-GuideSortOrder_PADDING+GuideSortOrder_GAP+((GuideSortOrder_Option_H+GuideSortOrder_GAP)*item)+pad_y, GuideSortOrder_X+GuideSortOrder_PADDING+GuideSortOrder_Option_W-GuideSortOrder_GAP, "Start", fc, COLOR_None);
					break;
				case FNT_Size_1622:
					Font_Osd_PutString1622( rgn, GuideSortOrder_X+GuideSortOrder_PADDING+GuideSortOrder_GAP, GuideSortOrder_Y-GuideSortOrder_PADDING+GuideSortOrder_GAP+((GuideSortOrder_Option_H+GuideSortOrder_GAP)*item)+pad_y, GuideSortOrder_X+GuideSortOrder_PADDING+GuideSortOrder_Option_W-GuideSortOrder_GAP, "Start", fc, COLOR_None);
					break;
				case FNT_Size_1926:
					Font_Osd_PutString1926( rgn, GuideSortOrder_X+GuideSortOrder_PADDING+GuideSortOrder_GAP, GuideSortOrder_Y-GuideSortOrder_PADDING+GuideSortOrder_GAP+((GuideSortOrder_Option_H+GuideSortOrder_GAP)*item)+pad_y, GuideSortOrder_X+GuideSortOrder_PADDING+GuideSortOrder_Option_W-GuideSortOrder_GAP, "Start", fc, COLOR_None);
					break;
			}
			break;
		case GUIDE_SORTRATING:
			if( item == selected ){
				if( GuideSortOrder_ItemHigh != NULL ){
					TAP_Osd_RestoreBox( rgn, GuideSortOrder_X+GuideSortOrder_PADDING, GuideSortOrder_Y+GuideSortOrder_GAP+((GuideSortOrder_Option_H+GuideSortOrder_GAP)*item)-GuideSortOrder_PADDING, GuideSortOrder_Option_W, GuideSortOrder_Option_H, GuideSortOrder_ItemHigh );
				} else {
					TAP_Osd_FillBox(rgn, GuideSortOrder_X+GuideSortOrder_PADDING, GuideSortOrder_Y+GuideSortOrder_GAP+((GuideSortOrder_Option_H+GuideSortOrder_GAP)*item)-GuideSortOrder_PADDING, GuideSortOrder_Option_W, GuideSortOrder_Option_H, bc);
				}
			} else {
				if( GuideSortOrder_ItemBase != NULL ){
					TAP_Osd_RestoreBox( rgn, GuideSortOrder_X+GuideSortOrder_PADDING, GuideSortOrder_Y+GuideSortOrder_GAP+((GuideSortOrder_Option_H+GuideSortOrder_GAP)*item)-GuideSortOrder_PADDING, GuideSortOrder_Option_W, GuideSortOrder_Option_H, GuideSortOrder_ItemBase );
				} else {
					TAP_Osd_FillBox(rgn, GuideSortOrder_X+GuideSortOrder_PADDING, GuideSortOrder_Y+GuideSortOrder_GAP+((GuideSortOrder_Option_H+GuideSortOrder_GAP)*item)-GuideSortOrder_PADDING, GuideSortOrder_Option_W, GuideSortOrder_Option_H, bc);
				}
			}
//			TAP_Osd_FillBox(rgn, GuideSortOrder_X+GuideSortOrder_PADDING, GuideSortOrder_Y+GuideSortOrder_GAP+((GuideSortOrder_Option_H+GuideSortOrder_GAP)*item)-GuideSortOrder_PADDING, GuideSortOrder_Option_W, GuideSortOrder_Option_H, bc);
			switch( fntSize ){
				case FNT_Size_1419:
					Font_Osd_PutString1419( rgn, GuideSortOrder_X+GuideSortOrder_PADDING+GuideSortOrder_GAP, GuideSortOrder_Y-GuideSortOrder_PADDING+GuideSortOrder_GAP+((GuideSortOrder_Option_H+GuideSortOrder_GAP)*item)+pad_y, GuideSortOrder_X+GuideSortOrder_PADDING+GuideSortOrder_Option_W-GuideSortOrder_GAP, "Rating", fc, COLOR_None);
					break;
				case FNT_Size_1622:
					Font_Osd_PutString1622( rgn, GuideSortOrder_X+GuideSortOrder_PADDING+GuideSortOrder_GAP, GuideSortOrder_Y-GuideSortOrder_PADDING+GuideSortOrder_GAP+((GuideSortOrder_Option_H+GuideSortOrder_GAP)*item)+pad_y, GuideSortOrder_X+GuideSortOrder_PADDING+GuideSortOrder_Option_W-GuideSortOrder_GAP, "Rating", fc, COLOR_None);
					break;
				case FNT_Size_1926:
					Font_Osd_PutString1926( rgn, GuideSortOrder_X+GuideSortOrder_PADDING+GuideSortOrder_GAP, GuideSortOrder_Y-GuideSortOrder_PADDING+GuideSortOrder_GAP+((GuideSortOrder_Option_H+GuideSortOrder_GAP)*item)+pad_y, GuideSortOrder_X+GuideSortOrder_PADDING+GuideSortOrder_Option_W-GuideSortOrder_GAP, "Rating", fc, COLOR_None);
					break;
			}
			break;
	}
}

// Function to draw the yes no options to the region
void GuideSortOrder_DrawDetails( word rgn ){
	int i;
	for( i = 0; i < GUIDE_SORTTYPES; i++ ){
		GuideSortOrder_DrawItem( rgn, i, GuideSortOrder_Selected );
	}
}

// Function to set the colours of the Timer window
void GuideSortOrder_SetColours( dword fc, dword bc, dword fhc, dword bhc, dword base ){
	GuideSortOrder_Fc = fc;
	GuideSortOrder_HghBc = bhc;
	GuideSortOrder_HghFc = fhc;
	GuideSortOrder_Bc = bc;
	GuideSortOrder_BaseColour = base;
}

void GuideSortOrder_DrawBackground( word rgn ){
	if( GuideSortOrder_Base != NULL ) TAP_MemFree(GuideSortOrder_Base);
	GuideSortOrder_Base = TAP_Osd_SaveBox(GuideSortOrder_RGN, GuideSortOrder_X, GuideSortOrder_Y, GuideSortOrder_W, GuideSortOrder_H);	// Save what is under the Timer screen
	TAP_Osd_FillBox(rgn, GuideSortOrder_X, GuideSortOrder_Y, GuideSortOrder_W, GuideSortOrder_H, GuideSortOrder_BaseColour);
}


dword GuideSortOrder_KeyHandler( dword param1 ){
	if( param1 == RKEY_Exit ){
		GuideSortOrder_Deactivate(FALSE);
		return 0;
	}
	// Do the navigation stuff now
	if( param1 == RKEY_ChUp ){
		GuideSortOrder_PrevSelected = GuideSortOrder_Selected;
		GuideSortOrder_Selected--;
		if( GuideSortOrder_Selected < SORTFAVPRIORITY ){
			GuideSortOrder_Selected = GUIDE_SORTTYPES-1;
		}
		GuideSortOrder_DrawItem( GuideSortOrder_MemRGN, GuideSortOrder_PrevSelected, GuideSortOrder_Selected );
		GuideSortOrder_DrawItem( GuideSortOrder_MemRGN, GuideSortOrder_Selected, GuideSortOrder_Selected );
		TAP_Osd_Copy( GuideSortOrder_MemRGN, GuideSortOrder_RGN, GuideSortOrder_X, GuideSortOrder_Y, GuideSortOrder_W, GuideSortOrder_H, GuideSortOrder_X, GuideSortOrder_Y, FALSE);
	}
	// Do the navigation stuff now
	if( param1 == RKEY_ChDown ){
		GuideSortOrder_PrevSelected = GuideSortOrder_Selected;
		GuideSortOrder_Selected++;
		if( GuideSortOrder_Selected > GUIDE_SORTTYPES-1 ){
			GuideSortOrder_Selected = SORTFAVPRIORITY;
		}
		GuideSortOrder_DrawItem( GuideSortOrder_MemRGN, GuideSortOrder_PrevSelected, GuideSortOrder_Selected );
		GuideSortOrder_DrawItem( GuideSortOrder_MemRGN, GuideSortOrder_Selected, GuideSortOrder_Selected );
		TAP_Osd_Copy( GuideSortOrder_MemRGN, GuideSortOrder_RGN, GuideSortOrder_X, GuideSortOrder_Y, GuideSortOrder_W, GuideSortOrder_H, GuideSortOrder_X, GuideSortOrder_Y, FALSE);
	}
	if( param1 == RKEY_Ok ){
		GuideSortOrder_Deactivate(TRUE);
	}
	return 0;
}
