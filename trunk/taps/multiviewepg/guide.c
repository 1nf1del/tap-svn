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
#include "fontman.h"
#include "guidesortorder.h"
#include "skin.h"
#include "settings.h"
#include "timerconflict.h"
#include "extinfo.h"
#include "timereditsmall.h"
#include "yesnobox.h"
#include "threeoption.h"
#include "menu.h"
#include "guide.h"
#include "menu.h"

bool Guide_isActive( void ){
	return Guide_Rendered;
}


// Desc: This routine will redraw the display after the defined period of inactiveness

void Guide_Idle( void ){
	if( Guide_isActive() ){
		if( lastPress + OSD_RedrawDelay < TAP_GetTick() ){
			if( Guide_LastRendered + OSD_RedrawDelay < TAP_GetTick() ){
				Guide_Redraw = TRUE;
				Guide_RenderDisplay();
			}
		}
	}
}


// Desc: This routine reset the location to the first element

void Guide_ResetLoc( void ){
	Guide_SelectedRow = 0;
	Guide_MinRow=0;
	Guide_MaxRow=GUIDE_ITEMS;
}

// Desc: This routine frees the memory being used by the save boxes in the creation of the display

void Guide_MemFree( void ){
	if (Guide_BaseLin) TAP_MemFree(Guide_BaseLin);
	if (Guide_HighLin) TAP_MemFree(Guide_HighLin);
	if (Guide_BaseRight) TAP_MemFree(Guide_BaseRight);
	if (Guide_PartialBase) TAP_MemFree(Guide_PartialBase);
	if (Guide_RecBase) TAP_MemFree(Guide_RecBase);
	if (Guide_PartialHigh) TAP_MemFree(Guide_PartialHigh);
	if (Guide_RecHigh) TAP_MemFree(Guide_RecHigh);
	if (Guide_RecNotAval) TAP_MemFree(Guide_RecNotAval);
	if (Guide_PartialNotAval) TAP_MemFree(Guide_PartialNotAval);
	Guide_BaseLin = NULL;
	Guide_HighLin = NULL;
	Guide_BaseRight = NULL;
	Guide_PartialBase = NULL;
	Guide_RecBase = NULL;
	Guide_PartialHigh = NULL;
	Guide_RecHigh = NULL;
	Guide_RecNotAval = NULL;
	Guide_PartialNotAval = NULL;
}

// Desc: This routine is for the creation of the gradients for this component

void Guide_CreateGradients( void ){
	word rgn;
	Guide_MemFree();
	if( Settings_GradientFactor > 0 ){
		rgn	= TAP_Osd_Create(0, 0, DISPLAY_ITEM_W, DISPLAY_ITEM_H, 0, OSD_Flag_MemRgn);
		// Default
		createGradient(rgn, DISPLAY_ITEM_W, DISPLAY_ITEM_H, Settings_GradientFactor, DISPLAY_ITEM);
		Guide_BaseLin = TAP_Osd_SaveBox(rgn, 0, 0, DISPLAY_ITEM_W, DISPLAY_ITEM_H);
		Guide_BaseRight = TAP_Osd_SaveBox(rgn, 0, 0, Panel_Bottom_W, DISPLAY_ITEM_H);
		// Highlighted
		createGradient(rgn, DISPLAY_ITEM_W, DISPLAY_ITEM_H, Settings_GradientFactor, DISPLAY_ITEMSELECTED);
		Guide_HighLin = TAP_Osd_SaveBox(rgn, 0, 0, DISPLAY_ITEM_W, DISPLAY_ITEM_H);
		TAP_Osd_Delete(rgn);
	}
}

// Desc: Routine to render the title of the guide with repect to the sorting of the events

void Guide_RenderTitle( void ){
	switch( sortOrder ){
		case SORT_NAME:
			Display_RenderTitle("Program Guide [ A-Z ]");
			break;
		case SORT_CHANNEL:
			Display_RenderTitle("Program Guide [ Channel ]");
			break;
		case SORT_START:
			Display_RenderTitle("Program Guide [ Start Times ]");
			break;
		case SORT_RATING:
			Display_RenderTitle("Program Guide [ Rating ]");
			break;
		default:
			Display_RenderTitle( "Program Guide" );
			break;
	}
}

// Desc: Routine to render a blank highlighted item at the defined location
// IN:
//     rgn - Region to render to
//     position - location of element to render

void Guide_RenderBlankHighItem( word rgn, int position ){
	int x_pos = 0;
	int y_pos = 0;
	x_pos = Panel_Left_X;
	y_pos = Panel_Left_Y + (( DISPLAY_ITEM_H + DISPLAY_GAP ) * (position));
	// Sanity Check
	if( y_pos > 575 ) return;
	if( x_pos > 719 ) return;
	if( y_pos < 0 ) return;
	if( x_pos < 0 ) return;
	if( Guide_HighLin != NULL ){
		TAP_Osd_RestoreBox( rgn, x_pos, y_pos, DISPLAY_ITEM_W, DISPLAY_ITEM_H, Guide_HighLin );
	} else {
		TAP_Osd_FillBox(rgn, x_pos, y_pos, DISPLAY_ITEM_W, DISPLAY_ITEM_H, DISPLAY_ITEM);
	}
}

// Desc: Routine to render a blank item at the location
// IN:
//     rgn - Region to render to
//     position - location of element to render

void Guide_RenderBlankItem( word rgn, int position ){
	int x_pos = 0;
	int y_pos = 0;
	x_pos = Panel_Left_X;
	y_pos = Panel_Left_Y + (( DISPLAY_ITEM_H + DISPLAY_GAP ) * (position));
	// Sanity Check
	if( y_pos > 575 ) return;
	if( x_pos > 719 ) return;
	if( y_pos < 0 ) return;
	if( x_pos < 0 ) return;
	if( Guide_BaseLin != NULL ){
		TAP_Osd_RestoreBox( rgn, x_pos, y_pos, DISPLAY_ITEM_W, DISPLAY_ITEM_H, Guide_BaseLin );
	} else {
		TAP_Osd_FillBox(rgn, x_pos, y_pos, DISPLAY_ITEM_W, DISPLAY_ITEM_H, DISPLAY_ITEM);
	}
}

// Desc: Routine to render the description area blank of the vent in the bottom panel..
// IN:
//     row - Pointer to the event in the guideevents array

void Guide_RenderBlankEventDetails( void ){
	int x = Panel_Left_X;
	int y = Panel_Left_Y + (( DISPLAY_ITEM_H + DISPLAY_GAP ) * 10)+2;
	if( Menu_Active ) return;
	if( y > 575 ) return;
	if( x > 719 ) return;
	if( y < 0 ) return;
	if( x < 0 ) return;
	TAP_Osd_FillBox(Display_MemRGN, x, y, DISPLAY_ITEM_W, (( DISPLAY_ITEM_H + DISPLAY_GAP ) * 4), DISPLAY_MAIN );
	TAP_Osd_Copy( Display_MemRGN, Display_RGN, x, y, DISPLAY_ITEM_W, (( DISPLAY_ITEM_H + DISPLAY_GAP ) * 4), x, y, FALSE);
}

// Desc: Routine to render the description of the event in the bottom panel..
// IN:
//     row - Pointer to the event in the guideevents array

void Guide_RenderEventDetails( int row ){
	int x = Panel_Left_X;
	int y = Panel_Left_Y + (( DISPLAY_ITEM_H + DISPLAY_GAP ) * 10)+2;
	char text[512];
	char *desc;
	dword fc = DISPLAY_EVENT;
	dword bc = DISPLAY_MAIN;
	TYPE_TapEvent event;
	if( Menu_Active ) return;
	if( row < 0 ){
		Guide_RenderBlankEventDetails();
		return;
	}
	if( y > 575 ) return;
	if( x > 719 ) return;
	if( y < 0 ) return;
	if( x < 0 ) return;
	event = Guide_GetEvent(row);
	memset(text, '\0', sizeof(text ) );
	// Fill the display with the background colour
	TAP_Osd_FillBox(Display_MemRGN, x, y, DISPLAY_ITEM_W, (( DISPLAY_ITEM_H + DISPLAY_GAP ) * 4), DISPLAY_MAIN );
	// Place the channel and event name in the box
	TAP_SPrint( text, "%s - %s", Channel_Name( GetSvcType( event.svcId ), GetSvcNum(event.svcId) ), event.eventName);//,Parental_Value(event.parentalRating));	// Get the channel name and place in local text array for printing
	Font_Osd_PutString1622( Display_MemRGN, x+DISPLAY_GAP+DISPLAY_GAP, y+DISPLAY_GAP+DISPLAY_GAP, x+DISPLAY_ITEM_W-(DISPLAY_GAP+DISPLAY_GAP), text, DISPLAY_EPGTIMES, bc );
	// Description extender support
	if( PatchIsInstalled((dword*) 0x80000000, "De") || strlen(event.description) > 128 ){
		desc = event.eventName + event.description[127];
		strcpy(text,desc);
		if( strlen(text) > 0 ){
			WrapPutStr(Display_MemRGN, text, x+DISPLAY_GAP+DISPLAY_GAP, y+DISPLAY_GAP+(DISPLAY_ITEM_H*1)+2, DISPLAY_ITEM_W-((DISPLAY_GAP+DISPLAY_GAP)*2), fc, bc, 2, FNT_Size_1622, 3);
		} else {
			WrapPutStr(Display_MemRGN, "Not avaliable", x+DISPLAY_GAP+DISPLAY_GAP, y+DISPLAY_GAP+(DISPLAY_ITEM_H*1)+2, DISPLAY_ITEM_W-((DISPLAY_GAP+DISPLAY_GAP)*2), fc, bc, 2, FNT_Size_1622, 3);
		}
	} else {
		if( strlen(event.description) > 0 ){
			WrapPutStr(Display_MemRGN, event.description, x+DISPLAY_GAP+DISPLAY_GAP, y+DISPLAY_GAP+(DISPLAY_ITEM_H*1)+2, DISPLAY_ITEM_W-((DISPLAY_GAP+DISPLAY_GAP)*2), fc, bc, 2, FNT_Size_1622, 3);
		} else {
			WrapPutStr(Display_MemRGN, "Not avaliable", x+DISPLAY_GAP+DISPLAY_GAP, y+DISPLAY_GAP+(DISPLAY_ITEM_H*1)+2, DISPLAY_ITEM_W-((DISPLAY_GAP+DISPLAY_GAP)*2), fc, bc, 2, FNT_Size_1622, 3);
		}
	}
	// Print the start, end and duration
	TAP_SPrint( text, "%s, %s ~ %s, %3d mins", Time_DOWDDMMSpec( event.startTime ), Time_HHMM(event.startTime), Endtime_HHMM(event.startTime,TimeDiff(event.startTime,event.endTime)), TimeDiff(event.startTime,event.endTime));
	Font_Osd_PutString1622( Display_MemRGN, x+DISPLAY_GAP+DISPLAY_GAP, LastWrapPutStr_Y+3, x+DISPLAY_ITEM_W-(DISPLAY_GAP+DISPLAY_GAP), text, DISPLAY_EPGTIMES, bc );
	// Copy from the memory region to the foreground
	TAP_Osd_Copy( Display_MemRGN, Display_RGN, x, y, DISPLAY_ITEM_W, (( DISPLAY_ITEM_H + DISPLAY_GAP ) * 4), x, y, FALSE);
}

// Desc: Routine to render a event in the list on the left hand side of the display
// IN:
//     rgn - Region to render to
//     row - Pointer to the event to render
//     selected - The number of the item that is selected
//     prevselected - The number of the previous selected item
//     fresh - Whether to render the row fresh or only if there has been a change

void Guide_RenderItem( word rgn, int row, int selected, int prevselected, bool fresh ){
	int x_pos = 0;
	int y_pos = 0;
	int x_add = 0;
	char text[128];
	TYPE_TapChInfo channelInfo;
	int channelNetId;
	int rec1NetId, rec2NetId;
	dword fc = DISPLAY_EVENT;
	dword bc = DISPLAY_ITEM;
	TYPE_TapEvent event;
	if( row < 0 ) return;
	memset( text, '\0', sizeof(text) );
	event = Guide_GetEvent(row);
	x_pos = Panel_Left_X;
	y_pos = Panel_Left_Y + (( DISPLAY_ITEM_H + DISPLAY_GAP ) * (row-Guide_MinRow));
	// Sanity Check
	if( y_pos > 575 ) return;
	if( x_pos > 719 ) return;
	if( y_pos < 0 ) return;
	if( x_pos < 0 ) return;
	// Check for whether event is current
	if( event.startTime <= Now(0) && event.endTime >= Now(0) ){ fc = DISPLAY_NOWEVENT; }
	if( event.endTime <= Now(0) ){ fc = DISPLAY_EVENTPAST; }
	// Check whether tuner is avaliable and render in grey if not accessible
	if( HDD_isRecording(0) && HDD_isRecording(1) ){
		// Get the svc information to compare the transponder/freq information
		Update_RecordingInfo( 0 );
		Update_RecordingInfo( 1 );
		TAP_Channel_GetInfo( Guide_GetEventSvcType(Guide_SelectedRow), Guide_GetEventSvcNum(Guide_SelectedRow), &channelInfo );
		channelNetId = channelInfo.orgNetId;
		// Lets compare with the running recordings
		TAP_Channel_GetInfo( Recording_GetRecInfo(0).svcType, Recording_GetRecInfo(0).svcNum, &channelInfo );
		rec1NetId = channelInfo.orgNetId;
		TAP_Channel_GetInfo( Recording_GetRecInfo(1).svcType, Recording_GetRecInfo(1).svcNum, &channelInfo );
		rec2NetId = channelInfo.orgNetId;
		if( rec1NetId != channelNetId &&
			rec2NetId != channelNetId ){
			bc = DISPLAY_CHANNELNOTAVAL;
		}
	}
	// Render the background for a selected item
	if( row == selected ){
		if( Guide_HighLin != NULL ){
			TAP_Osd_RestoreBox( rgn, x_pos, y_pos, DISPLAY_ITEM_W, DISPLAY_ITEM_H, Guide_HighLin );
		} else {
			TAP_Osd_FillBox(rgn, x_pos, y_pos, DISPLAY_ITEM_W, DISPLAY_ITEM_H, DISPLAY_ITEMSELECTED);
		}
		// Render the round circles indicating a timer
		switch( Timers_HasTimer( Guide_GetEventPointerRow(row) ) ){
			case TIMER_FULL:
				if( Guide_RecHigh != NULL ){
					TAP_Osd_RestoreBox( rgn, x_pos, y_pos, 20, DISPLAY_ITEM_H, Guide_RecHigh );
				}
				x_add = (5+5+5+5);
				break;
			case TIMER_PARTIAL:
				if( Guide_PartialHigh != NULL ){
					TAP_Osd_RestoreBox( rgn, x_pos, y_pos, 20, DISPLAY_ITEM_H, Guide_PartialHigh );
				}
				x_add = (5+5+5+5);
				break;
		}
	} else {
		if( bc == DISPLAY_CHANNELNOTAVAL ){
			// If the channel cannot be view because of timers then render difference colour to indicate this
			TAP_Osd_FillBox(rgn, x_pos, y_pos, DISPLAY_ITEM_W, DISPLAY_ITEM_H, DISPLAY_CHANNELNOTAVAL);
			// Check for full or partial and render appropriatly
			switch( Timers_HasTimer( Guide_GetEventPointerRow(row) ) ){
				case TIMER_FULL:
					if( Guide_RecNotAval != NULL ){
						TAP_Osd_RestoreBox( rgn, x_pos, y_pos, 20, DISPLAY_ITEM_H, Guide_RecNotAval );
					}
					x_add = (5+5+5+5);
					break;
				case TIMER_PARTIAL:
					if( Guide_PartialNotAval != NULL ){
						TAP_Osd_RestoreBox( rgn, x_pos, y_pos, 20, DISPLAY_ITEM_H, Guide_PartialNotAval );
					}
					x_add = (5+5+5+5);
					break;
			}
		} else {
			if( row == prevselected ){
				if( Guide_BaseLin != NULL ){
					TAP_Osd_RestoreBox( rgn, x_pos, y_pos, DISPLAY_ITEM_W, DISPLAY_ITEM_H, Guide_BaseLin );
				} else {
					TAP_Osd_FillBox(rgn, x_pos, y_pos, DISPLAY_ITEM_W, DISPLAY_ITEM_H, DISPLAY_ITEM);
				}
				// Check for full or partial and render appropriatly
				switch( Timers_HasTimer( Guide_GetEventPointerRow(row) ) ){
					case TIMER_FULL:
						if( Guide_RecBase != NULL ){
							TAP_Osd_RestoreBox( rgn, x_pos, y_pos, 20, DISPLAY_ITEM_H, Guide_RecBase );
						}
						x_add = (5+5+5+5);
						break;
					case TIMER_PARTIAL:
						if( Guide_PartialBase != NULL ){
							TAP_Osd_RestoreBox( rgn, x_pos, y_pos, 20, DISPLAY_ITEM_H, Guide_PartialBase );
						}
						x_add = (5+5+5+5);
						break;
				}
			} else {
				if( fresh ){
					if( Guide_BaseLin != NULL ){
						TAP_Osd_RestoreBox( rgn, x_pos, y_pos, DISPLAY_ITEM_W, DISPLAY_ITEM_H, Guide_BaseLin );
					} else {
						TAP_Osd_FillBox(rgn, x_pos, y_pos, DISPLAY_ITEM_W, DISPLAY_ITEM_H, DISPLAY_ITEM);
					}
					// Check for full or partial and render appropriatly
					switch( Timers_HasTimer( Guide_GetEventPointerRow(row) ) ){
						case TIMER_FULL:
							if( Guide_RecBase != NULL ){
								TAP_Osd_RestoreBox( rgn, x_pos, y_pos, 20, DISPLAY_ITEM_H, Guide_RecBase );
							}
							x_add = (5+5+5+5);
							break;
						case TIMER_PARTIAL:
							if( Guide_PartialBase != NULL ){
								TAP_Osd_RestoreBox( rgn, x_pos, y_pos, 20, DISPLAY_ITEM_H, Guide_PartialBase );
							}
							x_add = (5+5+5+5);
							break;
					}
				} else {
					// Check for full or partial and render appropriatly
					switch( Timers_HasTimer( Guide_GetEventPointerRow(row) ) ){
						case TIMER_FULL:
							if( Guide_RecBase != NULL ){
								TAP_Osd_RestoreBox( rgn, x_pos, y_pos, 20, DISPLAY_ITEM_H, Guide_RecBase );
							}
							x_add = (5+5+5+5);
							break;
						case TIMER_PARTIAL:
							if( Guide_PartialBase != NULL ){
								TAP_Osd_RestoreBox( rgn, x_pos, y_pos, 20, DISPLAY_ITEM_H, Guide_PartialBase );
							}
							x_add = (5+5+5+5);
							break;
					}
				}
			}
		}
	}
	if( Settings_LinearEPGTimes ){
		sprintf( text, "%s - %s\0", Time_HHMM(event.startTime), event.eventName);
	} else {
		// Lets place the text on the item now
		sprintf( text, "%s\0", event.eventName);
	}
	Font_Osd_PutString1622( rgn, x_pos+DISPLAY_GAP+DISPLAY_GAP+x_add, y_pos + 3, x_pos + DISPLAY_ITEM_W - DISPLAY_GAP - x_add, text, fc, COLOR_None );
}


void Guide_RenderNextTimers( word rgn ){
	TYPE_TimerInfo	currentTimer;
	TYPE_TimerInfo	tmp;
	TYPE_TimerInfo timers[70];
	int timers_ptr = 0;
	int totalTimers = 0;
	int i, j;
	totalTimers = TAP_Timer_GetTotalNum();
	for ( i = 0; i < totalTimers; i++ ){
		if( TAP_Timer_GetInfo(i, &currentTimer) ){
			timers[timers_ptr] = currentTimer;
			timers_ptr++;
		}
	}
	// Quick bubble sort
	for( i = 0; i < timers_ptr; i++ ){
		for( j = 0; j < timers_ptr-1; j++ ){
			if( i != j ){
				if( timers[i].startTime < timers[j].startTime ){
					tmp = timers[i];
					timers[i] = timers[j];
					timers[j] = tmp;
				}
			}
		}
	}
	// Render background
	TAP_Osd_FillBox(rgn, Panel_Top_X, Panel_Top_Y, Panel_Top_W, (( DISPLAY_ITEM_H + DISPLAY_GAP ) * (6))+DISPLAY_GAP+1, DISPLAY_MAIN);
	// Render the title of this section	
	TAP_Osd_FillBox(rgn, Panel_Top_X, Panel_Top_Y + (( DISPLAY_ITEM_H + DISPLAY_GAP ) * (0)), Panel_Top_W, DISPLAY_ITEM_H, DISPLAY_MAIN);	
	Font_Osd_PutString1622( rgn, Panel_Top_X+5, Panel_Top_Y + (( DISPLAY_ITEM_H + DISPLAY_GAP ) * (0))+3, Panel_Top_X+Panel_Top_W, "Next Timers", DISPLAY_TITLETEXT, DISPLAY_MAIN );
	for( i = 1; i < timers_ptr && i < 6; i++ ){
		if( Guide_BaseRight != NULL ){
			TAP_Osd_RestoreBox( rgn, Panel_Top_X, Panel_Top_Y + (( DISPLAY_ITEM_H + DISPLAY_GAP ) * (i)), Panel_Top_W, DISPLAY_ITEM_H, Guide_BaseRight );
		} else {
			TAP_Osd_FillBox(rgn, Panel_Top_X, Panel_Top_Y + (( DISPLAY_ITEM_H + DISPLAY_GAP ) * (i)), Panel_Top_W, DISPLAY_ITEM_H, DISPLAY_ITEM);
		}
		// Lets add the start time and the name of the timer to the display
		StripFileName(timers[i-1].fileName);
		Font_Osd_PutString1622( rgn, Panel_Top_X+5, Panel_Top_Y + (( DISPLAY_ITEM_H + DISPLAY_GAP ) * (i))+3, Panel_Top_X+Panel_Top_W, timers[i-1].fileName, DISPLAY_EVENT, COLOR_None );
	}
}

void Guide_RenderNextTimersInBottomCount( word rgn, int num ){
	TYPE_TimerInfo	currentTimer;
	TYPE_TimerInfo	tmp;
	TYPE_TimerInfo timers[70];
	int timers_ptr = 0;
	int totalTimers = 0;
	int i, j;
	// Manually search for overlap timers. Will only be 2 timers as API would have prevented any more
	totalTimers = TAP_Timer_GetTotalNum();
	for ( i = 0; i < totalTimers; i++ ){
		if( TAP_Timer_GetInfo(i, &currentTimer) ){ // Sequentially get timer details and compare with those in the display range
			timers[timers_ptr] = currentTimer;
			timers_ptr++;
		}
	}
	// Quick bubble sort
	for( i = 0; i < timers_ptr; i++ ){
		for( j = 0; j < timers_ptr-1; j++ ){
			if( i != j ){
				if( timers[i].startTime < timers[j].startTime ){
					tmp = timers[i];
					timers[i] = timers[j];
					timers[j] = tmp;
				}
			}
		}
	}
	// Render background
	TAP_Osd_FillBox(rgn, Panel_Bottom_X, Panel_Bottom_Y, Panel_Bottom_W, Panel_Bottom_H, DISPLAY_MAIN);
	// Render the title of this section	
	TAP_Osd_FillBox(rgn, Panel_Bottom_X, Panel_Bottom_Y + (( DISPLAY_ITEM_H + DISPLAY_GAP ) * (0)), Panel_Bottom_W, DISPLAY_ITEM_H, DISPLAY_MAIN);	
	Font_Osd_PutString1622( rgn, Panel_Bottom_X+5, Panel_Bottom_Y + (( DISPLAY_ITEM_H + DISPLAY_GAP ) * (0))+3, Panel_Bottom_X+Panel_Bottom_W, "Next Timers", DISPLAY_TITLETEXT, DISPLAY_MAIN );
	for( i = 1; i < timers_ptr && i < num; i++ ){
		if( Guide_BaseRight != NULL ){
			TAP_Osd_RestoreBox( rgn, Panel_Bottom_X, Panel_Bottom_Y + (( DISPLAY_ITEM_H + DISPLAY_GAP ) * (i)), Panel_Bottom_W, DISPLAY_ITEM_H, Guide_BaseRight );
		} else {
			TAP_Osd_FillBox(rgn, Panel_Bottom_X, Panel_Bottom_Y + (( DISPLAY_ITEM_H + DISPLAY_GAP ) * (i)), Panel_Bottom_W, DISPLAY_ITEM_H, DISPLAY_ITEM);
		}
		// Lets add the start time and the name of the timer to the display
		StripFileName(timers[i-1].fileName);
		Font_Osd_PutString1622( rgn, Panel_Bottom_X+5, Panel_Bottom_Y + (( DISPLAY_ITEM_H + DISPLAY_GAP ) * (i))+3, Panel_Bottom_X+Panel_Bottom_W, timers[i-1].fileName, DISPLAY_EVENT, COLOR_None );
	}
}

void Guide_RenderNextTimersInBottom( word rgn ){
	TYPE_TimerInfo	currentTimer;
	TYPE_TimerInfo	tmp;
	TYPE_TimerInfo timers[70];
	int timers_ptr = 0;
	int totalTimers = 0;
	int i, j;
	// Manually search for overlap timers. Will only be 2 timers as API would have prevented any more
	totalTimers = TAP_Timer_GetTotalNum();
	for ( i = 0; i < totalTimers; i++ ){
		if( TAP_Timer_GetInfo(i, &currentTimer) ){ // Sequentially get timer details and compare with those in the display range
			timers[timers_ptr] = currentTimer;
			timers_ptr++;
		}
	}
	// Quick bubble sort
	for( i = 0; i < timers_ptr; i++ ){
		for( j = 0; j < timers_ptr-1; j++ ){
			if( i != j ){
				if( timers[i].startTime < timers[j].startTime ){
					tmp = timers[i];
					timers[i] = timers[j];
					timers[j] = tmp;
				}
			}
		}
	}
	// Render background
	TAP_Osd_FillBox(rgn, Panel_Bottom_X, Panel_Bottom_Y, Panel_Bottom_W, Panel_Bottom_H, DISPLAY_MAIN);
	// Render the title of this section	
	TAP_Osd_FillBox(rgn, Panel_Bottom_X, Panel_Bottom_Y + (( DISPLAY_ITEM_H + DISPLAY_GAP ) * (0)), Panel_Bottom_W, DISPLAY_ITEM_H, DISPLAY_MAIN);	
	Font_Osd_PutString1622( rgn, Panel_Bottom_X+5, Panel_Bottom_Y + (( DISPLAY_ITEM_H + DISPLAY_GAP ) * (0))+3, Panel_Bottom_X+Panel_Bottom_W, "Next Timers", DISPLAY_TITLETEXT, DISPLAY_MAIN );
	for( i = 1; i < timers_ptr && i < 7; i++ ){
		if( Guide_BaseRight != NULL ){
			TAP_Osd_RestoreBox( rgn, Panel_Bottom_X, Panel_Bottom_Y + (( DISPLAY_ITEM_H + DISPLAY_GAP ) * (i)), Panel_Bottom_W, DISPLAY_ITEM_H, Guide_BaseRight );
		} else {
			TAP_Osd_FillBox(rgn, Panel_Bottom_X, Panel_Bottom_Y + (( DISPLAY_ITEM_H + DISPLAY_GAP ) * (i)), Panel_Bottom_W, DISPLAY_ITEM_H, DISPLAY_ITEM);
		}
		// Lets add the start time and the name of the timer to the display
		StripFileName(timers[i-1].fileName);
		Font_Osd_PutString1622( rgn, Panel_Bottom_X+5, Panel_Bottom_Y + (( DISPLAY_ITEM_H + DISPLAY_GAP ) * (i))+3, Panel_Bottom_X+Panel_Bottom_W, timers[i-1].fileName, DISPLAY_EVENT, COLOR_None );
	}
	Guide_ShowingTimers = TRUE;
}

void Guide_BuildCircles( void ){
	int x_pos = 0;
	int y_pos = 0;
	// Sort out the X position
	x_pos = Panel_Left_X;
	// Sort out the row for the item
	y_pos = Panel_Left_Y + (( DISPLAY_ITEM_H + DISPLAY_GAP ) * (0));
	// Sanity Check
	if( y_pos > 575 ) return;
	if( x_pos > 719 ) return;
	if( y_pos < 0 ) return;
	if( x_pos < 0 ) return;
	// ALter the text colour if required
	if( Guide_BaseLin != NULL ){
		TAP_Osd_RestoreBox( Display_RGN, x_pos, y_pos, DISPLAY_ITEM_W, DISPLAY_ITEM_H, Guide_BaseLin );
	} else {
		TAP_Osd_FillBox(Display_RGN, x_pos, y_pos, DISPLAY_ITEM_W, DISPLAY_ITEM_H, DISPLAY_ITEM);
	}
	drawDisk(Display_RGN, Panel_Left_X+5+5, y_pos+(DISPLAY_ITEM_H/2), 5, DISPLAY_RECORDINGPARTIAL);
	Guide_PartialBase = TAP_Osd_SaveBox(Display_RGN, x_pos, y_pos, 20, DISPLAY_ITEM_H);
	drawDisk(Display_RGN, Panel_Left_X+5+5, y_pos+(DISPLAY_ITEM_H/2), 5, DISPLAY_RECORDINGFULL);
	Guide_RecBase = TAP_Osd_SaveBox(Display_RGN, x_pos, y_pos, 20, DISPLAY_ITEM_H);
	if( Guide_HighLin != NULL ){
		TAP_Osd_RestoreBox( Display_RGN, x_pos, y_pos, DISPLAY_ITEM_W, DISPLAY_ITEM_H, Guide_HighLin );
	} else {
		TAP_Osd_FillBox(Display_RGN, x_pos, y_pos, DISPLAY_ITEM_W, DISPLAY_ITEM_H, DISPLAY_ITEM);
	}
	drawDisk(Display_RGN, Panel_Left_X+5+5, y_pos+(DISPLAY_ITEM_H/2), 5, DISPLAY_RECORDINGPARTIAL);
	Guide_PartialHigh = TAP_Osd_SaveBox(Display_RGN, x_pos, y_pos, 20, DISPLAY_ITEM_H);
	drawDisk(Display_RGN, Panel_Left_X+5+5, y_pos+(DISPLAY_ITEM_H/2), 5, DISPLAY_RECORDINGFULL);
	Guide_RecHigh = TAP_Osd_SaveBox(Display_RGN, x_pos, y_pos, 20, DISPLAY_ITEM_H);
	TAP_Osd_FillBox(Display_RGN, x_pos, y_pos, DISPLAY_ITEM_W, DISPLAY_ITEM_H, DISPLAY_CHANNELNOTAVAL);
	drawDisk(Display_RGN, Panel_Left_X+5+5, y_pos+(DISPLAY_ITEM_H/2), 5, DISPLAY_RECORDINGPARTIAL);
	Guide_PartialNotAval = TAP_Osd_SaveBox(Display_RGN, x_pos, y_pos, 20, DISPLAY_ITEM_H);
	drawDisk(Display_RGN, Panel_Left_X+5+5, y_pos+(DISPLAY_ITEM_H/2), 5, DISPLAY_RECORDINGFULL);
	Guide_RecNotAval = TAP_Osd_SaveBox(Display_RGN, x_pos, y_pos, 20, DISPLAY_ITEM_H);
}

void Guide_RenderDisplay( void ){
	int i;
	int c = 0;
	bool loadingNeeded = FALSE;
	Display_CreateRegions();
	if( !Display_BackgroundDrawn ){
		Screen_Set(Display_RGN);
		// No template so we need to redraw the display from scratch
		Display_DrawBackground();
		Guide_BuildCircles();
		// Render the template as the loading will take some time
		for( i = 0; i < GUIDE_ITEMS; i++ ){
			Guide_RenderBlankItem( Display_RGN, i );
		}
		// Draw the blank panel at the bottom
		if( Setting_PIG ){
			Display_PIGScale();		
		} else {
			Guide_RenderNextTimers(Display_RGN);
		}
		if( Guide_ShowingTimers ){
			Guide_RenderNextTimersInBottom(Display_RGN);
		} else {
			Guide_RenderHelp();
		}
		// Draw the time and scale the pig
		Guide_RenderTitle();
		Display_DrawTime();
		Guide_Rendered = TRUE;
		// Draw the template on the display by fading in
		if( !Guide_HasEvents || dataCollected != DATA_COLLECTED ){
			Screen_FadeIn(Display_RGN,Setting_FadeIn);
			loadingNeeded = TRUE;
		}
		if( dataCollected != DATA_COLLECTED ){
			Events_Get();
			Guide_HasEvents = FALSE;
		}
		Guide_PopulateList();	//Populate the mirror array that we will be using for this component
		Guide_MoveToCurrent();
		TAP_Osd_Copy( Display_RGN, Display_MemRGN, Panel_Left_X, Panel_Left_Y, DISPLAY_ITEM_W, Panel_Left_Y + (( DISPLAY_ITEM_H + DISPLAY_GAP ) * (GUIDE_ITEMS-1)), Panel_Left_X, Panel_Left_Y, FALSE);
		c = 0;
		for( i = Guide_MinRow; i < Guide_MaxRow; i++ ){
			Guide_RenderItem(Display_MemRGN,i,Guide_SelectedRow, Guide_SelectedPrevRow,FALSE);
			c++;
		}
		for( i = c; i < GUIDE_ITEMS; i++ ){
			Guide_RenderBlankItem( Display_MemRGN, i );
		}
		TAP_Osd_Copy( Display_MemRGN, Display_RGN, Panel_Left_X, Panel_Left_Y, DISPLAY_ITEM_W, Panel_Left_Y + (( DISPLAY_ITEM_H + DISPLAY_GAP ) * (GUIDE_ITEMS-1)), Panel_Left_X, Panel_Left_Y, FALSE);
		Guide_RenderTitle();
		Guide_RenderEventDetails(Guide_SelectedRow);
		if( !loadingNeeded ){
			Screen_FadeIn(Display_RGN,Setting_FadeIn);
		}
		// If we didn't need to laoding the events then we need to fade in the region as it would have been skipped earlier
	} else {
		// Background is rendered so lets check the range and determine what to do
		if( Guide_Redraw ){
			// Copy current into memory region to apply changes there then copy back. Hopefully reduce the flickering
			if( !Display_TitleDrawn ){
				Guide_RenderTitle();
			}
			if( dataCollected != DATA_COLLECTED ){
				Events_Get();
				Guide_HasEvents = FALSE;
			}
			Guide_PopulateList();	//Populate the mirror array that we will be using for this component
			Display_DrawTime();
			c = 0;
			for( i = Guide_MinRow; i < Guide_MaxRow; i++ ){
				Guide_RenderItem(Display_MemRGN,i,Guide_SelectedRow, Guide_SelectedPrevRow,TRUE);
				c++;
			}
			for( i = c; i < GUIDE_ITEMS; i++ ){
				Guide_RenderBlankItem( Display_MemRGN, i );
			}
			TAP_Osd_Copy( Display_MemRGN, Display_RGN, Panel_Left_X, Panel_Left_Y, DISPLAY_ITEM_W, Panel_Left_Y + (( DISPLAY_ITEM_H + DISPLAY_GAP ) * (GUIDE_ITEMS-1)), Panel_Left_X, Panel_Left_Y, FALSE);
			Guide_RenderEventDetails(Guide_SelectedRow);
			Guide_Redraw = FALSE;
		} else {
			// Only render previous item again and current item
			if( Guide_SelectedPrevRow == Guide_SelectedRow ){
				Guide_RenderItem(Display_MemRGN,Guide_SelectedRow,Guide_SelectedRow, Guide_SelectedPrevRow,FALSE);
				TAP_Osd_Copy( Display_MemRGN, Display_RGN, Panel_Left_X, Panel_Left_Y + (( DISPLAY_ITEM_H + DISPLAY_GAP ) * (Guide_SelectedRow-Guide_MinRow)), DISPLAY_ITEM_W, DISPLAY_ITEM_H, Panel_Left_X, Panel_Left_Y + (( DISPLAY_ITEM_H + DISPLAY_GAP ) * (Guide_SelectedRow-Guide_MinRow)), FALSE);
				Guide_RenderEventDetails(Guide_SelectedRow);
			} else {
				Guide_RenderItem(Display_MemRGN,Guide_SelectedPrevRow,Guide_SelectedRow, Guide_SelectedPrevRow,FALSE);
				Guide_RenderItem(Display_MemRGN,Guide_SelectedRow,Guide_SelectedRow, Guide_SelectedPrevRow,FALSE);
				// Copy to main display
				TAP_Osd_Copy( Display_MemRGN, Display_RGN, Panel_Left_X, Panel_Left_Y + (( DISPLAY_ITEM_H + DISPLAY_GAP ) * (Guide_SelectedPrevRow-Guide_MinRow)), DISPLAY_ITEM_W, DISPLAY_ITEM_H, Panel_Left_X, Panel_Left_Y + (( DISPLAY_ITEM_H + DISPLAY_GAP ) * (Guide_SelectedPrevRow-Guide_MinRow)), FALSE);
				TAP_Osd_Copy( Display_MemRGN, Display_RGN, Panel_Left_X, Panel_Left_Y + (( DISPLAY_ITEM_H + DISPLAY_GAP ) * (Guide_SelectedRow-Guide_MinRow)), DISPLAY_ITEM_W, DISPLAY_ITEM_H, Panel_Left_X, Panel_Left_Y + (( DISPLAY_ITEM_H + DISPLAY_GAP ) * (Guide_SelectedRow-Guide_MinRow)), FALSE);
				Guide_RenderEventDetails(Guide_SelectedRow);
			}
		}
	}
	Guide_LastRendered = TAP_GetTick();
	Guide_Rendered = TRUE;
}

// HELP PANEL
void Guide_RenderHelp( void ){
	if( Keyboard_IsActive() ) return;
	// Fill the background
	TAP_Osd_FillBox(Display_MemRGN, Panel_Bottom_X, Panel_Bottom_Y, Panel_Bottom_W, Panel_Bottom_H, DISPLAY_MAIN);
	
	// If we are display the PIG then existing timers are not on screen so render the next 3		
	if( Setting_PIG ){
		Guide_RenderNextTimersInBottomCount(Display_MemRGN, 4);
	} else {
		// Need a fill of something for alternate display
	}
	// Draw the keyboard and Timer edit help
	TAP_Osd_PutGd( Display_MemRGN, Panel_Bottom_X+5, Panel_Bottom_Y+((22)*(6))+7, &_redGd, TRUE );	// Draw highlighted
	Font_Osd_PutString1419( Display_MemRGN, Panel_Bottom_X+30, Panel_Bottom_Y+((22)*(6))+7, Panel_Bottom_X-5+Panel_Bottom_W-5, "Search Title", DISPLAY_EVENT, COLOR_None);
	TAP_Osd_PutGd( Display_MemRGN, Panel_Bottom_X+5+115, Panel_Bottom_Y+((22)*(6))+7, &_greenGd, TRUE );	// Draw highlighted
	Font_Osd_PutString1419( Display_MemRGN, Panel_Bottom_X+30+115, Panel_Bottom_Y+((22)*(6))+7, Panel_Bottom_X-5+Panel_Bottom_W-5, "Search Desc", DISPLAY_EVENT, COLOR_None);
	// Rewind/Forward
	TAP_Osd_PutGd( Display_MemRGN, Panel_Bottom_X+5, Panel_Bottom_Y+((22)*(7))+7, &_rewindGd, TRUE );	// Draw highlighted
	TAP_Osd_PutGd( Display_MemRGN, Panel_Bottom_X+5+23, Panel_Bottom_Y+((22)*(7))+7, &_forwardGd, TRUE );	// Draw highlighted
	Font_Osd_PutString1419( Display_MemRGN, Panel_Bottom_X+52, Panel_Bottom_Y+((22)*(7))+7, Panel_Bottom_X-5+Panel_Bottom_W-5, "Channel/Character", DISPLAY_EVENT, COLOR_None);		
	// Pause, Record and White
	TAP_Osd_PutGd( Display_MemRGN, Panel_Bottom_X+5, Panel_Bottom_Y+((22)*(8))+7, &_pauseGd, TRUE );	// Draw highlighted
	Font_Osd_PutString1419( Display_MemRGN, Panel_Bottom_X+30, Panel_Bottom_Y+((22)*(8))+7, Panel_Bottom_X-5+Panel_Bottom_W-5, "Sort Order", DISPLAY_EVENT, COLOR_None);
	TAP_Osd_PutGd( Display_MemRGN, Panel_Bottom_X+30+70+5, Panel_Bottom_Y+((22)*(8))+7, &_recordGd, TRUE );	// Draw highlighted
	Font_Osd_PutString1419( Display_MemRGN, Panel_Bottom_X+60+70, Panel_Bottom_Y+((22)*(8))+7, Panel_Bottom_X-5+Panel_Bottom_W-5, "Record", DISPLAY_EVENT, COLOR_None);
	TAP_Osd_PutGd( Display_MemRGN, Panel_Bottom_X+30+150+5, Panel_Bottom_Y+((22)*(8))+7, &_whiteGd, TRUE );	// Draw highlighted
	Font_Osd_PutString1419( Display_MemRGN, Panel_Bottom_X+60+150, Panel_Bottom_Y+((22)*(8))+7, Panel_Bottom_X-5+Panel_Bottom_W-5, "Delete", DISPLAY_EVENT, COLOR_None);
	// Teletext and Fav
	TAP_Osd_PutGd( Display_MemRGN, Panel_Bottom_X+5, Panel_Bottom_Y+((22)*(9))+7, &_teletextGd, TRUE );	// Draw highlighted
	Font_Osd_PutString1419( Display_MemRGN, Panel_Bottom_X+35, Panel_Bottom_Y+((22)*(9))+7, Panel_Bottom_X-5+Panel_Bottom_W-5, "Edit Timer", DISPLAY_EVENT, COLOR_None);
	TAP_Osd_PutGd( Display_MemRGN, Panel_Bottom_X+30+80, Panel_Bottom_Y+((22)*(9))+7, &_favGd, TRUE );	// Draw highlighted
	Font_Osd_PutString1419( Display_MemRGN, Panel_Bottom_X+60+80, Panel_Bottom_Y+((22)*(9))+7, Panel_Bottom_X-5+Panel_Bottom_W-5, "New Favourite", DISPLAY_EVENT, COLOR_None);
	TAP_Osd_Copy( Display_MemRGN, Display_RGN, Panel_Bottom_X, Panel_Bottom_Y, Panel_Bottom_W, Panel_Bottom_H, Panel_Bottom_X, Panel_Bottom_Y, FALSE);
	Guide_ShowingTimers = FALSE;
}

// SORT ORDER DIALOG
void Guide_SortOrderCallback( bool yes, int selected ){
	TAP_Osd_Copy( Display_RGN, Display_MemRGN, 0, 0, 720, 576, 0, 0, FALSE);	// Copy the main region to memory to avoid corruption from the previous OSD
	if( yes ){
		switch( selected ){
			case GUIDE_SORTNAME:
				Guide_SortByName();
				break;
			case GUIDE_SORTCHANNEL:
				Guide_SortByChannel();
				break;
			case GUIDE_SORTSTART:
				Guide_SortByStart();
				break;
			case GUIDE_SORTRATING:
				Guide_SortByRating();
				break;
		}
		Guide_Redraw = TRUE;
		Guide_RenderTitle();
		Guide_RenderDisplay();
	}
}

// NAVIGATION FUNCTIONS

// function to change to channel
void Guide_ChangeChannel( int row ){
	if( Update_Pi() ){
		if( pi.playMode != PLAYMODE_Playing ){
			TAP_Channel_Start( 1, Guide_GetEventSvcType(row), GetSvcNum(Guide_GetEventSvcId(row)) );
		} else {
			Message_Display( "Please stop playback to change channel." );
		}
	}
}

void Guide_MoveToPrevChar( char * character, int selected ){
	int i;
	int position = -1;
	TYPE_TapEvent event;
	for( i = selected-1; i > -1; i-- ){
		event = Guide_GetEvent(i);
		if( strncmp(event.eventName,character, 1) != 0){
			position = i;
			i = guideevents_ptr;
		}
	}
	if( position < 0 ){
		position = guideevents_ptr-1;
	}
	if( position >= 0 ){
		Guide_SelectedPrevRow = position;
		Guide_SelectedRow= position;
		Guide_MinRow=position;
		Guide_MaxRow=position+GUIDE_ITEMS;
		if( Guide_MaxRow > guideevents_ptr ){
			Guide_MaxRow = guideevents_ptr;
			Guide_MinRow = Guide_MaxRow - GUIDE_ITEMS;
			if( Guide_MinRow < 0 ){
				Guide_MinRow = 0;
			}
		}
		Guide_Redraw = TRUE;
		Guide_RenderDisplay();
	}
}

void Guide_MoveToNextChar( char * character, int selected ){
	char text[128];
	int i;
	int position = -1;
	TYPE_TapEvent event;
	memset( text, '\0', sizeof(text) );
	for( i = selected; i < guideevents_ptr; i++ ){
		event = Guide_GetEvent(i);
		if( strncmp(event.eventName,character, 1) != 0){
			position = i;
			i = guideevents_ptr;
		}
	}
	if( position >= 0 ){
//		event = Guide_GetEvent(position);
//		TAP_SPrint( text, "Moved to \"%s\"\0", event.eventName);
		Guide_SelectedPrevRow = position;
		Guide_SelectedRow= position;
		Guide_MinRow=position;
		Guide_MaxRow=position+GUIDE_ITEMS;
		if( Guide_MaxRow > guideevents_ptr ){
			Guide_MaxRow = guideevents_ptr;
			Guide_MinRow = Guide_MaxRow - GUIDE_ITEMS;
			if( Guide_MinRow < 0 ){
				Guide_MinRow = 0;
			}
		}
		Guide_Redraw = TRUE;
		Guide_RenderDisplay();
//		Message_Display( text );	
	}
}

void Guide_MoveToPrevChan( int selected ){
	int i, x;
	int position = -1;
	int selectedChannel;
	TYPE_TapEvent event;
	TYPE_TapEvent event2;
	int tvChannels, radioChannels;
	event = Guide_GetEvent(selected);
	selectedChannel = event.runningStatus;
	if( selectedChannel > 0 ){
		for( i = selected; i > -1; i-- ){
			event2 = Guide_GetEvent(i);
			if( selectedChannel != event2.runningStatus || event.satIdx != event2.satIdx ){
				if( event2.startTime <= Now(0) && event2.endTime > Now(0) ){
					position = i;
					i = -1;
					x = 10;
				}
			}
		}
	} else {
		TAP_Channel_GetTotalNum( &tvChannels, &radioChannels );
		selectedChannel = tvChannels-1;	// Get the last channel and search for it
		for( i = 0; i < guideevents_ptr; i++ ){
			event2 = Guide_GetEvent(i);
			if( selectedChannel == event2.runningStatus || event.satIdx != event2.satIdx ){
				if( event2.startTime <= Now(0) && event2.endTime > Now(0) ){
					position = i;
					i = guideevents_ptr;
					x = 10;
				}
			}
		}
		if( position < 0 ){
			position = 0;
		}
	}
	if( position >= 0 ){
		Guide_SelectedPrevRow = position;
		Guide_SelectedRow= position;
		Guide_MinRow=position;
		Guide_MaxRow=position+GUIDE_ITEMS;
		if( Guide_MaxRow > guideevents_ptr ){
			Guide_MaxRow = guideevents_ptr;
			Guide_MinRow = Guide_MaxRow - GUIDE_ITEMS;
			if( Guide_MinRow < 0 ){
				Guide_MinRow = 0;
			}
		}
		Guide_Redraw = TRUE;
		Guide_RenderDisplay();
	}
}

void Guide_MoveToCurrent( void ){
	int i;
	int position = -1;
	TYPE_TapEvent event;
	for( i = 0; i < guideevents_ptr; i++ ){
		event = Guide_GetEvent(i);
		if( event.runningStatus == Guide_Chan && event.satIdx == Guide_Mode ){
			// FOund the channel now locate the current event
			if( event.startTime <= Now(0) && event.endTime > Now(0) ){
				position = i;
				i = guideevents_ptr;
			}
		}
	}
	if( position > -1 ){
		Guide_SelectedPrevRow = position;
		Guide_SelectedRow= position;
		Guide_MinRow=position;
		Guide_MaxRow=position+GUIDE_ITEMS;
		if( Guide_MaxRow > guideevents_ptr ){
			Guide_MaxRow = guideevents_ptr;
			Guide_MinRow = Guide_MaxRow - GUIDE_ITEMS;
			if( Guide_MinRow < 0 ){
				Guide_MinRow = 0;
			}
		}
	} else {
		Guide_SelectedPrevRow = 0;
		Guide_SelectedRow= 0;
		Guide_MinRow=0;
		Guide_MaxRow=0+GUIDE_ITEMS;
		if( Guide_MaxRow > guideevents_ptr ){
			Guide_MaxRow = guideevents_ptr;
		}
		if( Guide_MaxRow < 1 ){
			Guide_SelectedPrevRow = -1;
			Guide_SelectedRow= -1;
		}
	}
}

void Guide_MoveToNextChan( int selected ){
	int i;
	int position = -1;
	TYPE_TapEvent event;
	TYPE_TapEvent event2;
	event = Guide_GetEvent(selected);
	for( i = selected; i < guideevents_ptr; i++ ){
		event2 = Guide_GetEvent(i);
		if( event.runningStatus != event2.runningStatus || event.satIdx != event2.satIdx ){	// Not the same svcNum of svcType
			// Found the channel now locate the current event
			if( event2.startTime <= Now(0) && event2.endTime > Now(0) ){
				position = i;
				i = guideevents_ptr;
			}
		}
	}
	if( position > -1 ){
//		event = Guide_GetEvent(position);
//		TAP_SPrint( text, "Moved to \"%s\"\0", Channel_Name( GetSvcType( event.svcId ), GetSvcNum(event.svcId) ));
		Guide_SelectedPrevRow = position;
		Guide_SelectedRow= position;
		Guide_MinRow=position;
		Guide_MaxRow=position+GUIDE_ITEMS;
		if( Guide_MaxRow > guideevents_ptr ){
			Guide_MaxRow = guideevents_ptr;
			Guide_MinRow = Guide_MaxRow - GUIDE_ITEMS;
			if( Guide_MinRow < 0 ){
				Guide_MinRow = 0;
			}
		}
		Guide_Redraw = TRUE;
		Guide_RenderDisplay();
//		Message_Display( text );
	} else {
		position = -1;
		// Navigate to first channels current event
		for( i = 0; i < guideevents_ptr; i++ ){
			event2 = Guide_GetEvent(i);
			if( event.runningStatus != event2.runningStatus || event.satIdx != event2.satIdx ){
				// FOund the channel now locate the current event
				if( event2.startTime <= Now(0) && event2.endTime > Now(0) ){
					position = i;
					i = guideevents_ptr;
				}
			}
		}
		if( position > -1 ){
//			event = Guide_GetEvent(position);
//			TAP_SPrint( text, "Moved to \"%s\"\0", Channel_Name( GetSvcType( event.svcId ), GetSvcNum(event.svcId) ));
			Guide_SelectedPrevRow = position;
			Guide_SelectedRow= position;
			Guide_MinRow=position;
			Guide_MaxRow=position+GUIDE_ITEMS;
			if( Guide_MaxRow > guideevents_ptr ){
				Guide_MaxRow = guideevents_ptr;
				Guide_MinRow = Guide_MaxRow - GUIDE_ITEMS;
				if( Guide_MinRow < 0 ){
					Guide_MinRow = 0;
				}
			}
			Guide_Redraw = TRUE;
			Guide_RenderDisplay();
//			Message_Display( text );
		} else {
			// Move back to the top
//			event = Guide_GetEvent(0);
//			TAP_SPrint( text, "Moved to \"%s\"\0", Channel_Name( GetSvcType( event.svcId ), GetSvcNum(event.svcId) ));
			Guide_SelectedPrevRow = 0;
			Guide_SelectedRow= 0;
			Guide_MinRow=0;
			Guide_MaxRow=0+GUIDE_ITEMS;
			if( Guide_MaxRow > guideevents_ptr ){
				Guide_MaxRow = guideevents_ptr;
				Guide_MinRow = Guide_MaxRow - GUIDE_ITEMS;
				if( Guide_MinRow < 0 ){
					Guide_MinRow = 0;
				}
			}
			Guide_Redraw = TRUE;
			Guide_RenderDisplay();
//			Message_Display( text );
		}	
	}
}

// Calback function for when we close the keyboard
// Will render the bottom panel blank again for the time being
void Guide_KeyboardClose( char* text, bool cancel ){
	if( Guide_ShowingTimers ){
		Guide_RenderNextTimersInBottom(Display_MemRGN);
		TAP_Osd_Copy( Display_MemRGN, Display_RGN, Panel_Bottom_X, Panel_Bottom_Y, Panel_Bottom_W, Panel_Bottom_H, Panel_Bottom_X, Panel_Bottom_Y, FALSE);
	} else {
		Guide_RenderHelp();
	}
	Guide_SearchActive = SEARCH_NONE;
}

void Guide_InstantRecordCallback( bool yes ){
	int tuner = -1;
	TAP_Osd_Copy( Display_RGN, Display_MemRGN, 0, 0, 720, 576, 0, 0, FALSE);	// Copy the main region to memory to avoid corruption from the previous OSD
	if( yes ){	
		// Make sure we have a free tuner i.e tuner 1 and 2 not both rewcording
		if( !(HDD_isRecording(0) || HDD_isRecording(1)) ){	// See if we currently have a recording if not its easy so change and start recording
			// User has elected to start recording live event
			// Change to the channel and start the recording
			TAP_Channel_Start( 1, Guide_GetEventSvcType(Guide_SelectedRow), GetSvcNum(Guide_GetEventSvcId(Guide_SelectedRow)) );
			TAP_Hdd_StartRecord();										// Start the recording on that channel
			// We need to adjust the duration 			
			// Work out which tuner we started recording on
			if( HDD_isRecording(0) ){	// If Tuner 1 is recording check whether we were the one that just started it
				if( Update_RecordingInfo(0) ){
					if( Recording_GetRecInfo(0).svcNum == GetSvcNum(Guide_GetEventSvcId(Guide_SelectedRow)) ){	// Same service as repaired file so we must have started it
						tuner = 0;
					}
				}
			}
			if( HDD_isRecording(1) ){	// If Tuner 1 is recording check whether we were the one that just started it
				if( Update_RecordingInfo(1) ){
					if( Recording_GetRecInfo(1).svcNum == GetSvcNum(Guide_GetEventSvcId(Guide_SelectedRow)) ){	// Same service as repaired file so we must have started it
						tuner = 1;
					}
				}
			}						
			if( tuner > -1 && tuner < 2 ){
				if( !HDD_RECSlotSetDuration(tuner, (word)TimeDiff(Now(0), AddTime(Guide_GetEventEndTime(Guide_SelectedRow),Settings_EndBuffer))) ){	// Adjust the duration
					Message_Display( "Timer started but duration failed to adjust" );
					TAP_Delay(Setting_MessageTimeout);
					Guide_Redraw = TRUE;
					Guide_RenderDisplay();
				} else {
					Message_Display( "Recording of event has successfully started" );
					TAP_Delay(Setting_MessageTimeout);
					Guide_Redraw = TRUE;
					Guide_RenderDisplay();
				}
			}
		} else {
			tuner = -1;
			// The complex piece of the puzzle. We need to see if a recording is present on the channel if so extend to cover the event
			// Work out whether a recording is on a tuner currently recording
			if( HDD_isRecording(0) ){	// If Tuner 1 is recording check whether we were the one that just started it
				if( Update_RecordingInfo(0) ){
					if( Recording_GetRecInfo(0).svcNum == GetSvcNum(Guide_GetEventSvcId(Guide_SelectedRow)) ){	// Same service as repaired file so we must have started it
						tuner = 0;
					}
				}
			}
			if( HDD_isRecording(1) ){	// If Tuner 1 is recording check whether we were the one that just started it
				if( Update_RecordingInfo(1) ){
					if( Recording_GetRecInfo(1).svcNum == GetSvcNum(Guide_GetEventSvcId(Guide_SelectedRow)) ){	// Same service as repaired file so we must have started it
						tuner = 1;
					}
				}
			}						
			if( tuner > -1 && tuner < 2 ){
				// So a timer is recording on the same channel as this event
				// We need to adjust the duration to be until the end of this event
				if( !HDD_RECSlotSetDuration(tuner, (word)TimeDiff(Recording_GetRecInfo(tuner).startTime, AddTime(Guide_GetEventEndTime(Guide_SelectedRow),Settings_EndBuffer))) ){	// Adjust the duration
					Message_Display( "Currently recording on channel but failed to adjust the duration" );
					TAP_Delay(Setting_MessageTimeout);
					Guide_Redraw = TRUE;
					Guide_RenderDisplay();
				} else {
					Message_Display( "Current recording on channel has been extended to cover the event" );
					TAP_Delay(Setting_MessageTimeout);
					Guide_Redraw = TRUE;
					Guide_RenderDisplay();
				}
			} else {
				tuner = -1;
				// No recording in progress on that channel so see whether we have a free tuner
				if( !HDD_isRecording(0) || !HDD_isRecording(1) ){	// Either tuner 1 or 2 is free so we can start our instant record
					// User has elected to start recording live event
					// Change to the channel and start the recording
					TAP_Channel_Start( 1, Guide_GetEventSvcType(Guide_SelectedRow), GetSvcNum(Guide_GetEventSvcId(Guide_SelectedRow)) );
					TAP_Hdd_StartRecord();										// Start the recording on that channel
					// We need to adjust the duration 			
					// Work out which tuner we started recording on
					if( HDD_isRecording(0) ){	// If Tuner 1 is recording check whether we were the one that just started it
						if( Update_RecordingInfo(0) ){
							if( Recording_GetRecInfo(0).svcNum == GetSvcNum(Guide_GetEventSvcId(Guide_SelectedRow)) ){	// Same service as repaired file so we must have started it
								tuner = 0;
							}
						}
					}
					if( HDD_isRecording(1) ){	// If Tuner 1 is recording check whether we were the one that just started it
						if( Update_RecordingInfo(1) ){
							if( Recording_GetRecInfo(1).svcNum == GetSvcNum(Guide_GetEventSvcId(Guide_SelectedRow)) ){	// Same service as repaired file so we must have started it
								tuner = 1;
							}
						}
					}						
					if( tuner > -1 && tuner < 2 ){
						if( !HDD_RECSlotSetDuration(tuner, (word)TimeDiff(Now(0), AddTime(Guide_GetEventEndTime(Guide_SelectedRow),Settings_EndBuffer))) ){	// Adjust the duration
							Message_Display( "Timer started but duration failed to adjust" );
							TAP_Delay(Setting_MessageTimeout);
							Guide_Redraw = TRUE;
							Guide_RenderDisplay();
						} else {
							Message_Display( "Recording of event has successfully started" );
							TAP_Delay(Setting_MessageTimeout);
							Guide_Redraw = TRUE;
							Guide_RenderDisplay();
						}
					}
				} else {
					Message_Display( "Unable to start recording. No avaliable tuner" );
				}
			}
		}
	} else {
//		Message_Display( "Recording of event aborted" );
	}
}

void Guide_DeleteTimerCallback( int option ){
	int timerNo = -1;
	TYPE_TimerInfo timer;
	TYPE_TimerInfo temptimer;
	TYPE_TimerInfo temptimer2;
	dword endTime = 0;
	int retval = 0;
	TYPE_TapEvent event;
	TAP_Osd_Copy( Display_RGN, Display_MemRGN, 0, 0, 720, 576, 0, 0, FALSE);	// Copy the main region to memory to avoid corruption from the previous OSD
	if( option == THREEOPTION_OPTION1 ){
		if( TAP_Timer_Delete(Guide_GetTimer( Guide_SelectedRow )) ){
			Message_Display("Timer successfully removed");
		}
	} else {
		if( option == THREEOPTION_OPTION2 ){
			// User has selected to remove timer so proceed
			if( (timerNo = Guide_GetTimer( Guide_SelectedRow )) > -1 ){
				// So we have the timer number
				// Now we need to check whether its duration is that same as the event
				event = Guide_GetEvent(Guide_SelectedRow);
				// Compare with respect to the buffer
				if( TAP_Timer_GetInfo(timerNo, &timer) ){	// If we get the timer info correctly
					//  [    Event     ]
					//    [  Timer   ]
					if( timer.startTime >= AddTime(event.startTime,-Settings_StartBuffer) &&
						AddTime(timer.startTime, timer.duration) <= AddTime(event.endTime,Settings_EndBuffer)){
						// Event with buffers is the same time or longer than the timer so we can delete the timer
						if( TAP_Timer_Delete(timerNo) ){
							Guide_Redraw = TRUE;
							Guide_RenderDisplay();
							return;
						}
					} else {
						// Timer extends more than this event so we will need to split
						// Lets store the timer
						//  [ Event ]
						//  [    Timer     ]
						if( timer.startTime >= AddTime(event.startTime,-Settings_StartBuffer) &&
							AddTime(event.endTime,Settings_EndBuffer) <= AddTime(timer.startTime, timer.duration)){
							endTime = AddTime(timer.startTime, timer.duration);	// Store the ending time of the time
							timer.startTime = AddTime(event.endTime,-Settings_StartBuffer);	// Set the new start timer including the buffers
							timer.duration = TimeDiff(timer.startTime, endTime);
							StripFileName(timer.fileName);	// Remove the .rec extension
							// Attempt to modify the timer and return to the user the result via message
							if( Settings_RenameTimers ){	// If user has enable renaming then search for all events that this new timer covers and build new name
								strcpy(timer.fileName,Timers_BuildTimerName(timer));	// Copy the generated name to the fileName
							}
							strcat(timer.fileName, ".rec");	// Append .rec to the end of the string						
							retval = TAP_Timer_Modify( timerNo, &timer );
							if( retval == 0 ){
	//							Message_Display( "Timer successfully modified" );
							} else {
								Message_Display( "Timer failed to be modified" );
							}
						} else {
							//         [ Event ]
							//  [    Timer     ]
							if( timer.startTime < AddTime(event.startTime,-Settings_StartBuffer) &&
								AddTime(timer.startTime, timer.duration) <= AddTime(event.endTime,Settings_EndBuffer)){
								// We need to adjust the timer duration by subtracting the duration of the event. This will preserve the buffer
								// and because this is at the end thats the only thing needed to be adjusted
								timer.duration -= TimeDiff(event.startTime, event.endTime);
								StripFileName(timer.fileName);	// Remove the .rec extension
								// Attempt to modify the timer and return to the user the result via message
								if( Settings_RenameTimers ){	// If user has enable renaming then search for all events that this new timer covers and build new name
									strcpy(timer.fileName,Timers_BuildTimerName(timer));	// Copy the generated name to the fileName
								}
								strcat(timer.fileName, ".rec");	// Append .rec to the end of the string							
								retval = TAP_Timer_Modify( timerNo, &timer );
								if( retval == 0 ){
	//								Message_Display( "Timer successfully modified" );
								} else {
									Message_Display( "Timer failed to be modified" );
								}
							} else {
								//      [ Event ]
								//  [    Timer     ]
								// We need to create 2 new timers and delete the old one
								// Create the tiemr for the starting part
								temptimer = timer;
								// Adjust the duration of the event
								// Use the event start and add the buffer to calculate the ending time of the timer ( duration )
								temptimer.duration = TimeDiff( temptimer.startTime, AddTime(event.startTime,Settings_EndBuffer));
								StripFileName(temptimer.fileName);	// Remove the .rec extension
								if( Settings_RenameTimers ){	// If user has enable renaming then search for all events that this new timer covers and build new name
									strcpy(temptimer.fileName,Timers_BuildTimerName(temptimer));	// Copy the generated name to the fileName
								}
								strcat(temptimer.fileName, ".rec");	// Append .rec to the end of the string
								// Create the second timer
								// Need to adjust both the duration and start time
								temptimer2 = timer;
								endTime = AddTime(timer.startTime, timer.duration);	// Store the ending time of the timer
								// Adjust the start time using the event end mins the start buffer
								temptimer2.startTime = AddTime(event.endTime, -Settings_StartBuffer);
								temptimer2.duration = TimeDiff(temptimer2.startTime,endTime);
								StripFileName(temptimer2.fileName);	// Remove the .rec extension
								if( Settings_RenameTimers ){	// If user has enable renaming then search for all events that this new timer covers and build new name
									strcpy(temptimer2.fileName,Timers_BuildTimerName(temptimer2));	// Copy the generated name to the fileName
								}
								strcat(temptimer2.fileName, ".rec");	// Append .rec to the end of the string
								// Now we have created our 2 new timers lets delete the old one and create these 2 new
								if( TAP_Timer_Delete(timerNo) ){
									retval = TAP_Timer_Add( &temptimer );
									if( retval == 0 ){
										retval = TAP_Timer_Add( &temptimer2 );
										if( retval == 0 ){
	//										Message_Display( "Timer for event has been successfully removed." );
										} else {
											// Failed adding new timers so restore the old one
											retval = TAP_Timer_Add( &timer );
											if( retval == 0 ){
												Message_Display( "Timer for event restored because of failure in creation of new timers" );
											} else {
												Message_Display( "Failed restoring old timer with event recorded" );
											}
										}
									} else {
										// Failed adding new timers so restore the old one
										retval = TAP_Timer_Add( &timer );
										if( retval == 0 ){
											Message_Display( "Timer for event restored because of failure in creation of new timers" );
										} else {
											Message_Display( "Failed restoring old timer with event recorded" );
										}
									}
								} else {
									// Failed to delete the timer so make no changes
									Message_Display( "Timer for event no removed. Adjustment failed. No changes made." );
								}
								
							}
						}
					}
				} else {
					Message_Display( "Failed getting timer details" );
				}
			}
		}
	}
	TAP_Delay(Setting_MessageTimeout);
	Guide_Redraw = TRUE;
	Guide_RenderDisplay();
}


void Guide_TimerConflictCallback( bool result ){
	TAP_Osd_Copy( Display_RGN, Display_MemRGN, 0, 0, 720, 576, 0, 0, FALSE);	// Copy the main region to memory to avoid corruption from the previous OSD
	if( result ){
		Message_Display( "Timers resolved" );
		TAP_Delay(Setting_MessageTimeout);
	}
	Guide_Redraw = TRUE;
	Guide_RenderDisplay();
}

void Guide_CreateTimer( int row ){
	TYPE_TimerInfo timer;
	int retval;
	TYPE_TapEvent event;
	char str[128];
	char timerstring[128];
	int totalTimers = 0;
	memset( str, '\0', sizeof(str) );
	memset( timerstring, '\0', sizeof(timerstring) );
	event = Guide_GetEvent(row);
	if( HDD_isRecording(0) && HDD_isRecording(1) ) {
		Update_RecordingInfo(0);
		Update_RecordingInfo(1);		
		// Check that the current timers don't cause a conflict
		// If conflict detected just return
		if( AddTime(Recording_GetRecInfo(0).startTime, Recording_GetRecInfo(0).duration) >= AddTime(event.startTime,-Settings_StartBuffer) && AddTime(Recording_GetRecInfo(1).startTime, Recording_GetRecInfo(1).duration) >= AddTime(event.startTime,-Settings_StartBuffer) ){
			// Both timers recording now overlap the start of this event incl buffer so revoke and return
			return;
		}
	}
	// General Stuff
	timer.isRec = 1;
	timer.tuner = 3;
	timer.nameFix = 1;
	timer.reservationType = RESERVE_TYPE_Onetime;
	// Stuff used from the event
	timer.startTime = AddTime(event.startTime,-Settings_StartBuffer);
	timer.duration = TimeDiff(event.startTime,event.endTime)+Settings_StartBuffer+Settings_EndBuffer;
	timer.svcNum = GetSvcNum(Guide_GetEventSvcId(row));
	timer.svcType = GetSvcType(Guide_GetEventSvcId(row));
	strncpy(timerstring, event.eventName, 128);	// Copy the event name and ensure no overflow
	if( Settings_AppendEpisode ){
		// If we are small enough then attempt to append the episode name
		if( strlen(timerstring) < 40 ){
			strncpy(str, event.description,128);	// Set the max copy size
			if( GetEpisodeName( str, (int)"[" ) ){	// TO IMPLEMENT SETTING FOR CHARACTER/S	look for [ SETTINGS WILL CONFIGURE LATER
				strcat(timerstring," - ");	// Append seperator
				strncat(timerstring,str,128-strlen(timerstring)-4-3);	// Copy the episode title until the maximum size of the array
			}
		}
		strncpy(timer.fileName,timerstring,sizeof(timer.fileName)-4);	// copy across to the timer string with repect to its size
	} else {
		// Timers build timer name returns name with .rec extension
		if( Settings_RenameTimers ){	// If user has enable renaming then search for all events that this new timer covers and build new name
			strcpy(timer.fileName,Timers_BuildTimerName(timer));	// Copy the generated name to the fileName
		} else {
			strncpy(timer.fileName,timerstring,sizeof(timer.fileName)-4);	// copy across to the timer string with repect to its size
		}
	}
	strcat(timer.fileName, ".rec");	// Append .rec to the end of the string
	retval=TAP_Timer_Add(&timer);
	if( retval == 0 ){
		Guide_Redraw = TRUE;
		Guide_RenderDisplay();
	} ef( retval == 1 ){
		Message_Display( "ERROR: Timer setting failed. No resourse avaliable." );
		TAP_Delay(Setting_MessageTimeout);
		Guide_Redraw = TRUE;
		Guide_RenderDisplay();
	} ef( retval == 2 ){
		Message_Display( "ERROR: Timer setting failed. Invalid tuner." );
		TAP_Delay(Setting_MessageTimeout);
		Guide_Redraw = TRUE;
		Guide_RenderDisplay();
	} else {
		// Check for neighbouring timer if so then ask whether to join
		if( (Guide_AppendTimerNum=Timers_GetNeighbour(timer)) > -1 ){
			// We have a neighbour so prompt to extend
			YesNo_Init( Display_RGN, Display_MemRGN,  Panel_Bottom_X, Panel_Bottom_Y, Panel_Bottom_W, Panel_Bottom_H );
			YesNo_SetColours( DISPLAY_KEYBOARDITEMTEXT, DISPLAY_ITEM, DISPLAY_KEYBOARDITEMSELECTEDTEXT, DISPLAY_KEYBOARDITEMSELECTED, DISPLAY_MAIN );
			if( !YesNo_Activate("Join to surrounding timers?",&Guide_AppendCallback) ){
				Message_Display( "YesNo box failed." );	// Explained that the event has past and we can set because of that
			}
		} else {
			TimerConflict_Init( Display_RGN, Display_MemRGN,  Panel_Left_X, Panel_Left_Y, DISPLAY_ITEM_W, (Panel_Left_Y + (( DISPLAY_ITEM_H + DISPLAY_GAP ) * 14))-Panel_Left_Y );
			TimerConflict_Activate( timer, &Guide_TimerConflictCallback );// ){
			if( !TimerConflict_IsActive() ){	// We didn't activate so check for append
				totalTimers = TAP_Timer_GetTotalNum();
				if( totalTimers > 69 ){
					Message_Display( "Maximum number of timers are set." );
				} else {
					Message_Display( "ERROR: Unable to resolve issues" );
				}
				TAP_Delay(Setting_MessageTimeout);
				Guide_Redraw = TRUE;
				Guide_RenderDisplay();
			}
		}
	}
}

void Guide_TimerAppendConflictCallback(bool result) {
	TAP_Osd_Copy(Display_RGN, Display_MemRGN, 0, 0, 720, 576, 0, 0, FALSE); // Copy the main region to memory to avoid corruption from the previous OSD
	if( !result ){	// User has cancelled so restore the stored timer from append attempt
		// Failed so restore our stored timer from before
		TAP_Timer_Add(&Guide_RestoreTimer);
	}
	Guide_Redraw = TRUE;
	Guide_RenderDisplay();
}

void Guide_AppendCallback( bool yes ){
	TYPE_TapEvent event;
	TYPE_TimerInfo currentTimer;
	TYPE_TimerInfo appendTimer;
	dword endTime = 0;
	int appendNum = 0;
	int totalTimers = 0;
	int retval = 0;
	TAP_Osd_Copy( Display_RGN, Display_MemRGN, 0, 0, 720, 576, 0, 0, FALSE);	// Copy the main region to memory to avoid corruption from the previous OSD
	if( yes ){
		if( Guide_AppendTimerNum > -1 ){
			event = Guide_GetEvent(Guide_SelectedRow);
			TAP_Timer_GetInfo(Guide_AppendTimerNum, &currentTimer);
			Guide_RestoreTimer = currentTimer; // Store the previous state of the timer in global var to enable restoration if required
			// Lets adjust the duration of the timer
			if( currentTimer.startTime < event.startTime ){
				// We will set the duration to be
				// duration = ( event.endTime + Buffer ) - currentTimer.startTime
				currentTimer.duration = TimeDiff(currentTimer.startTime,AddTime(event.endTime,Settings_EndBuffer));
			} else {				
				// duration = currentTimer.endTime - ( event.startTime - startBuffer )
				// Buffer assumed in the the timer
				currentTimer.duration = TimeDiff(AddTime(event.startTime,-Settings_StartBuffer),AddTime(currentTimer.startTime,currentTimer.duration));
				// Adjust the start time to be the start time minus the buffer on the event			
				currentTimer.startTime = AddTime(event.startTime,-Settings_StartBuffer);
				// We wont rename atm TO DO
			}
			StripFileName(currentTimer.fileName);	// Remove the .rec extension
			if( Settings_RenameTimers ){	// If user has enable renaming then search for all events that this new timer covers and build new name
				strcpy(currentTimer.fileName,Timers_BuildTimerName(currentTimer));	// Copy the generated name to the fileName
			}
			strcat(currentTimer.fileName, ".rec");	// Append .rec to the end of the string
			retval = TAP_Timer_Modify(Guide_AppendTimerNum,&currentTimer);
			if( retval == 0 ){
//				Message_Display( "Timer successfully modified." );
			} else {
				// Look to see whether we have a neighbouring timer if so adjust start and duration to include it and remove that timer
				// Make sure we are not pointing at the same timer using withNum function
				if( (appendNum = Timers_GetNeighbourWithNum( Guide_AppendTimerNum, currentTimer )) > -1 ){	// We have a neighbour so we need to adjust
					if( TAP_Timer_GetInfo( appendNum, &appendTimer ) ){
						//   [ Append ]
						//          [ Current Timer ]
						if( appendTimer.startTime <= currentTimer.startTime &&
							AddTime(appendTimer.startTime, appendTimer.duration) >= currentTimer.startTime &&
							AddTime(appendTimer.startTime, appendTimer.duration) < AddTime(currentTimer.startTime, currentTimer.duration)
							){
							// Need to adjust the start and duration as appending timer fails before us
							endTime = AddTime(currentTimer.startTime,currentTimer.duration);
							currentTimer.startTime = appendTimer.startTime;
							currentTimer.duration = TimeDiff(appendTimer.startTime, endTime);
							// Attempt to re-set the timer. First remove the conflicting timer aka appendTimer
							if( TAP_Timer_Delete(appendNum) ){
								// The timer we are going to delete is less than our ajudtment timer so minus 1 to resync
								if( appendNum < Guide_AppendTimerNum ) Guide_AppendTimerNum--;
								StripFileName(currentTimer.fileName);	// Remove the .rec extension
								if( Settings_RenameTimers ){	// If user has enable renaming then search for all events that this new timer covers and build new name
									strcpy(currentTimer.fileName,Timers_BuildTimerName(currentTimer));	// Copy the generated name to the fileName
								}
								strcat(currentTimer.fileName, ".rec");	// Append .rec to the end of the string								
								retval = TAP_Timer_Modify(Guide_AppendTimerNum,&currentTimer);
								if( retval == 0 ){
//									Message_Display( "Timer successfully set for event." );
								} else {
									// Failed setting new timer so restore old on and inform the user
									retval = TAP_Timer_Add( &appendTimer );
									if( retval == 0 ){
										Message_Display( "Failed adjusting surrounding timers. No adjustments made." );
									} else {
										Message_Display( "Failed adjusting surrounding timers. Surrounding tiemrs have been changed/removed." );
									}
								}
							} else {
								// Don't know what to do so just fail
								// unknown case of failure so print message using failure values
								if( retval == 1 ){
									Message_Display( "ERROR: Timer setting failed. No resourse avaliable." );
								} ef( retval == 2 ){
									Message_Display( "ERROR: Timer setting failed. Invalid tuner." );
								} ef (retval == 0xffffffff){
									Message_Display( "ERROR: Timer setting failed. Error modifying." );
								} ef (retval >= 0xFFFE0000){
									Message_Display( "ERROR: Timer setting failed. Conflicts with other timers. Failed deleting old timer" );
								} else {
									Message_Display( "ERROR: Timer setting failed. Unknown cause." );
								}
							}
						} else {
							//            [ Append ]
							//  [ Current Timer ]
							if( currentTimer.startTime <= appendTimer.startTime &&
								AddTime( currentTimer.startTime, currentTimer.duration ) >= appendTimer.startTime &&
								AddTime( currentTimer.startTime, currentTimer.duration ) <= AddTime( appendTimer.startTime, appendTimer.duration )){
								// Only need to adjust the duration of the show to be the end time of the appending timer
								currentTimer.duration = TimeDiff( currentTimer.startTime, AddTime(appendTimer.startTime,appendTimer.duration));
								// Attempt to reset the timer. First remove the conflicting timer
								if( TAP_Timer_Delete(appendNum) ){
									// The timer we deleted is less than our adjustment timer so minus 1 to resync
									if( appendNum < Guide_AppendTimerNum ) Guide_AppendTimerNum--;
									StripFileName(currentTimer.fileName);	// Remove the .rec extension
									if( Settings_RenameTimers ){	// If user has enable renaming then search for all events that this new timer covers and build new name
										strcpy(currentTimer.fileName,Timers_BuildTimerName(currentTimer));	// Copy the generated name to the fileName
									}
									strcat(currentTimer.fileName, ".rec");	// Append .rec to the end of the string
									retval = TAP_Timer_Modify(Guide_AppendTimerNum,&currentTimer);
									if( retval == 0 ){
//										Message_Display( "Timer successfully set for event." );
									} else {
										// Failed setting new timer so restore old on and inform the user
										retval = TAP_Timer_Add( &appendTimer );
										if( retval == 0 ){
											Message_Display( "Failed adjusting surrounding timers. No adjustments made." );
										} else {
											Message_Display( "Failed adjusting surrounding timers. Surrounding tiemrs have been changed/removed." );
										}
									}
								} else {
									// Don't know what to do so just fail
									// unknown case of failure so print message using failure values
									if( retval == 1 ){
										Message_Display( "ERROR: Timer setting failed. No resourse avaliable." );
									} ef( retval == 2 ){
										Message_Display( "ERROR: Timer setting failed. Invalid tuner." );
									} ef (retval == 0xffffffff){
										Message_Display( "ERROR: Timer setting failed. Error modifying." );
									} ef (retval >= 0xFFFE0000){
										Message_Display( "ERROR: Timer setting failed. Conflicts with other timers. Failed deleting old timer" );
									} else {
										Message_Display( "ERROR: Timer setting failed. Unknown cause." );
									}
								}
							} else {
								// Check for conflicts on other channels given the checks for the conflicting on same channel have failed.
								// Launch the conflict handling mnodule fo the tap
								TimerConflict_Init( Display_RGN, Display_MemRGN,  Panel_Left_X, Panel_Left_Y, DISPLAY_ITEM_W, (Panel_Left_Y + (( DISPLAY_ITEM_H + DISPLAY_GAP ) * 14))-Panel_Left_Y );
								TimerConflict_Activate( currentTimer, &Guide_TimerAppendConflictCallback );// ){
								if( !TimerConflict_IsActive() ) { // We didn't activate so check for append
									totalTimers = TAP_Timer_GetTotalNum();
									if( totalTimers > 69 ) {
										Message_Display( "Maximum number of timers are set." );
									} else {
										if (retval == 1) {
											Message_Display("ERROR: Timer setting failed. No resourse avaliable.");
										}ef( retval == 2 ) {
											Message_Display( "ERROR: Timer setting failed. Invalid tuner." );
										}ef (retval	== 0xffffffff) {
											Message_Display( "ERROR: Timer setting failed. Error modifying." );
										}ef (retval	>= 0xFFFE0000) {
											Message_Display( "ERROR: Timer setting failed. Conflicts with other timers. Didn't know what to do" );
										} else {
											Message_Display( "ERROR: Timer setting failed. Unknown cause." );
										}
									}
									TAP_Delay(Setting_MessageTimeout);
									Guide_Redraw = TRUE;
									Guide_RenderDisplay();
									return;
								} else {
									// Conflict screen has launched so delete this timer. If user cancels callback will restore original timer
									if (TAP_Timer_Delete(Guide_AppendTimerNum) ) {	// Remove this appending timer from the unit
									}
									return;
								}
							}
						}
					} else {
						// unknown case of failure so print message using failure values
						Message_Display( "Failed to get the information for appending timer. No changes have been made" );
					}
				} else {
					// Check for conflicts on other channels given the checks for the conflicting on same channel have failed.
					// Launch the conflict handling mnodule fo the tap
					TimerConflict_Init( Display_RGN, Display_MemRGN,  Panel_Left_X, Panel_Left_Y, DISPLAY_ITEM_W, (Panel_Left_Y + (( DISPLAY_ITEM_H + DISPLAY_GAP ) * 14))-Panel_Left_Y );
					TimerConflict_Activate( currentTimer, &Guide_TimerAppendConflictCallback );// ){
					if( !TimerConflict_IsActive() ) { // We didn't activate so check for append
						totalTimers = TAP_Timer_GetTotalNum();
						if( totalTimers > 69 ) {
							Message_Display( "Maximum number of timers are set." );
						} else {
							if (retval == 1) {
								Message_Display("ERROR: Timer setting failed. No resourse avaliable.");
							}ef( retval == 2 ) {
								Message_Display( "ERROR: Timer setting failed. Invalid tuner." );
							}ef (retval	== 0xffffffff) {
								Message_Display( "ERROR: Timer setting failed. Error modifying." );
							}ef (retval	>= 0xFFFE0000) {
								Message_Display( "ERROR: Timer setting failed. Conflicts with other timers. Didn't know what to do" );
							} else {
								Message_Display( "ERROR: Timer setting failed. Unknown cause." );
							}
						}
						TAP_Delay(Setting_MessageTimeout);
						Guide_Redraw = TRUE;
						Guide_RenderDisplay();
						return;
					} else {
						// Conflict screen has launched so delete this timer. If user cancels callback will restore original timer
						if (TAP_Timer_Delete(Guide_AppendTimerNum) ) {	// Remove this appending timer from the unit
						}
						return;
					}

				}
			}
		}
	} else {
//		Message_Display( "No adjustment made" );
	}
//	TAP_Delay(Setting_MessageTimeout);
	Guide_Redraw = TRUE;
	Guide_RenderDisplay();
}

void Guide_RemoveRecEventCallback(bool yes) {
	int tuner = -1;
	TAP_Osd_Copy(Display_RGN, Display_MemRGN, 0, 0, 720, 576, 0, 0,	FALSE);
	if( yes ){
		if( Guide_GetEventEndTime(Guide_SelectedRow) >= Now(0) ){	// Only allow when event is not currently active
			if (HDD_isRecording(0) ) { // If Tuner 1 is recording check whether we were the one that just started it
				if (Update_RecordingInfo(0) ) {
					if (Recording_GetRecInfo(0).svcNum == GetSvcNum(Guide_GetEventSvcId(Guide_SelectedRow)) ) { // Same service as repaired file so we must have started it
						tuner = 0;
					}
				}
			}
			if (HDD_isRecording(1) ) { // If Tuner 1 is recording check whether we were the one that just started it
				if (Update_RecordingInfo(1) ) {
					if (Recording_GetRecInfo(1).svcNum == GetSvcNum(Guide_GetEventSvcId(Guide_SelectedRow)) ) { // Same service as repaired file so we must have started it
						tuner = 1;
					}
				}
			}
			if (tuner > -1&& tuner < 2) {
				// We have established the tuner which is being used. Now lets check that the 
				if ( !HDD_RECSlotSetDuration(tuner,(word)TimeDiff( Recording_GetRecInfo(tuner).startTime, AddTime(Guide_GetEventStartTime(Guide_SelectedRow), Settings_EndBuffer)))) { // Adjust the duration
					Message_Display("Failed to adjust duration of current recording to remove event.");
					TAP_Delay(Setting_MessageTimeout);
					Guide_Redraw = TRUE;
					Guide_RenderDisplay();
				} else {
					Guide_Redraw = TRUE;
					Guide_RenderDisplay();
				}
			}
		}
	}
}


// Function to get and set the variable associated with editing the timer
int Guide_GetTimer( int row ){
	TYPE_TimerInfo	currentTimer;
	int totalTimers = 0;
	int i, x;
	word year;
	byte month, day, weekDay;
	word year2;
	byte month2, day2, weekDay2;
	int result;
	TYPE_TapChInfo	currentChInfo;
	TYPE_TapEvent event;
	if( row < 0 ) return -1;
	event = Guide_GetEvent(row);
	totalTimers = TAP_Timer_GetTotalNum();
	// Store the event day of occurance
	TAP_ExtractMjd( event.startTime>>16, &year2, &month2, &day2, &weekDay2);
	for ( i = 0; i < totalTimers; i++ ){
		TAP_Timer_GetInfo(i, &currentTimer); // Sequentially get timer details and compare with those in the display range
		TAP_ExtractMjd( currentTimer.startTime>>16, &year, &month, &day, &weekDay);
		TAP_Channel_GetInfo( currentTimer.svcType, currentTimer.svcNum, &currentChInfo );

		switch( currentTimer.reservationType ){
			case RESERVE_TYPE_Onetime:
				if( (result=Timer_TimerFind( currentChInfo.svcId, Guide_GetEventSvcId(row), currentTimer.startTime, AddTime(currentTimer.startTime, currentTimer.duration), event.startTime, event.endTime )) != TIMER_NONE ){
					return i;
				}
				break;
			case RESERVE_TYPE_Everyday:
				// Initial check to determine against currentTimer start
				if( (result=Timer_TimerFind( currentChInfo.svcId, Guide_GetEventSvcId(row), currentTimer.startTime, AddTime(currentTimer.startTime, currentTimer.duration), event.startTime, event.endTime )) != TIMER_NONE ){
					return i;
				}
				// Not found on today so we need to go and increment for the next 7 days and check on weekdays
				for( x = 0; x < TIMERS_MAXDAYSAHEAD; x++ ){
					currentTimer.startTime = AddTime( currentTimer.startTime, 1440 );
					if( (result=Timer_TimerFind( currentChInfo.svcId, Guide_GetEventSvcId(row), currentTimer.startTime, AddTime(currentTimer.startTime, currentTimer.duration), event.startTime, event.endTime )) != TIMER_NONE ){
						return i;
					}
				}
				break;
			case RESERVE_TYPE_EveryWeekend:
				// if event is not on the weekend we need to check every week day from the start to determine
				// whether event has timer
				if( weekDay2 == SAT || weekDay2 == SUN ){
					// Initial check to determine against currentTimer start
					if( weekDay == SAT || weekDay == SUN  ){	// Sanity check to make sure timer is on a weekend
						if( (result=Timer_TimerFind( currentChInfo.svcId, Guide_GetEventSvcId(row), currentTimer.startTime, AddTime(currentTimer.startTime, currentTimer.duration), event.startTime, event.endTime )) != TIMER_NONE ){
							return i;
						}
					}
					// Not found on today so we need to go and increment for the next 7 days and check on weekdays
					for( x = 0; x < TIMERS_MAXDAYSAHEAD; x++ ){
						currentTimer.startTime = AddTime( currentTimer.startTime, 1440 );
						TAP_ExtractMjd( currentTimer.startTime>>16, &year, &month, &day, &weekDay);
						if( weekDay == SAT || weekDay == SUN  ){	// Sanity check to make sure timer is on a weekday
							if( (result=Timer_TimerFind( currentChInfo.svcId, Guide_GetEventSvcId(row), currentTimer.startTime, AddTime(currentTimer.startTime, currentTimer.duration), event.startTime, event.endTime )) != TIMER_NONE ){
								return i;
							}
						}
					}
				}
				break;
			case RESERVE_TYPE_Weekly:
				if( (result=Timer_TimerFind( currentChInfo.svcId, Guide_GetEventSvcId(row), currentTimer.startTime, AddTime(currentTimer.startTime, currentTimer.duration), event.startTime, event.endTime )) != TIMER_NONE ){
					return i;
				}
				for( x = 0; x < 7; x++ ){	// AddTime only handle 1440 max so loop the adding of the time
					currentTimer.startTime = AddTime(currentTimer.startTime, 1440 );
				}
				// Only search a week ahead atm as data shouldn't be anymore than that
				if( (result=Timer_TimerFind( currentChInfo.svcId, Guide_GetEventSvcId(row), currentTimer.startTime, AddTime(currentTimer.startTime, currentTimer.duration), event.startTime, event.endTime )) != TIMER_NONE ){
					return i;
				}
				break;
			case RESERVE_TYPE_WeekDay:
				// if event is not on the weekend we need to check every week day from the start to determine
				// whether event has timer
				if( weekDay2 != SAT && weekDay2 != SUN ){
					// Initial check to determine against currentTimer start
					if( weekDay != SAT && weekDay != SUN  ){	// Sanity check to make sure timer is on a weekday
						if( (result=Timer_TimerFind( currentChInfo.svcId, Guide_GetEventSvcId(row), currentTimer.startTime, AddTime(currentTimer.startTime, currentTimer.duration), event.startTime, event.endTime )) != TIMER_NONE ){
							return i;
						}
					}
					// Not found on today so we need to go and increment for the next 7 days and check on weekdays
					for( x = 0; x < TIMERS_MAXDAYSAHEAD; x++ ){
						currentTimer.startTime = AddTime( currentTimer.startTime, 1440 );
						TAP_ExtractMjd( currentTimer.startTime>>16, &year, &month, &day, &weekDay);
						if( weekDay != SAT && weekDay != SUN  ){	// Sanity check to make sure timer is on a weekday
							if( (result=Timer_TimerFind( currentChInfo.svcId, Guide_GetEventSvcId(row), currentTimer.startTime, AddTime(currentTimer.startTime, currentTimer.duration), event.startTime, event.endTime )) != TIMER_NONE ){
								return i;
							}
						}
					}
				}
				break;
		}
	}
	return -1;
}

bool String_Sub( char * str, char * str2 ){
	if( strstr(str,str2) != NULL ){
		return TRUE;
	}
	return FALSE;
}

void Guide_FindNextSearch( void ){
	int position = -1;
	int i = 0;
	char *p;
	TYPE_TapEvent event;
	switch( Guide_SearchActive ){
		case SEARCH_TITLE:
			for( i = Guide_SelectedRow+1; i < guideevents_ptr; i++ ){
				event = Guide_GetEvent(i);
				p = strstr(event.eventName, Keyboard_GetText());
				if( p != NULL ){
					// Substring found so exit
					position = i;
					i = guideevents_ptr;
				}	
			}
			if( position > -1 ){
				Guide_SelectedPrevRow = position;
				Guide_SelectedRow=position;
				Guide_MinRow=position;
				Guide_MaxRow=position+GUIDE_ITEMS;
				if( Guide_MaxRow > guideevents_ptr ){
					Guide_MaxRow = guideevents_ptr;
					Guide_MinRow = Guide_MaxRow - GUIDE_ITEMS;
					if( Guide_MinRow < 0 ){
						Guide_MinRow = 0;
					}
				}
				Guide_Redraw = TRUE;
				Guide_RenderDisplay();
			}
			break;
		case SEARCH_DESC:
			// Not found in the title so search in the description field
			for( i = Guide_SelectedRow+1; i < guideevents_ptr; i++ ){
				// Slow lenear search for single character search
				event = Guide_GetEvent(i);
				p = strstr(event.description, Keyboard_GetText());
				if( p != NULL ){
					// Substring found so exit
					position = i;
					i = guideevents_ptr;
				}		
			}
			if( position > -1 ){
				Guide_SelectedPrevRow = position;
				Guide_SelectedRow=position;
				Guide_MinRow=position;
				Guide_MaxRow=position+GUIDE_ITEMS;
				if( Guide_MaxRow > guideevents_ptr ){
					Guide_MaxRow = guideevents_ptr;
					Guide_MinRow = Guide_MaxRow - GUIDE_ITEMS;
					if( Guide_MinRow < 0 ){
						Guide_MinRow = 0;
					}
				}
				Guide_Redraw = TRUE;
				Guide_RenderDisplay();
			} 
			break;
	}

}

int Guide_KeyboardSearch( int max ){
	int position = -1;
	int i = 0;
	char *p;
	TYPE_TapEvent event;
	for( i = 0; i < max; i++ ){
		event = Guide_GetEvent(i);
		p = strstr(event.eventName, Keyboard_GetText());
		if( p != NULL ){
			position = i;
			i = max;
		}	
	}
	return position;
}

int Guide_KeyboardSearchAnywhere( int max ){
	int position = -1;
	int i = 0;
	char *p;
	TYPE_TapEvent event;
	for( i = 0; i < max; i++ ){
		// Slow linear search for single character search
		//strncasecmp( for case insensitive match
		event = Guide_GetEvent(i);
		p = strstr(event.description, Keyboard_GetText());
		if( p != NULL ){
			// Substring found so exit
			position = i;
			i = max;
		}		
	}
	return position;
}

void Guide_SwitchModes( void ){
	if( Guide_Mode == SVC_TYPE_Tv ){
		Guide_Mode = SVC_TYPE_Radio;
	} else {
		Guide_Mode = SVC_TYPE_Tv;
	}
	Guide_HasEvents = FALSE;
	Guide_PopulateList();
	if( guideevents_ptr < 0 ){	// If we have no events then switch back. 
		if( Guide_Mode == SVC_TYPE_Tv ){
			Guide_Mode = SVC_TYPE_Radio;
		} else {
			Guide_Mode = SVC_TYPE_Tv;
		}
		Guide_HasEvents = FALSE;
		Guide_PopulateList();
	}
	if( guideevents_ptr < 0 ) return;	// If after switching back we still have no events return without any change
	Guide_MinRow = 0;
	Guide_MaxRow = GUIDE_ITEMS;
	Guide_SelectedRow = 0;
	Guide_SelectedPrevRow = 0;
	if( Guide_MaxRow > guideevents_ptr ){
		Guide_MaxRow = guideevents_ptr;
	}
	if( Guide_MaxRow < 1 ){
		Guide_SelectedRow = -1;
		Guide_SelectedPrevRow = -1;	
	}
	Guide_Redraw = TRUE;
	Guide_RenderDisplay();
}

// For the linear list the guideevents array is any array of pointer that point to the main events array
// Then for the linear list all the sort order changes and displaying of event is based on this array copy
void Guide_PopulateList( void ){
	int i;
	if( Guide_HasEvents ) return;
	// Reset the array used for the linear list
	guideevents_ptr = 0;
	// Go thru each event in the events array and add all those which are Tv/Radio depending on what we are watching
	for( i = 0; i < eventsPointer; i++ ){
		if( Events_GetEventSvcType(i) == Guide_Mode ){	
			if( guideevents_ptr < sizeof(guideevents) ){
				if( events[i]->endTime >= Now(0) ){	// Must be an active event or into the future
					guideevents[guideevents_ptr] = events[i];	// Store an array of the events
					guideevents_ptr++;
				}
			}
		}
	}
	if( guideevents_ptr < 2 ){
		Guide_HasEvents = TRUE;
		return;
	}
	// Need to sort the events
	switch( sortOrder ){
		case SORT_NAME:
			Guide_SortByName();
			break;
		case SORT_CHANNEL:
			Guide_SortByChannel();
			break;
		case SORT_START:
			Guide_SortByStart();
			break;
		case SORT_RATING:
			Guide_SortByRating();
			break;
	}
	Guide_HasEvents = TRUE;
}

char * Guide_GetEventName( int item ){
	static TYPE_TapEvent event;
	// Sanity check
	if( item < guideevents_ptr ){
		event = *guideevents[item];
	}
	return event.eventName;
}

char * Guide_GetEventDesc( int item ){
	static TYPE_TapEvent event;
	// Sanity check
	if( item < guideevents_ptr ){
		event = *guideevents[item];
	}
	return event.description;
}

dword Guide_GetEventStartTime( int item ){
	static TYPE_TapEvent event;
	// Sanity check
	if( item < guideevents_ptr ){
		event = *guideevents[item];
	}
	return event.startTime;
}

dword Guide_GetEventEndTime( int item ){
	static TYPE_TapEvent event;
	// Sanity check
	if( item < guideevents_ptr ){
		event = *guideevents[item];
	}
	return event.endTime;
}

TYPE_TapEvent Guide_GetEvent( int item ){
	static TYPE_TapEvent event;
	// Sanity check
	if( item < guideevents_ptr ){
		event = *guideevents[item];
	}
	return event;
}

void Guide_SortByChannel( void ){
	TYPE_TapEvent *tmpEvent;
	int i, j, idx;
	char text[128];
	dword timeout;
	if( guideevents_ptr < 2 ) return;
	if( Display_BackgroundDrawn ){
		sprintf( text, "Sorting events by channel, start time.\0" );
		Message_Display(text);
	}
	timeout = TAP_GetTick();
	for( i = 0; i < guideevents_ptr; i++ ){
		idx = i;
		for( j = i; j < guideevents_ptr; j++ ){
			// SatIdx stores the svcType when we insert it. To do create new structure for these extra variables
			// I sneakely hide the svc num in runningStatus as it is easy to calculate whether it is running now
			if( guideevents[idx]->runningStatus > guideevents[j]->runningStatus ){	// Group the channels before the ordering of the times
				idx = j;
			} else {
				if( guideevents[idx]->runningStatus == guideevents[j]->runningStatus ){
					if( guideevents[idx]->startTime > guideevents[j]->startTime ){
						// Lets Swap
						idx = j;
					}
				}
			}
		}
		// Swap the pointers around
		tmpEvent = guideevents[i];
		guideevents[i] = guideevents[idx];
		guideevents[idx] = tmpEvent;
		if( timeout + 50 < TAP_GetTick() ){
			if( Display_BackgroundDrawn ){
				if( strlen(text) < 128 ){
					strcat( text, "." );
					Message_Display(text);
					Display_DrawTime();
				}
			}
			timeout = TAP_GetTick();
		}
	}
	sortOrder = SORT_CHANNEL;
	sorted = TRUE;
	Message_DisplayClear();
}

// Sorts the events by start time and the by channel
void Guide_SortByRating( void ){
	TYPE_TapEvent *tmpEvent;
	int i, j, idx;
	char text[128];
	dword timeout;
	if( guideevents_ptr < 2 ) return;
	if( Display_BackgroundDrawn ){
		sprintf( text, "Sorting events by rating, channel and start time .\0" );
		Message_Display(text);
	}
	timeout = TAP_GetTick();
	for( i = 0; i < guideevents_ptr; i++ ){
		idx = i;
		for( j = i; j < guideevents_ptr; j++ ){
			// I sneakely hide the svc num in runningStatus
			if( guideevents[i]->parentalRating > guideevents[j]->parentalRating ){
				// Lets Swap
				idx = j;
			} else {
				if( guideevents[i]->parentalRating == guideevents[j]->parentalRating ){
					if( guideevents[i]->runningStatus > guideevents[j]->runningStatus ){	// Group the channels before the ordering of the times
						idx = j;
					} else {
						if( guideevents[i]->runningStatus == guideevents[j]->runningStatus ){
							if( guideevents[i]->startTime > guideevents[j]->startTime ){
								// Lets Swap
								idx = j;
							}
						}
					}
				}
			}
		}
		// Swap the pointers around
		tmpEvent = guideevents[i];
		guideevents[i] = guideevents[idx];
		guideevents[idx] = tmpEvent;
		if( timeout + 50 < TAP_GetTick() ){
			if( Display_BackgroundDrawn ){
				if( strlen(text) < 128 ){
					strcat( text, "." );
					Message_Display(text);
					Display_DrawTime();
				}
			}
			timeout = TAP_GetTick();
		}
	}
	sortOrder = SORT_RATING;
	sorted = TRUE;
	Message_DisplayClear();
}


// Selection sort algorithm
void Guide_SortByName( void ){
	TYPE_TapEvent *tmpEvent;
	int i, j;
	int idx = 0;
	int tmp = 0;
	char text[128];
	dword timeout;
	if( guideevents_ptr < 2 ) return;
	if( Display_BackgroundDrawn ){
		sprintf( text, "Sorting events by name.\0" );
		Message_Display(text);
	}
	timeout = TAP_GetTick();
	for( i = 0; i < guideevents_ptr; i++ ){
		idx = i;
		for( j = i; j < guideevents_ptr; j++ ){
			// I sneakely hide the svc num in runningStatus
			if( (tmp = strcasecmp(guideevents[idx]->eventName,guideevents[j]->eventName)) > 0 ){
				// Lets Swap
				idx = j;
			} 
			else {
				// Check for same channel svcId
				if( tmp == 0 ){	// No need to strcmp again just store the result
					if( guideevents[idx]->runningStatus > guideevents[j]->runningStatus ){	// Group the channels before the ordering of the times
						idx = j;
					} else {
						if( guideevents[idx]->runningStatus == guideevents[j]->runningStatus ){	// Group the channels before the ordering of the times
							if( guideevents[idx]->startTime > guideevents[j]->startTime ){
								idx = j;
							}
						}
					}
				}
			}
		}
		// Swap the pointers around
		tmpEvent = guideevents[i];
		guideevents[i] = guideevents[idx];
		guideevents[idx] = tmpEvent;
		if( timeout + 50 < TAP_GetTick() ){
			if( Display_BackgroundDrawn ){
				if( strlen(text) < 128 ){
					strcat( text, "." );
					Message_Display(text);
					Display_DrawTime();
				}
			}
			timeout = TAP_GetTick();
		}
	}
	sortOrder = SORT_NAME;
	sorted = TRUE;
	Message_DisplayClear();
}

// Sorts the events by start time and the by channel
void Guide_SortByStart( void ){
	TYPE_TapEvent *tmpEvent;
	int i, j, idx;
	char text[128];
	dword timeout;
	if( guideevents_ptr < 2 ) return;
	if( Display_BackgroundDrawn ){
		sprintf( text, "Sorting events by start time then channel .\0" );
		Message_Display(text);
	}
	timeout = TAP_GetTick();
	for( i = 0; i < guideevents_ptr; i++ ){
		idx = i;
		for( j = i; j < guideevents_ptr; j++ ){
			// I sneakely hide the svc num in runningStatus
			if( guideevents[idx]->startTime > guideevents[j]->startTime ){
				// Lets Swap
				idx = j;
			} else {
				if( guideevents[idx]->startTime == guideevents[j]->startTime ){
					// Check the channel
					if( guideevents[idx]->runningStatus > guideevents[j]->runningStatus ){	// Group the channels before the ordering of the times
						idx = j;
					}
				}
			}
		}
		tmpEvent = guideevents[i];
		guideevents[i] = guideevents[idx];
		guideevents[idx] = tmpEvent;
		if( timeout + 50 < TAP_GetTick() ){
			if( Display_BackgroundDrawn ){
				if( strlen(text) < 128 ){
					strcat( text, "." );
					Message_Display(text);
					Display_DrawTime();
				}
			}
			timeout = TAP_GetTick();
		}
	}
	sortOrder = SORT_START;
	sorted = TRUE;
	Message_DisplayClear();
}

int Guide_GetEventSvcNum( int item ){
	TYPE_TapEvent event;
	if( item < guideevents_ptr ){
		event = *guideevents[item];
		return event.runningStatus;
	}
	return 0;
}

int Guide_GetEventSvcType( int item ){
	TYPE_TapEvent event;
	if( item < guideevents_ptr ){
		event = *guideevents[item];
		return event.satIdx;
	}
	return 0;
}

int Guide_GetEventSvcId( int item ){
	TYPE_TapEvent event;
	if( item < guideevents_ptr ){
		event = *guideevents[item];
	}
	return event.svcId;
}


void Guide_Activate( void ){
	int prevmode = Guide_Mode;
	TAP_Channel_GetCurrent( &Guide_Mode, &Guide_Chan );	// Set the current channel and mode
	if( prevmode != Guide_Mode ) Guide_HasEvents = FALSE;	// If the last mode we were inis different to now then flag to reget the events
	if( Events_Loading ){
		ShowMessageWin(_PROGRAM_NAME_, "Currently loading events", "Please try again later", 200);
		return;	// If we are loading events then disable loading until complete
	}
	Guide_CreateGradients();
	Guide_RenderDisplay();
}

void Guide_SearchStringIdle( void ){
	// Keyboard is active so lets see whether to search i.e outside the dealy time
	if( Keyboard_IsActive() ){
		if( lastPress + Setting_SearchDelay < TAP_GetTick() ){
			Guide_SearchString();
		}
	}
}

void Guide_SearchString( void ){
 	int resultsearch = 0;
	char text[128];
	if( Keyboard_GetPosition() < 1 ) return;
	if( Guide_SearchActive == SEARCH_NONE ) return;	// Search has search and there has been no change then return
	if( Keyboard_HasChanged() ){
		switch( Guide_SearchActive ){
			case SEARCH_TITLE:
				if( (resultsearch = Guide_KeyboardSearch(guideevents_ptr)) > -1 ){
					Guide_SelectedPrevRow = resultsearch;
					Guide_SelectedRow=resultsearch;
					Guide_MinRow=resultsearch;
					Guide_MaxRow=resultsearch+GUIDE_ITEMS;
					if( Guide_MaxRow > guideevents_ptr ){
						Guide_MaxRow = guideevents_ptr;
						Guide_MinRow = Guide_MaxRow - GUIDE_ITEMS;
						if( Guide_MinRow < 0 ){
							Guide_MinRow = 0;
						}
					}
					Guide_Redraw = TRUE;
					Guide_RenderDisplay();
				} else {
					TAP_SPrint( text, "Sorry. Not found." );
					Message_Display( text );
				}
				break;
			case SEARCH_DESC:
				if( (resultsearch = Guide_KeyboardSearchAnywhere(guideevents_ptr)) > -1 ){
					Guide_SelectedPrevRow = resultsearch;
					Guide_SelectedRow=resultsearch;
					Guide_MinRow=resultsearch;
					Guide_MaxRow=resultsearch+GUIDE_ITEMS;
					if( Guide_MaxRow > guideevents_ptr ){
						Guide_MaxRow = guideevents_ptr;
						Guide_MinRow = Guide_MaxRow - GUIDE_ITEMS;
						if( Guide_MinRow < 0 ){
							Guide_MinRow = 0;
						}
					}
					Guide_Redraw = TRUE;
					Guide_RenderDisplay();
				} else {
					TAP_SPrint( text, "Sorry. Not found." );
					Message_Display( text );
				}
				break;
		}
		Keyboard_SetChanged(FALSE);	// Set to not change as we have finished the search
	}
}

// Return from the timer edit component
void Guide_TimerEditReturn( int retval, bool cancel ){
	TAP_Osd_Copy( Display_RGN, Display_MemRGN, 0, 0, 720, 576, 0, 0, FALSE);	// Copy the main region to memory to avoid corruption from the previous OSD
	if( !cancel ){
		if( retval == 0 ){
//			Message_Display( "Timer successfully modified." );
		} ef( retval == 1 ){
			Message_Display( "ERROR: Timer setting failed. No resourse avaliable." );
		} ef( retval == 2 ){
			Message_Display( "ERROR: Timer setting failed. Invalid tuner." );
		} ef (retval == 0xffffffff){
			Message_Display( "ERROR: Timer setting failed. Error modifying." );
		} ef (retval >= 0xFFFE0000){
			Message_Display( "ERROR: Timer setting failed. Conflicts with other timers." );
		} else {
			Message_Display( "ERROR: Timer setting failed. Unknown cause." );
		}
		TAP_Delay(Setting_MessageTimeout);
		Guide_Redraw = TRUE;
		Guide_RenderDisplay();
	}
}

void Guide_FavNewReturn( favitem a, bool cancel ){
	TAP_Osd_Copy( Display_RGN, Display_MemRGN, 0, 0, 720, 576, 0, 0, FALSE);	// Copy the main region to memory to avoid corruption from the previous OSD
	if( !cancel ){	// User didnt cancel
		InsertFavItem( a );
		FavManage_Append();	// Save to file
		FavManage_FreeMemory();
		Message_Display( "Favourite Added" );	// Explained that the event has past and we can set because of that
	}
}

void Guide_NewFavourite( void ){
	favitem Item;
	TYPE_TapEvent event;
	event = Guide_GetEvent(Guide_SelectedRow);
	FavNew_Init( Display_RGN, Display_MemRGN,  (720/2)-(350/2)+DISPLAY_X_OFFSET, (576/2)-(400/2)+DISPLAY_Y_OFFSET, 350, 400 );
	FavNew_SetColours( DISPLAY_KEYBOARDITEMTEXT, DISPLAY_ITEM, DISPLAY_KEYBOARDITEMSELECTEDTEXT, DISPLAY_KEYBOARDITEMSELECTED, DISPLAY_MAIN );

	// Clear our favourite item
	memset(Item.term,0x0,sizeof(Item.term));
	memset(Item.channel,0x0,sizeof(Item.channel));
	memset(Item.day,0x0,sizeof(Item.day));
	Item.location = 0;
	Item.start = -1;
	Item.end = -1;
	Item.priority = 0;
	Item.adj = 0;
	Item.rpt = 0;

	// Now we need to see the values from the selected event
	strcpy(Item.term, event.eventName );
	sprintf( Item.channel, "%d\0", GetSvcLCN(event.svcId));
	strcpy(Item.day, "All\0" );
	Item.location = TITLE;
	Item.start = -1;//Time_MINS(event.startTime);
	Item.end = -1;//Endtime_MINS(event.startTime, event.duration);
	Item.priority = 1;
	Item.adj = 0;
	Item.rpt = 0;
	if( !FavNew_ActivateWithItem(Item, &Guide_FavNewReturn) ){
		Message_Display( "Favourite New Dialog Failed." );	// Explained that the event has past and we can set because of that
	}
}

void Guide_RecordingEditReturn( void ){
	Guide_Redraw = TRUE;
	Guide_RenderDisplay();
}

int Guide_GetEventPointer( void ){
	int i;
	for( i = 0; i < eventsPointer; i++ ){
		if( guideevents[Guide_SelectedRow] == events[i] ){
			return i;
		}
	}
	return -1;
}

int Guide_GetEventPointerRow( int row ){
	int i;
	for( i = 0; i < eventsPointer; i++ ){
		if( guideevents[row] == events[i] ){
			return i;
		}
	}
	return -1;
}


// Key handler for when guide is displayed
dword Guide_KeyHandler( dword param1 ){
	int tmp;
	Message_DisplayClear();	// If we have a message on the screen clear as the user has seen it if they have press a key
	if( ExtInfo_IsActive() ){	// Handle the extended information window
		return ExtInfo_Keyhandler( param1 );
	}
	if( TimerEditSmall_IsActive() ){
		return TimerEditSmall_KeyHandler(param1);
	}
	if( TimerEdit_IsActive() ){
		return TimerEdit_KeyHandler(param1);
	}
	if( RecordingEdit_IsActive() ){
		return RecordingEdit_KeyHandler(param1);
	}
	if( TimerEditNew_IsActive() ){
		return TimerEditNew_KeyHandler(param1);
	}
	if( FavEdit_IsActive() ){
		return FavEdit_KeyHandler(param1);
	}
	if( FavNew_IsActive() ){
		return FavNew_KeyHandler(param1);
	}
	if( GuideSortOrder_IsActive() ){
		return GuideSortOrder_KeyHandler(param1);
	}
	if( YesNo_IsActive() ){
		return YesNo_KeyHandler(param1);
	}
	if (ThreeOption_IsActive() ) {
		return ThreeOption_KeyHandler(param1);
	}
	if( TimerConflict_IsActive() ){
		return TimerConflict_Keyhandler(param1);
	}
	if( Keyboard_IsActive() ){	// Handle the keyboard
		if( param1 == RKEY_Next ){
			Guide_FindNextSearch();
			return 0;
		} else {
			return Keyboard_Keyhandler(param1);
		}
	}
	switch( param1 ){
		case RKEY_Menu:
	   		TAP_MemFree( _evtInfo );
	   		Guide_MemFree();
	   		Guide_Rendered = FALSE;
			Display_Hide();
			Menu_Render();
			return 0;
			break;
		case RKEY_0:
			Guide_ResetLoc();
			Guide_Redraw = TRUE;
			Guide_RenderDisplay();
			return 0;
			break;
		case RKEY_Info:
			Message_DisplayClear();
			tmp = Guide_GetEventPointer();
			if( tmp > -1 ){
				ExtInfo_Init( Display_RGN, Display_MemRGN, Panel_Left_X, Panel_Left_Y, DISPLAY_ITEM_W, (Panel_Left_Y + (( DISPLAY_ITEM_H + DISPLAY_GAP ) * GUIDE_ITEMS)-DISPLAY_GAP)-Panel_Left_Y );
				ExtInfo_Activate(tmp);
			}
			return 0;
			break;
		case RKEY_Ok:
			Guide_ChangeChannel(Guide_SelectedRow);
			return 0;
			break;
		case RKEY_ChDown:
			Guide_SelectedPrevRow = Guide_SelectedRow;
			if( guideevents_ptr < 1 ) return 0;
			Guide_SelectedRow++;
			if( Guide_SelectedRow > Guide_MaxRow-1 ){
				Guide_MinRow++;
				Guide_MaxRow++;
				Guide_Redraw = TRUE;
				if( Guide_MaxRow > guideevents_ptr-1 ){
					Guide_MinRow = guideevents_ptr-GUIDE_ITEMS;
					Guide_MaxRow = guideevents_ptr;
					if( Guide_MaxRow > guideevents_ptr ){
						Guide_MaxRow = guideevents_ptr;
					}
					if( Guide_MinRow < 0 ){
						Guide_MinRow = 0;
					}
				}
			}
			if( Guide_SelectedRow > guideevents_ptr-1 ){
				Guide_SelectedRow = 0;
				Guide_MinRow = 0;
				Guide_MaxRow = GUIDE_ITEMS;
				if( Guide_MaxRow > guideevents_ptr ){
					Guide_MaxRow = guideevents_ptr;
				}
				Guide_Redraw = TRUE;
			}
			Guide_RenderDisplay();
			return 0;
			break;
		case RKEY_ChUp:
			Guide_SelectedPrevRow = Guide_SelectedRow;
			if( guideevents_ptr < 1 ) return 0;
			Guide_SelectedRow--;
			if( Guide_SelectedRow < Guide_MinRow ){
				Guide_MinRow--;
				Guide_MaxRow--;
				Guide_Redraw = TRUE;
				if( Guide_MinRow < 0 ){
					Guide_MinRow = 0;
					Guide_MaxRow = GUIDE_ITEMS;
				}
				if( Guide_MaxRow > guideevents_ptr ){
					Guide_MaxRow = guideevents_ptr;
				}
			}
			if( Guide_SelectedRow < 0 ){
				Guide_SelectedRow = guideevents_ptr-1;
				Guide_MinRow = (guideevents_ptr)-GUIDE_ITEMS;
				Guide_MaxRow = guideevents_ptr;
				if( Guide_MinRow < 0 ){
					Guide_MinRow = 0;
				}
				Guide_Redraw = TRUE;
			}
			Guide_RenderDisplay();
			return 0;
			break;
		case RKEY_VolDown:
			Guide_SelectedPrevRow = Guide_SelectedRow;
			if( guideevents_ptr < 1 ) return 0;
			Guide_SelectedRow-=GUIDE_ITEMS;
			Guide_MinRow-=GUIDE_ITEMS;
			Guide_MaxRow-=GUIDE_ITEMS;
			if( Guide_SelectedRow < Guide_MinRow ){
				Guide_MinRow--;
				Guide_MaxRow--;
				if( Guide_MinRow < 0 ){
					Guide_MinRow = 0;
					Guide_MaxRow = GUIDE_ITEMS;
				}
				if( Guide_MaxRow > guideevents_ptr ){
					Guide_MaxRow = guideevents_ptr;
				}
			}
			if( Guide_SelectedRow < 0 ){
				Guide_SelectedRow = guideevents_ptr-1;
				Guide_MinRow = (guideevents_ptr)-GUIDE_ITEMS;
				Guide_MaxRow = guideevents_ptr;
			}
			if( Guide_MinRow < 0 ){
				Guide_MinRow = 0;
				Guide_SelectedRow = 0;
				Guide_MaxRow = GUIDE_ITEMS;
				if( Guide_MaxRow > guideevents_ptr ){
					Guide_MaxRow = guideevents_ptr;
				}
			}
			Guide_Redraw = TRUE;
			Guide_RenderDisplay();
			return 0;
			break;
		case RKEY_VolUp:
			Guide_SelectedPrevRow = Guide_SelectedRow;
			if( guideevents_ptr < 1 ) return 0;
			Guide_SelectedRow+=GUIDE_ITEMS;
			Guide_MinRow+=GUIDE_ITEMS;
			Guide_MaxRow+=GUIDE_ITEMS;
			if( Guide_SelectedRow > Guide_MaxRow-1 ){
				Guide_MinRow++;
				Guide_MaxRow++;
				if( Guide_MaxRow > guideevents_ptr-1 ){
					Guide_MinRow = guideevents_ptr-GUIDE_ITEMS;
					Guide_MaxRow = guideevents_ptr;
					if( Guide_MinRow < 0 ){
						Guide_MinRow = 0;
					}
				}
			}
			if( Guide_SelectedRow > guideevents_ptr-1 ){
				Guide_SelectedRow = 0;
				Guide_MinRow = 0;
				Guide_MaxRow = GUIDE_ITEMS;
				if( Guide_MaxRow > guideevents_ptr ){
					Guide_MaxRow = guideevents_ptr;
				}
			}
			Guide_Redraw = TRUE;
			Guide_RenderDisplay();
			return 0;
			break;
		case RKEY_Forward:
			if( sortOrder == SORT_CHANNEL ){
				Guide_MoveToNextChan( Guide_SelectedRow );
			} else {
				if( sortOrder == SORT_NAME ){
					Guide_MoveToNextChar(Guide_GetEventName(Guide_SelectedRow),Guide_SelectedRow);
				} else {
					Message_Display( "Must be sorted by name or channel to use this paging feature." );
				}
			}
			return 0;
			break;
		case RKEY_Rewind:
			if( sortOrder == SORT_CHANNEL ){
				Guide_MoveToPrevChan( Guide_SelectedRow );
			} else {
				if( sortOrder == SORT_NAME ){
//					Guide_MoveToPrevChar(Guide_GetEventName(Guide_SelectedRow),Guide_SelectedRow);
				} else {
					Message_Display( "Must be sorted by name or channel to use this paging feature." );
				}			
			}
			return 0;
			break;
		case RKEY_NewF1:
			// Initalise the keyboard position and colours
			Keyboard_Init( Display_RGN, Display_MemRGN,  Panel_Bottom_X, Panel_Bottom_Y, Panel_Bottom_W, Panel_Bottom_H, "Search Title" );	// Initalise the keyboard
			Keyboard_SetColours( DISPLAY_KEYBOARDITEM, DISPLAY_KEYBOARDITEMTEXT, DISPLAY_KEYBOARDITEMSELECTED, DISPLAY_KEYBOARDITEMSELECTEDTEXT, DISPLAY_KEYBOARDFIELDTEXT, DISPLAY_KEYBOARDFIELD, DISPLAY_KEYBOARDBACKGROUND );
			if( Keyboard_Activate("",&Guide_KeyboardClose) ){
				Guide_SearchActive = SEARCH_TITLE;
			}
			return 0;
			break;
		case RKEY_Bookmark:
			// Initalise the keyboard position and colours
			Keyboard_Init( Display_RGN, Display_MemRGN,  Panel_Bottom_X, Panel_Bottom_Y, Panel_Bottom_W, Panel_Bottom_H, "Search Description" );	// Initalise the keyboard
			Keyboard_SetColours( DISPLAY_KEYBOARDITEM, DISPLAY_KEYBOARDITEMTEXT, DISPLAY_KEYBOARDITEMSELECTED, DISPLAY_KEYBOARDITEMSELECTEDTEXT, DISPLAY_KEYBOARDFIELDTEXT, DISPLAY_KEYBOARDFIELD, DISPLAY_KEYBOARDBACKGROUND );
			if( Keyboard_Activate("",&Guide_KeyboardClose) ){
				Guide_SearchActive = SEARCH_DESC;
			}
			return 0;
			break;
		case RKEY_Recall:
			Events_Get();
			Guide_PopulateList();
			Guide_Redraw = TRUE;
			Guide_RenderDisplay();
			return 0;
			break;
		case RKEY_Record:
			// Call the edit timer component if timer completely covers event
			if( Timers_HasTimer(Guide_GetEventPointer()) == TIMER_FULL && Timers_IsRecordingEvent(Guide_GetEventPointer()) == TIMER_NONE ){
				TimerEditSmall_Init( Display_RGN, Display_MemRGN,  Panel_Bottom_X, Panel_Bottom_Y, Panel_Bottom_W, Panel_Bottom_H );
				TimerEditSmall_SetColours( DISPLAY_KEYBOARDITEMTEXT, DISPLAY_ITEM, DISPLAY_KEYBOARDITEMSELECTEDTEXT, DISPLAY_KEYBOARDITEMSELECTED, DISPLAY_MAIN );
				if( !TimerEditSmall_Activate(Guide_GetEventPointer(),&Guide_TimerEditReturn) ){
					Message_Display( "Timer not located." );	// Explained that the event has past and we can set because of that
				}
			} else {
				if( Timers_IsRecordingEvent(Guide_GetEventPointer()) == TIMER_PARTIAL ){
					int tuner = -1;
					if (HDD_isRecording(0) ) { // If Tuner 1 is recording check whether we were the one that just started it
						if (Update_RecordingInfo(0) ) {
							if (Recording_GetRecInfo(0).svcNum == GetSvcNum(Guide_GetEventSvcId(Guide_SelectedRow)) ) { // Same service as repaired file so we must have started it
								tuner = 0;
							}
						}
					}
					if (HDD_isRecording(1) ) { // If Tuner 1 is recording check whether we were the one that just started it
						if (Update_RecordingInfo(1) ) {
							if (Recording_GetRecInfo(1).svcNum == GetSvcNum(Guide_GetEventSvcId(Guide_SelectedRow)) ) { // Same service as repaired file so we must have started it
								tuner = 1;
							}
						}
					}
					if (tuner > -1&& tuner < 2) {
						if( Guide_GetEventStartTime(Guide_SelectedRow) > Recording_GetRecInfo(tuner).startTime &&
							Guide_GetEventEndTime(Guide_SelectedRow) > AddTime(Recording_GetRecInfo(tuner).startTime,Recording_GetRecInfo(tuner).duration) 
						){
							if ( !HDD_RECSlotSetDuration(tuner,(word)TimeDiff( Recording_GetRecInfo(tuner).startTime, AddTime(Guide_GetEventEndTime(Guide_SelectedRow), Settings_EndBuffer)))) { // Adjust the duration
								Message_Display("Timer started but duration failed to adjust");
								TAP_Delay(Setting_MessageTimeout);
								Guide_Redraw = TRUE;
								Guide_RenderDisplay();
							} else {
								Guide_Redraw = TRUE;
								Guide_RenderDisplay();
							}
						}
					}
				} else {
					if( Timers_IsRecordingEvent(Guide_GetEventPointer()) == TIMER_FULL ){	// If we are recording all of this event then launch the recording edit dialog
						int tuner = -1;
						if (HDD_isRecording(0) ) { // If Tuner 1 is recording check whether we were the one that just started it
							if (Update_RecordingInfo(0) ) {
								if (Recording_GetRecInfo(0).svcNum == GetSvcNum(Guide_GetEventSvcId(Guide_SelectedRow)) ) { // Same service as repaired file so we must have started it
									tuner = 0;
								}
							}
						}
						if (HDD_isRecording(1) ) { // If Tuner 1 is recording check whether we were the one that just started it
							if (Update_RecordingInfo(1) ) {
								if (Recording_GetRecInfo(1).svcNum == GetSvcNum(Guide_GetEventSvcId(Guide_SelectedRow)) ) { // Same service as repaired file so we must have started it
									tuner = 1;
								}
							}
						}
						if (tuner > -1&& tuner < 2) {
							RecordingEdit_Init( Display_RGN, Display_MemRGN,  Panel_Bottom_X, Panel_Bottom_Y, Panel_Bottom_W, 120 );
							RecordingEdit_SetColours(DISPLAY_KEYBOARDITEMTEXT, DISPLAY_ITEM,DISPLAY_KEYBOARDITEMSELECTEDTEXT,DISPLAY_KEYBOARDITEMSELECTED, DISPLAY_MAIN );
							if ( !RecordingEdit_Activate(tuner,&Guide_RecordingEditReturn) ) {
								Message_Display("Failed launching recording edit dialog."); // Explained that the event has past and we can set because of that
							}
						}
						return 0;
					} else {
						if( Timers_HasTimer(Guide_GetEventPointer()) == TIMER_PARTIAL ){
							// Seperated to allow for easy changes in the future
							// We have a partial timer so find it and launch the editing dialog
							Guide_CreateTimer(Guide_SelectedRow);
						} else {
							if( Guide_GetEventEndTime(Guide_SelectedRow) <= Now(0) ){
								Message_Display( "Event has finished." );	// Explained that the event has past and we can set because of that
								// TO DO - Add check of timeshift buffer and attempt to record from that.
							} else {
								if( Guide_GetEventStartTime(Guide_SelectedRow) <= Now(0) &&
									Timers_IsRecordingEvent(Guide_GetEventPointer()) == TIMER_NONE ){
									// Lets add the handling for changing to the channel and instant recording and adjusting the duration accordingly
									YesNo_Init( Display_RGN, Display_MemRGN,  Panel_Bottom_X, Panel_Bottom_Y, Panel_Bottom_W, Panel_Bottom_H );
									YesNo_SetColours( DISPLAY_KEYBOARDITEMTEXT, DISPLAY_ITEM, DISPLAY_KEYBOARDITEMSELECTEDTEXT, DISPLAY_KEYBOARDITEMSELECTED, DISPLAY_MAIN );
									if( !YesNo_Activate("Start recording event that has already started?",&Guide_InstantRecordCallback) ){
										Message_Display( "YesNo box failed." );	// Explained that the event has past and we can set because of that
									}
								} else {
									Guide_CreateTimer(Guide_SelectedRow);
								}
							}
						}
					}
				}
			}
			return 0;
			break;
		case RKEY_Teletext:
			if( Timers_HasTimer(Guide_GetEventPointer()) != TIMER_NONE &&
				Timers_IsRecordingEvent(Guide_GetEventPointer()) == TIMER_NONE ){
				// Seperated to allow for easy changes in the future
				// We have a partial timer so find it and launch the editing dialog
				TimerEditSmall_Init( Display_RGN, Display_MemRGN,  Panel_Bottom_X, Panel_Bottom_Y, Panel_Bottom_W, Panel_Bottom_H );
				TimerEditSmall_SetColours( DISPLAY_KEYBOARDITEMTEXT, DISPLAY_ITEM, DISPLAY_KEYBOARDITEMSELECTEDTEXT, DISPLAY_KEYBOARDITEMSELECTED, DISPLAY_MAIN );
				if( !TimerEditSmall_Activate(Guide_GetEventPointer(),&Guide_TimerEditReturn) ){
					Message_Display( "Timer not located." );	// Explained that the event has past and we can set because of that
				}
			} else {
				if( Timers_IsRecordingEvent(Guide_GetEventPointer()) != TIMER_NONE ){	// If we are recording all of this event then launch the recording edit dialog
					int tuner = -1;
					if (HDD_isRecording(0) ) { // If Tuner 1 is recording check whether we were the one that just started it
						if (Update_RecordingInfo(0) ) {
							if (Recording_GetRecInfo(0).svcNum == GetSvcNum(Guide_GetEventSvcId(Guide_SelectedRow)) ) { // Same service as repaired file so we must have started it
								tuner = 0;
							}
						}
					}
					if (HDD_isRecording(1) ) { // If Tuner 1 is recording check whether we were the one that just started it
						if (Update_RecordingInfo(1) ) {
							if (Recording_GetRecInfo(1).svcNum == GetSvcNum(Guide_GetEventSvcId(Guide_SelectedRow)) ) { // Same service as repaired file so we must have started it
								tuner = 1;
							}
						}
					}
					if (tuner > -1&& tuner < 2) {
						RecordingEdit_Init( Display_RGN, Display_MemRGN,  Panel_Bottom_X, Panel_Bottom_Y, Panel_Bottom_W, 125 );
						RecordingEdit_SetColours(DISPLAY_KEYBOARDITEMTEXT, DISPLAY_ITEM,DISPLAY_KEYBOARDITEMSELECTEDTEXT,DISPLAY_KEYBOARDITEMSELECTED, DISPLAY_MAIN );
						if ( !RecordingEdit_Activate(tuner,&Guide_RecordingEditReturn) ) {
							Message_Display("Failed launching recording edit dialog."); // Explained that the event has past and we can set because of that
						}
					}
					return 0;
				}
			}
			return 0;
			break;
		case RKEY_Pause:
			GuideSortOrder_Init( Display_RGN, Display_MemRGN,  Panel_Bottom_X, Panel_Bottom_Y, Panel_Bottom_W, Panel_Bottom_H );
			GuideSortOrder_SetColours( DISPLAY_KEYBOARDITEMTEXT, DISPLAY_ITEM, DISPLAY_KEYBOARDITEMSELECTEDTEXT, DISPLAY_KEYBOARDITEMSELECTED, DISPLAY_MAIN );
			if( !GuideSortOrder_Activate("Sort Order",&Guide_SortOrderCallback) ){
				Message_Display( "Option box failed." );	// Explained that the event has past and we can set because of that
			}
			return 0;
			break;
		case RKEY_Ab:
			if( Timers_HasTimer(Guide_GetEventPointer()) == TIMER_FULL &&
				Timers_IsRecordingEvent(Guide_GetEventPointer()) == TIMER_NONE){	// Only allow deletion of event if a timer fully covers it
				ThreeOption_Init( Display_RGN, Display_MemRGN,  Panel_Bottom_X, Panel_Bottom_Y, Panel_Bottom_W, Panel_Bottom_H );
				ThreeOption_SetColours( DISPLAY_KEYBOARDITEMTEXT, DISPLAY_ITEM, DISPLAY_KEYBOARDITEMSELECTEDTEXT, DISPLAY_KEYBOARDITEMSELECTED, DISPLAY_MAIN );
				if ( !ThreeOption_Activate("Delete timer or remove event?","Delete Timer","Remove Event",THREEOPTION_OPTION2,&Guide_DeleteTimerCallback) ) {
					Message_Display( "YesNo box failed." );	// Explained that the event has past and we can set because of that
				}
			} else {
				if( Timers_IsRecordingEvent(Guide_GetEventPointer()) == TIMER_FULL ){	// Current timer fully covers the event.
					YesNo_Init(Display_RGN, Display_MemRGN,  (720/2)-(250/2)+DISPLAY_X_OFFSET, (576/2)-(150/2)+DISPLAY_Y_OFFSET, 250, 200);
					YesNo_SetColours(DISPLAY_KEYBOARDITEMTEXT, DISPLAY_ITEM,DISPLAY_KEYBOARDITEMSELECTEDTEXT,DISPLAY_KEYBOARDITEMSELECTED, DISPLAY_MAIN );
					if ( !YesNo_Activate("Are you sure you want to remove event from current recording?",&Guide_RemoveRecEventCallback) ) {
						Message_Display("YesNo box failed."); // Explained that the event has past and we can set because of that
					}
				}
			}
			return 0;
			break;
		case RKEY_TvRadio:
			Guide_SwitchModes();	
			return 0;
			break;
		case RKEY_Fav:
			Guide_NewFavourite();
			return 0;
			break;
			
	}
	if( param1 == RKEY_Exit || param1 == Settings_LinearEPGKey ){
   		TAP_MemFree( _evtInfo );
   		Guide_MemFree();
   		Guide_Rendered = FALSE;
		Display_Hide();
		return 0;
	}
	return 0;
}

// Rountine to in the idle process alternate the colours of the events that are currently recording
void Guide_RecordingNowDisplay( void ){
	int i;
	static dword lastIdle = 0;
	static bool removed = FALSE;
	int recordingType = 0;
	if( lastIdle == 0 ){
		lastIdle = TAP_GetTick();
	}
	if( !Guide_Rendered ) return;
	if( ExtInfo_IsActive() ) return;
	if( Menu_IsActive() ) return;
	if( TimerConflict_IsActive() ) return;
	// Check every second
	if( lastIdle + 150 < TAP_GetTick() ){
		removed = !removed;
		lastIdle = TAP_GetTick();
		Update_RecordingInfo(0);
		Update_RecordingInfo(1);
		for( i = Guide_MinRow; i < Guide_MaxRow; i++ ){
			if( (recordingType = Timers_IsRecordingEvent(Guide_GetEventPointerRow(i))) != TIMER_NONE ){
				if( removed ){
					switch( recordingType ){
						case TIMER_FULL:
							drawDisk(Display_RGN, Panel_Left_X+5+5, Panel_Left_Y + (( DISPLAY_ITEM_H + DISPLAY_GAP ) * (i-Guide_MinRow))+(DISPLAY_ITEM_H/2), 5, DISPLAY_RECORDINGFULL);
							break;
						case TIMER_PARTIAL:
							drawDisk(Display_RGN, Panel_Left_X+5+5, Panel_Left_Y + (( DISPLAY_ITEM_H + DISPLAY_GAP ) * (i-Guide_MinRow))+(DISPLAY_ITEM_H/2), 5, DISPLAY_RECORDINGPARTIAL);
							break;
					}
				} else {
					drawDisk(Display_RGN, Panel_Left_X+5+5, Panel_Left_Y + (( DISPLAY_ITEM_H + DISPLAY_GAP ) * (i-Guide_MinRow))+(DISPLAY_ITEM_H/2), 5, DISPLAY_RECORDINGALT);
				}
			}
		}
	}
}

