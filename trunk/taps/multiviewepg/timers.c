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
#include "settings.h"
#include "events.h"
#include "menu.h"
#include "guide.h"
#include "tools.h"
#include "messagebox.h"
#include "timers.h"

// Function to return whether current event is recording
// Function to return whether current event is recording
int Timers_IsRecordingEvent( int row ){
	TYPE_TapEvent event;
	event = Events_GetEvent(row);
	Update_RecordingInfo(0);
	Update_RecordingInfo(1);
	// Now lets check the currently recording if there is any to see if we overlap
	if( HDD_isRecording(0) ){
		// Check for same channel
		if( GetSvcId(Recording_GetRecInfo(0).svcType, Recording_GetRecInfo(0).svcNum) == Events_GetEventSvcId(row) ){
			// Check for timer completely covering this event
			if( Recording_GetRecInfo(0).startTime <= event.startTime &&
				AddTime(Recording_GetRecInfo(0).startTime,Recording_GetRecInfo(0).duration ) >= event.endTime ){
				return TIMER_FULL;
			}
			if( 
				// Start overlap
				(Recording_GetRecInfo(0).startTime <= event.startTime &&
				AddTime(Recording_GetRecInfo(0).startTime,Recording_GetRecInfo(0).duration ) >= event.startTime )
				||
				// End overlap
				(Recording_GetRecInfo(0).startTime <= event.endTime &&
				AddTime(Recording_GetRecInfo(0).startTime,Recording_GetRecInfo(0).duration ) >= event.endTime )
				||
				(Recording_GetRecInfo(0).startTime >= event.startTime &&
				AddTime(Recording_GetRecInfo(0).startTime,Recording_GetRecInfo(0).duration ) <= event.endTime)){
				return TIMER_PARTIAL;
			}
		}
	}
	if( HDD_isRecording(1) ){
		if( GetSvcId(Recording_GetRecInfo(1).svcType,Recording_GetRecInfo(1).svcNum) == Events_GetEventSvcId(row) ){
			// Check for timer completely covering this event
			if( Recording_GetRecInfo(1).startTime <= event.startTime &&
				AddTime(Recording_GetRecInfo(1).startTime,Recording_GetRecInfo(1).duration ) >= event.endTime ){
				return TIMER_FULL;
			}
			if( 
				// Start overlap
				(Recording_GetRecInfo(1).startTime <= event.startTime &&
				AddTime(Recording_GetRecInfo(1).startTime,Recording_GetRecInfo(1).duration ) >= event.startTime )
				||
				// End overlap
				(Recording_GetRecInfo(1).startTime <= event.endTime &&
				AddTime(Recording_GetRecInfo(1).startTime,Recording_GetRecInfo(1).duration ) >= event.endTime )
				||
				// Recording in middle of event
				(Recording_GetRecInfo(1).startTime >= event.startTime &&
				AddTime(Recording_GetRecInfo(1).startTime,Recording_GetRecInfo(1).duration ) <= event.endTime)
				){
				return TIMER_PARTIAL;
			}
		}
	}
	return TIMER_NONE;
}

int Timer_TimerFind( word chId, word evtId, dword timerStart, dword timerEnd, dword eventStart, dword eventEnd ){
	if( chId == evtId ){
	// Check whether the timer completely covers the event
		if( timerStart <= eventStart &&
			timerEnd >= eventEnd ){
			return TIMER_FULL;
		}
		if( Setting_ShowPartialTimers ){
		// Check for partial cover at end and start
			if( 
			// Start overlap
			(timerStart <= eventStart &&
			timerEnd >= eventStart )
			||
			// End overlap
			(timerStart <= eventEnd &&
			timerEnd >= eventEnd )
			||
				// Middle overlap
			(timerStart >= eventStart &&
			timerEnd <= eventEnd )
			){
				return TIMER_PARTIAL;
			}
		}
	}
	return TIMER_NONE;
}

int Timers_HasTimerDebug( int row ){
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
	char debug[1024];
	memset( debug, 0, sizeof(debug) );
	if( row < 0 ) return -1;
	Update_RecordingInfo(0);
	Update_RecordingInfo(1);
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
					sprintf(debug, "Result of search %d\0", result);
					Log("JimsGuide.log", "JimsGuide", FALSE, TIMESTAMP_NONE, debug);
					return result;
				}
				break;
			case RESERVE_TYPE_Everyday:
				// Initial check to determine against currentTimer start
				if( (result=Timer_TimerFind( currentChInfo.svcId, Events_GetEventSvcId(row), currentTimer.startTime, AddTime(currentTimer.startTime, currentTimer.duration), event.startTime, event.endTime )) != TIMER_NONE ){
					sprintf(debug, "Result of search %d\0", result);
					Log("JimsGuide.log", "JimsGuide", FALSE, TIMESTAMP_NONE, debug);
					return result;
				}
				// Not found on today so we need to go and increment for the next 7 days and check on weekdays
				for( x = 0; x < TIMERS_MAXDAYSAHEAD; x++ ){
					currentTimer.startTime = AddTime( currentTimer.startTime, 1440 );
					if( (result=Timer_TimerFind( currentChInfo.svcId, Events_GetEventSvcId(row), currentTimer.startTime, AddTime(currentTimer.startTime, currentTimer.duration), event.startTime, event.endTime )) != TIMER_NONE ){
						sprintf(debug, "Result of search %d\0", result);
						Log("JimsGuide.log", "JimsGuide", FALSE, TIMESTAMP_NONE, debug);
						return result;
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
							sprintf(debug, "Result of search %d\0", result);
							Log("JimsGuide.log", "JimsGuide", FALSE, TIMESTAMP_NONE, debug);
							return result;
						}
					}
					// Not found on today so we need to go and increment for the next 7 days and check on weekdays
					for( x = 0; x < TIMERS_MAXDAYSAHEAD; x++ ){
						currentTimer.startTime = AddTime( currentTimer.startTime, 1440 );
						TAP_ExtractMjd( currentTimer.startTime>>16, &year, &month, &day, &weekDay);
						if( weekDay == SAT || weekDay == SUN  ){	// Sanity check to make sure timer is on a weekday
							if( (result=Timer_TimerFind( currentChInfo.svcId, Events_GetEventSvcId(row), currentTimer.startTime, AddTime(currentTimer.startTime, currentTimer.duration), event.startTime, event.endTime )) != TIMER_NONE ){
								sprintf(debug, "Result of search %d\0", result);
								Log("JimsGuide.log", "JimsGuide", FALSE, TIMESTAMP_NONE, debug);
								return result;
							}
						}
					}
				}
				break;
			case RESERVE_TYPE_Weekly:
				if( (result=Timer_TimerFind( currentChInfo.svcId, Events_GetEventSvcId(row), currentTimer.startTime, AddTime(currentTimer.startTime, currentTimer.duration), event.startTime, event.endTime )) != TIMER_NONE ){
					sprintf(debug, "Result of search %d\0", result);
					Log("JimsGuide.log", "JimsGuide", FALSE, TIMESTAMP_NONE, debug);
					return result;
				}
				for( x = 0; x < 7; x++ ){	// AddTime only handle 1440 max so loop the adding of the time
					currentTimer.startTime = AddTime(currentTimer.startTime, 1440 );
				}
				// Only search a week ahead atm as data shouldn't be anymore than that
				if( (result=Timer_TimerFind( currentChInfo.svcId, Events_GetEventSvcId(row), currentTimer.startTime, AddTime(currentTimer.startTime, currentTimer.duration), event.startTime, event.endTime )) != TIMER_NONE ){
					sprintf(debug, "Result of search %d\0", result);
					Log("JimsGuide.log", "JimsGuide", FALSE, TIMESTAMP_NONE, debug);
					return result;
				}
				break;
			case RESERVE_TYPE_WeekDay:
				// if event is not on the weekend we need to check every week day from the start to determine
				// whether event has timer
				if( weekDay2 != SAT && weekDay2 != SUN ){
					// Initial check to determine against currentTimer start
					if( weekDay != SAT && weekDay != SUN  ){	// Sanity check to make sure timer is on a weekday
						if( (result=Timer_TimerFind( currentChInfo.svcId, Events_GetEventSvcId(row), currentTimer.startTime, AddTime(currentTimer.startTime, currentTimer.duration), event.startTime, event.endTime )) != TIMER_NONE ){
							sprintf(debug, "Result of search %d\0", result);
							Log("JimsGuide.log", "JimsGuide", FALSE, TIMESTAMP_NONE, debug);
							return result;
						}
					}
					// Not found on today so we need to go and increment for the next 7 days and check on weekdays
					for( x = 0; x < TIMERS_MAXDAYSAHEAD; x++ ){
						currentTimer.startTime = AddTime( currentTimer.startTime, 1440 );
						TAP_ExtractMjd( currentTimer.startTime>>16, &year, &month, &day, &weekDay);
						if( weekDay != SAT && weekDay != SUN  ){	// Sanity check to make sure timer is on a weekday
							if( (result=Timer_TimerFind( currentChInfo.svcId, Events_GetEventSvcId(row), currentTimer.startTime, AddTime(currentTimer.startTime, currentTimer.duration), event.startTime, event.endTime )) != TIMER_NONE ){
								sprintf(debug, "Result of search %d\0", result);
								Log("JimsGuide.log", "JimsGuide", FALSE, TIMESTAMP_NONE, debug);
								return result;
							}
						}
					}
				}
				break;
		}
	}
	// Now lets check the currently recording if there is any to see if we overlap
	if( HDD_isRecording(0) ){
		// Check for same channel
		if( GetSvcId(Recording_GetRecInfo(0).svcType, Recording_GetRecInfo(0).svcNum) == Events_GetEventSvcId(row) ){
			// Check for timer completely covering this event
			if( Recording_GetRecInfo(0).startTime <= event.startTime &&
				AddTime(Recording_GetRecInfo(0).startTime,Recording_GetRecInfo(0).duration ) >= event.endTime ){
				Log("JimsGuide.log", "JimsGuide", FALSE, TIMESTAMP_NONE, "Result of search = Timer FULL");
				return TIMER_FULL;
			}
			if( 
				// Start overlap
				(Recording_GetRecInfo(0).startTime <= event.startTime &&
				AddTime(Recording_GetRecInfo(0).startTime,Recording_GetRecInfo(0).duration ) >= event.startTime )
				||
				// End overlap
				(Recording_GetRecInfo(0).startTime <= event.endTime &&
				AddTime(Recording_GetRecInfo(0).startTime,Recording_GetRecInfo(0).duration ) >= event.endTime )){
				Log("JimsGuide.log", "JimsGuide", FALSE, TIMESTAMP_NONE, "Result of search = Timer FULL");
				return TIMER_PARTIAL;
			}
			if( Recording_GetRecInfo(0).startTime >= event.startTime &&
				AddTime(Recording_GetRecInfo(0).startTime,Recording_GetRecInfo(0).duration ) <= event.endTime ){
				// Recording is middle part of the show
				return TIMER_PARTIAL;
			}
		}
	}
	if( HDD_isRecording(1) ){
		if( GetSvcId(Recording_GetRecInfo(1).svcType,Recording_GetRecInfo(1).svcNum) == Events_GetEventSvcId(row) ){
			// Check for timer completely covering this event
			if( Recording_GetRecInfo(1).startTime <= event.startTime &&
				AddTime(Recording_GetRecInfo(1).startTime,Recording_GetRecInfo(1).duration ) >= event.endTime ){
				return TIMER_FULL;
			}
			if( Setting_ShowPartialTimers ){
				// Check for partial cover
				if( 
					// Start overlap
					(Recording_GetRecInfo(1).startTime <= event.startTime &&
					AddTime(Recording_GetRecInfo(1).startTime,Recording_GetRecInfo(1).duration ) >= event.startTime )
					||
					// End overlap
					(Recording_GetRecInfo(1).startTime <= event.endTime &&
					AddTime(Recording_GetRecInfo(1).startTime,Recording_GetRecInfo(1).duration ) >= event.endTime )){
					return TIMER_PARTIAL;
				}
				if( Recording_GetRecInfo(1).startTime >= event.startTime &&
					AddTime(Recording_GetRecInfo(1).startTime,Recording_GetRecInfo(1).duration ) <= event.endTime ){
					return TIMER_PARTIAL;
				}
			}
		}
	}
	return TIMER_NONE;
}

int Timers_HasTimer( int row ){
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
	Update_RecordingInfo(0);
	Update_RecordingInfo(1);
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
					return result;
				}
				break;
			case RESERVE_TYPE_Everyday:
				// Initial check to determine against currentTimer start
				if( (result=Timer_TimerFind( currentChInfo.svcId, Events_GetEventSvcId(row), currentTimer.startTime, AddTime(currentTimer.startTime, currentTimer.duration), event.startTime, event.endTime )) != TIMER_NONE ){
					return result;
				}
				// Not found on today so we need to go and increment for the next 7 days and check on weekdays
				for( x = 0; x < TIMERS_MAXDAYSAHEAD; x++ ){
					currentTimer.startTime = AddTime( currentTimer.startTime, 1440 );
					if( (result=Timer_TimerFind( currentChInfo.svcId, Events_GetEventSvcId(row), currentTimer.startTime, AddTime(currentTimer.startTime, currentTimer.duration), event.startTime, event.endTime )) != TIMER_NONE ){
						return result;
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
							return result;
						}
					}
					// Not found on today so we need to go and increment for the next 7 days and check on weekdays
					for( x = 0; x < TIMERS_MAXDAYSAHEAD; x++ ){
						currentTimer.startTime = AddTime( currentTimer.startTime, 1440 );
						TAP_ExtractMjd( currentTimer.startTime>>16, &year, &month, &day, &weekDay);
						if( weekDay == SAT || weekDay == SUN  ){	// Sanity check to make sure timer is on a weekday
							if( (result=Timer_TimerFind( currentChInfo.svcId, Events_GetEventSvcId(row), currentTimer.startTime, AddTime(currentTimer.startTime, currentTimer.duration), event.startTime, event.endTime )) != TIMER_NONE ){
								return result;
							}
						}
					}
				}
				break;
			case RESERVE_TYPE_Weekly:
				if( (result=Timer_TimerFind( currentChInfo.svcId, Events_GetEventSvcId(row), currentTimer.startTime, AddTime(currentTimer.startTime, currentTimer.duration), event.startTime, event.endTime )) != TIMER_NONE ){
					return result;
				}
				for( x = 0; x < 7; x++ ){	// AddTime only handle 1440 max so loop the adding of the time
					currentTimer.startTime = AddTime(currentTimer.startTime, 1440 );
				}
				// Only search a week ahead atm as data shouldn't be anymore than that
				if( (result=Timer_TimerFind( currentChInfo.svcId, Events_GetEventSvcId(row), currentTimer.startTime, AddTime(currentTimer.startTime, currentTimer.duration), event.startTime, event.endTime )) != TIMER_NONE ){
					return result;
				}
				break;
			case RESERVE_TYPE_WeekDay:
				// if event is not on the weekend we need to check every week day from the start to determine
				// whether event has timer
				if( weekDay2 != SAT && weekDay2 != SUN ){
					// Initial check to determine against currentTimer start
					if( weekDay != SAT && weekDay != SUN  ){	// Sanity check to make sure timer is on a weekday
						if( (result=Timer_TimerFind( currentChInfo.svcId, Events_GetEventSvcId(row), currentTimer.startTime, AddTime(currentTimer.startTime, currentTimer.duration), event.startTime, event.endTime )) != TIMER_NONE ){
							return result;
						}
					}
					// Not found on today so we need to go and increment for the next 7 days and check on weekdays
					for( x = 0; x < TIMERS_MAXDAYSAHEAD; x++ ){
						currentTimer.startTime = AddTime( currentTimer.startTime, 1440 );
						TAP_ExtractMjd( currentTimer.startTime>>16, &year, &month, &day, &weekDay);
						if( weekDay != SAT && weekDay != SUN  ){	// Sanity check to make sure timer is on a weekday
							if( (result=Timer_TimerFind( currentChInfo.svcId, Events_GetEventSvcId(row), currentTimer.startTime, AddTime(currentTimer.startTime, currentTimer.duration), event.startTime, event.endTime )) != TIMER_NONE ){
								return result;
							}
						}
					}
				}
				break;
		}
	}
	// Now lets check the currently recording if there is any to see if we overlap
	if( HDD_isRecording(0) ){
		// Check for same channel
		if( GetSvcId(Recording_GetRecInfo(0).svcType, Recording_GetRecInfo(0).svcNum) == Events_GetEventSvcId(row) ){
			// Check for timer completely covering this event
			if( Recording_GetRecInfo(0).startTime <= event.startTime &&
				AddTime(Recording_GetRecInfo(0).startTime,Recording_GetRecInfo(0).duration ) >= event.endTime ){
				return TIMER_FULL;
			}
			if( Setting_ShowPartialTimers ){
				// Check for partial cover at end and start
				if( 
					// Start overlap
					(Recording_GetRecInfo(0).startTime <= event.startTime &&
					AddTime(Recording_GetRecInfo(0).startTime,Recording_GetRecInfo(0).duration ) >= event.startTime )
					||
					// End overlap
					(Recording_GetRecInfo(0).startTime <= event.endTime &&
					AddTime(Recording_GetRecInfo(0).startTime,Recording_GetRecInfo(0).duration ) >= event.endTime )){
					return TIMER_PARTIAL;
				}
				if( Recording_GetRecInfo(0).startTime >= event.startTime &&
					AddTime(Recording_GetRecInfo(0).startTime,Recording_GetRecInfo(0).duration ) <= event.endTime ){
					// Recording is middle part of the show
					return TIMER_PARTIAL;
				}
			}
		}
	}
	if( HDD_isRecording(1) ){
		if( GetSvcId(Recording_GetRecInfo(1).svcType,Recording_GetRecInfo(1).svcNum) == Events_GetEventSvcId(row) ){
			// Check for timer completely covering this event
			if( Recording_GetRecInfo(1).startTime <= event.startTime &&
				AddTime(Recording_GetRecInfo(1).startTime,Recording_GetRecInfo(1).duration ) >= event.endTime ){
				return TIMER_FULL;
			}
			if( Setting_ShowPartialTimers ){
				// Check for partial cover
				if( 
					// Start overlap
					(Recording_GetRecInfo(1).startTime <= event.startTime &&
					AddTime(Recording_GetRecInfo(1).startTime,Recording_GetRecInfo(1).duration ) >= event.startTime )
					||
					// End overlap
					(Recording_GetRecInfo(1).startTime <= event.endTime &&
					AddTime(Recording_GetRecInfo(1).startTime,Recording_GetRecInfo(1).duration ) >= event.endTime )){
					return TIMER_PARTIAL;
				}
				if( Recording_GetRecInfo(1).startTime >= event.startTime &&
					AddTime(Recording_GetRecInfo(1).startTime,Recording_GetRecInfo(1).duration ) <= event.endTime ){
					return TIMER_PARTIAL;
				}
			}
		}
	}
	return TIMER_NONE;
}


char * Timers_GetTimerName( int row ){
	TYPE_TimerInfo	currentTimer;
	int totalTimers = 0;
	int i, x;
	word year;
	byte month, day, weekDay;
	TYPE_TapChInfo	currentChInfo;
	TYPE_TapEvent event;
	word year2;
	byte month2, day2, weekDay2;
	int result;
	static char name[128];
	memset( name, '\0', sizeof(name) );
	event = Events_GetEvent(row);
	TAP_ExtractMjd( event.startTime>>16, &year2, &month2, &day2, &weekDay2);
	totalTimers = TAP_Timer_GetTotalNum();
	for ( i = 0; i < totalTimers; i++ ){
		TAP_Timer_GetInfo(i, &currentTimer); // Sequentially get timer details and compare with those in the display range
		TAP_ExtractMjd( currentTimer.startTime>>16, &year, &month, &day, &weekDay);
		TAP_Channel_GetInfo( currentTimer.svcType, currentTimer.svcNum, &currentChInfo );
		switch( currentTimer.reservationType ){
			case RESERVE_TYPE_Onetime:
				if( (result=Timer_TimerFind( currentChInfo.svcId, Events_GetEventSvcId(row), currentTimer.startTime, AddTime(currentTimer.startTime, currentTimer.duration), event.startTime, event.endTime )) != TIMER_NONE ){
					strcpy( name, currentTimer.fileName );
					return name;
				}
				break;
			case RESERVE_TYPE_Everyday:
				// Initial check to determine against currentTimer start
				if( (result=Timer_TimerFind( currentChInfo.svcId, Events_GetEventSvcId(row), currentTimer.startTime, AddTime(currentTimer.startTime, currentTimer.duration), event.startTime, event.endTime )) != TIMER_NONE ){
					strcpy( name, currentTimer.fileName );
					return name;
				}
				// Not found on today so we need to go and increment for the next 7 days and check on weekdays
				for( x = 0; x < TIMERS_MAXDAYSAHEAD; x++ ){
					currentTimer.startTime = AddTime( currentTimer.startTime, 1440 );
					if( (result=Timer_TimerFind( currentChInfo.svcId, Events_GetEventSvcId(row), currentTimer.startTime, AddTime(currentTimer.startTime, currentTimer.duration), event.startTime, event.endTime )) != TIMER_NONE ){
						strcpy( name, currentTimer.fileName );
						return name;
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
							strcpy( name, currentTimer.fileName );
							return name;
						}
					}
					// Not found on today so we need to go and increment for the next 7 days and check on weekdays
					for( x = 0; x < TIMERS_MAXDAYSAHEAD; x++ ){
						currentTimer.startTime = AddTime( currentTimer.startTime, 1440 );
						TAP_ExtractMjd( currentTimer.startTime>>16, &year, &month, &day, &weekDay);
						if( weekDay == SAT || weekDay == SUN  ){	// Sanity check to make sure timer is on a weekday
							if( (result=Timer_TimerFind( currentChInfo.svcId, Events_GetEventSvcId(row), currentTimer.startTime, AddTime(currentTimer.startTime, currentTimer.duration), event.startTime, event.endTime )) != TIMER_NONE ){
								strcpy( name, currentTimer.fileName );
								return name;
							}
						}
					}
				}
				break;
			case RESERVE_TYPE_Weekly:
				if( (result=Timer_TimerFind( currentChInfo.svcId, Events_GetEventSvcId(row), currentTimer.startTime, AddTime(currentTimer.startTime, currentTimer.duration), event.startTime, event.endTime )) != TIMER_NONE ){
					strcpy( name, currentTimer.fileName );
					return name;
				}
				for( x = 0; x < 7; x++ ){	// AddTime only handle 1440 max so loop the adding of the time
					currentTimer.startTime = AddTime(currentTimer.startTime, 1440 );
				}
				// Only search a week ahead atm as data shouldn't be anymore than that
				if( (result=Timer_TimerFind( currentChInfo.svcId, Events_GetEventSvcId(row), currentTimer.startTime, AddTime(currentTimer.startTime, currentTimer.duration), event.startTime, event.endTime )) != TIMER_NONE ){
					strcpy( name, currentTimer.fileName );
					return name;
				}
				break;
			case RESERVE_TYPE_WeekDay:
				// if event is not on the weekend we need to check every week day from the start to determine
				// whether event has timer
				if( weekDay2 != SAT && weekDay2 != SUN ){
					// Initial check to determine against currentTimer start
					if( weekDay != SAT && weekDay != SUN  ){	// Sanity check to make sure timer is on a weekday
						if( (result=Timer_TimerFind( currentChInfo.svcId, Events_GetEventSvcId(row), currentTimer.startTime, AddTime(currentTimer.startTime, currentTimer.duration), event.startTime, event.endTime )) != TIMER_NONE ){
							strcpy( name, currentTimer.fileName );
							return name;
						}
					}
					// Not found on today so we need to go and increment for the next 7 days and check on weekdays
					for( x = 0; x < TIMERS_MAXDAYSAHEAD; x++ ){
						currentTimer.startTime = AddTime( currentTimer.startTime, 1440 );
						TAP_ExtractMjd( currentTimer.startTime>>16, &year, &month, &day, &weekDay);
						if( weekDay != SAT && weekDay != SUN  ){	// Sanity check to make sure timer is on a weekday
							if( (result=Timer_TimerFind( currentChInfo.svcId, Events_GetEventSvcId(row), currentTimer.startTime, AddTime(currentTimer.startTime, currentTimer.duration), event.startTime, event.endTime )) != TIMER_NONE ){
								strcpy( name, currentTimer.fileName );
								return name;
							}
						}
					}
				}
				break;
		}
	}
	return name;
}

char * Timers_GetNeighbourName( TYPE_TimerInfo timer ){
	TYPE_TimerInfo	currentTimer;
	int totalTimers = 0;
	int i, x;
	word year;
	byte month, day, weekDay;
	word year2;
	byte month2, day2, weekDay2;
	TYPE_TapChInfo	currentChInfo;
	static char name[128];
	int result;
	memset( name, '\0', sizeof(name) );
	// Manually search for overlap timers. Will only be 2 timers as API would have prevented any more
	totalTimers = TAP_Timer_GetTotalNum();
	TAP_ExtractMjd( timer.startTime>>16, &year2, &month2, &day2, &weekDay2);
	for ( i = 0; i < totalTimers; i++ ){
		TAP_Timer_GetInfo(i, &currentTimer); // Sequentially get timer details and compare with those in the display range
		TAP_ExtractMjd( currentTimer.startTime>>16, &year, &month, &day, &weekDay);
		TAP_Channel_GetInfo( currentTimer.svcType, currentTimer.svcNum, &currentChInfo );
		switch( currentTimer.reservationType ){
			case RESERVE_TYPE_Onetime:
				if( (result=Timer_TimerFind( currentTimer.svcNum, timer.svcNum, currentTimer.startTime, AddTime(currentTimer.startTime, currentTimer.duration), timer.startTime, AddTime(timer.startTime, timer.duration) )) != TIMER_NONE ){
					strcpy( name, currentTimer.fileName );
					return name;
				}
				break;
			case RESERVE_TYPE_Everyday:
				// Initial check to determine against currentTimer start
				if( (result=Timer_TimerFind( currentTimer.svcNum, timer.svcNum, currentTimer.startTime, AddTime(currentTimer.startTime, currentTimer.duration), timer.startTime, AddTime(timer.startTime, timer.duration) )) != TIMER_NONE ){
					strcpy( name, currentTimer.fileName );
					return name;
				}
				// Not found on today so we need to go and increment for the next 7 days and check on weekdays
				for( x = 0; x < TIMERS_MAXDAYSAHEAD; x++ ){
					currentTimer.startTime = AddTime( currentTimer.startTime, 1440 );
					if( (result=Timer_TimerFind( currentTimer.svcNum, timer.svcNum, currentTimer.startTime, AddTime(currentTimer.startTime, currentTimer.duration), timer.startTime, AddTime(timer.startTime, timer.duration) )) != TIMER_NONE ){
						strcpy( name, currentTimer.fileName );
						return name;
					}
				}
				break;
			case RESERVE_TYPE_EveryWeekend:
				// if event is not on the weekend we need to check every week day from the start to determine
				// whether event has timer
				if( weekDay2 == SAT || weekDay2 == SUN ){
					// Initial check to determine against currentTimer start
					if( weekDay == SAT || weekDay == SUN  ){	// Sanity check to make sure timer is on a weekend
						if( (result=Timer_TimerFind( currentTimer.svcNum, timer.svcNum, currentTimer.startTime, AddTime(currentTimer.startTime, currentTimer.duration), timer.startTime, AddTime(timer.startTime, timer.duration) )) != TIMER_NONE ){
							strcpy( name, currentTimer.fileName );
							return name;
						}
					}
					// Not found on today so we need to go and increment for the next 7 days and check on weekdays
					for( x = 0; x < TIMERS_MAXDAYSAHEAD; x++ ){
						currentTimer.startTime = AddTime( currentTimer.startTime, 1440 );
						TAP_ExtractMjd( currentTimer.startTime>>16, &year, &month, &day, &weekDay);
						if( weekDay == SAT || weekDay == SUN  ){	// Sanity check to make sure timer is on a weekday
							if( (result=Timer_TimerFind( currentTimer.svcNum, timer.svcNum, currentTimer.startTime, AddTime(currentTimer.startTime, currentTimer.duration), timer.startTime, AddTime(timer.startTime, timer.duration) )) != TIMER_NONE ){
								strcpy( name, currentTimer.fileName );
								return name;
							}
						}
					}
				}
				break;
			case RESERVE_TYPE_Weekly:
				if( (result=Timer_TimerFind( currentTimer.svcNum, timer.svcNum, currentTimer.startTime, AddTime(currentTimer.startTime, currentTimer.duration), timer.startTime, AddTime(timer.startTime, timer.duration) )) != TIMER_NONE ){
					strcpy( name, currentTimer.fileName );
					return name;
				}
				for( x = 0; x < 7; x++ ){	// AddTime only handle 1440 max so loop the adding of the time
					currentTimer.startTime = AddTime(currentTimer.startTime, 1440 );
				}
				// Only search a week ahead atm as data shouldn't be anymore than that
				if( (result=Timer_TimerFind( currentTimer.svcNum, timer.svcNum, currentTimer.startTime, AddTime(currentTimer.startTime, currentTimer.duration), timer.startTime, AddTime(timer.startTime, timer.duration) )) != TIMER_NONE ){
					strcpy( name, currentTimer.fileName );
					return name;
				}
				break;
			case RESERVE_TYPE_WeekDay:
				// if event is not on the weekend we need to check every week day from the start to determine
				// whether event has timer
				if( weekDay2 != SAT && weekDay2 != SUN ){
					// Initial check to determine against currentTimer start
					if( weekDay != SAT && weekDay != SUN  ){	// Sanity check to make sure timer is on a weekday
						if( (result=Timer_TimerFind( currentTimer.svcNum, timer.svcNum, currentTimer.startTime, AddTime(currentTimer.startTime, currentTimer.duration), timer.startTime, AddTime(timer.startTime, timer.duration) )) != TIMER_NONE ){
							strcpy( name, currentTimer.fileName );
							return name;
						}
					}
					// Not found on today so we need to go and increment for the next 7 days and check on weekdays
					for( x = 0; x < TIMERS_MAXDAYSAHEAD; x++ ){
						currentTimer.startTime = AddTime( currentTimer.startTime, 1440 );
						TAP_ExtractMjd( currentTimer.startTime>>16, &year, &month, &day, &weekDay);
						if( weekDay != SAT && weekDay != SUN  ){	// Sanity check to make sure timer is on a weekday
							if( (result=Timer_TimerFind( currentTimer.svcNum, timer.svcNum, currentTimer.startTime, AddTime(currentTimer.startTime, currentTimer.duration), timer.startTime, AddTime(timer.startTime, timer.duration) )) != TIMER_NONE ){
								strcpy( name, currentTimer.fileName );
								return name;
							}
						}
					}
				}
				break;
		}
	}
//	for ( i = 0; i < totalTimers; i++ ){
//		TAP_Timer_GetInfo(i, &currentTimer); // Sequentially get timer details and compare with those in the display range
//		TAP_ExtractMjd( currentTimer.startTime>>16, &year, &month, &day, &weekDay);
//		TAP_Channel_GetInfo( currentTimer.svcType, currentTimer.svcNum, &currentChInfo );
//		if( currentTimer.svcNum == timer.svcNum ){	// If they are on the same channel then they don't overlap
//			if( 
//			// [ currentTimer ]
//			//       [ SetAttempt ]
//				(timer.startTime >= currentTimer.startTime && 
//				timer.startTime <= AddTime(currentTimer.startTime, currentTimer.duration))
//				||
//			//               [ Timer ]
//			// [ Attempted timer ]
//				( timer.startTime <= currentTimer.startTime &&
//				AddTime(timer.startTime, timer.duration) >= currentTimer.startTime)
//				||
//			// [        Timer        ]
//			//   [ Attempted timer ]
//				( timer.startTime >= currentTimer.startTime &&
//				AddTime(timer.startTime, timer.duration) <= AddTime(currentTimer.startTime, currentTimer.duration))
//			){
//				strcpy( name, currentTimer.fileName );
//				return name;
//			}
//		}
//	}
	return name;
}

int Timers_GetNumber( TYPE_TimerInfo timer ){
	TYPE_TimerInfo	currentTimer;
	int totalTimers = 0;
	int i, x;
	word year;
	byte month, day, weekDay;
	TYPE_TapChInfo	currentChInfo;
	word year2;
	byte month2, day2, weekDay2;
	int result;
	// Manually search for overlap timers. Will only be 2 timers as API would have prevented any more
	totalTimers = TAP_Timer_GetTotalNum();
	TAP_ExtractMjd( timer.startTime>>16, &year2, &month2, &day2, &weekDay2);
	for ( i = 0; i < totalTimers; i++ ){
		TAP_Timer_GetInfo(i, &currentTimer); // Sequentially get timer details and compare with those in the display range
		TAP_ExtractMjd( currentTimer.startTime>>16, &year, &month, &day, &weekDay);
		TAP_Channel_GetInfo( currentTimer.svcType, currentTimer.svcNum, &currentChInfo );
		switch( currentTimer.reservationType ){
			case RESERVE_TYPE_Onetime:
				if( (result=Timer_TimerFind( currentTimer.svcNum, timer.svcNum, currentTimer.startTime, AddTime(currentTimer.startTime, currentTimer.duration), timer.startTime, AddTime(timer.startTime, timer.duration) )) == TIMER_FULL ){
					return i;
				}
				break;
			case RESERVE_TYPE_Everyday:
				// Initial check to determine against currentTimer start
				if( (result=Timer_TimerFind( currentTimer.svcNum, timer.svcNum, currentTimer.startTime, AddTime(currentTimer.startTime, currentTimer.duration), timer.startTime, AddTime(timer.startTime, timer.duration) )) == TIMER_FULL ){
					return i;
				}
				// Not found on today so we need to go and increment for the next 7 days and check on weekdays
				for( x = 0; x < TIMERS_MAXDAYSAHEAD; x++ ){
					currentTimer.startTime = AddTime( currentTimer.startTime, 1440 );
					if( (result=Timer_TimerFind( currentTimer.svcNum, timer.svcNum, currentTimer.startTime, AddTime(currentTimer.startTime, currentTimer.duration), timer.startTime, AddTime(timer.startTime, timer.duration) )) == TIMER_FULL ){
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
						if( (result=Timer_TimerFind( currentTimer.svcNum, timer.svcNum, currentTimer.startTime, AddTime(currentTimer.startTime, currentTimer.duration), timer.startTime, AddTime(timer.startTime, timer.duration) )) == TIMER_FULL ){
							return i;
						}
					}
					// Not found on today so we need to go and increment for the next 7 days and check on weekdays
					for( x = 0; x < TIMERS_MAXDAYSAHEAD; x++ ){
						currentTimer.startTime = AddTime( currentTimer.startTime, 1440 );
						TAP_ExtractMjd( currentTimer.startTime>>16, &year, &month, &day, &weekDay);
						if( weekDay == SAT || weekDay == SUN  ){	// Sanity check to make sure timer is on a weekday
							if( (result=Timer_TimerFind( currentTimer.svcNum, timer.svcNum, currentTimer.startTime, AddTime(currentTimer.startTime, currentTimer.duration), timer.startTime, AddTime(timer.startTime, timer.duration) )) == TIMER_FULL ){
								return i;
							}
						}
					}
				}
				break;
			case RESERVE_TYPE_Weekly:
				if( (result=Timer_TimerFind( currentTimer.svcNum, timer.svcNum, currentTimer.startTime, AddTime(currentTimer.startTime, currentTimer.duration), timer.startTime, AddTime(timer.startTime, timer.duration) )) == TIMER_FULL ){
					return i;
				}
				for( x = 0; x < 7; x++ ){	// AddTime only handle 1440 max so loop the adding of the time
					currentTimer.startTime = AddTime(currentTimer.startTime, 1440 );
				}
				// Only search a week ahead atm as data shouldn't be anymore than that
				if( (result=Timer_TimerFind( currentTimer.svcNum, timer.svcNum, currentTimer.startTime, AddTime(currentTimer.startTime, currentTimer.duration), timer.startTime, AddTime(timer.startTime, timer.duration) )) == TIMER_FULL ){
					return i;
				}
				break;
			case RESERVE_TYPE_WeekDay:
				// if event is not on the weekend we need to check every week day from the start to determine
				// whether event has timer
				if( weekDay2 != SAT && weekDay2 != SUN ){
					// Initial check to determine against currentTimer start
					if( weekDay != SAT && weekDay != SUN  ){	// Sanity check to make sure timer is on a weekday
						if( (result=Timer_TimerFind( currentTimer.svcNum, timer.svcNum, currentTimer.startTime, AddTime(currentTimer.startTime, currentTimer.duration), timer.startTime, AddTime(timer.startTime, timer.duration) )) == TIMER_FULL ){
							return i;
						}
					}
					// Not found on today so we need to go and increment for the next 7 days and check on weekdays
					for( x = 0; x < TIMERS_MAXDAYSAHEAD; x++ ){
						currentTimer.startTime = AddTime( currentTimer.startTime, 1440 );
						TAP_ExtractMjd( currentTimer.startTime>>16, &year, &month, &day, &weekDay);
						if( weekDay != SAT && weekDay != SUN  ){	// Sanity check to make sure timer is on a weekday
							if( (result=Timer_TimerFind( currentTimer.svcNum, timer.svcNum, currentTimer.startTime, AddTime(currentTimer.startTime, currentTimer.duration), timer.startTime, AddTime(timer.startTime, timer.duration) )) == TIMER_FULL ){
								return i;
							}
						}
					}
				}
				break;
		}
	}
//	for ( i = 0; i < totalTimers; i++ ){
//		TAP_Timer_GetInfo(i, &currentTimer); // Sequentially get timer details and compare with those in the display range
//		TAP_ExtractMjd( currentTimer.startTime>>16, &year, &month, &day, &weekDay);
//		TAP_Channel_GetInfo( currentTimer.svcType, currentTimer.svcNum, &currentChInfo );
//		if( currentTimer.svcNum == timer.svcNum ){	// If they are on the same channel then they don't overlap
//			if( 	// Same channel and start and duration implies we found the timer
//				timer.startTime == currentTimer.startTime && 
//				timer.duration == currentTimer.duration
//			){
//				return i;
//			}
//		}
//	}
	return -1;
}

int Timers_GetNeighbour( TYPE_TimerInfo timer ){
	TYPE_TimerInfo	currentTimer;
	int totalTimers = 0;
	int i, x;
	word year;
	byte month, day, weekDay;
	word year2;
	byte month2, day2, weekDay2;
	int result;
	TYPE_TapChInfo	currentChInfo;
	// Manually search for overlap timers. Will only be 2 timers as API would have prevented any more
	totalTimers = TAP_Timer_GetTotalNum();
	Update_RecordingInfo(0);
	Update_RecordingInfo(1);
	TAP_ExtractMjd( timer.startTime>>16, &year2, &month2, &day2, &weekDay2);
	for ( i = 0; i < totalTimers; i++ ){
		TAP_Timer_GetInfo(i, &currentTimer); // Sequentially get timer details and compare with those in the display range
		TAP_ExtractMjd( currentTimer.startTime>>16, &year, &month, &day, &weekDay);
		TAP_Channel_GetInfo( currentTimer.svcType, currentTimer.svcNum, &currentChInfo );
		switch( currentTimer.reservationType ){
			case RESERVE_TYPE_Onetime:
				if( (result=Timer_TimerFind( currentTimer.svcNum, timer.svcNum, currentTimer.startTime, AddTime(currentTimer.startTime, currentTimer.duration), timer.startTime, AddTime(timer.startTime, timer.duration) )) == TIMER_PARTIAL ){
					return i;
				}
				break;
			case RESERVE_TYPE_Everyday:
				// Initial check to determine against currentTimer start
				if( (result=Timer_TimerFind( currentTimer.svcNum, timer.svcNum, currentTimer.startTime, AddTime(currentTimer.startTime, currentTimer.duration), timer.startTime, AddTime(timer.startTime, timer.duration) )) == TIMER_PARTIAL ){
					return i;
				}
				// Not found on today so we need to go and increment for the next 7 days and check on weekdays
				for( x = 0; x < TIMERS_MAXDAYSAHEAD; x++ ){
					currentTimer.startTime = AddTime( currentTimer.startTime, 1440 );
					if( (result=Timer_TimerFind( currentTimer.svcNum, timer.svcNum, currentTimer.startTime, AddTime(currentTimer.startTime, currentTimer.duration), timer.startTime, AddTime(timer.startTime, timer.duration) )) == TIMER_PARTIAL ){
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
						if( (result=Timer_TimerFind( currentTimer.svcNum, timer.svcNum, currentTimer.startTime, AddTime(currentTimer.startTime, currentTimer.duration), timer.startTime, AddTime(timer.startTime, timer.duration) )) == TIMER_PARTIAL ){
							return i;
						}
					}
					// Not found on today so we need to go and increment for the next 7 days and check on weekdays
					for( x = 0; x < TIMERS_MAXDAYSAHEAD; x++ ){
						currentTimer.startTime = AddTime( currentTimer.startTime, 1440 );
						TAP_ExtractMjd( currentTimer.startTime>>16, &year, &month, &day, &weekDay);
						if( weekDay == SAT || weekDay == SUN  ){	// Sanity check to make sure timer is on a weekday
							if( (result=Timer_TimerFind( currentTimer.svcNum, timer.svcNum, currentTimer.startTime, AddTime(currentTimer.startTime, currentTimer.duration), timer.startTime, AddTime(timer.startTime, timer.duration) )) == TIMER_PARTIAL ){
								return i;
							}
						}
					}
				}
				break;
			case RESERVE_TYPE_Weekly:
				if( (result=Timer_TimerFind( currentTimer.svcNum, timer.svcNum, currentTimer.startTime, AddTime(currentTimer.startTime, currentTimer.duration), timer.startTime, AddTime(timer.startTime, timer.duration) )) == TIMER_PARTIAL ){
					return i;
				}
				for( x = 0; x < 7; x++ ){	// AddTime only handle 1440 max so loop the adding of the time
					currentTimer.startTime = AddTime(currentTimer.startTime, 1440 );
				}
				// Only search a week ahead atm as data shouldn't be anymore than that
				if( (result=Timer_TimerFind( currentTimer.svcNum, timer.svcNum, currentTimer.startTime, AddTime(currentTimer.startTime, currentTimer.duration), timer.startTime, AddTime(timer.startTime, timer.duration) )) == TIMER_PARTIAL ){
					return i;
				}
				break;
			case RESERVE_TYPE_WeekDay:
				// if event is not on the weekend we need to check every week day from the start to determine
				// whether event has timer
				if( weekDay2 != SAT && weekDay2 != SUN ){
					// Initial check to determine against currentTimer start
					if( weekDay != SAT && weekDay != SUN  ){	// Sanity check to make sure timer is on a weekday
						if( (result=Timer_TimerFind( currentTimer.svcNum, timer.svcNum, currentTimer.startTime, AddTime(currentTimer.startTime, currentTimer.duration), timer.startTime, AddTime(timer.startTime, timer.duration) )) == TIMER_PARTIAL ){
							return i;
						}
					}
					// Not found on today so we need to go and increment for the next 7 days and check on weekdays
					for( x = 0; x < TIMERS_MAXDAYSAHEAD; x++ ){
						currentTimer.startTime = AddTime( currentTimer.startTime, 1440 );
						TAP_ExtractMjd( currentTimer.startTime>>16, &year, &month, &day, &weekDay);
						if( weekDay != SAT && weekDay != SUN  ){	// Sanity check to make sure timer is on a weekday
							if( (result=Timer_TimerFind( currentTimer.svcNum, timer.svcNum, currentTimer.startTime, AddTime(currentTimer.startTime, currentTimer.duration), timer.startTime, AddTime(timer.startTime, timer.duration) )) == TIMER_PARTIAL ){
								return i;
							}
						}
					}
				}
				break;
		}
	}
//	for ( i = 0; i < totalTimers; i++ ){
//		TAP_Timer_GetInfo(i, &currentTimer); // Sequentially get timer details and compare with those in the display range
//		TAP_ExtractMjd( currentTimer.startTime>>16, &year, &month, &day, &weekDay);
//		TAP_Channel_GetInfo( currentTimer.svcType, currentTimer.svcNum, &currentChInfo );
//		if( currentTimer.svcNum == timer.svcNum ){	// If they are on the same channel then they don't overlap
//			if( 
//			// [ currentTimer ]
//			//       [ SetAttempt ]
//				(timer.startTime >= currentTimer.startTime && 
//				timer.startTime <= AddTime(currentTimer.startTime, currentTimer.duration))
//				||
//			//               [ Timer ]
//			// [ Attempted timer ]
//				( timer.startTime <= currentTimer.startTime &&
//				AddTime(timer.startTime, timer.duration) >= currentTimer.startTime)
//				||
//			// [        Timer        ]
//			//   [ Attempted timer ]
//				( timer.startTime >= currentTimer.startTime &&
//				AddTime(timer.startTime, timer.duration) <= AddTime(currentTimer.startTime, currentTimer.duration))
//			){
//				return i;
//			}
//		}
//	}
	return -1;
}

int Timers_GetNeighbourWithNum( int timerNum, TYPE_TimerInfo timer ){
	TYPE_TimerInfo	currentTimer;
	int totalTimers = 0;
	int i, x;
	word year;
	byte month, day, weekDay;
	word year2;
	byte month2, day2, weekDay2;
	int result;
	TYPE_TapChInfo	currentChInfo;
	// Manually search for overlap timers. Will only be 2 timers as API would have prevented any more
	totalTimers = TAP_Timer_GetTotalNum();
	TAP_ExtractMjd( timer.startTime>>16, &year2, &month2, &day2, &weekDay2);
	for ( i = 0; i < totalTimers; i++ ){
		TAP_Timer_GetInfo(i, &currentTimer); // Sequentially get timer details and compare with those in the display range
		TAP_ExtractMjd( currentTimer.startTime>>16, &year, &month, &day, &weekDay);
		TAP_Channel_GetInfo( currentTimer.svcType, currentTimer.svcNum, &currentChInfo );
		switch( currentTimer.reservationType ){
			case RESERVE_TYPE_Onetime:
				if( (result=Timer_TimerFind( currentTimer.svcNum, timer.svcNum, currentTimer.startTime, AddTime(currentTimer.startTime, currentTimer.duration), timer.startTime, AddTime(timer.startTime, timer.duration) )) == TIMER_PARTIAL ){
					if( i != timerNum ){	// Don't point to same item
						return i;
					}
				}
				break;
			case RESERVE_TYPE_Everyday:
				// Initial check to determine against currentTimer start
				if( (result=Timer_TimerFind( currentTimer.svcNum, timer.svcNum, currentTimer.startTime, AddTime(currentTimer.startTime, currentTimer.duration), timer.startTime, AddTime(timer.startTime, timer.duration) )) == TIMER_PARTIAL ){
					if( i != timerNum ){	// Don't point to same item
						return i;
					}
				}
				// Not found on today so we need to go and increment for the next 7 days and check on weekdays
				for( x = 0; x < TIMERS_MAXDAYSAHEAD; x++ ){
					currentTimer.startTime = AddTime( currentTimer.startTime, 1440 );
					if( (result=Timer_TimerFind( currentTimer.svcNum, timer.svcNum, currentTimer.startTime, AddTime(currentTimer.startTime, currentTimer.duration), timer.startTime, AddTime(timer.startTime, timer.duration) )) == TIMER_PARTIAL ){
						if( i != timerNum ){	// Don't point to same item
							return i;
						}
					}
				}
				break;
			case RESERVE_TYPE_EveryWeekend:
				// if event is not on the weekend we need to check every week day from the start to determine
				// whether event has timer
				if( weekDay2 == SAT || weekDay2 == SUN ){
					// Initial check to determine against currentTimer start
					if( weekDay == SAT || weekDay == SUN  ){	// Sanity check to make sure timer is on a weekend
						if( (result=Timer_TimerFind( currentTimer.svcNum, timer.svcNum, currentTimer.startTime, AddTime(currentTimer.startTime, currentTimer.duration), timer.startTime, AddTime(timer.startTime, timer.duration) )) == TIMER_PARTIAL ){
							if( i != timerNum ){	// Don't point to same item
								return i;
							}
						}
					}
					// Not found on today so we need to go and increment for the next 7 days and check on weekdays
					for( x = 0; x < TIMERS_MAXDAYSAHEAD; x++ ){
						currentTimer.startTime = AddTime( currentTimer.startTime, 1440 );
						TAP_ExtractMjd( currentTimer.startTime>>16, &year, &month, &day, &weekDay);
						if( weekDay == SAT || weekDay == SUN  ){	// Sanity check to make sure timer is on a weekday
							if( (result=Timer_TimerFind( currentTimer.svcNum, timer.svcNum, currentTimer.startTime, AddTime(currentTimer.startTime, currentTimer.duration), timer.startTime, AddTime(timer.startTime, timer.duration) )) == TIMER_PARTIAL ){
								if( i != timerNum ){	// Don't point to same item
									return i;
								}
							}
						}
					}
				}
				break;
			case RESERVE_TYPE_Weekly:
				if( (result=Timer_TimerFind( currentTimer.svcNum, timer.svcNum, currentTimer.startTime, AddTime(currentTimer.startTime, currentTimer.duration), timer.startTime, AddTime(timer.startTime, timer.duration) )) == TIMER_PARTIAL ){
					if( i != timerNum ){	// Don't point to same item
						return i;
					}
				}
				for( x = 0; x < 7; x++ ){	// AddTime only handle 1440 max so loop the adding of the time
					currentTimer.startTime = AddTime(currentTimer.startTime, 1440 );
				}
				// Only search a week ahead atm as data shouldn't be anymore than that
				if( (result=Timer_TimerFind( currentTimer.svcNum, timer.svcNum, currentTimer.startTime, AddTime(currentTimer.startTime, currentTimer.duration), timer.startTime, AddTime(timer.startTime, timer.duration) )) == TIMER_PARTIAL ){
					if( i != timerNum ){	// Don't point to same item
						return i;
					}
				}
				break;
			case RESERVE_TYPE_WeekDay:
				// if event is not on the weekend we need to check every week day from the start to determine
				// whether event has timer
				if( weekDay2 != SAT && weekDay2 != SUN ){
					// Initial check to determine against currentTimer start
					if( weekDay != SAT && weekDay != SUN  ){	// Sanity check to make sure timer is on a weekday
						if( (result=Timer_TimerFind( currentTimer.svcNum, timer.svcNum, currentTimer.startTime, AddTime(currentTimer.startTime, currentTimer.duration), timer.startTime, AddTime(timer.startTime, timer.duration) )) == TIMER_PARTIAL ){
							if( i != timerNum ){	// Don't point to same item
								return i;
							}
						}
					}
					// Not found on today so we need to go and increment for the next 7 days and check on weekdays
					for( x = 0; x < TIMERS_MAXDAYSAHEAD; x++ ){
						currentTimer.startTime = AddTime( currentTimer.startTime, 1440 );
						TAP_ExtractMjd( currentTimer.startTime>>16, &year, &month, &day, &weekDay);
						if( weekDay != SAT && weekDay != SUN  ){	// Sanity check to make sure timer is on a weekday
							if( (result=Timer_TimerFind( currentTimer.svcNum, timer.svcNum, currentTimer.startTime, AddTime(currentTimer.startTime, currentTimer.duration), timer.startTime, AddTime(timer.startTime, timer.duration) )) == TIMER_PARTIAL ){
								if( i != timerNum ){	// Don't point to same item
									return i;
								}
							}
						}
					}
				}
				break;
		}
	}
	
//	for ( i = 0; i < totalTimers; i++ ){
//		TAP_Timer_GetInfo(i, &currentTimer); // Sequentially get timer details and compare with those in the display range
//		TAP_ExtractMjd( currentTimer.startTime>>16, &year, &month, &day, &weekDay);
//		TAP_Channel_GetInfo( currentTimer.svcType, currentTimer.svcNum, &currentChInfo );
//		if( currentTimer.svcNum == timer.svcNum ){	// If they are on the same channel then they don't overlap
//			if( 
//			// [ currentTimer ]
//			//       [ SetAttempt ]
//				(timer.startTime >= currentTimer.startTime && 
//				timer.startTime <= AddTime(currentTimer.startTime, currentTimer.duration))
//				||
//			//               [ Timer ]
//			// [ Attempted timer ]
//				( timer.startTime <= currentTimer.startTime &&
//				AddTime(timer.startTime, timer.duration) >= currentTimer.startTime)
//				||
//			// [        Timer        ]
//			//   [ Attempted timer ]
//				( timer.startTime >= currentTimer.startTime &&
//				AddTime(timer.startTime, timer.duration) <= AddTime(currentTimer.startTime, currentTimer.duration))
//			){
//				if( i != timerNum ){	// Don't point to same item
//					return i;
//				}
//			}
//		}
//	}
	return -1;
}

char * Timers_BuildTimerName( TYPE_TimerInfo timer ){
	static char name[45];
	bool foundFirst = FALSE;
	int i,j;
	word timerSvcId = 0;
	TYPE_TapEvent timerevents[5];	// So the TAP events so we can sort them before appending to the name
	int timerevent_ptr = 0;
	TYPE_TapEvent tmp;
	memset( name, '\0', sizeof(name) );
	timerSvcId = GetSvcId(timer.svcType, timer.svcNum);	// Get the svcId for the timer
	// Lets loop thru all events in the events array and build up the name of the timer
	for( i = 0; i < eventsPointer; i++ ){
		if( Events_GetEventSvcId(i) == timerSvcId ){	// Same service so check the times
			// Check when time fully covers the event if so store in array being sure to not overflow
			if( timer.startTime <= Events_GetEventStartTime(i) &&
				AddTime(timer.startTime, timer.duration) >= Events_GetEventEndTime(i) ){
				if( timerevent_ptr < sizeof(timerevents) ){
					timerevents[timerevent_ptr] = Events_GetEvent(i);
					timerevent_ptr++;
				} else {
					// Have our maximum so break out of the loop
					i = eventsPointer;
				}
			}
		}
	}
	// Sort out timerevents by start times to get the names in the right order
	// Quick bubble sort
	for( i = 0; i < timerevent_ptr; i++ ){
		for( j = 0; j < timerevent_ptr-1; j++ ){
			if( i != j ){
				if( timerevents[i].startTime < timerevents[j].startTime ){
					tmp = timerevents[i];
					timerevents[i] = timerevents[j];
					timerevents[j] = tmp;
				}
			}
		}
	}
	if( timerevent_ptr > 0 ){
		if( timerevent_ptr > 1 ){
			// Lets start the building of the timer name
			for( i = 0; i < timerevent_ptr; i++ ){
				if( strlen(name)+strlen(timerevents[i].eventName) < sizeof(name)-3 ){	// Make sure we dont overflow so check that the length will be less then the size of the array
					if( foundFirst ){
						strcat(name," + ");
						strcat(name,timerevents[i].eventName);
						foundFirst = TRUE;
					} else {
						strcat(name,timerevents[i].eventName);
						foundFirst = TRUE;
					}
				} else {
					// To append the full eventName will overflow the array so append the max numbers and break the loop
					strcat(name," + ");
					strncat(name,timerevents[i].eventName,(sizeof(name))-strlen(name));
					i = timerevent_ptr;
				}
			}
		} else {
			if( strlen(name)+strlen(timerevents[i].eventName) < sizeof(name)-3 ){	// Make sure we dont overflow so check that the length will be less then the size of the array
				strcat(name,timerevents[0].eventName);
			} else {
				// To append the full eventName will overflow the array so append the max numbers and break the loop
				strncat(name,timerevents[0].eventName,(sizeof(name))-strlen(name));
			}
		}
	} else {
		j = -1;
		for( i = 0; i < eventsPointer; i++ ){
			if( Events_GetEventSvcId(i) == timerSvcId ){	// Same service so check the times
				// Check when time fully covers the event if so store in array being sure to not overflow
				if( timer.startTime >= Events_GetEventStartTime(i) &&
					AddTime(timer.startTime, timer.duration) <= Events_GetEventEndTime(i) ){
					i = eventsPointer;
					j = i;
				}
			}
		}
		if( j > -1 && j < eventsPointer ){
			// We have a prgram name so cat to the name otherwise name will be unknown
			strcat(name,Events_GetEventName(j));	// No program name so this is a orphan timer. Lets see what show we are and rename
		} else {
			strcat(name,"Unknown");	// Add .rec to the end of the timername
		}
	}
//	strcat(name,".rec");	// Add .rec to the end of the timername
	return name;
}
