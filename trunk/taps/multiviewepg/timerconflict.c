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

#include "timerconflict.h"
#include "fontman.h"
#include "timeredit.h"
#include "keyboard.h"
#include "timereditnew.h"
#include "yesnobox.h"
#include "display.h"
#include "tools.h"
#include "skin.h"

bool TimerConflict_IsActive( void ){
	return TimerConflict_Active;
}

void TimerConflict_Init( word rgn, word memrgn, int x, int y, int w, int h ){
	TimerConflict_RGN = rgn;
	TimerConflict_MemRGN = memrgn;
	TimerConflict_X = x;
	TimerConflict_Y = y;
	TimerConflict_W = w;
	TimerConflict_H = h;
	TimerConflict_Option_H = (TimerConflict_H/3)-DISPLAY_GAP-(TimerConflict_Help_H+TimerConflict_Label_H/3);
	TimerConflict_Option_W = TimerConflict_W;
	TimerConflict_Initalised = TRUE;
	TimerConflict_CreateGradients();
}

void TimerConflict_Activate( TYPE_TimerInfo newtimer, void (*ReturningProcedure)( bool ) ){
	if( !TimerConflict_Initalised ) return;
	TimerConflict_SetAttempt = newtimer;	// Store the timer we attempted to set but failed
	if( !TimerConflict_GetTimers() ) return;		// Get the 2 conflicting timers otherwise return
	TimerConflict_Selected = 0;
	TimerConflict_Callback = ReturningProcedure;
	TimerConflict_PrevSelected = 0;
	TimerConflict_Render();
	TimerConflict_Active = TRUE;
}

void TimerConflict_MemFree( void ){
	if (TimerConflict_ItemBase) TAP_MemFree(TimerConflict_ItemBase);
	if (TimerConflict_ItemHigh) TAP_MemFree(TimerConflict_ItemHigh);
	TimerConflict_ItemBase = NULL;
	TimerConflict_ItemHigh = NULL;
}

void TimerConflict_CreateGradients( void ){
	word rgn;
	TimerConflict_MemFree();
	if( Settings_GradientFactor > 0 ){
		rgn	= TAP_Osd_Create(0, 0, TimerConflict_Option_W, TimerConflict_Option_H, 0, OSD_Flag_MemRgn);
		// Main default item
		createGradient(rgn, TimerConflict_Option_W, TimerConflict_Option_H, Settings_GradientFactor-1, DISPLAY_ITEM);
		TimerConflict_ItemBase = TAP_Osd_SaveBox(rgn, 0, 0, TimerConflict_Option_W, TimerConflict_Option_H);
		createGradient(rgn, TimerConflict_Option_W, TimerConflict_Option_H, Settings_GradientFactor-1, DISPLAY_ITEMSELECTED);
		TimerConflict_ItemHigh = TAP_Osd_SaveBox(rgn, 0, 0, TimerConflict_Option_W, TimerConflict_Option_H);
		TAP_Osd_Delete(rgn);
	}
}


void TimerConflict_Deactivate( bool result ){
	if( TimerConflict_Base ){
		TAP_Osd_RestoreBox(TimerConflict_RGN, TimerConflict_X, TimerConflict_Y, TimerConflict_W, TimerConflict_H, TimerConflict_Base);
		TAP_MemFree(TimerConflict_Base);
	}
	TimerConflict_Base = NULL;
	TimerConflict_MemFree();
	TimerConflict_Active = FALSE;
	TimerConflict_Selected = 0;
	TimerConflict_PrevSelected = 0;
	TimerConflict_Callback(result);
}

void TimerConflict_TimerEditCallback( int retval, bool cancel ){
	int retval2;
	TAP_Osd_Copy( TimerConflict_RGN, TimerConflict_MemRGN, 0, 0, 720, 576, 0, 0, FALSE);
	if( retval == 0 ){	// Timer was updated so redraw
		TimerConflict_GetTimers();	// Reget the timers incase of timer activation
		TimerConflict_Redraw = TRUE;
		TimerConflict_Render();
		// Lets attempt to set the new timer. If successful then close this window as there is no longer a conflict
		retval2=TAP_Timer_Add(&TimerConflict_SetAttempt);
		if( retval2 == 0 ){	// We succeed so close window
			TimerConflict_Deactivate(TRUE);
		}
	}
}

void TimerConflict_TimerEditNewCallback( int retval, bool changed, TYPE_TimerInfo newtimer ){
	TAP_Osd_Copy( TimerConflict_RGN, TimerConflict_MemRGN, 0, 0, 720, 576, 0, 0, FALSE);
	if( !changed ) return;	// If user has quick the dialog then we are not needed
	if( retval == 0 ){	// Timer was updated so redraw
		TimerConflict_Deactivate(TRUE);	// we return that the timer setting adjustments worked so lets deactivate as there are no more conflicts
	} else {
		// We are called for the new timer only so upadte it with the returned value and rerender
		TimerConflict_SetAttempt = newtimer;
		// Update the conflicting timers incase the user has altered so that different timers conflict
		TimerConflict_GetTimers();
		// Redraw to update the display
		TimerConflict_Redraw = TRUE;
		TimerConflict_Render();
	}
}

dword TimerConflict_Keyhandler( dword param1 ){
	// Handle all the modules key requirements first
	if( TimerEdit_IsActive() ){
		return TimerEdit_KeyHandler(param1);
	}
	if( TimerEditNew_IsActive() ){
		return TimerEditNew_KeyHandler(param1);
	}
	if( YesNo_IsActive() ){
		return YesNo_KeyHandler(param1);
	}
	if( Keyboard_IsActive() ){	// Handle the keyboard
		return Keyboard_Keyhandler(param1);
	}
	if( param1 == RKEY_Exit ){
		TimerConflict_Deactivate(FALSE);
		return 0;
	}
	// Do the navigation stuff now
	if( param1 == RKEY_ChUp ){
		TimerConflict_PrevSelected = TimerConflict_Selected;
		TimerConflict_Selected--;
		if( TimerConflict_Selected < 0 ){
			TimerConflict_Selected = TIMERCONFLICT_TYPES-1;
		}
		TimerConflict_RenderTimer( TimerConflict_MemRGN, TimerConflict_PrevSelected, TimerConflict_Selected );
		TimerConflict_RenderTimer( TimerConflict_MemRGN, TimerConflict_Selected, TimerConflict_Selected );
		TAP_Osd_Copy( TimerConflict_MemRGN, TimerConflict_RGN, TimerConflict_X, TimerConflict_Y, TimerConflict_W, TimerConflict_H, TimerConflict_X, TimerConflict_Y, FALSE);
	}
	// Do the navigation stuff now
	if( param1 == RKEY_ChDown ){
		TimerConflict_PrevSelected = TimerConflict_Selected;
		TimerConflict_Selected++;
		if( TimerConflict_Selected > TIMERCONFLICT_TYPES-1 ){
			TimerConflict_Selected = TIMERCONFLICT_1;
		}
		TimerConflict_RenderTimer( TimerConflict_MemRGN, TimerConflict_PrevSelected, TimerConflict_Selected );
		TimerConflict_RenderTimer( TimerConflict_MemRGN, TimerConflict_Selected, TimerConflict_Selected );
		TAP_Osd_Copy( TimerConflict_MemRGN, TimerConflict_RGN, TimerConflict_X, TimerConflict_Y, TimerConflict_W, TimerConflict_H, TimerConflict_X, TimerConflict_Y, FALSE);
	}
	if( param1 == RKEY_Ok ){
		// User selected to edit a timer. Will call the edit timer routine
		TimerEdit_Init( Display_RGN, Display_MemRGN,  Panel_Bottom_X, Panel_Bottom_Y, Panel_Bottom_W, Panel_Bottom_H );
		TimerEdit_SetColours( DISPLAY_KEYBOARDITEMTEXT, DISPLAY_ITEM, DISPLAY_KEYBOARDITEMSELECTEDTEXT, DISPLAY_KEYBOARDITEMSELECTED, DISPLAY_MAIN );
		switch( TimerConflict_Selected ){
			case TIMERCONFLICT_1:
				TimerEdit_ActivateWithTimer( TimerConflict_1, TimerConflict_1_Num, &TimerConflict_TimerEditCallback );
				break;
			case TIMERCONFLICT_2:
				TimerEdit_ActivateWithTimer( TimerConflict_2, TimerConflict_2_Num, &TimerConflict_TimerEditCallback );
				break;
			case TIMERCONFLICT_NEWTIMER:
				TimerEditNew_Init( Display_RGN, Display_MemRGN,  Panel_Bottom_X, Panel_Bottom_Y, Panel_Bottom_W, Panel_Bottom_H );
				TimerEditNew_SetColours( DISPLAY_KEYBOARDITEMTEXT, DISPLAY_ITEM, DISPLAY_KEYBOARDITEMSELECTEDTEXT, DISPLAY_KEYBOARDITEMSELECTED, DISPLAY_MAIN );
				TimerEditNew_Activate( TimerConflict_SetAttempt, &TimerConflict_TimerEditNewCallback );	
				break;
		}
	}
	if( param1 == RKEY_Ab ){
		switch( TimerConflict_Selected ){
			case TIMERCONFLICT_1:
			case TIMERCONFLICT_2:
				YesNo_Init( Display_RGN, Display_MemRGN,  Panel_Bottom_X, Panel_Bottom_Y+30, Panel_Bottom_W, Panel_Bottom_H-70 );
				YesNo_SetColours( DISPLAY_KEYBOARDITEMTEXT, DISPLAY_ITEM, DISPLAY_KEYBOARDITEMSELECTEDTEXT, DISPLAY_KEYBOARDITEMSELECTED, DISPLAY_MAIN );
				if( !YesNo_Activate("Delete selected timer?",&TimerConflict_DeleteCallback) ){
					Message_Display( "YesNo box failed." );	// Explained that the event has past and we can set because of that
				}
				break;
			case TIMERCONFLICT_NEWTIMER:
				TimerConflict_Deactivate(TRUE);	// Close the conflict module as the user has elected to delete the new timer
				break;
		}
	}
	return 0;
}

void TimerConflict_DeleteCallback( bool yes ){
	int retval2;
	bool returnVal = FALSE;
	if( yes ){
		// Check for changes in the timer list. If so we need to reget the conflicting timers and there number of we will edit/delete the wrong timer	
		if( TimerConflict_TimerNums != TAP_Timer_GetTotalNum() ){
			if( !TimerConflict_GetTimers() ){
				TimerConflict_RenderTimer( TimerConflict_MemRGN, TIMERCONFLICT_1, TimerConflict_Selected );
				TimerConflict_RenderTimer( TimerConflict_MemRGN, TIMERCONFLICT_2, TimerConflict_Selected );
				TAP_Osd_Copy( TimerConflict_MemRGN, TimerConflict_RGN, TimerConflict_X, TimerConflict_Y, TimerConflict_W, TimerConflict_H, TimerConflict_X, TimerConflict_Y, FALSE);
				return;
			}
		}
		switch( TimerConflict_Selected ){
			case TIMERCONFLICT_1:
				returnVal = TAP_Timer_Delete( TimerConflict_1_Num );
				break;
			case TIMERCONFLICT_2:
				returnVal = TAP_Timer_Delete( TimerConflict_2_Num );
				break;
		}
		// If we failed to delete then return that it wasn't resolved
		if( !returnVal ){
			TimerConflict_Deactivate(FALSE);
			return;
		}
		// Set the set attempt timer and return success or failure of this. Given user deleted onflicting timer should be successful the majority of times
		retval2=TAP_Timer_Add(&TimerConflict_SetAttempt);
		if( retval2 == 0 ){	// We succeed so close window
			TimerConflict_Deactivate(TRUE);
		} else {
			if( retval2 == 1 || retval2 == 2 ){
				TimerConflict_Deactivate(FALSE);	// failed so inform the user
			} else {
				// we failed so there is still a conflict
				TimerConflict_GetTimers();	// Reget the timers incase of timer activation
				// Redraw to update the display
				TimerConflict_RenderTimer( TimerConflict_MemRGN, TIMERCONFLICT_1, TimerConflict_Selected );
				TimerConflict_RenderTimer( TimerConflict_MemRGN, TIMERCONFLICT_2, TimerConflict_Selected );
				TAP_Osd_Copy( TimerConflict_MemRGN, TimerConflict_RGN, TimerConflict_X, TimerConflict_Y, TimerConflict_W, TimerConflict_H, TimerConflict_X, TimerConflict_Y, FALSE);
			}
		}
	}
}

void TimerConflict_RenderTimer( word rgn, int timer, int selected ){
	char text[128];
	switch( timer ){
		case TIMERCONFLICT_1:
			if( timer == selected ){
				if( TimerConflict_ItemHigh != NULL ){
					TAP_Osd_RestoreBox( rgn, TimerConflict_X, TimerConflict_Y+TimerConflict_Label_H, TimerConflict_W, (TimerConflict_H/3)-DISPLAY_GAP-(TimerConflict_Help_H+TimerConflict_Label_H/3), TimerConflict_ItemHigh );
				} else {
					TAP_Osd_FillBox(rgn, TimerConflict_X, TimerConflict_Y+TimerConflict_Label_H, TimerConflict_W, (TimerConflict_H/3)-DISPLAY_GAP-(TimerConflict_Help_H+TimerConflict_Label_H/3), DISPLAY_ITEMSELECTED);
				}
			} else {
				if( TimerConflict_ItemBase != NULL ){
					TAP_Osd_RestoreBox( rgn, TimerConflict_X, TimerConflict_Y+TimerConflict_Label_H, TimerConflict_W, (TimerConflict_H/3)-DISPLAY_GAP-(TimerConflict_Help_H+TimerConflict_Label_H/3), TimerConflict_ItemBase );
				} else {
					TAP_Osd_FillBox(rgn, TimerConflict_X, TimerConflict_Y+TimerConflict_Label_H, TimerConflict_W, (TimerConflict_H/3)-DISPLAY_GAP-(TimerConflict_Help_H+TimerConflict_Label_H/3), DISPLAY_ITEM);
				}
			}
//			if( selected == timer ){
//				TAP_Osd_FillBox(rgn, TimerConflict_X, TimerConflict_Y+TimerConflict_Label_H, TimerConflict_W, (TimerConflict_H/3)-DISPLAY_GAP-(TimerConflict_Help_H+TimerConflict_Label_H/3), DISPLAY_ITEMSELECTED);
//			} else {
//				TAP_Osd_FillBox(rgn, TimerConflict_X, TimerConflict_Y+TimerConflict_Label_H, TimerConflict_W, (TimerConflict_H/3)-DISPLAY_GAP-(TimerConflict_Help_H+TimerConflict_Label_H/3), DISPLAY_ITEM);
//			}
			Font_Osd_PutString1622( rgn, TimerConflict_X+DISPLAY_GAP, TimerConflict_Y+5+TimerConflict_Label_H, TimerConflict_X-DISPLAY_GAP+TimerConflict_W, TimerConflict_1.fileName, DISPLAY_EVENT, COLOR_None);
			Font_Osd_PutString1622( rgn, TimerConflict_X+DISPLAY_GAP, TimerConflict_Y+5+25+TimerConflict_Label_H, TimerConflict_X-DISPLAY_GAP+TimerConflict_W, Channel_Name( TimerConflict_1.svcType, TimerConflict_1.svcNum ), DISPLAY_EVENT, COLOR_None);
			TAP_SPrint( text, "%s, %s ~ %s, %3d mins", Time_DOWDDMMSpec( TimerConflict_1.startTime ), Time_HHMM(TimerConflict_1.startTime), Endtime_HHMM(TimerConflict_1.startTime,TimerConflict_1.duration), TimerConflict_1.duration);			
			Font_Osd_PutString1622( rgn, TimerConflict_X+DISPLAY_GAP, TimerConflict_Y+5+50+TimerConflict_Label_H, TimerConflict_X-DISPLAY_GAP+TimerConflict_W, text, DISPLAY_EVENT, COLOR_None);
			break;
		case TIMERCONFLICT_2:
			if( timer == selected ){
				if( TimerConflict_ItemHigh != NULL ){
					TAP_Osd_RestoreBox( rgn, TimerConflict_X, TimerConflict_Y+(TimerConflict_H/3)-(TimerConflict_Help_H+TimerConflict_Label_H/3)+TimerConflict_Label_H, TimerConflict_W, (TimerConflict_H/3)-DISPLAY_GAP-(TimerConflict_Help_H+TimerConflict_Label_H/3), TimerConflict_ItemHigh );
				} else {
					TAP_Osd_FillBox(rgn, TimerConflict_X, TimerConflict_Y+(TimerConflict_H/3)-(TimerConflict_Help_H+TimerConflict_Label_H/3)+TimerConflict_Label_H, TimerConflict_W, (TimerConflict_H/3)-DISPLAY_GAP-(TimerConflict_Help_H+TimerConflict_Label_H/3), DISPLAY_ITEMSELECTED);
				}
			} else {
				if( TimerConflict_ItemBase != NULL ){
					TAP_Osd_RestoreBox( rgn, TimerConflict_X, TimerConflict_Y+(TimerConflict_H/3)-(TimerConflict_Help_H+TimerConflict_Label_H/3)+TimerConflict_Label_H, TimerConflict_W, (TimerConflict_H/3)-DISPLAY_GAP-(TimerConflict_Help_H+TimerConflict_Label_H/3), TimerConflict_ItemBase );
				} else {
					TAP_Osd_FillBox(rgn, TimerConflict_X, TimerConflict_Y+(TimerConflict_H/3)-(TimerConflict_Help_H+TimerConflict_Label_H/3)+TimerConflict_Label_H, TimerConflict_W, (TimerConflict_H/3)-DISPLAY_GAP-(TimerConflict_Help_H+TimerConflict_Label_H/3), DISPLAY_ITEM);
				}
			}
//			if( selected == timer ){
//				TAP_Osd_FillBox(rgn, TimerConflict_X, TimerConflict_Y+(TimerConflict_H/3)-(TimerConflict_Help_H+TimerConflict_Label_H/3)+TimerConflict_Label_H, TimerConflict_W, (TimerConflict_H/3)-DISPLAY_GAP-(TimerConflict_Help_H+TimerConflict_Label_H/3), DISPLAY_ITEMSELECTED);
//			} else {
//				TAP_Osd_FillBox(rgn, TimerConflict_X, TimerConflict_Y+(TimerConflict_H/3)-(TimerConflict_Help_H+TimerConflict_Label_H/3)+TimerConflict_Label_H, TimerConflict_W, (TimerConflict_H/3)-DISPLAY_GAP-(TimerConflict_Help_H+TimerConflict_Label_H/3), DISPLAY_ITEM);
//			}
			Font_Osd_PutString1622( rgn, TimerConflict_X+DISPLAY_GAP, TimerConflict_Y+(TimerConflict_H/3)-(TimerConflict_Help_H+TimerConflict_Label_H/3)+5+TimerConflict_Label_H, TimerConflict_X-DISPLAY_GAP+TimerConflict_W, TimerConflict_2.fileName, DISPLAY_EVENT, COLOR_None);
			Font_Osd_PutString1622( rgn, TimerConflict_X+DISPLAY_GAP, TimerConflict_Y+(TimerConflict_H/3)-(TimerConflict_Help_H+TimerConflict_Label_H/3)+5+25+TimerConflict_Label_H, TimerConflict_X-DISPLAY_GAP+TimerConflict_W, Channel_Name( TimerConflict_2.svcType, TimerConflict_2.svcNum ), DISPLAY_EVENT, COLOR_None);
			TAP_SPrint( text, "%s, %s ~ %s, %3d mins", Time_DOWDDMMSpec( TimerConflict_2.startTime ), Time_HHMM(TimerConflict_2.startTime), Endtime_HHMM(TimerConflict_2.startTime,TimerConflict_2.duration), TimerConflict_2.duration);			
			Font_Osd_PutString1622( rgn, TimerConflict_X+DISPLAY_GAP, TimerConflict_Y+(TimerConflict_H/3)-(TimerConflict_Help_H+TimerConflict_Label_H/3)+5+50+TimerConflict_Label_H, TimerConflict_X-DISPLAY_GAP+TimerConflict_W, text, DISPLAY_EVENT, COLOR_None);
			break;
		case TIMERCONFLICT_NEWTIMER:
			if( timer == selected ){
				if( TimerConflict_ItemHigh != NULL ){
					TAP_Osd_RestoreBox( rgn, TimerConflict_X, TimerConflict_Y+((TimerConflict_H/3)*2)-((TimerConflict_Help_H+TimerConflict_Label_H/3)*2)+(TimerConflict_Label_H*2), TimerConflict_W, (TimerConflict_H/3)-DISPLAY_GAP-(TimerConflict_Help_H+TimerConflict_Label_H/3), TimerConflict_ItemHigh );
				} else {
					TAP_Osd_FillBox(rgn, TimerConflict_X, TimerConflict_Y+((TimerConflict_H/3)*2)-((TimerConflict_Help_H+TimerConflict_Label_H/3)*2)+(TimerConflict_Label_H*2), TimerConflict_W, (TimerConflict_H/3)-DISPLAY_GAP-(TimerConflict_Help_H+TimerConflict_Label_H/3), DISPLAY_ITEMSELECTED);
				}
			} else {
				if( TimerConflict_ItemBase != NULL ){
					TAP_Osd_RestoreBox( rgn, TimerConflict_X, TimerConflict_Y+((TimerConflict_H/3)*2)-((TimerConflict_Help_H+TimerConflict_Label_H/3)*2)+(TimerConflict_Label_H*2), TimerConflict_W, (TimerConflict_H/3)-DISPLAY_GAP-(TimerConflict_Help_H+TimerConflict_Label_H/3), TimerConflict_ItemBase );
				} else {
					TAP_Osd_FillBox(rgn, TimerConflict_X, TimerConflict_Y+((TimerConflict_H/3)*2)-((TimerConflict_Help_H+TimerConflict_Label_H/3)*2)+(TimerConflict_Label_H*2), TimerConflict_W, (TimerConflict_H/3)-DISPLAY_GAP-(TimerConflict_Help_H+TimerConflict_Label_H/3), DISPLAY_ITEM);
				}
			}
//			if( selected == timer ){
//				TAP_Osd_FillBox(rgn, TimerConflict_X, TimerConflict_Y+((TimerConflict_H/3)*2)-((TimerConflict_Help_H+TimerConflict_Label_H/3)*2)+(TimerConflict_Label_H*2), TimerConflict_W, (TimerConflict_H/3)-DISPLAY_GAP-(TimerConflict_Help_H+TimerConflict_Label_H/3), DISPLAY_ITEMSELECTED);
//			} else {
//				TAP_Osd_FillBox(rgn, TimerConflict_X, TimerConflict_Y+((TimerConflict_H/3)*2)-((TimerConflict_Help_H+TimerConflict_Label_H/3)*2)+(TimerConflict_Label_H*2), TimerConflict_W, (TimerConflict_H/3)-DISPLAY_GAP-(TimerConflict_Help_H+TimerConflict_Label_H/3), DISPLAY_ITEM);
//			}		
			Font_Osd_PutString1622( rgn, TimerConflict_X+DISPLAY_GAP, TimerConflict_Y+((TimerConflict_H/3)*2)-((TimerConflict_Help_H+TimerConflict_Label_H/3)*2)+5+(TimerConflict_Label_H*2), TimerConflict_X-DISPLAY_GAP+TimerConflict_W, TimerConflict_SetAttempt.fileName, DISPLAY_EVENT, COLOR_None);
			Font_Osd_PutString1622( rgn, TimerConflict_X+DISPLAY_GAP, TimerConflict_Y+((TimerConflict_H/3)*2)-((TimerConflict_Help_H+TimerConflict_Label_H/3)*2)+5+25+(TimerConflict_Label_H*2), TimerConflict_X-DISPLAY_GAP+TimerConflict_W, Channel_Name( TimerConflict_SetAttempt.svcType,TimerConflict_SetAttempt.svcNum ), DISPLAY_EVENT, COLOR_None);
			TAP_SPrint( text, "%s, %s ~ %s, %3d mins", Time_DOWDDMMSpec( TimerConflict_SetAttempt.startTime ), Time_HHMM(TimerConflict_SetAttempt.startTime), Endtime_HHMM(TimerConflict_SetAttempt.startTime,TimerConflict_SetAttempt.duration), TimerConflict_SetAttempt.duration);			
			Font_Osd_PutString1622( rgn, TimerConflict_X+DISPLAY_GAP, TimerConflict_Y+((TimerConflict_H/3)*2)-((TimerConflict_Help_H+TimerConflict_Label_H/3)*2)+5+50+(TimerConflict_Label_H*2), TimerConflict_X-DISPLAY_GAP+TimerConflict_W, text, DISPLAY_EVENT, COLOR_None);
			break;
	}
}

void TimerConflict_DrawBackground( word rgn ){
	if( !TimerConflict_Redraw ){	// If we are not redrawing then save
		if( TimerConflict_Base != NULL ) TAP_MemFree(TimerConflict_Base);
		TimerConflict_Base = TAP_Osd_SaveBox(TimerConflict_RGN, TimerConflict_X, TimerConflict_Y, TimerConflict_W, TimerConflict_H);	// Save what is under the Timer screen
	}
	TAP_Osd_FillBox(rgn, TimerConflict_X, TimerConflict_Y, TimerConflict_W, TimerConflict_H, DISPLAY_MAIN);
	Font_Osd_PutString1622( rgn, TimerConflict_X+DISPLAY_GAP, TimerConflict_Y+DISPLAY_GAP, TimerConflict_W-(2*DISPLAY_GAP), "Conflicting Timers", DISPLAY_EVENT, COLOR_None);
	Font_Osd_PutString1622( rgn, TimerConflict_X+DISPLAY_GAP, (TimerConflict_Y+((TimerConflict_H/3)*2)-((TimerConflict_Help_H+TimerConflict_Label_H/3)*2)+(TimerConflict_Label_H*2))-TimerConflict_Label_H+3, TimerConflict_W-(2*DISPLAY_GAP), "Attempted Timer", DISPLAY_EVENT, COLOR_None);
	// Lets render the help at the bottom of the screen

	TAP_Osd_PutGd( rgn, TimerConflict_X+DISPLAY_GAP, (TimerConflict_Y+((TimerConflict_H/3)*3)-((TimerConflict_Help_H+TimerConflict_Label_H/3)*3)+(TimerConflict_Label_H*3))-TimerConflict_Label_H+5, &_channelupGd, TRUE );	// Draw highlighted
	TAP_Osd_PutGd(rgn, TimerConflict_X+DISPLAY_GAP+22, (TimerConflict_Y+((TimerConflict_H/3)*3)-((TimerConflict_Help_H+TimerConflict_Label_H/3)*3)+(TimerConflict_Label_H*3))-TimerConflict_Label_H+5, &_channeldownGd, TRUE );	// Draw highlighted
	Font_Osd_PutString1419( rgn, TimerConflict_X+DISPLAY_GAP+44, (TimerConflict_Y+((TimerConflict_H/3)*3)-((TimerConflict_Help_H+TimerConflict_Label_H/3)*3)+(TimerConflict_Label_H*3))-TimerConflict_Label_H+5, TimerConflict_W-(2*DISPLAY_GAP), "Navigation", DISPLAY_TITLETEXT, COLOR_None);

	TAP_Osd_PutGd( rgn, TimerConflict_X+DISPLAY_GAP+4, (TimerConflict_Y+((TimerConflict_H/3)*3)-((TimerConflict_Help_H+TimerConflict_Label_H/3)*3)+(TimerConflict_Label_H*3))-TimerConflict_Label_H+5+24, &_okGd, TRUE );	// Draw highlighted
	Font_Osd_PutString1419( rgn, TimerConflict_X+DISPLAY_GAP+30, (TimerConflict_Y+((TimerConflict_H/3)*3)-((TimerConflict_Help_H+TimerConflict_Label_H/3)*3)+(TimerConflict_Label_H*3))-TimerConflict_Label_H+5+22, TimerConflict_W-(2*DISPLAY_GAP), "Edit timer", DISPLAY_TITLETEXT, COLOR_None);

	TAP_Osd_PutGd( rgn, TimerConflict_X+DISPLAY_GAP+4, (TimerConflict_Y+((TimerConflict_H/3)*3)-((TimerConflict_Help_H+TimerConflict_Label_H/3)*3)+(TimerConflict_Label_H*3))-TimerConflict_Label_H+5+44, &_whiteGd, TRUE );	// Draw highlighted
	Font_Osd_PutString1419( rgn, TimerConflict_X+DISPLAY_GAP+30, (TimerConflict_Y+((TimerConflict_H/3)*3)-((TimerConflict_Help_H+TimerConflict_Label_H/3)*3)+(TimerConflict_Label_H*3))-TimerConflict_Label_H+5+44, TimerConflict_W-(2*DISPLAY_GAP), "Delete selected timer", DISPLAY_TITLETEXT, COLOR_None);
	
	TAP_Osd_PutGd( rgn, TimerConflict_X+DISPLAY_GAP, (TimerConflict_Y+((TimerConflict_H/3)*3)-((TimerConflict_Help_H+TimerConflict_Label_H/3)*3)+(TimerConflict_Label_H*3))-TimerConflict_Label_H+5+66, &_exitGd, TRUE );	// Draw highlighted
	Font_Osd_PutString1419( rgn, TimerConflict_X+DISPLAY_GAP+30, (TimerConflict_Y+((TimerConflict_H/3)*3)-((TimerConflict_Help_H+TimerConflict_Label_H/3)*3)+(TimerConflict_Label_H*3))-TimerConflict_Label_H+5+66, TimerConflict_W-(2*DISPLAY_GAP), "Cancel setting of new timer", DISPLAY_TITLETEXT, COLOR_None);
}


void TimerConflict_Render( void ){
	int i;
	TimerConflict_DrawBackground(TimerConflict_MemRGN);
	for( i = 0; i < TIMERCONFLICT_TYPES; i++ ){
		TimerConflict_RenderTimer( TimerConflict_MemRGN, i, TimerConflict_Selected );
	}
	TAP_Osd_Copy( TimerConflict_MemRGN, TimerConflict_RGN, TimerConflict_X, TimerConflict_Y, TimerConflict_W, TimerConflict_H, TimerConflict_X, TimerConflict_Y, FALSE);
	TimerConflict_Redraw = FALSE;
}

bool TimerConflict_GetTimers( void  ){
	TYPE_TimerInfo	currentTimer;
	int totalTimers = 0;
	int i;
	word year;
	byte month, day, weekDay;
	TYPE_TapChInfo	currentChInfo;
	bool returnVal = FALSE;
	TYPE_TimerInfo	overlapTimers[4];
	int overlapTimersNum[4];
	int timerOverlap_ptr = 0;
	int x, y;

	// Manually search for overlap timers. Will only be 2 timers as API would have prevented any more
	totalTimers = TAP_Timer_GetTotalNum();
	TimerConflict_TimerNums = totalTimers;
	for ( i = 0; i < totalTimers; i++ ){
		TAP_Timer_GetInfo(i, &currentTimer); // Sequentially get timer details and compare with those in the display range
		TAP_ExtractMjd( currentTimer.startTime>>16, &year, &month, &day, &weekDay);
		TAP_Channel_GetInfo( currentTimer.svcType, currentTimer.svcNum, &currentChInfo );
		if( currentTimer.svcNum != TimerConflict_SetAttempt.svcNum ){	// If they are on the same channel then they don't overlap
			if( 
			// [ currentTimer ]
			//       [ SetAttempt ]
				(TimerConflict_SetAttempt.startTime >= currentTimer.startTime && 
				TimerConflict_SetAttempt.startTime <= AddTime(currentTimer.startTime, currentTimer.duration))
				||
			//               [ Timer ]
			// [ Attempted timer ]
				( TimerConflict_SetAttempt.startTime <= currentTimer.startTime &&
				AddTime(TimerConflict_SetAttempt.startTime, TimerConflict_SetAttempt.duration) >= currentTimer.startTime)
				||
			// [        Timer        ]
			//   [ Attempted timer ]
				( TimerConflict_SetAttempt.startTime >= currentTimer.startTime &&
				AddTime(TimerConflict_SetAttempt.startTime, TimerConflict_SetAttempt.duration) <= AddTime(currentTimer.startTime, currentTimer.duration))
			){
				// Find all timers that overlap the attempted one to see which timers overlap of them
				if( timerOverlap_ptr < sizeof(overlapTimers) ){
					overlapTimers[timerOverlap_ptr] = currentTimer;
					overlapTimersNum[timerOverlap_ptr] = i;
					timerOverlap_ptr++;
				}
			}
		}
	}
	if( timerOverlap_ptr > 0 ){	// We have some timers
		// Loop thru each timer and compare against a loop of others. Break when we find the first overlap
		for( x = 0; x < timerOverlap_ptr; x++ ){
			for( y = 0; y < timerOverlap_ptr; y++ ){
				if( x != y ){	// Not the same item
					if( 
					// [ currentTimer ]
					//       [ SetAttempt ]
						(overlapTimers[x].startTime >= overlapTimers[y].startTime && 
						overlapTimers[x].startTime <= AddTime(overlapTimers[y].startTime, overlapTimers[y].duration))
						||
					//               [ Timer ]
					// [ Attempted timer ]
						( overlapTimers[x].startTime <= overlapTimers[y].startTime &&
						AddTime(overlapTimers[x].startTime, overlapTimers[x].duration) >= overlapTimers[y].startTime)
						||
					// [        Timer        ]
					//   [ Attempted timer ]
						( overlapTimers[x].startTime >= overlapTimers[y].startTime &&
						AddTime(overlapTimers[x].startTime, overlapTimers[x].duration) <= AddTime(overlapTimers[y].startTime, overlapTimers[y].duration))
					){
						// These timers overlap so lets store and break the loop
						TimerConflict_1 = overlapTimers[x];
						TimerConflict_1_Num = overlapTimersNum[x];
						// Store timer 2
						TimerConflict_2 = overlapTimers[y];
						TimerConflict_2_Num = overlapTimersNum[y];
						// Break the loops
						returnVal = TRUE;
						x = y = timerOverlap_ptr;
					}		
				}
			}
		}
	}
	return returnVal;
}
