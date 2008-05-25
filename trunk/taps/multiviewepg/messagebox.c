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

#include "display.h"
#include "menu.h"
#include "favmanage.h"
#include "gridepg.h"
#include "messagebox.h"

// Function to render the item in the array on the screen
void Message_Display( char * text ){
	dword fc = DISPLAY_EVENT;
	dword bc = DISPLAY_MAIN;
	int x = Panel_Left_X;
	int y = Panel_Left_Y + (( DISPLAY_ITEM_H + DISPLAY_GAP ) * 10);
	if( !Display_BackgroundDrawn ) return;
	if( y > 575 ) return;
	if( x > 719 ) return;
	if( y < 0 ) return;
	if( x < 0 ) return;
	if( Message_Base == NULL ){	// Only save if we have nothing already save. Will help with multiple displays before timeout
		if( Guide_Rendered ){	// Guide is onscreen at the mo so send the key to its handler
			Message_Base = TAP_Osd_SaveBox(Display_RGN, x, y, DISPLAY_ITEM_W, (( DISPLAY_ITEM_H + DISPLAY_GAP ) * 5));	// Save what is under the box
		} else {
			if( GridEPG_Active ){
				Message_Base = TAP_Osd_SaveBox(Display_RGN, Panel_Left_X, Panel_Left_Y + (( GRIDEPG_ROWHEIGHT + DISPLAY_GAP ) * (GRIDEPG_CHANNELS)), 720-((Panel_Left_X-DISPLAY_X_OFFSET)*2)-100, (428+Panel_Left_Y)-(Panel_Left_Y + (( GRIDEPG_ROWHEIGHT + DISPLAY_GAP ) * (GRIDEPG_CHANNELS))));	// Save what is under the box
			} else {
				Message_Base = TAP_Osd_SaveBox(Display_RGN, Panel_Left_X, Panel_Left_Y + (( DISPLAY_ITEM_H + DISPLAY_GAP ) * (FavManage_MaxLIST)), 720-((Panel_Left_X-DISPLAY_X_OFFSET)*2), (( DISPLAY_ITEM_H + DISPLAY_GAP ) * 1));	// Save what is under the box
			// Not the gfuide so we need to set the location for timer, and favourites
			}
		}
	}
	
	if( Guide_Rendered ){	// Guide is onscreen at the mo so send the key to its handler
		// Render the display at the bottom
		TAP_Osd_FillBox(Display_MemRGN, x, y, DISPLAY_ITEM_W, (( DISPLAY_ITEM_H + DISPLAY_GAP ) * 5), DISPLAY_MAIN );
		// Allow for a really long message	
		WrapPutStr(Display_MemRGN, text, x+DISPLAY_GAP+DISPLAY_GAP, y+DISPLAY_GAP, DISPLAY_ITEM_W-(DISPLAY_GAP+DISPLAY_GAP), fc, bc, 4, FNT_Size_1622, 5);
		TAP_Osd_Copy(Display_MemRGN, Display_RGN, x, y, DISPLAY_ITEM_W, (( DISPLAY_ITEM_H + DISPLAY_GAP ) * 5), x, y, FALSE );
	} else {
		if( GridEPG_Active ){
			// Render the display at the bottom
			TAP_Osd_FillBox(Display_MemRGN, Panel_Left_X, Panel_Left_Y + (( GRIDEPG_ROWHEIGHT + DISPLAY_GAP ) * (GRIDEPG_CHANNELS)), 720-((Panel_Left_X-DISPLAY_X_OFFSET)*2)-100, (428+Panel_Left_Y)-(Panel_Left_Y + (( GRIDEPG_ROWHEIGHT + DISPLAY_GAP ) * (GRIDEPG_CHANNELS))), DISPLAY_MAIN );
			// Allow for a really long message	
			WrapPutStr(Display_MemRGN, text, x+DISPLAY_GAP+DISPLAY_GAP, DISPLAY_GAP+Panel_Left_Y + (( GRIDEPG_ROWHEIGHT + DISPLAY_GAP ) * (GRIDEPG_CHANNELS)), 720-((Panel_Left_X-DISPLAY_X_OFFSET)*2)-(DISPLAY_GAP+DISPLAY_GAP)-150, fc, bc, 3, FNT_Size_1622, 5);
			TAP_Osd_Copy(Display_MemRGN, Display_RGN, Panel_Left_X, Panel_Left_Y + (( GRIDEPG_ROWHEIGHT + DISPLAY_GAP ) * (GRIDEPG_CHANNELS)), 720-((Panel_Left_X-DISPLAY_X_OFFSET)*2)-100, (428+Panel_Left_Y)-(Panel_Left_Y + (( GRIDEPG_ROWHEIGHT + DISPLAY_GAP ) * (GRIDEPG_CHANNELS))), Panel_Left_X, Panel_Left_Y + (( GRIDEPG_ROWHEIGHT + DISPLAY_GAP ) * (GRIDEPG_CHANNELS)),FALSE );
		} else {
			// Render the display at the bottom
			TAP_Osd_FillBox(Display_MemRGN, Panel_Left_X, Panel_Left_Y + (( DISPLAY_ITEM_H + DISPLAY_GAP ) * (FavManage_MaxLIST)), 720-((Panel_Left_X-DISPLAY_X_OFFSET)*2), (( DISPLAY_ITEM_H + DISPLAY_GAP ) * 1), DISPLAY_MAIN );
			// Allow for a really long message	
			WrapPutStr(Display_MemRGN, text, x+DISPLAY_GAP+DISPLAY_GAP, Panel_Left_Y + (( DISPLAY_ITEM_H + DISPLAY_GAP ) * (FavManage_MaxLIST))+DISPLAY_GAP, 720-((Panel_Left_X-DISPLAY_X_OFFSET)*2)-(DISPLAY_GAP+DISPLAY_GAP), fc, bc, 2, FNT_Size_1622, 5);
			TAP_Osd_Copy(Display_MemRGN, Display_RGN, Panel_Left_X, Panel_Left_Y + (( DISPLAY_ITEM_H + DISPLAY_GAP ) * (FavManage_MaxLIST)), 720-((Panel_Left_X-DISPLAY_X_OFFSET)*2), (( DISPLAY_ITEM_H + DISPLAY_GAP ) * 1),	Panel_Left_X, Panel_Left_Y + (( DISPLAY_ITEM_H + DISPLAY_GAP ) * (FavManage_MaxLIST)),FALSE );
		}
	}
	Message_ClearTicks = TAP_GetTick();
}

void Message_DisplayClear( void ){
	int x = Panel_Left_X;
	int y = Panel_Left_Y + (( DISPLAY_ITEM_H + DISPLAY_GAP ) * 10);
	if( Menu_Active || !Display_BackgroundDrawn || Guide_ExtShowing ){
		TAP_MemFree(Message_Base);
		Message_Base = NULL;
		return;
	}
	if( y > 575 ) return;
	if( x > 719 ) return;
	if( y < 0 ) return;
	if( x < 0 ) return;
	if( Message_Base ){
		if( Guide_Rendered ){	// Guide is onscreen at the mo so send the key to its handler
			TAP_Osd_RestoreBox(Display_RGN,x, y, DISPLAY_ITEM_W, (( DISPLAY_ITEM_H + DISPLAY_GAP ) * 5), Message_Base);
		} else {
			if( GridEPG_Active ){
				TAP_Osd_RestoreBox(Display_RGN, Panel_Left_X, Panel_Left_Y + (( GRIDEPG_ROWHEIGHT + DISPLAY_GAP ) * (GRIDEPG_CHANNELS)), 720-((Panel_Left_X-DISPLAY_X_OFFSET)*2)-100, (428+Panel_Left_Y)-(Panel_Left_Y + (( GRIDEPG_ROWHEIGHT + DISPLAY_GAP ) * (GRIDEPG_CHANNELS))), Message_Base);
			} else {
				TAP_Osd_RestoreBox(Display_RGN,Panel_Left_X, Panel_Left_Y + (( DISPLAY_ITEM_H + DISPLAY_GAP ) * (FavManage_MaxLIST)), 720-((Panel_Left_X-DISPLAY_X_OFFSET)*2), (( DISPLAY_ITEM_H + DISPLAY_GAP ) * 1), Message_Base);
			}
		}
		TAP_MemFree(Message_Base);
	}
	Message_Base = NULL;
	Message_ClearTicks = 0;
}

void Message_Idle( void ){
	if( Message_ClearTicks != 0 ){
		if( Message_ClearTicks + Setting_MessageTimeout < TAP_GetTick() ){
			Message_DisplayClear();
		}
	}
}

