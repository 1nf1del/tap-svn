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
#include "events.h"
#include "settings.h"
#include "timerconflict.h"
#include "extinfo.h"
#include "yesnobox.h"
#include "menu.h"
#include "multilistepg.h"


bool MultiList_isActive( void ){
	return MultiList_Active;
}

dword MultiList_Keyhandler( dword param1 ){
	if( param1 == RKEY_Exit ){
		MultiList_Deactivate();
	}
	return 0;
}

// Function to render the guide item on the screen. Send it the required text
void MultiList_RenderItem( word rgn, char * text, int eventNum, int item, int row, int col, int selectedrow, int selectedcol, int prevselectedrow, int prevselectedcol, bool fresh ){
	int x, y, w, h;
	dword fc = DISPLAY_EVENT;
	dword bc = DISPLAY_ITEM;
	TYPE_TapEvent event;
	event = Events_GetEvent(eventNum);
	// Calculate the position to render the item
//	w = (720-((Panel_Left_X-DISPLAY_X_OFFSET)*2))/MultiList_Cols;	// [    ][    ][    ]
	w = DISPLAY_ROW_WIDTH/MultiList_Cols;	// [    ][    ][    ]
	x = Panel_Left_X + (w*col) + DISPLAY_GAP;	// Offset plus the coloumn
	h = 22;
	y = (Panel_Left_Y + (( h + DISPLAY_GAP) * (item-MultiList_Min))+( h + DISPLAY_GAP))+(((( h + DISPLAY_GAP) * (MultiList_MaxEvents))+( h + DISPLAY_GAP))*row);
	
	if( eventNum > -1 ){
		if( event.startTime <= Now(0) && event.endTime >= Now(0) ){ fc = DISPLAY_NOWEVENT; }
		if( event.endTime <= Now(0) ){ fc = DISPLAY_EVENTPAST; }
	}
	
	// Check for full or partial and render appropriatly
	switch( Timers_HasTimer( eventNum ) ){
		case TIMER_FULL:
			bc = DISPLAY_RECORDINGFULL;
			break;
		case TIMER_PARTIAL:
			bc = DISPLAY_RECORDINGPARTIAL;
	}
	// Check for full or partial and render appropriatly
	switch( Timers_IsRecordingEvent( eventNum ) ){
		case TIMER_FULL:
			bc = DISPLAY_RECORDINGFULL;
			break;
		case TIMER_PARTIAL:
			bc = DISPLAY_RECORDINGPARTIAL;
	}	
	
	// Draw background for item it previous or selected otherwise the background on text will do the job
	// TO DO ADD NAVIAGTION AND HIGHLIGHTING	

	TAP_Osd_FillBox(rgn, x-1, y, w-2, h, bc);
	TAP_Osd_PutStringAf1419( rgn, x+5, y+1, x+(w-10), text, fc, COLOR_None );
}

// Function to render the guide item on the screen. Send it the required text
void MultiList_RenderChannel( word rgn, word svcId, int row, int col ){
	int x, y, w, h;
	dword fc = DISPLAY_EVENT;
	int svcNum, svcType;
 	TYPE_TapChInfo chInfo;
	char text[128];
	// Calculate the position to render the item
//	w = (720-((Panel_Left_X-DISPLAY_X_OFFSET)*2))/MultiList_Cols;	// [    ][    ][    ]
	w = DISPLAY_ROW_WIDTH/MultiList_Cols;	// [    ][    ][    ]
	x = Panel_Left_X + (w*col) + DISPLAY_GAP;	// Offset plus the coloumn
	h = 22;
	y = (Panel_Left_Y + (( h + DISPLAY_GAP) * (0)))+(((( h + DISPLAY_GAP) * (MultiList_MaxEvents))+( h + DISPLAY_GAP))*row);
	// Draw background for item it previous or selected otherwise the background on text will do the job
	TAP_Osd_FillBox(rgn, x-1, y, w-2, h, DISPLAY_MAIN);
	svcNum = GetSvcNum(svcId);
	svcType = GetSvcType(svcId);
	sprintf( text, "%d, %d, %s\0", svcNum, svcType, Channel_Name(svcNum,svcType));
	TAP_Channel_GetInfo( svcType, svcNum, &chInfo );
	TAP_Osd_PutStringAf1419( rgn, x+5, y+1, x+(w-10), chInfo.chName, fc, COLOR_None );
}

void MultiList_Render( void ){
	int i = 0;
	int j = 0;
	int k = 0;
	int c = 0;
	word svcId = 0;
	int foundItem = -1;
	bool loadingNeeded = FALSE;
	Display_CreateRegions();
	if( !MultiList_Active ){
		Screen_Set(Display_RGN);
		// No template so we need to redraw the display from scratch
		Display_DrawBackground();
		Display_DrawTime();
		Display_RenderTitle("Program Guide");
		for( i = 0; i < MultiList_Cols; i++ ){
			for( j = 0; j < MultiList_Rows; j++ ){
				for( k = 0; k < MultiList_MaxEvents; k++ ){
					MultiList_RenderItem( Display_RGN, "", 0, k, j, i, 0, 0, 0, 0, TRUE );
				}
			}
		}
		// Only get and sort if we haven't already done so
		if (dataCollected != DATA_COLLECTED ) {
			Screen_FadeIn(Display_RGN, Setting_FadeIn);
			loadingNeeded = TRUE;
			Events_Get(); // Populate the static events array
			Events_SortByChannel();
		}
		// Lets go thru and render the epg events from now onwards
		MultiList_StartTime = Now(0);
		c = 0;
		if( !loadingNeeded ){
			Screen_FadeIn(Display_RGN,Setting_FadeIn);
		}
		TAP_Osd_Copy( Display_RGN, Display_MemRGN, 0, 0, 720, 576, 0, 0, FALSE);
		for( i = 0; i < MultiList_Cols; i++ ){
			for( j = 0; j < MultiList_Rows; j++ ){
				svcId = GetSvcId(MultiList_Mode, (j*MultiList_Cols)+i);
				for( k = 0; k < eventsPointer; k++ ){
					// Lets go thru and get the events to populate the grid. Given we have grabed the data and sorted we only need to find the first event
					if( events[k]->svcId == svcId ){	// Right channel
						if( events[k]->startTime <= MultiList_StartTime && events[k]->endTime >= MultiList_StartTime ){
							foundItem = k;
							k = eventsPointer;
						}
					}
				}
				MultiList_RenderChannel( Display_MemRGN, svcId, j, i );
				if( foundItem > -1 ){	// We found the first item so lets render the next
					c = 0;
					for( k = foundItem; k < (foundItem+MultiList_MaxEvents) && k < eventsPointer; k++ ){
						MultiList_RenderItem( Display_MemRGN, events[k]->eventName, k, c, j, i, MultiList_SelectedRow, MultiList_SelectedCol, MultiList_PrevSelectedRow, MultiList_PrevSelectedCol, TRUE );
						c++;
					}
					for( k = c; k < MultiList_MaxEvents; k++ ){	// Render the remainding blank items
						MultiList_RenderItem( Display_MemRGN, "", -1, k, j, i, MultiList_SelectedRow, MultiList_SelectedCol, MultiList_PrevSelectedRow, MultiList_PrevSelectedCol, TRUE );
					}
				}
				k = c = 0;
				foundItem = -1;
			}
		}
		// Need to copy to the foreground
		TAP_Osd_Copy( Display_MemRGN, Display_RGN, 0, 0, 720, 576, 0, 0, FALSE);
	} else {
		if( MultiList_Redraw ){
			// Rerender the display without background
		} else {
			// Render changed items
		}
	}
	MultiList_Active = TRUE;
}

void MultiList_Activate( void ){
	MultiList_Render();
}

void MultiList_Deactivate( void ){
	MultiList_Active = FALSE;
	Display_Hide();
}
