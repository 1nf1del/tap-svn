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

#include "gridepg.h"
#include "fontman.h"
#include "skin.h"
#include "guide.h"
#include "yesnobox.h"
#include "threeoption.h"
#include "messagebox.h"
#include "timereditsmall.h"
#include "timerconflict.h"

void GridEPG_Idle( void ){
	if( GridEPG_IsActive() ){
		if( lastPress + OSD_RedrawDelay < TAP_GetTick() ){
			if( GridEPG_LastRendered + OSD_RedrawDelay < TAP_GetTick() ){
				if( GridEPG_Channels_ptr < 1){
					GridEPG_LastRendered = TAP_GetTick();
					return;
				}
				GridEPG_ReturnStart();
				GridEPG_SelectedRowChange();
				GridEPG_Redraw = TRUE;
				GridEPG_Render();
			}
		}
	}
}

bool GridEPG_IsActive(void) {
	return GridEPG_Active;
}

void GridEPG_MemFree( void ){
	if (GridEPG_ItemBase) TAP_MemFree(GridEPG_ItemBase);
	GridEPG_ItemBase = NULL;
}

void GridEPG_CreateGradients( void ){
	word rgn;
	GridEPG_MemFree();
	if( Settings_GradientFactor > 0 ){
		rgn	= TAP_Osd_Create(0, 0, DISPLAY_ROW_WIDTH, GRIDEPG_ROWHEIGHT, 0, OSD_Flag_MemRgn);
		// Main default item
		createGradient(rgn, DISPLAY_ROW_WIDTH, GRIDEPG_ROWHEIGHT, Settings_GradientFactor, DISPLAY_ITEM);
		GridEPG_ItemBase = TAP_Osd_SaveBox(rgn, 0, 0, DISPLAY_ROW_WIDTH, GRIDEPG_ROWHEIGHT);
		TAP_Osd_Delete(rgn);
	}
}

void GridEPG_Close(void) {
	GridEPG_Redraw = FALSE;
	GridEPG_SelectedCol = 0;
	GridEPG_PrevSelectedCol = 0;
	GridEPG_SelectedRow = 0;
	GridEPG_PrevSelectedRow = 0;
	GridEPG_Min = 0;
	GridEPG_Max = 0;
	GridEPG_TvChannels = 0;
	GridEPG_RdChannels = 0;
	GridEPG_AppendTimerNum = 0;
	// Change sort order back to previous incarnation
	GridEPG_Active = FALSE;
	if( GridEPG_ChannelExcludesChanged ){
		Settings_Save();	// Save the new excludes to our settings file
		GridEPG_ChannelExcludesChanged = FALSE;
	}
	GridEPG_MemFree();
	Display_Hide();
}

void GridEPG_AddExclude( int svcType, int svcNum ){
	GridEPG_ChannelExcludes[GridEPG_ChannelExcludes_ptr] = GetSvcLCN(GetSvcId(svcType, svcNum));
	GridEPG_ChannelExcludes_ptr++;
}

char * GridEPG_ExportExcludes( void ){
	int i;
	static char text[256];
	char buff[10];
	memset(text, 0, sizeof(text));
	if( GridEPG_ChannelExcludes_ptr < 1 ){
		// No excludes so just copy and return the settings value
		strcpy(text, Settings_ChannelExcludes);
		return text;
	}
	for( i = 0; i < GridEPG_ChannelExcludes_ptr; i++ ){
		if( GridEPG_ChannelExcludes[i] != 0 ){
			sprintf( buff, "%d\0", GridEPG_ChannelExcludes[i]);
			strcat( text, buff );
			if( i+1 < GridEPG_ChannelExcludes_ptr ){	// If not the last add the divider/token
				strcat( text, "," );
			}
		}
	}
	strcpy(Settings_ChannelExcludes, text);	// Copy to the settings variable
	return text;
}

void GridEPG_BuildExcludes( void ){
	int count = 100;	// Maximum number of excludes
	char *ptr;
	char store[256];//static so each iteration reads ok.
	int j,k=0;
	GridEPG_ChannelExcludes_ptr = 0;
	strcpy(store,Settings_ChannelExcludes);
	for (j=0;j<=count;j++) //have to keep getting next comma seperated LCN from the last iteration when this routine is called
	{
		if ((ptr = strchr(store,',')) == NULL) //no more comma's for null
		{
			GridEPG_ChannelExcludes[GridEPG_ChannelExcludes_ptr] = atoi(store);
			GridEPG_ChannelExcludes_ptr++;
			break;
		}
		else  //pointer will be set to point to the comma at this point
		{
			k=count+1;
			*ptr='\0';
			GridEPG_ChannelExcludes[GridEPG_ChannelExcludes_ptr] = atoi(store);
			GridEPG_ChannelExcludes_ptr++;
			strcpy(store,ptr+1);
		}
	}
}

void GridEPG_BuildChannels( void ){
	int i, j;
	int tv, rd;
	bool exclude = FALSE;
	TYPE_TapChInfo channel;
	if( GridEPG_ChannelsBuilt ) return;
	GridEPG_Channels_ptr = 0;
	TAP_Channel_GetTotalNum( &tv, &rd );
	GridEPG_BuildExcludes();
	// TV
	if( GridEPG_Mode == SVC_TYPE_Tv){
		for( i = 0; i < tv; i++ ){
			exclude = FALSE;
			TAP_Channel_GetInfo( SVC_TYPE_Tv, i, &channel );	// Get channel details
			// Look thru array of excludes to determine whether this channel is to be excluded
			for( j = 0; j < GridEPG_ChannelExcludes_ptr; j++ ){
				if( channel.logicalChNum == GridEPG_ChannelExcludes[j] ){
					exclude = TRUE;
					break;	// Break out of the loop
				}
			}
			if( !exclude ){
				if( GridEPG_Channels_ptr < sizeof(GridEPG_Channels) ){	// Bounds check
					GridEPG_Channels[GridEPG_Channels_ptr] = i;
					GridEPG_Channels_ptr++;
				}
			}
		}
	} else {
		// Radio Mode
		for( i = 0; i < rd; i++ ){
			exclude = FALSE;
			TAP_Channel_GetInfo( SVC_TYPE_Radio, i, &channel );	// Get channel details
			// Look thru array of excludes to determine whether this channel is to be excluded
			for( j = 0; j < GridEPG_ChannelExcludes_ptr; j++ ){
				if( channel.logicalChNum == GridEPG_ChannelExcludes[j] ){
					exclude = TRUE;
					break;	// Break out of the loop
				}
			}
			if( !exclude ){
				if( GridEPG_Channels_ptr < sizeof(GridEPG_Channels) ){	// Bounds check
					GridEPG_Channels[GridEPG_Channels_ptr] = i;
					GridEPG_Channels_ptr++;
				}
			}
		}
	}
	GridEPG_ChannelExcludesChanged = FALSE;
	GridEPG_ChannelsBuilt = TRUE;
}

// Function that will remove the selected channel from the display
void GridEPG_RemoveChannel( bool yes ){
	int i;
	TAP_Osd_Copy( Display_RGN, Display_MemRGN, 0, 0, 720, 576, 0, 0, FALSE);	// Copy the main region to memory to avoid corruption from the previous OSD
	if( yes ){
		GridEPG_AddExclude(GridEPG_Mode, GridEPG_Channels[GridEPG_SelectedRow]);
		for( i = GridEPG_SelectedRow; i < GridEPG_Channels_ptr-1; i++ ){
			// Move the items above us down in the array
			GridEPG_Channels[i] = GridEPG_Channels[i+1];
		}
		GridEPG_Channels_ptr--;
		if( GridEPG_Max > GridEPG_Channels_ptr-1 ){
			GridEPG_Max = GridEPG_Channels_ptr;	
			GridEPG_Min = GridEPG_Max - GRIDEPG_CHANNELS;
			if( GridEPG_Min < 0 ){
				GridEPG_Min = 0;
			}
		}
		if(GridEPG_SelectedRow >= GridEPG_Channels_ptr ) {
			GridEPG_SelectedRow = GridEPG_Channels_ptr-1;
			GridEPG_PrevSelectedRow = GridEPG_SelectedRow;
		}
		GridEPG_SelectedRowChange();
		for (i = 0; i < GRIDEPG_CHANNELS; i++) {
			GridEPG_RenderBlankRow(Display_MemRGN, i );
		}
		GridEPG_ChannelExcludesChanged = TRUE;
		GridEPG_Redraw = TRUE;
		GridEPG_Render();
	}
}

void GridEPG_ResetChannels( bool yes ){
	TAP_Osd_Copy( Display_RGN, Display_MemRGN, 0, 0, 720, 576, 0, 0, FALSE);	// Copy the main region to memory to avoid corruption from the previous OSD
	if( yes ){
		GridEPG_ChannelsBuilt = FALSE;
		memset( Settings_ChannelExcludes, 0, sizeof(Settings_ChannelExcludes) );	// Clear the settings as user has reset
		GridEPG_BuildChannels();
		GridEPG_Min = 0;
		GridEPG_Max = GRIDEPG_CHANNELS;
		if (GridEPG_Max > GridEPG_Channels_ptr ) {
			GridEPG_Max = GridEPG_Channels_ptr;
		}
		GridEPG_ChannelExcludes_ptr = 0;	// Reset the exclude channels
		GridEPG_ChannelExcludesChanged = TRUE;
		GridEPG_SelectedRowChange();
		GridEPG_Redraw = TRUE;
		GridEPG_Render();
	}
}

void GridEPG_SelectedRowChange(void) {
	int i;
	int col = 0;
	bool found = FALSE;
	// Lets find the location of the first element in our array for this channel
	for (i = 0; i < eventsPointer; i++) {
		// If its the right channel the start comparing and that it is the right type Tv/Radio
		if (Events_GetEventSvcNum(i) == GridEPG_Channels[GridEPG_SelectedRow] &&
			Events_GetEventSvcType(i) == GridEPG_Mode ) {
			if (
				// If we start before the grid but end after			
				(Events_GetEventStartTime(i) < GridEPG_StartTime && Events_GetEventEndTime(i) > GridEPG_StartTime ) ||
				// If we start after but finish before grid end
				(Events_GetEventStartTime(i) >= GridEPG_StartTime && Events_GetEventEndTime(i) < AddTime(GridEPG_StartTime,GridEPG_Size) )||
				// If we start before the grid ends but finsih after
				(Events_GetEventStartTime(i) >= GridEPG_StartTime && Events_GetEventStartTime(i) < AddTime(GridEPG_StartTime,GridEPG_Size) )) {
				if (Events_GetEventStartTime(i) <= GridEPG_SelectColTime && Events_GetEventEndTime(i) > GridEPG_SelectColTime ) {
					GridEPG_SelectedCol = col;
					GridEPG_SelectColTime = Events_GetEventStartTime(i);
					found = TRUE;
					i = eventsPointer;
				}
				col++;
			} else {
				if (AddTime(GridEPG_StartTime, GridEPG_Size) < Events_GetEventStartTime(i)) {
					// Past the end of the fgrid so break the loop
					i = eventsPointer;
				}
			}
		}
	}
	if ( !found ) { // If we didn't find a event set the default
		GridEPG_SelectedCol = 0;
		GridEPG_SelectColTime = GridEPG_StartTime;
	}
}

void GridEPG_SelectedColChange(void) {
	int i;
	i = GridEPG_GetEventPointer(); // Get the new selected event pointer
	if (i > -1) {
		GridEPG_SelectColTime = Events_GetEventStartTime(i);
	} else {
		GridEPG_SelectColTime = GridEPG_StartTime; // Reset to default
	}

}

dword GridEPG_Keyhandler(dword param1) {
	Message_DisplayClear(); // If we have a message on the screen clear as the user has seen it if they have press a key
	if (ExtInfo_IsActive() ) { // Handle the extended information window
		return ExtInfo_Keyhandler(param1 );
	}
	if (TimerEditSmall_IsActive() ) {
		return TimerEditSmall_KeyHandler(param1);
	}
	if (TimerEdit_IsActive() ) {
		return TimerEdit_KeyHandler(param1);
	}
	if (RecordingEdit_IsActive() ) {
		return RecordingEdit_KeyHandler(param1);
	}
	if (TimerEditNew_IsActive() ) {
		return TimerEditNew_KeyHandler(param1);
	}
	if (FavEdit_IsActive() ) {
		return FavEdit_KeyHandler(param1);
	}
	if (FavNew_IsActive() ) {
		return FavNew_KeyHandler(param1);
	}
	if (Menu_IsActive() ) {
		return Menu_Keyhandler(param1);
	}
	if (YesNo_IsActive() ) {
		return YesNo_KeyHandler(param1);
	}
	if (ThreeOption_IsActive() ) {
		return ThreeOption_KeyHandler(param1);
	}
	if (TimerConflict_IsActive() ) {
		return TimerConflict_Keyhandler(param1);
	}
	if (Keyboard_IsActive() ) { // Handle the keyboard
		return Keyboard_Keyhandler(param1);
	}
	switch (param1 ) {
		case RKEY_Menu:
			GridEPG_Close();
			Menu_Render();
			return 0;
			break;
		case RKEY_Check:
			if( GridEPG_Channels_ptr < 1) return 0;	// Disable movement up and down as there is nothing to move to
			GridEPG_StartTime = AddTime(GridEPG_StartTime, 1440); // Nearest half hour block
			GridEPG_SelectColTime = AddTime(GridEPG_SelectColTime, 1440); // Nearest half hour block
			GridEPG_SelectedRowChange();
			GridEPG_Redraw = TRUE;
			GridEPG_Render();
			return 0;
			break;
		case RKEY_NewF1:
			if( GridEPG_Channels_ptr < 1) return 0;	// Disable movement up and down as there is nothing to move to
			GridEPG_StartTime = AddTime(GridEPG_StartTime, -1440); // Nearest half hour block
			GridEPG_SelectColTime = AddTime(GridEPG_SelectColTime, -1440); // Nearest half hour block
			if (GridEPG_StartTime < AddTime(Now(0), -180) ) {
				GridEPG_ReturnStart(); // TO far behind so return to Now
			}
			GridEPG_SelectedRowChange();
			GridEPG_Redraw = TRUE;
			GridEPG_Render();
			return 0;
			break;
		case RKEY_Goto:
			if( GridEPG_Channels_ptr < 1) return 0;	// Disable movement up and down as there is nothing to move to
			GridEPG_StartTime = AddTime(GridEPG_StartTime, GridEPG_Size); // Nearest half hour block
			GridEPG_SelectColTime = AddTime(GridEPG_SelectColTime, GridEPG_Size); // Nearest half hour block
			GridEPG_SelectedRowChange();
			GridEPG_Redraw = TRUE;
			GridEPG_Render();
			return 0;
			break;
		case RKEY_Bookmark:
			if( GridEPG_Channels_ptr < 1) return 0;	// Disable movement up and down as there is nothing to move to
			GridEPG_StartTime = AddTime(GridEPG_StartTime, -GridEPG_Size); // Nearest half hour block
			GridEPG_SelectColTime = AddTime(GridEPG_SelectColTime, -GridEPG_Size); // Nearest half hour block
			GridEPG_SelectedRowChange();
			GridEPG_Redraw = TRUE;
			GridEPG_Render();
			return 0;
			break;
		case RKEY_Stop:
			if( GridEPG_Channels_ptr < 1) return 0;	// Disable movement up and down as there is nothing to move to
			GridEPG_ReturnStart();
			GridEPG_SelectedRowChange();
			GridEPG_Redraw = TRUE;
			GridEPG_Render();
			return 0;
			break;
		case RKEY_TvRadio:
			GridEPG_SwitchMode();
			return 0;
			break;
		case RKEY_VolUp:
			if( GridEPG_Channels_ptr < 1) return 0;	// Disable movement up and down as there is nothing to move to
			GridEPG_PrevSelectedRow = GridEPG_SelectedRow;
			GridEPG_PrevSelectedCol = GridEPG_SelectedCol;
			GridEPG_SelectedCol++;
			if (GridEPG_NumEvents(GridEPG_SelectedRow) < GridEPG_SelectedCol ) { // Less events then the selected row. Increment the start time
				GridEPG_StartTime = AddTime(GridEPG_StartTime, GridEPG_Size); // Nearest half hour block
				//				GridEPG_SelectColTime = AddTime(GridEPG_SelectColTime, GridEPG_Size);	// Nearest half hour block
				GridEPG_SelectedRowChange();
				GridEPG_Redraw = TRUE; // Need to redraw as we have changed pages
			} else {
				GridEPG_SelectedColChange();
			}
			GridEPG_Render();
			return 0;
			break;
		case RKEY_VolDown:
			if( GridEPG_Channels_ptr < 1) return 0;	// Disable movement up and down as there is nothing to move to
			GridEPG_PrevSelectedRow = GridEPG_SelectedRow;
			GridEPG_PrevSelectedCol = GridEPG_SelectedCol;
			GridEPG_SelectedCol--;
			if (GridEPG_SelectedCol < 0) {
				GridEPG_StartTime = AddTime(GridEPG_StartTime, -GridEPG_Size);
				GridEPG_SelectColTime = AddTime(GridEPG_StartTime, GridEPG_Size);	// Set the time
				GridEPG_SelectedCol = GridEPG_NumEvents(GridEPG_SelectedRow);		// Set the end column
				GridEPG_Redraw = TRUE; // Need to redraw as we have changed pages
			} else {
				GridEPG_SelectedColChange();
			}
			GridEPG_Render();
			return 0;
			break;
		case RKEY_Rewind:
			GridEPG_PrevSelectedRow = GridEPG_SelectedRow;
			if( GridEPG_Channels_ptr < 1) return 0;	// Disable movement up and down as there is nothing to move to
			GridEPG_SelectedRow-=GRIDEPG_CHANNELS;
			GridEPG_Min-=GRIDEPG_CHANNELS;
			GridEPG_Max-=GRIDEPG_CHANNELS;
			GridEPG_Redraw = TRUE; // Need to redraw as we have changed pages
			if( GridEPG_Min < 0 ){
				GridEPG_Min = 0;
				GridEPG_Max = GRIDEPG_CHANNELS;
				if( GridEPG_Max > GridEPG_Channels_ptr-1 ) GridEPG_Max = GridEPG_Channels_ptr;
			}
			if( GridEPG_SelectedRow < 0 ){
				GridEPG_SelectedRow = GridEPG_Channels_ptr-1;
				GridEPG_Max = GridEPG_Channels_ptr;
				GridEPG_Min = GridEPG_Max-GRIDEPG_CHANNELS;
				if( GridEPG_Min < 0 ) GridEPG_Min = 0;
			}
			GridEPG_SelectedRowChange();
			GridEPG_Render();
			return 0;
			break;
		case RKEY_ChUp:
			GridEPG_PrevSelectedRow = GridEPG_SelectedRow;
			if( GridEPG_Channels_ptr < 1) return 0;	// Disable movement up and down as there is nothing to move to
			GridEPG_SelectedRow--;
			if (GridEPG_SelectedRow < GridEPG_Min ) {
				GridEPG_Min--;
				GridEPG_Max--;
				GridEPG_Redraw = TRUE; // Need to redraw as we have changed pages
				GridEPG_PrevSelectedRow = GridEPG_SelectedRow;
				if (GridEPG_Min < 0) {
					// Beyond the first channel so move to the end
					GridEPG_Max = GridEPG_Channels_ptr;
					GridEPG_Min = GridEPG_Max - GRIDEPG_CHANNELS;
					GridEPG_SelectedRow = GridEPG_Max -1;
					GridEPG_PrevSelectedRow = GridEPG_SelectedRow;
					if (GridEPG_Min < 0) {
						GridEPG_Min = 0;
					}
				} else {
					// Need to move display down
					// Let move the display up on the mem region and render the last 2 items
					GridEPG_SelectedRowChange();
					GridEPG_PrevSelectedRow = GridEPG_SelectedRow;
					// Copy the section of no change in the memory region
					TAP_Osd_Copy(Display_MemRGN,Display_MemRGN,Panel_Left_X,Panel_Left_Y + (( GRIDEPG_ROWHEIGHT + DISPLAY_GAP ) * (0)),DISPLAY_ROW_WIDTH,(( GRIDEPG_ROWHEIGHT + DISPLAY_GAP )*(GRIDEPG_CHANNELS-1)),Panel_Left_X,Panel_Left_Y + (( GRIDEPG_ROWHEIGHT + DISPLAY_GAP ) * (1)), FALSE);
					GridEPG_RenderRow(Display_MemRGN, GridEPG_Min );
					GridEPG_RenderRow(Display_MemRGN, GridEPG_Min+1 );
					GridEPG_RenderTimers(Display_MemRGN );
					GridEPG_RenderBottomPanel(Display_MemRGN,GridEPG_GetEventPointer());
					TAP_Osd_Copy(Display_MemRGN, Display_RGN, 0, 0, 720, 576, 0, 0,	FALSE);
					return 0;
				}
			}
			GridEPG_SelectedRowChange();
			GridEPG_Render();
			return 0;
			break;
		case RKEY_Forward:
			GridEPG_PrevSelectedRow = GridEPG_SelectedRow;
			if( GridEPG_Channels_ptr < 1) return 0;	// Disable movement up and down as there is nothing to move to
			GridEPG_SelectedRow+=GRIDEPG_CHANNELS;
			GridEPG_Min+=GRIDEPG_CHANNELS;
			GridEPG_Max+=GRIDEPG_CHANNELS;
			GridEPG_Redraw = TRUE; // Need to redraw as we have changed pages
			if( GridEPG_Max > GridEPG_Channels_ptr-1 ){
				GridEPG_Max = GridEPG_Channels_ptr;
				GridEPG_Min = GridEPG_Max - GRIDEPG_CHANNELS;
			}
			if( GridEPG_SelectedRow > GridEPG_Channels_ptr-1 ){
				GridEPG_SelectedRow = 0;
				GridEPG_Min = 0;
				GridEPG_Max = GRIDEPG_CHANNELS;
				if( GridEPG_Max > GridEPG_Channels_ptr-1 ) GridEPG_Max = GridEPG_Channels_ptr;
			}
			if( GridEPG_Min < 0 ) GridEPG_Min = 0;
			GridEPG_SelectedRowChange();
			GridEPG_Render();
			return 0;
			break;		
		case RKEY_ChDown:
			GridEPG_PrevSelectedRow = GridEPG_SelectedRow;
			if( GridEPG_Channels_ptr < 1) return 0;	// Disable movement up and down as there is nothing to move to
			GridEPG_SelectedRow++;
			// Selected is beyond the max so adjust
			if (GridEPG_SelectedRow > GridEPG_Max-1) {
				GridEPG_Min++;
				GridEPG_Max++;
				GridEPG_PrevSelectedRow = GridEPG_SelectedRow;
				if (GridEPG_Max > GridEPG_Channels_ptr ) {
					// Beyond the maimum so we need to reset to the start
					GridEPG_Redraw = TRUE; // Need to redraw as we have changed pages
					GridEPG_Min = 0;
					GridEPG_Max = GRIDEPG_CHANNELS;
					if( GridEPG_Max > GridEPG_Channels_ptr -1 ){
						GridEPG_Max = GridEPG_Channels_ptr;
					}
					GridEPG_SelectedRow = GridEPG_Min;
					GridEPG_PrevSelectedRow = GridEPG_SelectedRow;
				} else {
					// Let move the display up on the mem region and render the last 2 items
					GridEPG_SelectedRowChange();
					GridEPG_PrevSelectedRow = GridEPG_SelectedRow;
					// Copy the section of no change in the memory region
					TAP_Osd_Copy(Display_MemRGN,Display_MemRGN,Panel_Left_X,Panel_Left_Y + (( GRIDEPG_ROWHEIGHT + DISPLAY_GAP ) * (1)),DISPLAY_ROW_WIDTH,(( GRIDEPG_ROWHEIGHT + DISPLAY_GAP )*(GRIDEPG_CHANNELS-2)),Panel_Left_X,Panel_Left_Y + (( GRIDEPG_ROWHEIGHT + DISPLAY_GAP ) * (0)), FALSE);
					GridEPG_RenderRow(Display_MemRGN, GridEPG_Min+(GRIDEPG_CHANNELS-2) );
					GridEPG_RenderRow(Display_MemRGN, GridEPG_Min+(GRIDEPG_CHANNELS-1) );
					GridEPG_RenderTimers(Display_MemRGN );
					GridEPG_RenderBottomPanel(Display_MemRGN,GridEPG_GetEventPointer());
					TAP_Osd_Copy(Display_MemRGN, Display_RGN, 0, 0, 720, 576, 0, 0,	FALSE);
					return 0;
				}
			}
			GridEPG_SelectedRowChange();
			GridEPG_Render();
			return 0;
			break;
		case RKEY_Info:
			Message_DisplayClear();
			if( GridEPG_Channels_ptr < 1) return 0;	// Disable movement up and down as there is nothing to move to
			ExtInfo_Init2(Display_RGN, Display_MemRGN, Panel_Left_X, Panel_Left_Y, DISPLAY_ROW_WIDTH,(Panel_Left_Y + (( GRIDEPG_ROWHEIGHT + DISPLAY_GAP )* (GRIDEPG_CHANNELS)))-Panel_Left_Y-DISPLAY_GAP, FALSE );
			ExtInfo_Activate(GridEPG_GetEventPointer());
			return 0;
			break;
		case RKEY_Ab:
			if( GridEPG_Channels_ptr < 1) return 0;	// Disable movement up and down as there is nothing to move to
			if (Timers_HasTimer(GridEPG_GetEventPointer()) == TIMER_FULL &&
				Timers_IsRecordingEvent(GridEPG_GetEventPointer()) == TIMER_NONE ) { // Only allow deletion of event if a timer fully covers it
				ThreeOption_Init(Display_RGN, Display_MemRGN,  (720/2)-(250/2)+DISPLAY_X_OFFSET, (576/2)-(150/2)+DISPLAY_Y_OFFSET, 250, 150);
				ThreeOption_SetColours(DISPLAY_KEYBOARDITEMTEXT, DISPLAY_ITEM,DISPLAY_KEYBOARDITEMSELECTEDTEXT,DISPLAY_KEYBOARDITEMSELECTED, DISPLAY_MAIN );
				if ( !ThreeOption_Activate("Delete timer or remove event?","Delete Timer","Remove Event",THREEOPTION_OPTION2,&GridEPG_DeleteTimerCallback) ) {
					Message_Display("YesNo box failed."); // Explained that the event has past and we can set because of that
				}
			} else {
				if( Timers_IsRecordingEvent(GridEPG_GetEventPointer()) == TIMER_FULL ){	// Current timer fully covers the event.
					YesNo_Init(Display_RGN, Display_MemRGN,  (720/2)-(250/2)+DISPLAY_X_OFFSET, (576/2)-(150/2)+DISPLAY_Y_OFFSET, 250, 200);
					YesNo_SetColours(DISPLAY_KEYBOARDITEMTEXT, DISPLAY_ITEM,DISPLAY_KEYBOARDITEMSELECTEDTEXT,DISPLAY_KEYBOARDITEMSELECTED, DISPLAY_MAIN );
					if ( !YesNo_Activate("Are you sure you want to remove event from current recording?",&GridEPG_RemoveRecEventCallback) ) {
						Message_Display("YesNo box failed."); // Explained that the event has past and we can set because of that
					}
				}
			}
			return 0;
			break;
		case RKEY_Fav:
			if( GridEPG_Channels_ptr < 1) return 0;	// Disable movement up and down as there is nothing to move to
			GridEPG_NewFavourite();
			return 0;
			break;
		case RKEY_Teletext:
			if( GridEPG_Channels_ptr < 1) return 0;	// Disable movement up and down as there is nothing to move to
			if (Timers_HasTimer(GridEPG_GetEventPointer()) != TIMER_NONE &&
				Timers_IsRecordingEvent(GridEPG_GetEventPointer()) == TIMER_NONE ) {	// Can't currently be recording
				// Seperated to allow for easy changes in the future
				// We have a partial timer so find it and launch the editing dialog
				TimerEditSmall_Init(Display_RGN, Display_MemRGN, (720/2)-(250/2)+DISPLAY_X_OFFSET,(576/2)-(220/2)+DISPLAY_Y_OFFSET, 250, 220);
				TimerEditSmall_SetColours(DISPLAY_KEYBOARDITEMTEXT, DISPLAY_ITEM,DISPLAY_KEYBOARDITEMSELECTEDTEXT,DISPLAY_KEYBOARDITEMSELECTED, DISPLAY_MAIN );
				if ( !TimerEditSmall_Activate(GridEPG_GetEventPointer(),&GridEPG_TimerEditReturn) ) {
					Message_Display("Timer not located."); // Explained that the event has past and we can set because of that
				}
			} else {
				if( Timers_IsRecordingEvent(GridEPG_GetEventPointer()) != TIMER_NONE  ){	// If we are recording this event then launch the recording edit dialog
					int tuner = -1;
					if (HDD_isRecording(0) ) { // If Tuner 1 is recording check whether we were the one that just started it
						if (Update_RecordingInfo(0) ) {
							if (Recording_GetRecInfo(0).svcNum == GetSvcNum(Events_GetEventSvcId(GridEPG_GetEventPointer())) ) { // Same service as repaired file so we must have started it
								tuner = 0;
							}
						}
					}
					if (HDD_isRecording(1) ) { // If Tuner 1 is recording check whether we were the one that just started it
						if (Update_RecordingInfo(1) ) {
							if (Recording_GetRecInfo(1).svcNum == GetSvcNum(Events_GetEventSvcId(GridEPG_GetEventPointer())) ) { // Same service as repaired file so we must have started it
								tuner = 1;
							}
						}
					}
					if (tuner > -1&& tuner < 2) {
						RecordingEdit_Init(Display_RGN, Display_MemRGN, (720/2)-(250/2)+DISPLAY_X_OFFSET,	(576/2)-(120/2)+DISPLAY_Y_OFFSET, 250, 120);
						RecordingEdit_SetColours(DISPLAY_KEYBOARDITEMTEXT, DISPLAY_ITEM,DISPLAY_KEYBOARDITEMSELECTEDTEXT,DISPLAY_KEYBOARDITEMSELECTED, DISPLAY_MAIN );
						if ( !RecordingEdit_Activate(tuner,&GridEPG_RecordingEditReturn) ) {
							Message_Display("Failed launching recording edit dialog."); // Explained that the event has past and we can set because of that
						}
					}
					return 0;
				}
			}
			return 0;
			break;
		case RKEY_Record:
			if( GridEPG_Channels_ptr < 1) return 0;	// Disable movement up and down as there is nothing to move to
			// Call the edit timer component if timer completely covers event
			if (Timers_HasTimer(GridEPG_GetEventPointer()) == TIMER_FULL &&
			    Timers_IsRecordingEvent(GridEPG_GetEventPointer()) == TIMER_NONE ) {
				TimerEditSmall_Init(Display_RGN, Display_MemRGN, (720/2)-(250/2)+DISPLAY_X_OFFSET,	(576/2)-(220/2)+DISPLAY_Y_OFFSET, 250, 220);
				TimerEditSmall_SetColours(DISPLAY_KEYBOARDITEMTEXT, DISPLAY_ITEM,DISPLAY_KEYBOARDITEMSELECTEDTEXT,DISPLAY_KEYBOARDITEMSELECTED, DISPLAY_MAIN );
				if ( !TimerEditSmall_Activate(GridEPG_GetEventPointer(),&GridEPG_TimerEditReturn) ) {
					Message_Display("Timer not located."); // Explained that the event has past and we can set because of that
				}
			} else {
				// If event is only partially covered by the current recording adjust the duration
				if( Timers_IsRecordingEvent(GridEPG_GetEventPointer()) == TIMER_PARTIAL ){
					int tuner = -1;
					if (HDD_isRecording(0) ) { // If Tuner 1 is recording check whether we were the one that just started it
						if (Update_RecordingInfo(0) ) {
							if (Recording_GetRecInfo(0).svcNum == GetSvcNum(Events_GetEventSvcId(GridEPG_GetEventPointer())) ) { // Same service as repaired file so we must have started it
								tuner = 0;
							}
						}
					}
					if (HDD_isRecording(1) ) { // If Tuner 1 is recording check whether we were the one that just started it
						if (Update_RecordingInfo(1) ) {
							if (Recording_GetRecInfo(1).svcNum == GetSvcNum(Events_GetEventSvcId(GridEPG_GetEventPointer())) ) { // Same service as repaired file so we must have started it
								tuner = 1;
							}
						}
					}
					if (tuner > -1&& tuner < 2) {
						if( Events_GetEventStartTime(GridEPG_GetEventPointer()) > Recording_GetRecInfo(tuner).startTime &&
							Events_GetEventEndTime(GridEPG_GetEventPointer()) > AddTime(Recording_GetRecInfo(tuner).startTime,Recording_GetRecInfo(tuner).duration) 
						){
							if ( !HDD_RECSlotSetDuration(tuner,(word)TimeDiff( Recording_GetRecInfo(tuner).startTime, AddTime(Events_GetEventEndTime(GridEPG_GetEventPointer()), Settings_EndBuffer)))) { // Adjust the duration
								Message_Display("Timer started but duration failed to adjust");
								TAP_Delay(Setting_MessageTimeout);
								GridEPG_Redraw = TRUE;
								GridEPG_Render();
							} else {
								GridEPG_Redraw = TRUE;
								GridEPG_Render();
							}
						}
					}
				} else {
					if( Timers_IsRecordingEvent(GridEPG_GetEventPointer()) == TIMER_FULL ){	// If we are recording all of this event then launch the recording edit dialog
						int tuner = -1;
						if (HDD_isRecording(0) ) { // If Tuner 1 is recording check whether we were the one that just started it
							if (Update_RecordingInfo(0) ) {
								if (Recording_GetRecInfo(0).svcNum == GetSvcNum(Events_GetEventSvcId(GridEPG_GetEventPointer())) ) { // Same service as repaired file so we must have started it
									tuner = 0;
								}
							}
						}
						if (HDD_isRecording(1) ) { // If Tuner 1 is recording check whether we were the one that just started it
							if (Update_RecordingInfo(1) ) {
								if (Recording_GetRecInfo(1).svcNum == GetSvcNum(Events_GetEventSvcId(GridEPG_GetEventPointer())) ) { // Same service as repaired file so we must have started it
									tuner = 1;
								}
							}
						}
						if (tuner > -1&& tuner < 2) {
							RecordingEdit_Init(Display_RGN, Display_MemRGN, (720/2)-(250/2)+DISPLAY_X_OFFSET,	(576/2)-(120/2)+DISPLAY_Y_OFFSET, 250, 120);
							RecordingEdit_SetColours(DISPLAY_KEYBOARDITEMTEXT, DISPLAY_ITEM,DISPLAY_KEYBOARDITEMSELECTEDTEXT,DISPLAY_KEYBOARDITEMSELECTED, DISPLAY_MAIN );
							if ( !RecordingEdit_Activate(tuner,&GridEPG_RecordingEditReturn) ) {
								Message_Display("Failed launching recording edit dialog."); // Explained that the event has past and we can set because of that
							}
						}
						return 0;
					} else {
						// If event is only partially covered by the current recording adjust the duration
						if (Timers_HasTimer(GridEPG_GetEventPointer()) == TIMER_PARTIAL ) {
							// Seperated to allow for easy changes in the future
							// We have a partial timer so find it and launch the editing dialog
							GridEPG_CreateTimer(GridEPG_GetEventPointer());
						} else {
							if (Events_GetEventEndTime(GridEPG_GetEventPointer()) <= Now(0) ) {
								Message_Display("Event has finished."); // Explain that the event has past and we can't set because of that
								// TO DO - Add check of timeshift buffer and attempt to record from that.
							} else {
								if (Events_GetEventStartTime(GridEPG_GetEventPointer())	<= Now(0) &&
									Timers_IsRecordingEvent(GridEPG_GetEventPointer()) == TIMER_NONE ) {
									// Lets add the handling for changing to the channel and instant recording and adjusting the duration accordingly
									YesNo_Init(Display_RGN, Display_MemRGN, (720/2)-(250/2)+DISPLAY_X_OFFSET, (576/2)-(150/2)+DISPLAY_Y_OFFSET, 250, 150);
									YesNo_SetColours(DISPLAY_KEYBOARDITEMTEXT,DISPLAY_ITEM, DISPLAY_KEYBOARDITEMSELECTEDTEXT,DISPLAY_KEYBOARDITEMSELECTED, DISPLAY_MAIN );
									if ( !YesNo_Activate("Start recording event that has already started?",&GridEPG_InstantRecordCallback) ) {
										Message_Display("YesNo box failed."); // Explained that the event has past and we can set because of that
									}
								} else {
									GridEPG_CreateTimer(GridEPG_GetEventPointer());
								}
							}
						}
					} 
				}
			}
			return 0;
			break;
		case RKEY_Ok:
			if( GridEPG_Channels_ptr < 1) return 0;	// Disable movement up and down as there is nothing to move to
			GridEPG_ChangeChannel(GridEPG_GetEventPointer());
			return 0;
			break;
		case RKEY_Recall:
			Events_Get();
			Events_SortByChannel();
			GridEPG_Redraw = TRUE; // Need to redraw as we have changed pages
			GridEPG_Render();
			return 0;
			break;
		case RKEY_1:
			GridEPG_Size = 60;
			GridEPG_Redraw = TRUE; // Need to redraw as we have changed pages
			GridEPG_Render();
			return 0;
			break;
		case RKEY_2:
			GridEPG_Size = 120;
			GridEPG_Redraw = TRUE; // Need to redraw as we have changed pages
			GridEPG_Render();
			return 0;
			break;
		case RKEY_3:
			GridEPG_Size = 180;
			GridEPG_Redraw = TRUE; // Need to redraw as we have changed pages
			GridEPG_Render();
			return 0;
			break;
		case RKEY_4:
			GridEPG_Size = 240;
			GridEPG_Redraw = TRUE; // Need to redraw as we have changed pages
			GridEPG_Render();
			return 0;
			break;
		case RKEY_5:
			GridEPG_Size = 300;
			GridEPG_Redraw = TRUE; // Need to redraw as we have changed pages
			GridEPG_Render();
			return 0;
			break;
		case RKEY_6:
			GridEPG_Size = 360;
			GridEPG_Redraw = TRUE; // Need to redraw as we have changed pages
			GridEPG_Render();
			return 0;
			break;
		case RKEY_Sleep:	// For removing the channels
			if( GridEPG_Channels_ptr < 1) return 0;	// Disable movement up and down as there is nothing to move to
			YesNo_Init(Display_RGN, Display_MemRGN,  (720/2)-(250/2)+DISPLAY_X_OFFSET, (576/2)-(150/2)+DISPLAY_Y_OFFSET, 250, 150);
			YesNo_SetColours(DISPLAY_KEYBOARDITEMTEXT, DISPLAY_ITEM,DISPLAY_KEYBOARDITEMSELECTEDTEXT,DISPLAY_KEYBOARDITEMSELECTED, DISPLAY_MAIN );
			if ( !YesNo_Activate("Are you sure you want to remove channel?",&GridEPG_RemoveChannel) ) {
				Message_Display("YesNo box failed."); // Explained that the event has past and we can set because of that
			}
			return 0;
			break;
		case RKEY_Uhf:
			if( GridEPG_Channels_ptr < 1) return 0;	// Disable movement up and down as there is nothing to move to
			YesNo_Init(Display_RGN, Display_MemRGN,  (720/2)-(250/2)+DISPLAY_X_OFFSET, (576/2)-(150/2)+DISPLAY_Y_OFFSET, 250, 150);
			YesNo_SetColours(DISPLAY_KEYBOARDITEMTEXT, DISPLAY_ITEM,DISPLAY_KEYBOARDITEMSELECTEDTEXT,DISPLAY_KEYBOARDITEMSELECTED, DISPLAY_MAIN );
			if ( !YesNo_Activate("Are you sure you want to reset channels?",&GridEPG_ResetChannels) ) {
				Message_Display("YesNo box failed."); // Explained that the event has past and we can set because of that
			}
			return 0;
			break;
	}
	if( param1 == RKEY_Exit || param1 == Settings_GridEPGKey ){
		GridEPG_Close();
		return 0;
	}
	return 0;
}

void GridEPG_RenderTimeLoc(void) {
	int y_pos = Panel_Left_Y-28;//GRIDEPG_ROWHEIGHT;
	int x_pos = 0;
	TAP_Osd_FillBox(Display_RGN, Panel_Left_X, y_pos+14, DISPLAY_ROW_WIDTH, 10, DISPLAY_MAIN);
	TAP_Osd_FillBox(Display_MemRGN, Panel_Left_X, y_pos+14, DISPLAY_ROW_WIDTH, 10, DISPLAY_MAIN);
	if (TimeDiff(GridEPG_StartTime, Now(0)) >= 0 && TimeDiff(GridEPG_StartTime,Now(0)) < GridEPG_Size ) {
		// Time is onscreen
		x_pos = GRIDEPG_EVTSTART+(TimeDiff(GridEPG_StartTime, Now(0))*GRIDEPG_EVTWIDTH)/GridEPG_Size;
		TAP_Osd_FillBox(Display_RGN, x_pos-1, y_pos+14, 2, 10, DISPLAY_EPGTIMES);
		TAP_Osd_FillBox(Display_MemRGN, x_pos-1, y_pos+14, 2, 10,DISPLAY_EPGTIMES);
	}
}

void GridEPG_RenderTopPanel(int rgn) {
	int i = 0;
	int y_pos = Panel_Left_Y-28;//GRIDEPG_ROWHEIGHT;
	int x_pos = Panel_Left_X;
	// DOW DD/MM is to squashed when logos are activate so change to DOW DD
	if( Settings_GridEPGLogos ){
		Font_Osd_PutString1622(rgn, x_pos+GRIDEPG_GAP, y_pos-8, (GRIDEPG_EVTSTART+GRIDEPG_EVTWIDTH)-45, Time_DOWDDSpec(GridEPG_StartTime),DISPLAY_EPGTIMES, DISPLAY_MAIN );
	} else {
		Font_Osd_PutString1622(rgn, x_pos+GRIDEPG_GAP, y_pos-8, (GRIDEPG_EVTSTART+GRIDEPG_EVTWIDTH)-45, Time_DOWDDMMSpec(GridEPG_StartTime),DISPLAY_EPGTIMES, DISPLAY_MAIN );
	}
	// For each hour we are displaying render the time HH:MM
	for (i = 0; i < (GridEPG_Size/60); i++) {
		x_pos = GRIDEPG_EVTSTART+((i*60)*GRIDEPG_EVTWIDTH)/GridEPG_Size;
		Font_Osd_PutString1622(rgn, x_pos+GRIDEPG_GAP, y_pos-8,(GRIDEPG_EVTSTART+GRIDEPG_EVTWIDTH)-45, Time_HHMM(AddTime(GridEPG_StartTime, (i*60))), DISPLAY_EPGTIMES,DISPLAY_MAIN );
	}
	GridEPG_RenderTimeLoc();
}

// IN: Location in events array of the event to render the details of
void GridEPG_RenderEmptyBottomPanel(int rgn) {
	int y_pos = Panel_Left_Y + (( GRIDEPG_ROWHEIGHT + DISPLAY_GAP )	* (GRIDEPG_CHANNELS));
	int x_pos = Panel_Left_X;
	TAP_Osd_FillBox(rgn, x_pos, y_pos, DISPLAY_ROW_WIDTH-80,(428+Panel_Left_Y)-y_pos, DISPLAY_MAIN);
}

// IN: Location in events array of the event to render the details of
void GridEPG_RenderBottomPanel(int rgn, int item) {
	int y_pos = Panel_Left_Y + (( GRIDEPG_ROWHEIGHT + DISPLAY_GAP )	* (GRIDEPG_CHANNELS));
	int x_pos = Panel_Left_X;
	char text[512];
	char *desc;
//	byte *eventDesc = NULL;
	TYPE_TapEvent event;
	// Sanity Checks
	if (item < 0)
		return;
	if (item > eventsPointer-1)
		return;
	memset(text, '\0', sizeof(text ) );
	event = Events_GetEvent(item);
//	TAP_Osd_FillBox(rgn, x_pos, y_pos, DISPLAY_ROW_WIDTH-80,(( 28-DISPLAY_GAP ) * (5)), DISPLAY_MAIN);
	TAP_Osd_FillBox(rgn, x_pos, y_pos, DISPLAY_ROW_WIDTH-80,(428+Panel_Left_Y)-y_pos, DISPLAY_MAIN);
	Font_Osd_PutString1622(rgn, x_pos+GRIDEPG_GAP+GRIDEPG_GAP+GRIDEPG_GAP, y_pos + 5, (GRIDEPG_EVTSTART+GRIDEPG_EVTWIDTH-80), Events_GetEventName(item), DISPLAY_EPGTIMES, DISPLAY_MAIN );
	// Add description extender support
	if( PatchIsInstalled((dword*) 0x80000000, "De") || strlen(event.description) > 128 ){
//		// Description extender is present so use the eventname + event description for the extended info
		desc = event.eventName + event.description[127];
		strcpy(text,desc);
		if( strlen(text) > 0 ){
			if( GRIDEPG_CHANNELS == GridEPG_AltNum ){
				WrapPutStr(rgn, text, x_pos+GRIDEPG_GAP+GRIDEPG_GAP+GRIDEPG_GAP, y_pos + 8+(( 28-DISPLAY_GAP ) * (1)), DISPLAY_ROW_WIDTH-80-((GRIDEPG_GAP+GRIDEPG_GAP+GRIDEPG_GAP)*2), DISPLAY_EVENT, DISPLAY_MAIN, 1, FNT_Size_1622, 3);
			} else {
				WrapPutStr(rgn, text, x_pos+GRIDEPG_GAP+GRIDEPG_GAP+GRIDEPG_GAP, y_pos + 8+(( 28-DISPLAY_GAP ) * (1)), DISPLAY_ROW_WIDTH-80-((GRIDEPG_GAP+GRIDEPG_GAP+GRIDEPG_GAP)*2), DISPLAY_EVENT, DISPLAY_MAIN, 2, FNT_Size_1622, 3);
			}
		} else {
			WrapPutStr(rgn, "Description not avaliable", x_pos+GRIDEPG_GAP+GRIDEPG_GAP+GRIDEPG_GAP, y_pos + 8+(( 28-DISPLAY_GAP ) * (1)), DISPLAY_ROW_WIDTH-80-((GRIDEPG_GAP+GRIDEPG_GAP+GRIDEPG_GAP)*2), DISPLAY_EVENT, DISPLAY_MAIN, 1, FNT_Size_1622, 3);
		}
//		Font_Osd_PutString1622(rgn, x_pos+GRIDEPG_GAP+GRIDEPG_GAP+GRIDEPG_GAP, y_pos + 8+(( GRIDEPG_ROWHEIGHT-DISPLAY_GAP ) * (1)), (GRIDEPG_EVTSTART+GRIDEPG_EVTWIDTH-80), desc, DISPLAY_EVENT, DISPLAY_MAIN );
	} else {
		// No description extender support so use the default extended information
		if( strlen(event.description) > 0 ){
			if( GRIDEPG_CHANNELS == GridEPG_AltNum ){
				WrapPutStr(rgn, event.description, x_pos+GRIDEPG_GAP+GRIDEPG_GAP+GRIDEPG_GAP, y_pos + 8+(( 28-DISPLAY_GAP ) * (1)), DISPLAY_ROW_WIDTH-80-((GRIDEPG_GAP+GRIDEPG_GAP+GRIDEPG_GAP)*2), DISPLAY_EVENT, DISPLAY_MAIN, 1, FNT_Size_1622, 3);
			} else {
				WrapPutStr(rgn, event.description, x_pos+GRIDEPG_GAP+GRIDEPG_GAP+GRIDEPG_GAP, y_pos + 8+(( 28-DISPLAY_GAP ) * (1)), DISPLAY_ROW_WIDTH-80-((GRIDEPG_GAP+GRIDEPG_GAP+GRIDEPG_GAP)*2), DISPLAY_EVENT, DISPLAY_MAIN, 2, FNT_Size_1622, 3);
			}
		} else {
			WrapPutStr(rgn, "Description not avaliable", x_pos+GRIDEPG_GAP+GRIDEPG_GAP+GRIDEPG_GAP, y_pos + 8+(( 28-DISPLAY_GAP ) * (1)), DISPLAY_ROW_WIDTH-80-((GRIDEPG_GAP+GRIDEPG_GAP+GRIDEPG_GAP)*2), DISPLAY_EVENT, DISPLAY_MAIN, 1, FNT_Size_1622, 3);
		}
		// Use font width calculations
//		if( TAP_Osd_GetW(event.description,0,FNT_Size_1622) < DISPLAY_ROW_WIDTH-80-((GRIDEPG_GAP+GRIDEPG_GAP+GRIDEPG_GAP)*2)){
//		if( strlen(event.description) < 50 ){
//			// Lets place the extended information of the fourth row
//			eventDesc = TAP_EPG_GetExtInfo( &event );	
//			if( eventDesc != NULL ){
//				if( strlen(eventDesc) > 0 ){
//					WrapPutStr(rgn, eventDesc, x_pos+GRIDEPG_GAP+GRIDEPG_GAP+GRIDEPG_GAP, y_pos + 8+((28-DISPLAY_GAP ) * (2))+2, DISPLAY_ROW_WIDTH-80-((GRIDEPG_GAP+GRIDEPG_GAP+GRIDEPG_GAP)*2), DISPLAY_EVENT, DISPLAY_MAIN, 1, FNT_Size_1622, 3);
//					WrapPutStr_StartPos = LastWrapPutStr_P;
//				}
//				TAP_MemFree(eventDesc);
//			}
//		}
	}
	memset(text, '\0', sizeof(text ) );
	TAP_SPrint(text, "%s, %s ~ %s, %3d mins\0", Time_DOWDDMMSpec(Events_GetEventStartTime(item)), Time_HHMM(Events_GetEventStartTime(item)), Endtime_HHMM(Events_GetEventStartTime(item), TimeDiff(Events_GetEventStartTime(item), Events_GetEventEndTime(item))), TimeDiff(Events_GetEventStartTime(item), Events_GetEventEndTime(item)));
	Font_Osd_PutString1622(rgn, x_pos+GRIDEPG_GAP+GRIDEPG_GAP+GRIDEPG_GAP, LastWrapPutStr_Y+3, (GRIDEPG_EVTSTART+GRIDEPG_EVTWIDTH-80), text, DISPLAY_EPGTIMES, DISPLAY_MAIN );
}

// Function that will go thru and render the timers on the screen
// ____________________________________
//
void GridEPG_RenderTimers(int rgn) {
	TYPE_TimerInfo currentTimer;
	int totalTimers = 0;
	int i, j;
	int width;
	int x_pos = 0;
	int y_pos = 0;
	bool valid = TRUE;
	char text[512];
	memset(text, 0, sizeof(text));
	totalTimers = TAP_Timer_GetTotalNum();
	for ( i = 0; i < totalTimers; i++ ) {
		TAP_Timer_GetInfo(i, &currentTimer); // Sequentially get timer details and compare with those in the display range
		if(
		// If we start before the grid but end after			
		( currentTimer.startTime < GridEPG_StartTime && AddTime(currentTimer.startTime, currentTimer.duration) > GridEPG_StartTime ) ||
		// If we start after but finish before grid end
		( currentTimer.startTime >= GridEPG_StartTime && AddTime(currentTimer.startTime, currentTimer.duration) < AddTime(GridEPG_StartTime,GridEPG_Size) )||
		// If we start before the grid ends but finsih after
		( currentTimer.startTime >= GridEPG_StartTime && currentTimer.startTime <= AddTime(GridEPG_StartTime,GridEPG_Size) )
		) {
			// Timer is part of the display
			for( j = GridEPG_Min; j < GridEPG_Max; j++ ){	// Loop thru the min to the max range and compare with the Channels array
				// Work out the x and width and use the svcNum for y. Also check that the mode is correct Tv/Radio
				if( currentTimer.svcNum == GridEPG_Channels[j] &&
					currentTimer.svcType == GridEPG_Mode ) {
					y_pos = Panel_Left_Y + (( GRIDEPG_ROWHEIGHT + DISPLAY_GAP ) * (j-GridEPG_Min));//(currentTimer.svcNum-GridEPG_Min));
					if( currentTimer.startTime < GridEPG_StartTime ) {
						// Begining of the grid
						x_pos = GRIDEPG_EVTSTART;
						width = (TimeDiff(GridEPG_StartTime, AddTime(currentTimer.startTime, currentTimer.duration))*GRIDEPG_EVTWIDTH)/GridEPG_Size;
					} else {
						if( AddTime(currentTimer.startTime,currentTimer.duration) > AddTime(GridEPG_StartTime,GridEPG_Size) ) {
							// End of the grid with some duration past the edge
							x_pos = GRIDEPG_EVTSTART+(TimeDiff(GridEPG_StartTime,currentTimer.startTime)*GRIDEPG_EVTWIDTH)/GridEPG_Size;
							// Calculate how many mins will be shown
							width = (GRIDEPG_EVTSTART+GRIDEPG_EVTWIDTH)-x_pos;
						} else {
							x_pos = GRIDEPG_EVTSTART+(TimeDiff(GridEPG_StartTime,currentTimer.startTime)*GRIDEPG_EVTWIDTH)/GridEPG_Size;
							width = (currentTimer.duration*GRIDEPG_EVTWIDTH)/GridEPG_Size; // This is correct
						}
					}
					valid = TRUE;
					// Sanity Checks
					if( width > GRIDEPG_EVTWIDTH ){
						width = GRIDEPG_EVTWIDTH;
					}
					if( x_pos+width > 719 ){
						valid = FALSE;
					}
					if( x_pos <= 0 ){
						valid = FALSE;
					}
					if( y_pos+GRIDEPG_ROWHEIGHT > 575 ){
						valid = FALSE;
					}
					if( y_pos <= 0 ){
						valid = FALSE;
					}
					if( width <= (GRIDEPG_GAP*2) ){
						valid = FALSE;
					}
					if( valid ){
						TAP_Osd_FillBox(rgn, x_pos, y_pos+(GRIDEPG_ROWHEIGHT-(GRIDEPG_GAP*2)), width, GRIDEPG_GAP*2, DISPLAY_EPGREC);
					}
					break;
				}
			}
		}
	}
	Update_RecordingInfo(0);
	Update_RecordingInfo(1);
	// Now render any current recordings
	if( HDD_isRecording(0) ) {
		for( i = GridEPG_Min; i < GridEPG_Max; i++ ){	// Loop thru the min to the max range and compare with the Channels array
		// Check for same channel
			if( Recording_GetRecInfo(0).svcNum == GridEPG_Channels[i] &&
				Recording_GetRecInfo(0).svcType == GridEPG_Mode ) { // Only do if the channel is currently displaying
			//			sprintf(text, "Recording Tuner 1 : Start %s, End %s, Duration %d\0", Time_HHMM(Recording_GetRecInfo(0).startTime), Time_HHMM2(Recording_GetRecInfo(0).endTime), Recording_GetRecInfo(0).duration );
			//			Log("JimsGuide.log", "JimsGuide", FALSE, TIMESTAMP_NONE, text);
				if(
				// If we start before the grid but end after			
				( Recording_GetRecInfo(0).startTime < GridEPG_StartTime && AddTime(Recording_GetRecInfo(0).startTime, Recording_GetRecInfo(0).duration) > GridEPG_StartTime ) ||
				// If we start after but finish before grid end
				( Recording_GetRecInfo(0).startTime >= GridEPG_StartTime && AddTime(Recording_GetRecInfo(0).startTime, Recording_GetRecInfo(0).duration) < AddTime(GridEPG_StartTime,GridEPG_Size) )||
				// If we start before the grid ends but finsih after
				( Recording_GetRecInfo(0).startTime >= GridEPG_StartTime && Recording_GetRecInfo(0).startTime <= AddTime(GridEPG_StartTime,GridEPG_Size) )
				) {
					y_pos = Panel_Left_Y + (( GRIDEPG_ROWHEIGHT + DISPLAY_GAP ) * (i-GridEPG_Min));//(Recording_GetRecInfo(0).svcNum-GridEPG_Min));
					if( Recording_GetRecInfo(0).startTime < GridEPG_StartTime ) {
						// Begining of the grid
						x_pos = GRIDEPG_EVTSTART;
						width = (TimeDiff(GridEPG_StartTime, AddTime(Recording_GetRecInfo(0).startTime, Recording_GetRecInfo(0).duration))*GRIDEPG_EVTWIDTH)/GridEPG_Size;
					} else {
						if( AddTime(Recording_GetRecInfo(0).startTime,Recording_GetRecInfo(0).duration) > AddTime(GridEPG_StartTime,GridEPG_Size) ) {
							// End of the grid with some duration past the edge
							x_pos = GRIDEPG_EVTSTART+(TimeDiff(GridEPG_StartTime,Recording_GetRecInfo(0).startTime)*GRIDEPG_EVTWIDTH)/GridEPG_Size;
							// Calculate how many mins will be shown
							width = (GRIDEPG_EVTSTART+GRIDEPG_EVTWIDTH)-x_pos;
						} else {
							x_pos = GRIDEPG_EVTSTART+(TimeDiff(GridEPG_StartTime,Recording_GetRecInfo(0).startTime)*GRIDEPG_EVTWIDTH)/GridEPG_Size;
							width = (Recording_GetRecInfo(0).duration*GRIDEPG_EVTWIDTH)/GridEPG_Size; // This is correct
						}
					}
					// Sanity Checks
					valid = TRUE;
					if( width > GRIDEPG_EVTWIDTH ) width = GRIDEPG_EVTWIDTH;
					if( x_pos+width > 719 )	valid = FALSE;
					if( x_pos <= 0 ) valid = FALSE;;
					if( y_pos+GRIDEPG_ROWHEIGHT > 575 ) valid = FALSE;;
					if( y_pos <= 0 ) valid = FALSE;;
					if( width <= (GRIDEPG_GAP*2) ) valid = FALSE;
					if( valid ){
						TAP_Osd_FillBox(rgn, x_pos, y_pos+(GRIDEPG_ROWHEIGHT-(GRIDEPG_GAP*2)), width, GRIDEPG_GAP*2, DISPLAY_EPGREC);
					}
					break;
				}
			}
		}
	}
	if( HDD_isRecording(1) ){
		for( i = GridEPG_Min; i < GridEPG_Max; i++ ){	// Loop thru the min to the max range and compare with the Channels array
			// Check for same channel
			if( Recording_GetRecInfo(1).svcNum == GridEPG_Channels[i] &&
				Recording_GetRecInfo(1).svcType == GridEPG_Mode ) { // Only do if the channel is currently displaying
				if(
				// If we start before the grid but end after			
				( Recording_GetRecInfo(1).startTime < GridEPG_StartTime && AddTime(Recording_GetRecInfo(1).startTime, Recording_GetRecInfo(1).duration) > GridEPG_StartTime ) ||
				// If we start after but finish before grid end
				( Recording_GetRecInfo(1).startTime >= GridEPG_StartTime && AddTime(Recording_GetRecInfo(1).startTime, Recording_GetRecInfo(1).duration) < AddTime(GridEPG_StartTime,GridEPG_Size) )||
				// If we start before the grid ends but finsih after
				( Recording_GetRecInfo(1).startTime >= GridEPG_StartTime && Recording_GetRecInfo(1).startTime <= AddTime(GridEPG_StartTime,GridEPG_Size) )
				) {
					y_pos = Panel_Left_Y + (( GRIDEPG_ROWHEIGHT + DISPLAY_GAP ) * (i-GridEPG_Min));//(Recording_GetRecInfo(1).svcNum-GridEPG_Min));
					if( Recording_GetRecInfo(1).startTime < GridEPG_StartTime ) {
						// Begining of the grid
						x_pos = GRIDEPG_EVTSTART;
						width = (TimeDiff(GridEPG_StartTime, AddTime(Recording_GetRecInfo(1).startTime, Recording_GetRecInfo(1).duration))*GRIDEPG_EVTWIDTH)/GridEPG_Size;
					} else {
						if( AddTime(Recording_GetRecInfo(1).startTime,Recording_GetRecInfo(1).duration) > AddTime(GridEPG_StartTime,GridEPG_Size) ) {
							// End of the grid with some duration past the edge
							x_pos = GRIDEPG_EVTSTART+(TimeDiff(GridEPG_StartTime,Recording_GetRecInfo(1).startTime)*GRIDEPG_EVTWIDTH)/GridEPG_Size;
							// Calculate how many mins will be shown
							width = (GRIDEPG_EVTSTART+GRIDEPG_EVTWIDTH)-x_pos;
						} else {
							x_pos = GRIDEPG_EVTSTART+(TimeDiff(GridEPG_StartTime,Recording_GetRecInfo(1).startTime)*GRIDEPG_EVTWIDTH)/GridEPG_Size;
							width = (Recording_GetRecInfo(1).duration*GRIDEPG_EVTWIDTH)/GridEPG_Size; // This is correct
						}
					}
					valid = TRUE;
					// Sanity Checks
					if( width > GRIDEPG_EVTWIDTH ) width = GRIDEPG_EVTWIDTH;
					if( x_pos+width > 719 ) valid = FALSE;
					if( x_pos <= 0 ) valid = FALSE;
					if( y_pos+GRIDEPG_ROWHEIGHT > 575 ) valid = FALSE;
					if( y_pos <= 0 ) valid = FALSE;
					if( width <= (GRIDEPG_GAP*2) ) valid = FALSE;
					if( valid ){
						TAP_Osd_FillBox(rgn, x_pos, y_pos+(GRIDEPG_ROWHEIGHT-(GRIDEPG_GAP*2)), width, GRIDEPG_GAP*2, DISPLAY_EPGREC);
					}
					break;
				}
			}
		}
	}
}

int GridEPG_RenderItem(int rgn, dword starttime, int duration, int channel,	char * event, bool selected) {
	int x_pos, y_pos;
	int width;
	int retval;
	TYPE_TapChInfo channelInfo;
	int channelNetId;
	int rec1NetId, rec2NetId;
	char text[512];
	dword bc = DISPLAY_ITEM;
	dword fc = DISPLAY_EVENT;
	memset(text, 0, sizeof(text));

	// ALter the text colour if required
	if( starttime <= Now(0) && AddTime(starttime, duration) >= Now(0) ) {fc = DISPLAY_NOWEVENT;}
	if( AddTime(starttime, duration) <= Now(0) ) {fc = DISPLAY_EVENTPAST;}

	if( selected ) {
		bc = DISPLAY_ITEMSELECTED;
	}
	
	// If recording on both tuners check whether this is not on those channels
	if( HDD_isRecording(0) && HDD_isRecording(1) ){
		// If recording 2 channels and this event isn't 1 of them then change the background colour
		// Need to get the svc information to compare the transponder/freq information
		Update_RecordingInfo( 0 );
		Update_RecordingInfo( 1 );
		TAP_Channel_GetInfo( GridEPG_Mode, GridEPG_Channels[channel], &channelInfo );
		channelNetId = channelInfo.orgNetId;
		// Lets compare with the running recordings
		TAP_Channel_GetInfo( GridEPG_Mode, Recording_GetRecInfo(0).svcNum, &channelInfo );
		rec1NetId = channelInfo.orgNetId;
		TAP_Channel_GetInfo( GridEPG_Mode, Recording_GetRecInfo(1).svcNum, &channelInfo );
		rec2NetId = channelInfo.orgNetId;
		if( rec1NetId != channelNetId &&
			rec2NetId != channelNetId ){
			bc = DISPLAY_CHANNELNOTAVAL;
		}
	}
	y_pos = Panel_Left_Y + (( GRIDEPG_ROWHEIGHT + DISPLAY_GAP ) * (channel-GridEPG_Min));
	
	if( (starttime < GridEPG_StartTime) &&
	    (AddTime(starttime,duration) > AddTime(GridEPG_StartTime,GridEPG_Size)) ) {
		x_pos = GRIDEPG_EVTSTART;
		width = (duration*GRIDEPG_EVTWIDTH)/GridEPG_Size; // This is correct
		retval = STARTENDOVER;
	} else {
		if( starttime < GridEPG_StartTime ) {
			// Begining of the grid
			x_pos = GRIDEPG_EVTSTART;
			width = (TimeDiff(GridEPG_StartTime, AddTime(starttime, duration))*GRIDEPG_EVTWIDTH)/GridEPG_Size;
			retval = STARTBEFORE;
			//		sprintf(text, "Before - Channel : %d, Event : %s, Start : %s, Grid Start : %s, Duration : %d, x : %d, y : %d, width : %d\0", channel, event, Time_HHMM(starttime), Time_HHMM2(GridEPG_StartTime), duration, x_pos, y_pos, width );
		} else {
			if( AddTime(starttime,duration) > AddTime(GridEPG_StartTime,GridEPG_Size) ) {
				// End of the grid with some duration past the edge
				x_pos = GRIDEPG_EVTSTART+(TimeDiff(GridEPG_StartTime,starttime)*GRIDEPG_EVTWIDTH)/GridEPG_Size;
				// Calculate how many mins will be shown
				width = (GRIDEPG_EVTSTART+GRIDEPG_EVTWIDTH)-x_pos;
				retval = FINISHAFTER;
				//			sprintf(text, "After - Channel : %d, Event : %s, Start : %s, Grid Start : %s, Duration : %d, x : %d, y : %d, width : %d\0", channel, event, Time_HHMM(starttime), Time_HHMM2(GridEPG_StartTime), duration, x_pos, y_pos, width );
			} else {
				x_pos = GRIDEPG_EVTSTART+(TimeDiff(GridEPG_StartTime,starttime)*GRIDEPG_EVTWIDTH)/GridEPG_Size;
				width = (duration*GRIDEPG_EVTWIDTH)/GridEPG_Size; // This is correct
				retval = MIDDLE;
				//			sprintf(text, "Middle - Channel : %d, Event : %s, Start : %s, Grid Start : %s, Duration : %d, x : %d, y : %d, width : %d\0", channel, event, Time_HHMM(starttime), Time_HHMM2(GridEPG_StartTime), duration, x_pos, y_pos, width );
			}
		}
	}
	// Output to the log to see what is happening
	//	Log("JimsGuide.log", "JimsGuide", FALSE, TIMESTAMP_NONE, text);
	if( width > GRIDEPG_EVTWIDTH ) width = GRIDEPG_EVTWIDTH;
	if( x_pos+width > 719 ) return retval; // Sanity check
	if( x_pos <= 0 ) return retval;
	if( y_pos+GRIDEPG_ROWHEIGHT > 575 ) return retval;
	if( y_pos <= 0 ) return retval;
	if( width <= (GRIDEPG_GAP*2) ){
		if( width >= 1 ){
			if( selected || bc == DISPLAY_CHANNELNOTAVAL ){	// If we are selected or disabled then render accordingly
				TAP_Osd_FillBox(rgn, x_pos, y_pos, width, GRIDEPG_ROWHEIGHT, bc);
			}
			TAP_Osd_FillBox(rgn, x_pos, y_pos, GRIDEPG_GAP, GRIDEPG_ROWHEIGHT, DISPLAY_MAIN);
		}
		return retval;
	}
	// Lets print some debugging info tocheck the location is rendered properly
	// Render the black sections at the start/end of event block
	TAP_Osd_FillBox(rgn, x_pos, y_pos, GRIDEPG_GAP, GRIDEPG_ROWHEIGHT, DISPLAY_MAIN);
	TAP_Osd_FillBox(rgn, x_pos+width, y_pos, GRIDEPG_GAP, GRIDEPG_ROWHEIGHT, DISPLAY_MAIN);
	// Render the item background
	if( selected ){
		TAP_Osd_FillBox(rgn, x_pos+GRIDEPG_GAP, y_pos, width-(GRIDEPG_GAP), GRIDEPG_ROWHEIGHT, DISPLAY_ITEMSELECTED);
	} else {
		if( bc == DISPLAY_CHANNELNOTAVAL ){
			TAP_Osd_FillBox(rgn, x_pos+GRIDEPG_GAP, y_pos, width-(GRIDEPG_GAP), GRIDEPG_ROWHEIGHT, DISPLAY_CHANNELNOTAVAL);
		}
	}
	if( width > 20 ){
		Font_Osd_PutString1622( rgn, x_pos+(GRIDEPG_GAP*3), y_pos + ((GRIDEPG_ROWHEIGHT-22)/2), x_pos + width-(GRIDEPG_GAP*3), event, fc, COLOR_None );
	}
	return retval;
}

void GridEPG_RenderChannel(int rgn, int channel) {
	TYPE_TapChInfo chInfo;
	int x_pos, y_pos;
	if( (channel-GridEPG_Min) < 0 ) return;
	if( channel > GridEPG_Max ) return;
	x_pos = Panel_Left_X;
	y_pos = Panel_Left_Y + (( GRIDEPG_ROWHEIGHT + DISPLAY_GAP ) * (channel-GridEPG_Min));
	if( Settings_GridEPGChannels == GridEPG_AltNum && Settings_GridEPGLogos ){
		DisplayLogo( rgn, x_pos, y_pos, GridEPG_Channels[channel], GridEPG_Mode );
		return;
	} else {
		// Small logos for the channel identification
		if( Settings_GridEPGLogos ){
			DisplaySMLogo( rgn, x_pos, y_pos, GridEPG_Channels[channel], GridEPG_Mode );
			return;
		} else {
			if( channel == GridEPG_SelectedRow ) {
				TAP_Osd_FillBox(rgn, x_pos, y_pos, GRIDEPG_CHANNELWIDTH, GRIDEPG_ROWHEIGHT, DISPLAY_ITEMSELECTED);
			}
		}
	}
	TAP_Channel_GetInfo(GridEPG_Mode, GridEPG_Channels[channel], &chInfo );
	Font_Osd_PutString1622(rgn, x_pos+DISPLAY_GAP+DISPLAY_GAP, y_pos + ((GRIDEPG_ROWHEIGHT-22)/2), x_pos + GRIDEPG_CHANNELWIDTH-DISPLAY_GAP, chInfo.chName, DISPLAY_EVENT, COLOR_None );
}

int GridEPG_NumEvents(int channel) {
	int i;
	int row = 0;
	char text[512];
	memset(text, 0, sizeof(text));
	// Lets find the location of the first element in our array for this channel
	for( i = 0; i < eventsPointer; i++ ) {
		// If its the right channel the start comparing also check for mode Tv/Radio
		if( Events_GetEventSvcNum(i) == GridEPG_Channels[channel] &&
			Events_GetEventSvcType(i) == GridEPG_Mode ) {
			if(
			// If we start before the grid but end after			
			( Events_GetEventStartTime(i) < GridEPG_StartTime && Events_GetEventEndTime(i) > GridEPG_StartTime ) ||
			// If we start after but finish before grid end
			( Events_GetEventStartTime(i) >= GridEPG_StartTime && Events_GetEventEndTime(i) < AddTime(GridEPG_StartTime,GridEPG_Size) )||
			// If we start before the grid ends but finsih after
			( Events_GetEventStartTime(i) >= GridEPG_StartTime && Events_GetEventStartTime(i) < AddTime(GridEPG_StartTime,GridEPG_Size) )
			) {
				row++;
			} else {
				if( AddTime(GridEPG_StartTime,GridEPG_Size) < Events_GetEventStartTime(i) ) {
					// Past the end of the fgrid so break the loop
					i = eventsPointer;
				}
			}
		}
	}
	return row-1;
}

void GridEPG_RecordingEditReturn( void ){
	TAP_Osd_Copy(Display_RGN, Display_MemRGN, 0, 0, 720, 576, 0, 0, FALSE); // Copy the main region to memory to avoid corruption from the previous OSD
	GridEPG_Redraw = TRUE;
	GridEPG_Render();
}

int GridEPG_GetEventPointer(void) {
	int i;
	int col = 0;
	char text[512];
	memset(text, 0, sizeof(text));
	// Lets find the location of the first element in our array for this channel
	for( i = 0; i < eventsPointer; i++ ) {
		// If its the right channel the start comparing
		if( Events_GetEventSvcNum(i) == GridEPG_Channels[GridEPG_SelectedRow] &&
			Events_GetEventSvcType(i) == GridEPG_Mode ) {
			if(
			// If we start before the grid but end after			
			( Events_GetEventStartTime(i) < GridEPG_StartTime && Events_GetEventEndTime(i) > GridEPG_StartTime ) ||
			// If we start after but finish before grid end
			( Events_GetEventStartTime(i) >= GridEPG_StartTime && Events_GetEventEndTime(i) < AddTime(GridEPG_StartTime,GridEPG_Size) )||
			// If we start before the grid ends but finsih after
			( Events_GetEventStartTime(i) >= GridEPG_StartTime && Events_GetEventStartTime(i) < AddTime(GridEPG_StartTime,GridEPG_Size) )
			) {
				if( GridEPG_SelectedCol == col ) {
					return i;
				}
				col++;
			} else {
				if( AddTime(GridEPG_StartTime,GridEPG_Size) < Events_GetEventStartTime(i) ) {
					// Past the end of the fgrid so break the loop
					i = eventsPointer;
				}
			}
		}
	}
	return -1;
}

void GridEPG_RenderEvents(int rgn, int channel) {
	int i;
	int col = 0;
	char text[512];
	int tmpVal = 0;
	int x_pos, y_pos;
	bool startBefore = FALSE;
	bool endAfter = FALSE;
	memset(text, 0, sizeof(text));
	// Lets find the location of the first element in our array for this channel
	for( i = 0; i < eventsPointer; i++ ) {
		// If its the right channel the start comparing
		if( Events_GetEventSvcNum(i) == GridEPG_Channels[channel] &&
			Events_GetEventSvcType(i) == GridEPG_Mode ) {
			if(
			// If we start before the grid but end after			
			( Events_GetEventStartTime(i) < GridEPG_StartTime && Events_GetEventEndTime(i) > GridEPG_StartTime ) ||
			// If we start after but finish before grid end
			( Events_GetEventStartTime(i) >= GridEPG_StartTime && Events_GetEventEndTime(i) < AddTime(GridEPG_StartTime,GridEPG_Size) )||
			// If we start before the grid ends but finsih after
			( Events_GetEventStartTime(i) >= GridEPG_StartTime && Events_GetEventStartTime(i) < AddTime(GridEPG_StartTime,GridEPG_Size) )
			) {
				if( GridEPG_SelectedCol == col && GridEPG_SelectedRow == channel ) {
					if( (tmpVal=GridEPG_RenderItem( rgn, Events_GetEventStartTime(i), TimeDiff(Events_GetEventStartTime(i), Events_GetEventEndTime(i)), channel, Events_GetEventName(i), TRUE )) != MIDDLE ) {
						if( tmpVal == STARTENDOVER ) {
							startBefore = TRUE;
							endAfter = TRUE;
						} else {
							if( tmpVal == STARTBEFORE ) {
								startBefore = TRUE;
							} else {
								endAfter = TRUE;
							}
						}
					}
				} else {
					if( (tmpVal=GridEPG_RenderItem( rgn, Events_GetEventStartTime(i), TimeDiff(Events_GetEventStartTime(i), Events_GetEventEndTime(i)), channel, Events_GetEventName(i), FALSE )) != MIDDLE ) {
						if( tmpVal == STARTENDOVER ) {
							startBefore = TRUE;
							endAfter = TRUE;
						} else {
							if( tmpVal == STARTBEFORE ) {
								startBefore = TRUE;
							} else {
								endAfter = TRUE;
							}
						}
					}
				}
				col++;
			} else {
				if( AddTime(GridEPG_StartTime,GridEPG_Size) < Events_GetEventStartTime(i) ) {
					// Past the end of the fgrid so break the loop
					i = eventsPointer;
				}
			}
		}
	}
	// Now lets render the indicatpr for event starts before and events ends after
	x_pos = GRIDEPG_EVTSTART;
	y_pos = Panel_Left_Y + (( GRIDEPG_ROWHEIGHT + DISPLAY_GAP ) * (channel-GridEPG_Min));
	if( startBefore ) {
		Font_Osd_PutString1622( rgn, x_pos-12+GRIDEPG_GAP, y_pos + ((GRIDEPG_ROWHEIGHT-22)/2), x_pos + 12-GRIDEPG_GAP, "<", DISPLAY_EVENT, COLOR_None );
	}
	x_pos = GRIDEPG_EVTSTART+(GridEPG_Size*GRIDEPG_EVTWIDTH)/GridEPG_Size;
	if( endAfter ) {
		Font_Osd_PutString1622( rgn, x_pos+GRIDEPG_GAP, y_pos+((GRIDEPG_ROWHEIGHT-22)/2), x_pos +GRIDEPG_GAP + 12, ">", DISPLAY_EVENT, COLOR_None );
	}
}

// To Do. Allow radio channels
void GridEPG_RenderBlankRow(int rgn, int channel) {
	int x_pos, y_pos;
	if( (channel-GridEPG_Min) < 0 ) return;
	// Channel column
	x_pos = Panel_Left_X;
	y_pos = Panel_Left_Y + (( GRIDEPG_ROWHEIGHT + DISPLAY_GAP ) * (channel-GridEPG_Min));
	if( GridEPG_ItemBase != NULL ){
		TAP_Osd_RestoreBox( rgn, x_pos, y_pos, DISPLAY_ROW_WIDTH, GRIDEPG_ROWHEIGHT, GridEPG_ItemBase );
	} else {
		TAP_Osd_FillBox(rgn, x_pos, y_pos, DISPLAY_ROW_WIDTH, GRIDEPG_ROWHEIGHT, DISPLAY_ITEM);
	}
	TAP_Osd_FillBox(rgn, x_pos+GRIDEPG_CHANNELWIDTH, y_pos, GRIDEPG_GAP, GRIDEPG_ROWHEIGHT, DISPLAY_MAIN);
}

// To Do. Allow radio channels
void GridEPG_RenderRow(int rgn, int channel) {
	int x_pos, y_pos;
	if( (channel-GridEPG_Min) < 0 ) return;
	if( channel > GridEPG_Max-1 ) return;
	y_pos = Panel_Left_Y + (( GRIDEPG_ROWHEIGHT + DISPLAY_GAP ) * (channel-GridEPG_Min));
	x_pos = (DISPLAY_X_OFFSET+GRIDEPG_EVTSTART);
	if( GridEPG_ItemBase != NULL ){
		TAP_Osd_RestoreBox( rgn, Panel_Left_X, y_pos, DISPLAY_ROW_WIDTH, GRIDEPG_ROWHEIGHT, GridEPG_ItemBase );
	} else {
		TAP_Osd_FillBox(rgn, Panel_Left_X, y_pos, DISPLAY_ROW_WIDTH, GRIDEPG_ROWHEIGHT, DISPLAY_ITEM);
	}
	TAP_Osd_FillBox(rgn, Panel_Left_X+GRIDEPG_CHANNELWIDTH, y_pos, GRIDEPG_GAP,	GRIDEPG_ROWHEIGHT, DISPLAY_MAIN);
	GridEPG_RenderChannel(rgn, channel); // Render the channel column
	GridEPG_RenderEvents(rgn, channel); // Render the event for that channel
}

void GridEPG_RenderHelp( int rgn ){
	int y_pos = Panel_Left_Y + (( GRIDEPG_ROWHEIGHT + DISPLAY_GAP )	* (GRIDEPG_CHANNELS))+2;
	int x_pos = Panel_Left_X+DISPLAY_ROW_WIDTH-80;
	TAP_Osd_FillBox(rgn, x_pos, y_pos, 80, 100, DISPLAY_MAIN);
	TAP_Osd_PutGd( rgn, x_pos+DISPLAY_BORDER, y_pos+DISPLAY_BORDER+3, &_redGd, TRUE );	// Draw highlighted
	Font_Osd_PutString1419( rgn, x_pos+DISPLAY_BORDER+20, y_pos+DISPLAY_BORDER, x_pos + 80, "-24 Hr", DISPLAY_EVENT, COLOR_None);
	TAP_Osd_PutGd( rgn, x_pos+DISPLAY_BORDER, y_pos+DISPLAY_BORDER+3+25, &_greenGd, TRUE );	// Draw highlighted
	Font_Osd_PutString1419( rgn, x_pos+DISPLAY_BORDER+20, y_pos+DISPLAY_BORDER+25, x_pos + 80, "< Page", DISPLAY_EVENT, COLOR_None);
	TAP_Osd_PutGd( rgn, x_pos+DISPLAY_BORDER, y_pos+DISPLAY_BORDER+3+50, &_yellowGd, TRUE );	// Draw highlighted
	Font_Osd_PutString1419( rgn, x_pos+DISPLAY_BORDER+20, y_pos+DISPLAY_BORDER+50, x_pos + 80, "> Page", DISPLAY_EVENT, COLOR_None);
	TAP_Osd_PutGd( rgn, x_pos+DISPLAY_BORDER, y_pos+DISPLAY_BORDER+3+75, &_blueGd, TRUE );	// Draw highlighted
	Font_Osd_PutString1419( rgn, x_pos+DISPLAY_BORDER+20, y_pos+DISPLAY_BORDER+75, x_pos + 80, "+24 hr", DISPLAY_EVENT, COLOR_None);
}

void GridEPG_Render(void) {
	int i;
	bool loadingNeeded = FALSE;
	Display_CreateRegions();
	if ( !Display_BackgroundDrawn ) {
		Screen_Set(Display_RGN);
		GridEPG_Active = TRUE;
		loadingNeeded = FALSE;
		// No template so we need to redraw the display from scratch
		Display_DrawBackground();
		// Render the template as the loading will take some time
		GridEPG_RenderTopPanel(Display_RGN);
		GridEPG_RenderHelp(Display_RGN);
		Display_DrawTime();
		// Draw the template on the display by fading in
		for (i = 0; i < GRIDEPG_CHANNELS; i++) {
			GridEPG_RenderBlankRow(Display_RGN, i );
		}
		if (dataCollected != DATA_COLLECTED ) {
			Screen_FadeIn(Display_RGN, Setting_FadeIn);
			loadingNeeded = TRUE;
			Events_Get(); // Populate the static events array
			Events_SortByChannel();
		}
		TAP_Osd_Copy(Display_RGN, Display_MemRGN, 0, 0, 720, 576, 0, 0,	FALSE);
		for (i = GridEPG_Min; i < GridEPG_Max; i++) {
			GridEPG_RenderRow(Display_MemRGN, i );
		}
		GridEPG_RenderTimers(Display_MemRGN );
		GridEPG_RenderBottomPanel(Display_MemRGN, GridEPG_GetEventPointer());
		TAP_Osd_Copy(Display_MemRGN, Display_RGN, 0, 0, 720, 576, 0, 0,	FALSE);
		if ( !loadingNeeded ) {
			Screen_FadeIn(Display_RGN, Setting_FadeIn);
		}
	} else {
		if( GridEPG_Redraw ){
			//				TAP_Osd_Copy( Display_RGN, Display_MemRGN, 0, 0, 720, 576, 0, 0, FALSE);
			GridEPG_RenderTopPanel(Display_MemRGN);
			for (i = GridEPG_Min; i < GridEPG_Max; i++) {
				GridEPG_RenderRow(Display_MemRGN, i );
			}
			GridEPG_RenderTimers(Display_MemRGN );
			GridEPG_RenderBottomPanel(Display_MemRGN,GridEPG_GetEventPointer());
			TAP_Osd_Copy(Display_MemRGN, Display_RGN, 0, 0, 720, 576, 0, 0,	FALSE);
			GridEPG_Redraw = FALSE;
		} else {
			if (GridEPG_SelectedRow != GridEPG_PrevSelectedRow ) {
				// Previous Selected Row
				TAP_Osd_Copy(Display_RGN, Display_MemRGN, Panel_Left_X,Panel_Left_Y + (( GRIDEPG_ROWHEIGHT + DISPLAY_GAP )* (GridEPG_PrevSelectedRow-GridEPG_Min)), 720-((Panel_Left_X-DISPLAY_X_OFFSET)),GRIDEPG_ROWHEIGHT, Panel_Left_X, Panel_Left_Y+ (( GRIDEPG_ROWHEIGHT + DISPLAY_GAP )* (GridEPG_PrevSelectedRow-GridEPG_Min)),FALSE);
				GridEPG_RenderRow(Display_MemRGN, GridEPG_PrevSelectedRow );
//				GridEPG_RenderTimers(Display_MemRGN );
				// Selected Row
				TAP_Osd_Copy(Display_RGN, Display_MemRGN, Panel_Left_X,Panel_Left_Y+ (( GRIDEPG_ROWHEIGHT + DISPLAY_GAP )* (GridEPG_SelectedRow-GridEPG_Min)),720-((Panel_Left_X-DISPLAY_X_OFFSET)),GRIDEPG_ROWHEIGHT, Panel_Left_X, Panel_Left_Y+ (( GRIDEPG_ROWHEIGHT + DISPLAY_GAP )* (GridEPG_SelectedRow-GridEPG_Min)),FALSE);
				GridEPG_RenderRow(Display_MemRGN, GridEPG_SelectedRow );
				GridEPG_RenderTimers(Display_MemRGN );
				GridEPG_RenderBottomPanel(Display_MemRGN,GridEPG_GetEventPointer());
				TAP_Osd_Copy(Display_MemRGN, Display_RGN, Panel_Left_X,Panel_Left_Y + (( GRIDEPG_ROWHEIGHT + DISPLAY_GAP )* (GridEPG_PrevSelectedRow-GridEPG_Min)), 720-((Panel_Left_X-DISPLAY_X_OFFSET)),GRIDEPG_ROWHEIGHT, Panel_Left_X, Panel_Left_Y+ (( GRIDEPG_ROWHEIGHT + DISPLAY_GAP )* (GridEPG_PrevSelectedRow-GridEPG_Min)),FALSE);
				TAP_Osd_Copy(Display_MemRGN, Display_RGN, Panel_Left_X,Panel_Left_Y+ (( GRIDEPG_ROWHEIGHT + DISPLAY_GAP )* (GridEPG_SelectedRow-GridEPG_Min)),720-((Panel_Left_X-DISPLAY_X_OFFSET)),GRIDEPG_ROWHEIGHT, Panel_Left_X, Panel_Left_Y+ (( GRIDEPG_ROWHEIGHT + DISPLAY_GAP )* (GridEPG_SelectedRow-GridEPG_Min)),FALSE);
//				TAP_Osd_Copy(Display_MemRGN, Display_RGN, Panel_Left_X, Panel_Left_Y + (( GRIDEPG_ROWHEIGHT + DISPLAY_GAP )	* (GRIDEPG_CHANNELS)), 720-((Panel_Left_X-DISPLAY_X_OFFSET)),(( GRIDEPG_ROWHEIGHT-DISPLAY_GAP ) * (5)),Panel_Left_X, Panel_Left_Y+ (( GRIDEPG_ROWHEIGHT + DISPLAY_GAP )* (GRIDEPG_CHANNELS)), FALSE);
				TAP_Osd_Copy(Display_MemRGN, Display_RGN, Panel_Left_X, Panel_Left_Y + (( GRIDEPG_ROWHEIGHT + DISPLAY_GAP )	* (GRIDEPG_CHANNELS)), 720-((Panel_Left_X-DISPLAY_X_OFFSET)),(( 28-DISPLAY_GAP ) * (5)),Panel_Left_X, Panel_Left_Y+ (( GRIDEPG_ROWHEIGHT + DISPLAY_GAP )* (GRIDEPG_CHANNELS)), FALSE);
			} else {
				if (GridEPG_SelectedCol != GridEPG_PrevSelectedCol ) {
					// Change in the column so re render the slected row
					// Selected Row
					TAP_Osd_Copy(Display_RGN,Display_MemRGN,Panel_Left_X,Panel_Left_Y+ (( GRIDEPG_ROWHEIGHT + DISPLAY_GAP )* (GridEPG_SelectedRow-GridEPG_Min)),720-((Panel_Left_X-DISPLAY_X_OFFSET)),GRIDEPG_ROWHEIGHT,Panel_Left_X,Panel_Left_Y+ (( GRIDEPG_ROWHEIGHT + DISPLAY_GAP )* (GridEPG_SelectedRow-GridEPG_Min)),FALSE);
					GridEPG_RenderRow(Display_MemRGN, GridEPG_SelectedRow );
					GridEPG_RenderTimers(Display_MemRGN );
					GridEPG_RenderBottomPanel(Display_MemRGN,GridEPG_GetEventPointer());
					TAP_Osd_Copy(Display_MemRGN,Display_RGN,Panel_Left_X,Panel_Left_Y+ (( GRIDEPG_ROWHEIGHT + DISPLAY_GAP )* (GridEPG_SelectedRow-GridEPG_Min)),720-((Panel_Left_X-DISPLAY_X_OFFSET)),GRIDEPG_ROWHEIGHT,Panel_Left_X,Panel_Left_Y+ (( GRIDEPG_ROWHEIGHT + DISPLAY_GAP )* (GridEPG_SelectedRow-GridEPG_Min)),FALSE);
//					TAP_Osd_Copy(Display_MemRGN, Display_RGN, Panel_Left_X, Panel_Left_Y + (( GRIDEPG_ROWHEIGHT + DISPLAY_GAP )	* (GRIDEPG_CHANNELS)), 720-((Panel_Left_X-DISPLAY_X_OFFSET)),(( GRIDEPG_ROWHEIGHT-DISPLAY_GAP ) * (5)),Panel_Left_X, Panel_Left_Y+ (( GRIDEPG_ROWHEIGHT + DISPLAY_GAP )* (GRIDEPG_CHANNELS)), FALSE);
					TAP_Osd_Copy(Display_MemRGN, Display_RGN, Panel_Left_X, Panel_Left_Y + (( GRIDEPG_ROWHEIGHT + DISPLAY_GAP )	* (GRIDEPG_CHANNELS)), 720-((Panel_Left_X-DISPLAY_X_OFFSET)),(( 28-DISPLAY_GAP ) * (5)),Panel_Left_X, Panel_Left_Y+ (( GRIDEPG_ROWHEIGHT + DISPLAY_GAP )* (GRIDEPG_CHANNELS)), FALSE);
				}
			}
		}
	}
	GridEPG_LastRendered = TAP_GetTick();
	GridEPG_Active = TRUE;
}

void GridEPG_ReturnStart(void) {
	word mjd;
	byte hour, min, sec;
	TAP_GetTime( &mjd, &hour, &min, &sec );
	GridEPG_StartTime = Now(0);
	GridEPG_StartTime = AddTime(GridEPG_StartTime, -(min%60)); // Nearest half hour block
	GridEPG_SelectColTime = GridEPG_StartTime;
}

// Function used to switch between radio and tv
void GridEPG_SwitchMode(void) {
	word mjd;
	byte hour, min, sec;
	int currentChan;
	bool found = FALSE;
	int i;
	// Switch the mode
	if( GridEPG_Mode == SVC_TYPE_Tv ){
		GridEPG_Mode = SVC_TYPE_Radio;
	} else {
		GridEPG_Mode = SVC_TYPE_Tv;
	}
	GridEPG_ChannelsBuilt = FALSE;
	GridEPG_BuildChannels();
	// Drwa the base template on the back
	for (i = 0; i < GRIDEPG_CHANNELS; i++) {
		GridEPG_RenderBlankRow(Display_MemRGN, i );
	}
	GridEPG_RenderEmptyBottomPanel(Display_MemRGN);
	TAP_Osd_Copy(Display_MemRGN, Display_RGN, 0, 0, 720, 576, 0, 0,	FALSE);
	// Initalise for testing
	GRIDEPG_EVTSTART = Panel_Left_X + GRIDEPG_CHANNELWIDTH+12;
	// Set the grid start time
	TAP_GetTime( &mjd, &hour, &min, &sec );
	GridEPG_StartTime = Now(0);
	GridEPG_StartTime = AddTime(GridEPG_StartTime, -(min%60)); // Nearest hour block
	GridEPG_SelectColTime = GridEPG_StartTime;
	GridEPG_Min = 0;
	GridEPG_Max = GRIDEPG_CHANNELS;
	// Set the selected row to the current channel
	if (TAP_Channel_GetCurrent( &GridEPG_Type, &currentChan ) ) {
		found = FALSE;
		for( i = 0; i < GridEPG_Channels_ptr; i++ ){
			if( GridEPG_Channels[i] == currentChan ){	// Found the channel we are currently on now set it as the selected one
				GridEPG_Min = 0;
				GridEPG_Max = GRIDEPG_CHANNELS;
				GridEPG_SelectedRow = i;
				GridEPG_PrevSelectedRow = i;
				if( GridEPG_SelectedRow > GridEPG_Max-1 ){
					GridEPG_Max = GridEPG_SelectedRow+1;
					GridEPG_Min = GridEPG_Max - GRIDEPG_CHANNELS;
				}
				if (GridEPG_Max > GridEPG_Channels_ptr ) {
					GridEPG_Max = GridEPG_Channels_ptr;
					GridEPG_Min = GridEPG_Max - GRIDEPG_CHANNELS;
				}
				if( GridEPG_Min < 0 ){
					GridEPG_Min = 0;
				}
				found = TRUE;
				break;	// Break out of the loop
			}
		}
		if( !found ){	// If we didn't find the channel we are on then uit must be excluded so defulat to first item
			GridEPG_Min = 0;
			GridEPG_Max = GRIDEPG_CHANNELS;
			if( GridEPG_Channels_ptr < 1 ){	// No channels so remove the highlight
				GridEPG_SelectedCol = 0;
				GridEPG_PrevSelectedCol = 0;
				GridEPG_SelectedRow = -1;
				GridEPG_PrevSelectedRow = -1;
			} else {
				GridEPG_SelectedCol = 0;
				GridEPG_PrevSelectedCol = 0;
				GridEPG_SelectedRow = 0;
				GridEPG_PrevSelectedRow = 0;
			}
			if (GridEPG_Max > GridEPG_Channels_ptr ) {
				GridEPG_Max = GridEPG_Channels_ptr;
			}
		}
	} else {
		// Failed getting the channel we are on so default to first channel
		GridEPG_Min = 0;
		GridEPG_Max = GRIDEPG_CHANNELS;
		if( GridEPG_Channels_ptr < 1 ){	// No channels so remove the highlight
			GridEPG_SelectedCol = 0;
			GridEPG_PrevSelectedCol = 0;
			GridEPG_SelectedRow = -1;
			GridEPG_PrevSelectedRow = -1;
		} else {
			GridEPG_SelectedCol = 0;
			GridEPG_PrevSelectedCol = 0;
			GridEPG_SelectedRow = 0;
			GridEPG_PrevSelectedRow = 0;
		}
		if (GridEPG_Max > GridEPG_Channels_ptr ) {
			GridEPG_Max = GridEPG_Channels_ptr;
		}
	}
	GridEPG_Redraw = TRUE;
	GridEPG_Render();
}

void GridEPG_Activate(void) {
	word mjd;
	byte hour, min, sec;
	int currentChan;
	bool found = FALSE;
	int i;
	char text[1024];
	
	memset( text, 0, sizeof(text) );
	
	TAP_Channel_GetTotalNum( &GridEPG_TvChannels, &GridEPG_RdChannels );
	
	if( Events_Loading ){
		ShowMessageWin(_PROGRAM_NAME_, "Currently loading events", "Please try again later", 200);
		return;	// If we are loading events then disable loading until complete
	}
	// Setup start and end points for the render
	Display_Init();
	// Default for now. Will have it so it is the last state but need this for testing	
	
	// Set the number of channels and set the size of the panels
	if( Settings_GridEPGChannels == 10 ){
		GRIDEPG_CHANNELS = 10;
		GRIDEPG_ROWHEIGHT = 28;
		if( Settings_GridEPGLogos ){
			GRIDEPG_EVTWIDTH = 533;
			GRIDEPG_CHANNELWIDTH = 47;
		} else {
			GRIDEPG_EVTWIDTH = 450;
			GRIDEPG_CHANNELWIDTH = 130;
		}
	} else {
		GRIDEPG_CHANNELS = GridEPG_AltNum;
		GRIDEPG_ROWHEIGHT = GridEPG_AltHeight;
		if( Settings_GridEPGLogos ){
			GRIDEPG_EVTWIDTH = 520;
			GRIDEPG_CHANNELWIDTH = 60;
		} else {
			GRIDEPG_EVTWIDTH = 450;
			GRIDEPG_CHANNELWIDTH = 130;
		}
	}
	// Initalise for testing
	GRIDEPG_EVTSTART = Panel_Left_X + GRIDEPG_CHANNELWIDTH+12;
	
	// Set the grid start time
	TAP_GetTime( &mjd, &hour, &min, &sec );
	GridEPG_StartTime = Now(0);
	GridEPG_StartTime = AddTime(GridEPG_StartTime, -(min%60)); // Nearest hour block
	GridEPG_SelectColTime = GridEPG_StartTime;
	
	// Set the selected row to the current channel
	if (TAP_Channel_GetCurrent( &GridEPG_Mode, &currentChan ) ) {
		found = FALSE;
		GridEPG_ChannelsBuilt = FALSE;	
		GridEPG_BuildChannels();
		for( i = 0; i < GridEPG_Channels_ptr; i++ ){
			if( GridEPG_Channels[i] == currentChan ){	// Found the channel we are currently on now set it as the selected one
				GridEPG_Min = 0;
				GridEPG_Max = GRIDEPG_CHANNELS;
				GridEPG_SelectedRow = i;
				GridEPG_PrevSelectedRow = i;
				if( GridEPG_SelectedRow > GridEPG_Max-1 ){
					GridEPG_Max = GridEPG_SelectedRow+1;
					GridEPG_Min = GridEPG_Max - GRIDEPG_CHANNELS;
				}
				if (GridEPG_Max > GridEPG_Channels_ptr ) {
					GridEPG_Max = GridEPG_Channels_ptr;
					GridEPG_Min = GridEPG_Max - GRIDEPG_CHANNELS;
				}
				if( GridEPG_Min < 0 ){
					GridEPG_Min = 0;
				}
				found = TRUE;
				break;	// Break out of the loop
			}
		}
		if( !found ){	// If we didn't find the channel we are on then uit must be excluded so defulat to first item
			GridEPG_Min = 0;
			GridEPG_Max = GRIDEPG_CHANNELS;
			if (GridEPG_Max > GridEPG_Channels_ptr ) {
				GridEPG_Max = GridEPG_Channels_ptr;
			}
		}
	} else {
		GridEPG_ChannelsBuilt = FALSE;	
		GridEPG_BuildChannels();
		// Failed getting the channel we are on so default to first channel
		GridEPG_Min = 0;
		GridEPG_Max = GRIDEPG_CHANNELS;
		if (GridEPG_Max > GridEPG_Channels_ptr ) {
			GridEPG_Max = GridEPG_Channels_ptr;
		}
	}
	GridEPG_CreateGradients();
	GridEPG_Render();
}

void GridEPG_RemoveRecEventCallback(bool yes) {
	int tuner = -1;
	TAP_Osd_Copy(Display_RGN, Display_MemRGN, 0, 0, 720, 576, 0, 0,	FALSE);
	if( yes ){
		if( Events_GetEventEndTime(GridEPG_GetEventPointer()) >= Now(0) ){	// Only allow when event is not currently active
			if (HDD_isRecording(0) ) { // If Tuner 1 is recording check whether we were the one that just started it
				if (Update_RecordingInfo(0) ) {
					if (Recording_GetRecInfo(0).svcNum == GetSvcNum(Events_GetEventSvcId(GridEPG_GetEventPointer())) ) { // Same service as repaired file so we must have started it
						tuner = 0;
					}
				}
			}
			if (HDD_isRecording(1) ) { // If Tuner 1 is recording check whether we were the one that just started it
				if (Update_RecordingInfo(1) ) {
					if (Recording_GetRecInfo(1).svcNum == GetSvcNum(Events_GetEventSvcId(GridEPG_GetEventPointer())) ) { // Same service as repaired file so we must have started it
						tuner = 1;
					}
				}
			}
			if (tuner > -1&& tuner < 2) {
				// We have established the tuner which is being used. Now lets check that the 
				if ( !HDD_RECSlotSetDuration(tuner,(word)TimeDiff( Recording_GetRecInfo(tuner).startTime, AddTime(Events_GetEventStartTime(GridEPG_GetEventPointer()), Settings_EndBuffer)))) { // Adjust the duration
					Message_Display("Failed to adjust duration");
					TAP_Delay(Setting_MessageTimeout);
					GridEPG_Redraw = TRUE;
					GridEPG_Render();
				} else {
					GridEPG_Redraw = TRUE;
					GridEPG_Render();
				}
			}
		}
	}
}

void GridEPG_DeleteTimerCallback(int option){
	int timerNo = -1;
	TYPE_TimerInfo timer;
	TYPE_TimerInfo temptimer;
	TYPE_TimerInfo temptimer2;
	dword endTime = 0;
	int retval = 0;
	TYPE_TapEvent event;
	TAP_Osd_Copy(Display_RGN, Display_MemRGN, 0, 0, 720, 576, 0, 0, FALSE); // Copy the main region to memory to avoid corruption from the previous OSD
	if( option == THREEOPTION_OPTION1 ){
		if( TAP_Timer_Delete(GridEPG_GetTimer(GridEPG_GetEventPointer())) ){
//			Message_Display("Timer successfully removed");
		}
	} else {
		if( option == THREEOPTION_OPTION2 ){
			// User has selected to remove timer so proceed
			if ( (timerNo = GridEPG_GetTimer(GridEPG_GetEventPointer())) > -1) {
				// So we have the timer number
				// Now we need to check whether its duration is that same as the event
				event = Events_GetEvent(GridEPG_GetEventPointer());
				// Compare with respect to the buffer
				if (TAP_Timer_GetInfo(timerNo, &timer) ) { // If we get the timer info correctly
					//  [    Event     ]
					//    [  Timer   ]
					if (timer.startTime >= AddTime(event.startTime,-Settings_StartBuffer) && 
					    AddTime(timer.startTime,timer.duration) <= AddTime(event.endTime,Settings_EndBuffer)) {
						// Event with buffers is the same time or longer than the timer so we can delete the timer
						if (TAP_Timer_Delete(timerNo) ) {
							GridEPG_Redraw = TRUE;
							GridEPG_Render();
							return;
						}
					} else {
						// Timer extends more than this event so we will need to split
						// Lets store the timer
						//  [ Event ]
						//  [    Timer     ]
						if (timer.startTime >= AddTime(event.startTime,-Settings_StartBuffer) &&
						    AddTime(event.endTime,Settings_EndBuffer) <= AddTime(timer.startTime,timer.duration)){
							endTime = AddTime(timer.startTime, timer.duration); // Store the ending time of the time
							timer.startTime = AddTime(event.endTime,-Settings_StartBuffer); // Set the new start timer including the buffers
							timer.duration = TimeDiff(timer.startTime, endTime);
							StripFileName(timer.fileName); // Remove the .rec extension
							// Attempt to modify the timer and return to the user the result via message
							if (Settings_RenameTimers ) { // If user has enable renaming then search for all events that this new timer covers and build new name
								strcpy(timer.fileName, Timers_BuildTimerName(timer)); // Copy the generated name to the fileName
							}
							strcat(timer.fileName, ".rec"); // Append .rec to the end of the string
							retval = TAP_Timer_Modify(timerNo, &timer );
							if (retval == 0) {
	//							Message_Display("Timer successfully modified");
							} else {
								Message_Display("Timer failed to be modified");
							}
						} else {
							//         [ Event ]
							//  [    Timer     ]
							if (timer.startTime < AddTime(event.startTime,-Settings_StartBuffer) &&
								AddTime(timer.startTime, timer.duration) <= AddTime(event.endTime, Settings_EndBuffer)) {
								// We need to adjust the timer duration by subtracting the duration of the event. This will preserve the buffer
								// and because this is at the end thats the only thing needed to be adjusted
								timer.duration -= TimeDiff(event.startTime,event.endTime);
								StripFileName(timer.fileName); // Remove the .rec extension
								// Attempt to modify the timer and return to the user the result via message
								if (Settings_RenameTimers ) { // If user has enable renaming then search for all events that this new timer covers and build new name
									strcpy(timer.fileName,Timers_BuildTimerName(timer)); // Copy the generated name to the fileName
								}
								strcat(timer.fileName, ".rec"); // Append .rec to the end of the string
								retval = TAP_Timer_Modify(timerNo, &timer );
								if (retval == 0) {
	//								Message_Display("Timer successfully modified");
								} else {
									Message_Display("Timer failed to be modified");
								}
							} else {
								//      [ Event ]
								//  [    Timer     ]
								// We need to create 2 new timers and delete the old one
								// Create the tiemr for the starting part
								temptimer = timer;
								// Adjust the duration of the event
								// Use the event start and add the buffer to calculate the ending time of the timer ( duration )
								temptimer.duration = TimeDiff(temptimer.startTime,AddTime(event.startTime,Settings_EndBuffer));
								StripFileName(temptimer.fileName); // Remove the .rec extension
								if (Settings_RenameTimers ) { // If user has enable renaming then search for all events that this new timer covers and build new name
									strcpy(temptimer.fileName,Timers_BuildTimerName(temptimer)); // Copy the generated name to the fileName
								}
								strcat(temptimer.fileName, ".rec"); // Append .rec to the end of the string
								// Create the second timer
								// Need to adjust both the duration and start time
								temptimer2 = timer;
								StripFileName(temptimer2.fileName); // Remove the .rec extension
								endTime = AddTime(timer.startTime, timer.duration); // Store the ending time of the timer
								// Adjust the start time using the event end mins the start buffer
								temptimer2.startTime = AddTime(event.endTime,-Settings_StartBuffer);
								temptimer2.duration = TimeDiff(temptimer2.startTime, endTime);
								if (Settings_RenameTimers ) { // If user has enable renaming then search for all events that this new timer covers and build new name
									strcpy(temptimer2.fileName,Timers_BuildTimerName(temptimer2)); // Copy the generated name to the fileName
								}
								strcat(temptimer2.fileName, ".rec"); // Append .rec to the end of the string
								// Now we have created our 2 new timers lets delete the old one and create these 2 new
								if (TAP_Timer_Delete(timerNo) ) {
									retval = TAP_Timer_Add( &temptimer );
									if (retval == 0) {
										retval = TAP_Timer_Add( &temptimer2 );
										if (retval == 0) {
											Message_Display("Timer for event has been successfully removed.");
										} else {
											// Failed adding new timers so restore the old one
											retval = TAP_Timer_Add( &timer );
											if (retval == 0) {
												Message_Display("Timer for event restored because of failure in creation of new timers");
											} else {
												Message_Display("Failed restoring old timer with event recorded");
											}
										}
									} else {
										// Failed adding new timers so restore the old one
										retval = TAP_Timer_Add( &timer );
										if (retval == 0) {
											Message_Display("Timer for event restored because of failure in creation of new timers");
										} else {
											Message_Display("Failed restoring old timer with event recorded");
										}
									}
								} else {
									// Failed to delete the timer so make no changes
									Message_Display("Timer for event no removed. Adjustment failed. No changes made.");
								}

							}
						}
					}
				} else {
					Message_Display("Failed getting timer details");
				}
			}
		}
	}
	GridEPG_Render();
}

void GridEPG_NewFavourite(void) {
	favitem Item;
	TYPE_TapEvent event;
	event = Events_GetEvent(GridEPG_GetEventPointer());
	// Load the favourite if we haven't done so
//	if ( !FavManage_ReadFav ) { // Haven't read the file yet so we will read and flag as no changes
//		Message_Display("Loading favourite file."); // Explained that the event has past and we can set because of that
//		TAP_Hdd_ChangeDir("/ProgramFiles/");
//		FavManage_LoadFavFile("favourites.ini");
//		FavManage_Changed = FALSE;
//		TAP_Delay(Setting_MessageTimeout);
//		Message_DisplayClear();
//	}
	FavNew_Init(Display_RGN, Display_MemRGN, (720/2)-(350/2)+DISPLAY_X_OFFSET, (576/2)-(400/2)+DISPLAY_Y_OFFSET, 350, 400);
	FavNew_SetColours(DISPLAY_KEYBOARDITEMTEXT, DISPLAY_ITEM,DISPLAY_KEYBOARDITEMSELECTEDTEXT, DISPLAY_KEYBOARDITEMSELECTED,DISPLAY_MAIN );

	// Clear our favourite item
	memset(Item.term, 0x0, sizeof(Item.term));
	memset(Item.channel,0x0,sizeof(Item.channel));
	memset(Item.day,0x0,sizeof(Item.day));
	Item.location = 0;
	Item.start = -1;
	Item.end = -1;
	Item.priority = 0;
	Item.adj = 0;
	Item.rpt = 1;

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
	if( !FavNew_ActivateWithItem(Item, &GridEPG_FavNewReturn) ) {
		Message_Display( "Favourite New Dialog Failed." ); // Explained that the event has past and we can set because of that
	}
}

void GridEPG_FavNewReturn(favitem a, bool cancel) {
	TAP_Osd_Copy(Display_RGN, Display_MemRGN, 0, 0, 720, 576, 0, 0, FALSE); // Copy the main region to memory to avoid corruption from the previous OSD
	if ( !cancel ) { // User didnt cancel
		InsertFavItem( a );
		FavManage_Append(); // Save to file
		FavManage_FreeMemory();
//		FavManage_ReadFav = FALSE;
		Message_Display("Favourite Added"); // Explained that the event has past and we can set because of that
	}
}

// Return from the timer edit component
void GridEPG_TimerEditReturn(int retval, bool cancel) {
	TAP_Osd_Copy(Display_RGN, Display_MemRGN, 0, 0, 720, 576, 0, 0, FALSE); // Copy the main region to memory to avoid corruption from the previous OSD
	if ( !cancel ) {
		if (retval == 0) {
		//		Message_Display("Timer successfully modified.");
		} ef( retval == 1 ) {
			Message_Display( "ERROR: Timer setting failed. No resourse avaliable." );
			TAP_Delay(Setting_MessageTimeout);
		} ef( retval == 2 ) {
			Message_Display( "ERROR: Timer setting failed. Invalid tuner." );
			TAP_Delay(Setting_MessageTimeout);
		}ef (retval	== 0xffffffff) {
			Message_Display( "ERROR: Timer setting failed. Error modifying." );
			TAP_Delay(Setting_MessageTimeout);
		}ef (retval	>= 0xFFFE0000) {
			Message_Display( "ERROR: Timer setting failed. Conflicts with other timers." );
			TAP_Delay(Setting_MessageTimeout);
		} else {
			Message_Display( "ERROR: Timer setting failed. Unknown cause." );
			TAP_Delay(Setting_MessageTimeout);
		}
		GridEPG_Redraw = TRUE;
		GridEPG_Render();
	}
}

void GridEPG_AppendCallback(bool yes) {
	TYPE_TapEvent event;
	TYPE_TimerInfo currentTimer;
	TYPE_TimerInfo appendTimer;
	dword endTime = 0;
	int appendNum = 0;
	int retval = 0;
	int totalTimers = 0;
	TAP_Osd_Copy(Display_RGN, Display_MemRGN, 0, 0, 720, 576, 0, 0,	FALSE); // Copy the main region to memory to avoid corruption from the previous OSD
	if (yes ) {
		if (GridEPG_AppendTimerNum > -1) {
			event = Events_GetEvent(GridEPG_GetEventPointer());
			TAP_Timer_GetInfo(GridEPG_AppendTimerNum, &currentTimer);
			GridEPG_RestoreTimer = currentTimer; // Store the previous state of the timer in global var to enable restoration if required
			StripFileName(currentTimer.fileName); // Remove the .rec extension
			// Lets adjust the duration of the timer
			if (currentTimer.startTime < event.startTime) {
				// We will set the duration to be
				// duration = ( event.endTime + Buffer ) - currentTimer.startTime
				currentTimer.duration = TimeDiff(currentTimer.startTime, AddTime(event.endTime,Settings_EndBuffer));
			} else {
				// duration = currentTimer.endTime - ( event.startTime - startBuffer )
				// Buffer assumed in the the timer
				currentTimer.duration = TimeDiff(AddTime(event.startTime, -Settings_StartBuffer),AddTime(currentTimer.startTime,currentTimer.duration));
				// Adjust the start time to be the start time minus the buffer on the event			
				currentTimer.startTime = AddTime(event.startTime,-Settings_StartBuffer);
				// We wont rename atm TO DO
			}
			if (Settings_RenameTimers ) { // If user has enable renaming then search for all events that this new timer covers and build new name
				strcpy(currentTimer.fileName,Timers_BuildTimerName(currentTimer)); // Copy the generated name to the fileName
			}
			strcat(currentTimer.fileName, ".rec"); // Append .rec to the end of the string
			retval = TAP_Timer_Modify(GridEPG_AppendTimerNum,&currentTimer);
			if (retval == 0) {
//				Message_Display("Timer successfully modified.");
			} else {
				// Look to see whether we have a neighbouring timer if so adjust start and duration to include it and remove that timer
				// Make sure we are not pointing at the same timer using withNum function
				if ( (appendNum = Timers_GetNeighbourWithNum(GridEPG_AppendTimerNum, currentTimer )) > -1) { // We have a neighbour so we need to adjust
					if (TAP_Timer_GetInfo(appendNum, &appendTimer ) ) {
						//   [ Append ]
						//          [ Current Timer ]
						if (appendTimer.startTime <= currentTimer.startTime && 
						    AddTime(appendTimer.startTime,appendTimer.duration) >= currentTimer.startTime &&
						    AddTime(appendTimer.startTime,appendTimer.duration) < AddTime(currentTimer.startTime,currentTimer.duration)) {
							// Need to adjust the start and duration as appending timer fails before us
							endTime = AddTime(currentTimer.startTime,currentTimer.duration);
							currentTimer.startTime = appendTimer.startTime;
							currentTimer.duration = TimeDiff(appendTimer.startTime, endTime);
							StripFileName(currentTimer.fileName); // Remove the .rec extension
							// Attempt to re-set the timer. First remove the conflicting timer aka appendTimer
							if (TAP_Timer_Delete(appendNum) ) {
								// The timer we are going to delete is less than our ajudtment timer so minus 1 to resync
								if (appendNum < GridEPG_AppendTimerNum )
									GridEPG_AppendTimerNum--;
								if (Settings_RenameTimers ) { // If user has enable renaming then search for all events that this new timer covers and build new name
									strcpy(currentTimer.fileName,Timers_BuildTimerName(currentTimer)); // Copy the generated name to the fileName
								}
								strcat(currentTimer.fileName, ".rec"); // Append .rec to the end of the string
								retval = TAP_Timer_Modify(GridEPG_AppendTimerNum,&currentTimer);
								if (retval == 0) {
//									Message_Display("Timer successfully set for event.");
								} else {
									// Failed setting new timer so restore old on and inform the user
									retval = TAP_Timer_Add( &appendTimer );
									if (retval == 0) {
										Message_Display("Failed adjusting surrounding timers. No adjustments made.");
									} else {
										Message_Display("Failed adjusting surrounding timers. Surrounding tiemrs have been changed/removed.");
									}
									TAP_Delay(Setting_MessageTimeout);
								}
							} else {
								// Don't know what to do so just fail
								// unknown case of failure so print message using failure values
								if (retval == 1) {
									Message_Display("ERROR: Timer setting failed. No resourse avaliable.");
								}ef( retval == 2 ) {
									Message_Display( "ERROR: Timer setting failed. Invalid tuner." );
								}ef (retval == 0xffffffff) {
									Message_Display( "ERROR: Timer setting failed. Error modifying." );
								}ef (retval	>= 0xFFFE0000) {
									Message_Display( "ERROR: Timer setting failed. Conflicts with other timers. Failed deleting old timer" );
								} else {
									Message_Display( "ERROR: Timer setting failed. Unknown cause." );
								}
								TAP_Delay(Setting_MessageTimeout);
							}
						} else {
							//            [ Append ]
							//  [ Current Timer ]
							if (currentTimer.startTime <= appendTimer.startTime &&
								AddTime(currentTimer.startTime,currentTimer.duration) >= appendTimer.startTime &&
								AddTime(currentTimer.startTime,currentTimer.duration) <= AddTime(appendTimer.startTime,appendTimer.duration)) {
								// Only need to adjust the duration of the show to be the end time of the appending timer
								currentTimer.duration = TimeDiff(currentTimer.startTime,AddTime(appendTimer.startTime,appendTimer.duration));
								StripFileName(currentTimer.fileName); // Remove the .rec extension
								// Attempt to reset the timer. First remove the conflicting timer
								if (TAP_Timer_Delete(appendNum) ) {
									// The timer we deleted is less than our adjustment timer so minus 1 to resync
									if (appendNum < GridEPG_AppendTimerNum )
										GridEPG_AppendTimerNum--;
									if (Settings_RenameTimers ) { // If user has enable renaming then search for all events that this new timer covers and build new name
										strcpy(currentTimer.fileName,Timers_BuildTimerName(currentTimer)); // Copy the generated name to the fileName
									}
									strcat(currentTimer.fileName,".rec"); // Append .rec to the end of the string
									retval = TAP_Timer_Modify(GridEPG_AppendTimerNum,&currentTimer);
									if (retval == 0) {
//										Message_Display("Timer successfully set for event.");
									} else {
										// Failed setting new timer so restore old on and inform the user
										retval = TAP_Timer_Add( &appendTimer );
										if (retval == 0) {
											Message_Display("Failed adjusting surrounding timers. No adjustments made.");
										} else {
											Message_Display("Failed adjusting surrounding timers. Surrounding tiemrs have been changed/removed.");
										}
										TAP_Delay(Setting_MessageTimeout);
									}
								} else {
									// Don't know what to do so just fail
									// unknown case of failure so print message using failure values
									if (retval == 1) {
										Message_Display("ERROR: Timer setting failed. No resourse avaliable.");
									}ef( retval == 2 ) {
										Message_Display( "ERROR: Timer setting failed. Invalid tuner." );
									}ef (retval	== 0xffffffff) {
										Message_Display( "ERROR: Timer setting failed. Error modifying." );
									}ef (retval	>= 0xFFFE0000) {
										Message_Display( "ERROR: Timer setting failed. Conflicts with other timers. Failed deleting old timer" );
									} else {
										Message_Display( "ERROR: Timer setting failed. Unknown cause." );
									}
									TAP_Delay(Setting_MessageTimeout);
								}
							} else {
								// Check for conflicts on other channels given the checks for the conflicting on same channel have failed.
								// Launch the conflict handling mnodule fo the tap
								TimerConflict_Init( Display_RGN, Display_MemRGN,Panel_Left_X, Panel_Left_Y, DISPLAY_ROW_WIDTH, (Panel_Left_Y + (( GRIDEPG_ROWHEIGHT + DISPLAY_GAP ) * (GRIDEPG_CHANNELS+3)))-Panel_Left_Y-DISPLAY_GAP+GRIDEPG_ROWHEIGHT );
								TimerConflict_Activate( currentTimer, &GridEPG_TimerAppendConflictCallback );// ){
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
									GridEPG_Redraw = TRUE;
									GridEPG_Render();
								} else {
									// Conflict screen has launched so delete this timer. If user cancels callback will restore original timer
									if (TAP_Timer_Delete(GridEPG_AppendTimerNum) ) {	// Remove this appending timer from the unit
									}
									return;
								}
							}
						}
					} else {
						// unknown case of failure so print message using failure values
						Message_Display("Failed to get the information for appending timer. No changes have been made");
						TAP_Delay(Setting_MessageTimeout);
					}
				} else {
					TimerConflict_Init( Display_RGN, Display_MemRGN, Panel_Left_X, Panel_Left_Y, DISPLAY_ROW_WIDTH, (Panel_Left_Y + (( GRIDEPG_ROWHEIGHT + DISPLAY_GAP ) * (GRIDEPG_CHANNELS+3)))-Panel_Left_Y-DISPLAY_GAP+GRIDEPG_ROWHEIGHT );
					TimerConflict_Activate( currentTimer, &GridEPG_TimerAppendConflictCallback );// ){
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
								Message_Display( "ERROR: Timer setting failed. Conflicts with other timers. Failed to find neighbour" );
							} else {
								Message_Display( "ERROR: Timer setting failed. Unknown cause." );
							}
						}
						TAP_Delay(Setting_MessageTimeout);
						GridEPG_Redraw = TRUE;
						GridEPG_Render();
					} else {
						// Conflict screen has launched so delete this timer. If user cancels callback will restore original timer
						if (TAP_Timer_Delete(GridEPG_AppendTimerNum) ) {	// Remove this appending timer from the unit
						}
						return;
					}
				}
			}
		}
	}
	GridEPG_Redraw = TRUE;
	GridEPG_Render();
}

void GridEPG_CreateTimer(int row) {
	TYPE_TimerInfo timer;
	int retval;
	TYPE_TapEvent event;
	char str[128];
	char timerstring[128];
	int totalTimers = 0;
	memset(str, '\0', sizeof(str) );
	memset( timerstring, '\0', sizeof(timerstring) );
	event = Events_GetEvent(row);
	// Now render any current recordings
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
	timer.svcType = Events_GetEventSvcType(row);
	timer.nameFix = 1;
	timer.reservationType = RESERVE_TYPE_Onetime;
	// Stuff used from the event
	timer.startTime = AddTime(event.startTime,-Settings_StartBuffer);
	timer.duration = TimeDiff(event.startTime,event.endTime)+Settings_StartBuffer+Settings_EndBuffer;
	timer.svcNum = GetSvcNum(Events_GetEventSvcId(row));
	strncpy(timerstring, event.eventName, 128); // Copy the event name and ensure no overflow
	if( Settings_AppendEpisode ) {
		// If we are small enough then attempt to append the episode name
		if( strlen(timerstring) < 40 ) {
			strncpy(str, event.description,128); // Set the max copy size
			if( GetEpisodeName( str, (int)"[" ) ) { // TO IMPLEMENT SETTING FOR CHARACTER/S	look for [ SETTINGS WILL CONFIGURE LATER
				strcat(timerstring," - "); // Append seperator
				strncat(timerstring,str,128-strlen(timerstring)-4-3); // Copy the episode title until the maximum size of the array
			}
		}
		strncpy(timer.fileName,timerstring,sizeof(timer.fileName)-4); // copy across to the timer string with repect to its size
	} else {
		// Timers build timer name returns name with .rec extension
		if( Settings_RenameTimers ) { // If user has enable renaming then search for all events that this new timer covers and build new name
			strcpy(timer.fileName,Timers_BuildTimerName(timer)); // Copy the generated name to the fileName
		} else {
			strncpy(timer.fileName,timerstring,sizeof(timer.fileName)-4); // copy across to the timer string with repect to its size
		}
	}
	strcat(timer.fileName, ".rec"); // Append .rec to the end of the string
	retval=TAP_Timer_Add(&timer);
	if( retval == 0 ) {
		GridEPG_Redraw = TRUE;
		GridEPG_Render();
	}ef( retval == 1 ) {
		GridEPG_Redraw = TRUE;
		GridEPG_Render();
	}ef( retval == 2 ) {
		GridEPG_Redraw = TRUE;
		GridEPG_Render();
	} else {
		// Check for neighbouring timer if so then ask whether to join
		if( (GridEPG_AppendTimerNum=Timers_GetNeighbour(timer)) > -1 ) {
			// We have a neighbour so prompt to extend
			YesNo_Init( Display_RGN, Display_MemRGN,  (720/2)-(250/2)+DISPLAY_X_OFFSET, (576/2)-(150/2)+DISPLAY_Y_OFFSET, 250, 150 );
			YesNo_SetColours( DISPLAY_KEYBOARDITEMTEXT, DISPLAY_ITEM, DISPLAY_KEYBOARDITEMSELECTEDTEXT, DISPLAY_KEYBOARDITEMSELECTED, DISPLAY_MAIN );
			if( !YesNo_Activate("Join to surrounding timers?",&GridEPG_AppendCallback) ) {
				Message_Display( "YesNo box failed." ); // Explained that the event has past and we can set because of that
			}
		} else {
			TimerConflict_Init( Display_RGN, Display_MemRGN, Panel_Left_X, Panel_Left_Y, DISPLAY_ROW_WIDTH, (Panel_Left_Y + (( 28 + DISPLAY_GAP ) * (10+3)))-Panel_Left_Y-DISPLAY_GAP+28 );
			TimerConflict_Activate( timer, &GridEPG_TimerConflictCallback );// ){
			if( !TimerConflict_IsActive() ) { // We didn't activate so check for append
				totalTimers = TAP_Timer_GetTotalNum();
				if( totalTimers > 69 ) {
					Message_Display( "Maximum number of timers are set." );
				} else {
					Message_Display( "ERROR: Unable to resolve issues" );
				}
				TAP_Delay(Setting_MessageTimeout);
				GridEPG_Redraw = TRUE;
				GridEPG_Render();
			}
		}
	}
}

void GridEPG_TimerAppendConflictCallback(bool result) {
	TAP_Osd_Copy(Display_RGN, Display_MemRGN, 0, 0, 720, 576, 0, 0, FALSE); // Copy the main region to memory to avoid corruption from the previous OSD
	if( !result ){	// User has cancelled so restore the stored timer from append attempt
		// Failed so restore our stored timer from before
		TAP_Timer_Add(&GridEPG_RestoreTimer);
	}
	GridEPG_Redraw = TRUE;
	GridEPG_Render();
}

void GridEPG_TimerConflictCallback(bool result) {
	TAP_Osd_Copy(Display_RGN, Display_MemRGN, 0, 0, 720, 576, 0, 0, FALSE); // Copy the main region to memory to avoid corruption from the previous OSD
	GridEPG_Redraw = TRUE;
	GridEPG_Render();
}

void GridEPG_InstantRecordCallback(bool yes) {
	int tuner = -1;
	TAP_Osd_Copy(Display_RGN, Display_MemRGN, 0, 0, 720, 576, 0, 0, FALSE); // Copy the main region to memory to avoid corruption from the previous OSD
	if (yes ) {
		// Make sure we have a free tuner i.e tuner 1 and 2 not both rewcording
		if ( !(HDD_isRecording(0) || HDD_isRecording(1))) { // See if we currently have a recording if not its easy so change and start recording
			// User has elected to start recording live event
			// Change to the channel and start the recording
			TAP_Channel_Start( 1, Events_GetEventSvcType(GridEPG_GetEventPointer()), GetSvcNum(Events_GetEventSvcId(GridEPG_GetEventPointer())) );
			TAP_Hdd_StartRecord(); // Start the recording on that channel
			// We need to adjust the duration 			
			// Work out which tuner we started recording on
			if (HDD_isRecording(0) ) { // If Tuner 1 is recording check whether we were the one that just started it
				if (Update_RecordingInfo(0) ) {
					if (Recording_GetRecInfo(0).svcNum == GetSvcNum(Events_GetEventSvcId(GridEPG_GetEventPointer())) &&
						Recording_GetRecInfo(0).svcType == GridEPG_Mode) { // Same service as repaired file so we must have started it
						tuner = 0;
					}
				}
			}
			if (HDD_isRecording(1) ) { // If Tuner 1 is recording check whether we were the one that just started it
				if (Update_RecordingInfo(1) ) {
					if (Recording_GetRecInfo(1).svcNum == GetSvcNum(Events_GetEventSvcId(GridEPG_GetEventPointer()))&&
						Recording_GetRecInfo(1).svcType == GridEPG_Mode ) { // Same service as repaired file so we must have started it
						tuner = 1;
					}
				}
			}
			if (tuner > -1&& tuner < 2) {
				if ( !HDD_RECSlotSetDuration(tuner, (word)TimeDiff(Now(0), AddTime(Events_GetEventEndTime(GridEPG_GetEventPointer()), Settings_EndBuffer))) ) { // Adjust the duration
					Message_Display("Timer started but duration failed to adjust");
					TAP_Delay(Setting_MessageTimeout);
					GridEPG_Redraw = TRUE;
					GridEPG_Render();
				} else {
					GridEPG_Redraw = TRUE;
					GridEPG_Render();
				}
			}
		} else {
			tuner = -1;
			// The complex piece of the puzzle. We need to see if a recording is present on the channel if so extend to cover the event
			// Work out whether a recording is on a tuner currently recording
			if (HDD_isRecording(0) ) { // If Tuner 1 is recording check whether we were the one that just started it
				if (Update_RecordingInfo(0) ) {
					if (Recording_GetRecInfo(0).svcNum == GetSvcNum(Events_GetEventSvcId(GridEPG_GetEventPointer()))&&
						Recording_GetRecInfo(0).svcType == GridEPG_Mode ) { // Same service as repaired file so we must have started it
						tuner = 0;
					}
				}
			}
			if (HDD_isRecording(1) ) { // If Tuner 1 is recording check whether we were the one that just started it
				if (Update_RecordingInfo(1) ) {
					if (Recording_GetRecInfo(1).svcNum == GetSvcNum(Events_GetEventSvcId(GridEPG_GetEventPointer()))&&
						Recording_GetRecInfo(1).svcType == GridEPG_Mode ) { // Same service as repaired file so we must have started it
						tuner = 1;
					}
				}
			}
			if (tuner > -1&& tuner < 2) {
				// So a timer is recording on the same channel as this event
				// We need to adjust the duration to be until the end of this event
				if ( !HDD_RECSlotSetDuration(tuner, (word)TimeDiff(Recording_GetRecInfo(tuner).startTime, AddTime(Events_GetEventEndTime(GridEPG_GetEventPointer()), Settings_EndBuffer))) ) { // Adjust the duration
					Message_Display("Currently recording on channel but failed to adjust the duration");
					TAP_Delay(Setting_MessageTimeout);
					GridEPG_Redraw = TRUE;
					GridEPG_Render();
				} else {
					Message_Display("Current recording on channel has been extended to cover the event");
					TAP_Delay(Setting_MessageTimeout);
					GridEPG_Redraw = TRUE;
					GridEPG_Render();
				}
			} else {
				tuner = -1;
				// No recording in progress on that channel so see whether we have a free tuner
				if ( !HDD_isRecording(0) || !HDD_isRecording(1) ) { // Either tuner 1 or 2 is free so we can start our instant record
					// User has elected to start recording live event
					// Change to the channel and start the recording
					TAP_Channel_Start(1,Events_GetEventSvcType(GridEPG_GetEventPointer()),GetSvcNum(Events_GetEventSvcId(GridEPG_GetEventPointer())) );
					TAP_Hdd_StartRecord(); // Start the recording on that channel
					// We need to adjust the duration 			
					// Work out which tuner we started recording on
					if (HDD_isRecording(0) ) { // If Tuner 1 is recording check whether we were the one that just started it
						if (Update_RecordingInfo(0) ) {
							if (Recording_GetRecInfo(0).svcNum == GetSvcNum(Events_GetEventSvcId(GridEPG_GetEventPointer())) &&
								Recording_GetRecInfo(0).svcType == GridEPG_Mode) { // Same service as repaired file so we must have started it
								tuner = 0;
							}
						}
					}
					if (HDD_isRecording(1) ) { // If Tuner 1 is recording check whether we were the one that just started it
						if (Update_RecordingInfo(1) ) {
							if (Recording_GetRecInfo(1).svcNum == GetSvcNum(Events_GetEventSvcId(GridEPG_GetEventPointer())) &&
								Recording_GetRecInfo(1).svcType == GridEPG_Mode ) { // Same service as repaired file so we must have started it
								tuner = 1;
							}
						}
					}
					if (tuner > -1&& tuner < 2) {
						if ( !HDD_RECSlotSetDuration(tuner, (word)TimeDiff(Now(0), AddTime(Events_GetEventEndTime(GridEPG_GetEventPointer()), Settings_EndBuffer))) ) { // Adjust the duration
							Message_Display("Timer started but duration failed to adjust");
							TAP_Delay(Setting_MessageTimeout);
							GridEPG_Redraw = TRUE;
							GridEPG_Render();
						} else {
							GridEPG_Redraw = TRUE;
							GridEPG_Render();
						}
					}
				} else {
					Message_Display("Unable to start recording. No avaliable tuner");
				}
			}
		}
	}
}

void GridEPG_ChangeChannel(int row) {
	if (Update_Pi() ) {
		if (pi.playMode != PLAYMODE_Playing ) {
			TAP_Channel_Start( 1, Events_GetEventSvcType(GridEPG_GetEventPointer()), GetSvcNum(Events_GetEventSvcId(GridEPG_GetEventPointer())) );
			TAP_Delay(100);
			GridEPG_Close();
		} else {
			Message_Display("Please stop playback to change channel.");
		}
	}
}

// Function to get and set the variable associated with editing the timer
int GridEPG_GetTimer( int row ){
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
	event = Events_GetEvent(row);
	totalTimers = TAP_Timer_GetTotalNum();
	// Store the event day of occurance
	TAP_ExtractMjd( event.startTime>>16, &year2, &month2, &day2, &weekDay2);
	for ( i = 0; i < totalTimers; i++ ){
		TAP_Timer_GetInfo(i, &currentTimer); // Sequentially get timer details and compare with those in the display range
		TAP_ExtractMjd( currentTimer.startTime>>16, &year, &month, &day, &weekDay);
		TAP_Channel_GetInfo( currentTimer.svcType, currentTimer.svcNum, &currentChInfo );

		switch( currentTimer.reservationType ){
			case RESERVE_TYPE_Onetime:
				if( (result=Timer_TimerFind( currentChInfo.svcId, Events_GetEventSvcId(row), currentTimer.startTime, AddTime(currentTimer.startTime, currentTimer.duration), event.startTime, event.endTime )) != TIMER_NONE ){
					return i;
				}
				break;
			case RESERVE_TYPE_Everyday:
				// Initial check to determine against currentTimer start
				if( (result=Timer_TimerFind( currentChInfo.svcId, Events_GetEventSvcId(row), currentTimer.startTime, AddTime(currentTimer.startTime, currentTimer.duration), event.startTime, event.endTime )) != TIMER_NONE ){
					return i;
				}
				// Not found on today so we need to go and increment for the next 7 days and check on weekdays
				for( x = 0; x < TIMERS_MAXDAYSAHEAD; x++ ){
					currentTimer.startTime = AddTime( currentTimer.startTime, 1440 );
					if( (result=Timer_TimerFind( currentChInfo.svcId, Events_GetEventSvcId(row), currentTimer.startTime, AddTime(currentTimer.startTime, currentTimer.duration), event.startTime, event.endTime )) != TIMER_NONE ){
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
						if( (result=Timer_TimerFind( currentChInfo.svcId, Events_GetEventSvcId(row), currentTimer.startTime, AddTime(currentTimer.startTime, currentTimer.duration), event.startTime, event.endTime )) != TIMER_NONE ){
							return i;
						}
					}
					// Not found on today so we need to go and increment for the next 7 days and check on weekdays
					for( x = 0; x < TIMERS_MAXDAYSAHEAD; x++ ){
						currentTimer.startTime = AddTime( currentTimer.startTime, 1440 );
						TAP_ExtractMjd( currentTimer.startTime>>16, &year, &month, &day, &weekDay);
						if( weekDay == SAT || weekDay == SUN  ){	// Sanity check to make sure timer is on a weekday
							if( (result=Timer_TimerFind( currentChInfo.svcId, Events_GetEventSvcId(row), currentTimer.startTime, AddTime(currentTimer.startTime, currentTimer.duration), event.startTime, event.endTime )) != TIMER_NONE ){
								return i;
							}
						}
					}
				}
				break;
			case RESERVE_TYPE_Weekly:
				if( (result=Timer_TimerFind( currentChInfo.svcId, Events_GetEventSvcId(row), currentTimer.startTime, AddTime(currentTimer.startTime, currentTimer.duration), event.startTime, event.endTime )) != TIMER_NONE ){
					return i;
				}
				for( x = 0; x < 7; x++ ){	// AddTime only handle 1440 max so loop the adding of the time
					currentTimer.startTime = AddTime(currentTimer.startTime, 1440 );
				}
				// Only search a week ahead atm as data shouldn't be anymore than that
				if( (result=Timer_TimerFind( currentChInfo.svcId, Events_GetEventSvcId(row), currentTimer.startTime, AddTime(currentTimer.startTime, currentTimer.duration), event.startTime, event.endTime )) != TIMER_NONE ){
					return i;
				}
				break;
			case RESERVE_TYPE_WeekDay:
				// if event is not on the weekend we need to check every week day from the start to determine
				// whether event has timer
				if( weekDay2 != SAT && weekDay2 != SUN ){
					// Initial check to determine against currentTimer start
					if( weekDay != SAT && weekDay != SUN  ){	// Sanity check to make sure timer is on a weekday
						if( (result=Timer_TimerFind( currentChInfo.svcId, Events_GetEventSvcId(row), currentTimer.startTime, AddTime(currentTimer.startTime, currentTimer.duration), event.startTime, event.endTime )) != TIMER_NONE ){
							return i;
						}
					}
					// Not found on today so we need to go and increment for the next 7 days and check on weekdays
					for( x = 0; x < TIMERS_MAXDAYSAHEAD; x++ ){
						currentTimer.startTime = AddTime( currentTimer.startTime, 1440 );
						TAP_ExtractMjd( currentTimer.startTime>>16, &year, &month, &day, &weekDay);
						if( weekDay != SAT && weekDay != SUN  ){	// Sanity check to make sure timer is on a weekday
							if( (result=Timer_TimerFind( currentChInfo.svcId, Events_GetEventSvcId(row), currentTimer.startTime, AddTime(currentTimer.startTime, currentTimer.duration), event.startTime, event.endTime )) != TIMER_NONE ){
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
