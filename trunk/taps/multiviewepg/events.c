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
#include "messagebox.h"
#include "events.h"
#include "tools.h"

// Desc: Routine to free the memory that the events consume. Must be called when TAP exits

void Events_TAPExit( void ){
	TAP_MemFree( _evtInfo );
	Events_FreeMemory();
}

// Desc: Routine to return the svcNum of the event. *nb When events are inserted into the array the runningStatus has the svcNum stored in place
// IN:
//     item - Location in array of element

int Events_GetEventSvcNum( int item ){
	TYPE_TapEvent event;
	if( item < eventsPointer ){
		event = *events[item];
		return event.runningStatus;
	}
	return 0;
}

// Desc: Routine to return the svcType of the event
// IN:
//     item - Location in array of element

int Events_GetEventSvcType( int item ){
	TYPE_TapEvent event;
	if( item < eventsPointer ){
		event = *events[item];
		return event.satIdx;
	}
	return 0;
}

// Desc: Routine to return the svcId of the event
// IN:
//     item - Location in array of element

int Events_GetEventSvcId( int item ){
	TYPE_TapEvent event;
	if( item < eventsPointer ){
		event = *events[item];
	}
	return event.svcId;
}

// Desc: Routine to return a pointer to a static copy of the item.
// IN:
//     item - Location in array of element

TYPE_TapEvent Events_GetEvent( int item ){
	static TYPE_TapEvent event;
	if( item < eventsPointer ){
		event = *events[item];
	}
	return event;
}

// Desc: Routine to return the name of the event
// IN:
//     item - Location in array of element

char * Events_GetEventName( int item ){
	static TYPE_TapEvent event;
	if( item < eventsPointer ){
		event = *events[item];
	}
	return event.eventName;
}

// Desc: Routine to return the event desciption of the event
// IN:
//     item - Location in array of element

char * Events_GetEventDesc( int item ){
	static TYPE_TapEvent event;
	if( item < eventsPointer ){
		event = *events[item];
	}
	return event.description;
}

// Desc: Routine to return the start time of the event
// IN:
//     item - Location in array of element

dword Events_GetEventStartTime( int item ){
	static TYPE_TapEvent event;
	if( item < eventsPointer ){
		event = *events[item];
	}
	return event.startTime;
}

// Desc: Routine to return the endTime of the event
// IN:
//     item - Location in array of element

dword Events_GetEventEndTime( int item ){
	static TYPE_TapEvent event;
	if( item < eventsPointer ){
		event = *events[item];
	}
	return event.endTime;
}

// Desc: Routine to free the memory used by the events

void Events_FreeMemory( void ){
	IMEM_Kill();
	eventsPointer = 0;
}				

// Desc: Routine to insert event into events array
// IN:
//     event - Event to be inserted
//     svcType - Type of service i.e. Tv/Radio
//     svcNum - Service number that event belongs to
//
// To Do:
//     This routine need some small work on checking for overlap items and adjusting as required

void Events_InsertEvent( TYPE_TapEvent event, int svcType, int svcNum ){
	static int i = 0;
	bool dup = FALSE;
	if( eventsPointer < MAX_EVENTS ){	// Sanity check
		if( event.endTime > AddTime(Now(0),-120)){	// Past event check allow 60 minutes of old events
			if( eventsPointer > 0 ){
				// Scan and look whether event all ready stored
				for( i = 0; i < eventsPointer; i++ ){
					if( events[i]->startTime == event.startTime && events[i]->svcId == event.svcId ){
						dup = TRUE;
						i = eventsPointer;	// Break out of the loop
					}
				}
//				if( events[i-1]->svcId == event.svcId ){
//					// Same service so look for non matching wrong time match
//					if( event.startTime >= events[i-1]->startTime &&
//						event.endTime > events[i-1]->endTime &&
//						event.startTime < events[i-1]->endTime ){
//						dup = TRUE;
//					}
//				}
			
				if( !dup ){
					event.runningStatus = svcNum;
					event.satIdx = svcType;
					events[eventsPointer] = IMEM_Alloc( sizeof(TYPE_TapEvent) );
					if( events[eventsPointer] ){
						memcpy(events[eventsPointer], &event, sizeof(TYPE_TapEvent));
						eventsPointer++;
					}
				}
			} else { // First item so just insert
				event.runningStatus = svcNum;
				event.satIdx = svcType;
				events[eventsPointer] = IMEM_Alloc( sizeof(TYPE_TapEvent) );
				if( events[eventsPointer] ){
					memcpy(events[eventsPointer], &event, sizeof(TYPE_TapEvent));
					eventsPointer++;
				}
			}
		}
	}
}

// Desc: Routine to return a text representation of the sort order
// IN:
//     order - The order which the text representation is needed

char* Events_SortOrderText( int order ){
	switch( order ){
		case SORT_NAME:
			return "Name";
			break;
		case SORT_CHANNEL:
			return "Channel";
			break;		
		case SORT_START:
			return "Start";
		case SORT_RATING:
			return "Rating";
		default:
			return "Unknown";
			break;
	}
}

// Desc: Routine to sort the channels in the background i.e. No OSD

void Events_SortByChannelBackground( void ){
	TYPE_TapEvent *tmpEvent;
	int i, j, idx;
	Events_Loading = TRUE;
	for( i = 0; i < eventsPointer; i++ ){
		idx = i;
		for( j = i; j < eventsPointer; j++ ){
			// If the index is a radio station and the item we are looking at is a TV channel swap. This should bump radio to the end of the list
			if( events[idx]->satIdx != SVC_TYPE_Radio && events[j]->satIdx != SVC_TYPE_Tv ){
				idx = j;
			} else {
			// I sneakely hide the svc num in runningStatus
				if( events[idx]->runningStatus > events[j]->runningStatus ){	// Group the channels before the ordering of the times
					idx = j;
				} else {
					if( events[idx]->runningStatus == events[j]->runningStatus ){
						if( events[idx]->startTime > events[j]->startTime ){
							// Lets Swap
							idx = j;
						}
					}
				}
			}
		}
		// Swap the pointers around
		tmpEvent = events[i];
		events[i] = events[idx];
		events[idx] = tmpEvent;
	}
	sortOrder = SORT_CHANNEL;
	sorted = TRUE;
	Events_Loading = FALSE;
}

// Desc: Routine to sort the events array by channel with OSD. Use when sorting and a display is shown to indicate to the user

void Events_SortByChannel( void ){
	TYPE_TapEvent *tmpEvent;
	int i, j, idx;
	char text[128];
	dword timeout;
	if( Display_BackgroundDrawn ){
		sprintf( text, "Sorting events by channel, start time .\0" );
		Message_Display(text);
	}
	timeout = TAP_GetTick();
	for( i = 0; i < eventsPointer; i++ ){
		idx = i;
		for( j = i; j < eventsPointer; j++ ){
			// SatIdx stores the svcType when we insert it. To do create new structure for these extra variables
			if( events[idx]->satIdx != SVC_TYPE_Radio && events[j]->satIdx != SVC_TYPE_Tv ){
				idx = j;
			} else {
				// I sneakely hide the svc num in runningStatus as it is easy to calculate whether it is running now
				if( events[idx]->runningStatus > events[j]->runningStatus ){	// Group the channels before the ordering of the times
					idx = j;
				} else {
					if( events[idx]->runningStatus == events[j]->runningStatus ){
						if( events[idx]->startTime > events[j]->startTime ){
							// Lets Swap
							idx = j;
						}
					}
				}
			}
		}
		// Swap the pointers around
		tmpEvent = events[i];
		events[i] = events[idx];
		events[idx] = tmpEvent;
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
	Message_DisplayClear();
}

// Desc: Routine to remove old events from the array of events. The value is hard coded to 120 minutes so any event that start before 2 hours ago will be dropped

void Events_RemoveOld( void ){
	int i;
	int j;
	if( eventsPointer < 1 ) return;
	for( i = 0; i < eventsPointer; i++ ){
		if( events[i]->endTime <= AddTime(Now(0),-120) ){
			for( j = i; j < eventsPointer-1; j++ ){
				events[j] = events[j+1];
			}
			eventsPointer--;
			i = -1;
		}
	}
}

// Desc: Routine to populate the events array. Use when a OSD is present to indicate to the user

void Events_Get( void ){
	int i,j;
	int tvNum, radioNum;
	int _eventNum;
	int totalEvents = 0;
	char text[128];
	dword timeout;
	Guide_HasEvents = FALSE;	// Everytime we reload ensure that the Guide ( Linear list will regrab the events )
	TAP_Channel_GetTotalNum(&tvNum, &radioNum );
	Events_FreeMemory();	// Free the memory currently being used
	if( Display_BackgroundDrawn ){
		sprintf( text, "Please Wait.\0" );
		Message_Display(text);
	}
	timeout = TAP_GetTick();
	// Get the number of events we will need to allocate to the IMEM manager. Getthe total number of TV and Radio channels
	for( i = 0; i < tvNum; i++ ){
		_evtInfo = TAP_GetEvent( SVC_TYPE_Tv, i, &_eventNum );
		totalEvents += _eventNum;
		if( Display_BackgroundDrawn ){
			if( timeout + 10 < TAP_GetTick() ){
				strcat( text, "." );
				Message_Display(text);
				timeout = TAP_GetTick();
			}
		}
		if( _evtInfo ){
			TAP_MemFree( _evtInfo );
			_evtInfo = 0;
		}
	}
	for( i = 0; i < radioNum; i++ ){
		_evtInfo = TAP_GetEvent( SVC_TYPE_Radio, i, &_eventNum );
		totalEvents += _eventNum;
		if( Display_BackgroundDrawn ){
			if( timeout + 10 < TAP_GetTick() ){
				strcat( text, "." );
				Message_Display(text);
				timeout = TAP_GetTick();
			}
		}
		if( _evtInfo ){
			TAP_MemFree( _evtInfo );
			_evtInfo = 0;
		}
	}
	IMEM_Init((sizeof(TYPE_TapEvent)*totalEvents));
	eventsPointer = 0;
	// Ensure that the memory is freed
	if( _evtInfo ){
		TAP_MemFree( _evtInfo );
		_evtInfo = 0;
	}
	if( Display_BackgroundDrawn ){
		sprintf( text, "Please Wait. Loading events  \0" );
		Message_Display(text);
	}
	timeout = TAP_GetTick();
	for( i = 0; i < tvNum; i++ ){
		_evtInfo = TAP_GetEvent( SVC_TYPE_Tv, i, &_eventNum );
		for( j = 0; j < _eventNum; j++ ){
			Events_InsertEvent( _evtInfo[j], SVC_TYPE_Tv, i );
		}
		if( Display_BackgroundDrawn ){
			if( timeout + 10 < TAP_GetTick() ){
				strcat( text, "." );
				Message_Display(text);
				timeout = TAP_GetTick();
			}
		}
		if( _evtInfo ){
			TAP_MemFree( _evtInfo );
			_evtInfo = 0;
		}
	}
	for( i = 0; i < radioNum; i++ ){
		_evtInfo = TAP_GetEvent( SVC_TYPE_Radio, i, &_eventNum );
		for( j = 0; j < _eventNum; j++ ){
			Events_InsertEvent( _evtInfo[j], SVC_TYPE_Radio, i );
		}
		if( Display_BackgroundDrawn ){
			if( timeout + 10 < TAP_GetTick() ){
				strcat( text, "." );
				Message_Display(text);
				timeout = TAP_GetTick();
			}
		}
		if( _evtInfo ){
			TAP_MemFree( _evtInfo );
			_evtInfo = 0;
		}
	}
	if( _evtInfo ){
		TAP_MemFree( _evtInfo );
		_evtInfo = 0;
	}
	sprintf( text, "Total events - %d events\0", eventsPointer );
	Message_Display(text);
	TAP_Delay(100);
	Message_DisplayClear();
	dataCollected = DATA_COLLECTED;
}

// Desc: Routine to populate the events array in the background. Called by reload hour and by the startUp delay

void Events_GetBackground( void ){
	int i,j;
	int tvNum, radioNum;
	int _eventNum;
	int totalEvents = 0;
	dword timeout;
	TAP_Channel_GetTotalNum(&tvNum, &radioNum );
	Events_Loading = TRUE;
	Guide_HasEvents = FALSE;	// Everytime we reload ensure that the Guide ( Linear list will regrab the events )
	Events_FreeMemory();	// Free the memory currently being used
	// Get the number of events we will need to allocate to the IMEM manager
	for( i = 0; i < tvNum; i++ ){
		_evtInfo = TAP_GetEvent( SVC_TYPE_Tv, i, &_eventNum );
		totalEvents += _eventNum;
		if( _evtInfo ){
			TAP_MemFree( _evtInfo );
			_evtInfo = 0;
		}
		TAP_SystemProc();
	}
	for( i = 0; i < radioNum; i++ ){
		_evtInfo = TAP_GetEvent( SVC_TYPE_Radio, i, &_eventNum );
		totalEvents += _eventNum;
		if( _evtInfo ){
			TAP_MemFree( _evtInfo );
			_evtInfo = 0;
		}
		TAP_SystemProc();
	}
	IMEM_Init((sizeof(TYPE_TapEvent)*totalEvents));	// May include extra space becuase of duplicates though
	eventsPointer = 0;	// Reset the inserting of the events array
	if( _evtInfo ){
		TAP_MemFree( _evtInfo );
		_evtInfo = 0;
	}
	timeout = TAP_GetTick();
	for( i = 0; i < tvNum; i++ ){
		_evtInfo = TAP_GetEvent( SVC_TYPE_Tv, i, &_eventNum );
		for( j = 0; j < _eventNum; j++ ){
			Events_InsertEvent( _evtInfo[j], SVC_TYPE_Tv, i );
			TAP_SystemProc();
		}
		if( _evtInfo ){
			TAP_MemFree( _evtInfo );
			_evtInfo = 0;
		}
	}
	for( i = 0; i < radioNum; i++ ){
		_evtInfo = TAP_GetEvent( SVC_TYPE_Radio, i, &_eventNum );
		for( j = 0; j < _eventNum; j++ ){
			Events_InsertEvent( _evtInfo[j], SVC_TYPE_Radio, i );
		}
		if( _evtInfo ){
			TAP_MemFree( _evtInfo );
			_evtInfo = 0;
		}
	}
	if( _evtInfo ){
		TAP_MemFree( _evtInfo );
		_evtInfo = 0;
	}
	Events_Loading = FALSE;
	dataCollected = DATA_COLLECTED;
}

// Desc: Routine to reload the events at the users set reload hour. This will update in the background with no OSD

void Events_ReloadHourIdle( void ){
	// Only check every Minute
	if( Events_ReloadLastCheck + 6000 < TAP_GetTick() ){
		Events_ReloadLastCheck = TAP_GetTick();
		if( !Events_Reloaded ){
			if( Settings_ReloadHour >= 0 ){
				TAP_GetTime( &mjd, &hour, &min, &sec);
				if( Settings_ReloadHour == hour ){
					if( !Events_Loading ){
						Events_Reloaded = TRUE;	// Flag that we have reloaded the data to not end up back here in a endless loop
						Events_GetBackground();
						Events_SortByChannelBackground();
					}
				}
			}
		} else {
			if( Settings_ReloadHour >= 0 ){
				if( Events_Reloaded ){
					TAP_GetTime( &mjd, &hour, &min, &sec);
					if( Settings_ReloadHour != hour ){
						Events_Reloaded = FALSE;
					}
				}
			}
		}
	}
}

// Desc: Routine to populate the events at startup after the users selected delay period.

void Events_LoadAtStartIdle( void ){
	if( !Settings_LoadFile ){	// Settings file has been loaded
		if( Settings_LoadDelay >= 0 ){	// Must have a delay for event loading
			if( TAP_GetTick() >= Settings_LoadDelay ){	// We have had the users selected delay
				if( dataCollected != DATA_COLLECTED ){
					if( !Events_Loading ){
						Events_GetBackground();	// Load the events in the background
						Events_SortByChannelBackground();
					}
				}
			}
		}
	}
}

