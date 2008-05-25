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
#include "timers.h"
#include "timereditfull.h"
#include "timernew.h"
#include "skin.h"
#include "timerlist.h"


bool TimerList_IsActive( void ){
	return TimerList_Active;
}

void TimerList_Idle( void ){
	if( TimerList_IsActive() ){
		if( lastPress + OSD_RedrawDelay < TAP_GetTick() ){
			if( TimerList_LastRendered + OSD_RedrawDelay < TAP_GetTick() ){
				if( TimerList_TotalTimers != TAP_Timer_GetTotalNum() ){	// A timer has fired so reget the timers and redraw
					TimerList_GetTimers();
					TimerList_Min = 0;
					TimerList_Max = TimerList_OPTIONS;
					if( TimerList_Max > TimerList_Timers_ptr -1 ){
						TimerList_Max = TimerList_Timers_ptr;
					}
					TimerList_Redraw = TRUE;
					TimerList_Render();
				}
			}
		}
	}
}

// function used to hide TimerList
void TimerList_Hide( void ){
	TimerList_Active = FALSE;
	TimerList_Redraw = FALSE;
	TimerList_MemFree();
	Display_Hide();
}

void TimerList_MemFree( void ){
	if (TimerList_ItemBase) TAP_MemFree(TimerList_ItemBase);
	if (TimerList_ItemHigh) TAP_MemFree(TimerList_ItemHigh);
	TimerList_ItemBase = NULL;
	TimerList_ItemHigh = NULL;
}

void TimerList_CreateGradients( void ){
	word rgn;
	TimerList_MemFree();
	if( Settings_GradientFactor > 0 ){
		rgn	= TAP_Osd_Create(0, 0, DISPLAY_ROW_WIDTH, DISPLAY_ITEM_H, 0, OSD_Flag_MemRgn);
		// Main default item
		createGradient(rgn, DISPLAY_ROW_WIDTH, DISPLAY_ITEM_H, Settings_GradientFactor, DISPLAY_ITEM );
		TimerList_ItemBase = TAP_Osd_SaveBox(rgn, 0, 0, DISPLAY_ROW_WIDTH, DISPLAY_ITEM_H);
		createGradient(rgn, DISPLAY_ROW_WIDTH, DISPLAY_ITEM_H, Settings_GradientFactor, DISPLAY_ITEMSELECTED );
		TimerList_ItemHigh = TAP_Osd_SaveBox(rgn, 0, 0, DISPLAY_ROW_WIDTH, DISPLAY_ITEM_H);
		TAP_Osd_Delete(rgn);
	}
}

// Function to render the item in the array on the screen
void TimerList_RenderBlankItem( word rgn, int position ){
	int x_pos = 0;
	int y_pos = 0;
	// Sort out the X position
	x_pos = Panel_Left_X;
	// Sort out the row for the item
	y_pos = Panel_Left_Y + (( DISPLAY_ITEM_H + DISPLAY_GAP ) * (position));
	// Sanity Check
	if( y_pos > 575 ) return;
	if( x_pos > 719 ) return;
	if( y_pos < 0 ) return;
	if( x_pos < 0 ) return;
	// ALter the text colour if required
	if( TimerList_ItemBase != NULL ){
		TAP_Osd_RestoreBox( rgn, x_pos, y_pos, DISPLAY_ROW_WIDTH, DISPLAY_ITEM_H, TimerList_ItemBase );
	} else {
		TAP_Osd_FillBox(rgn, x_pos, y_pos, DISPLAY_ROW_WIDTH, DISPLAY_ITEM_H, DISPLAY_ITEM);
	}
	TAP_Osd_FillBox(rgn, x_pos+90, y_pos, DISPLAY_GAP, DISPLAY_ITEM_H, DISPLAY_MAIN);
	TAP_Osd_FillBox(rgn, x_pos+230, y_pos, DISPLAY_GAP, DISPLAY_ITEM_H, DISPLAY_MAIN);
	if( Settings_TimerListLogos ){
		TAP_Osd_FillBox(rgn, 720-(Panel_Left_X+DISPLAY_GAP-DISPLAY_X_OFFSET)-46+DISPLAY_GAP, y_pos, DISPLAY_GAP, DISPLAY_ITEM_H, DISPLAY_MAIN);
	}
}

void TimerList_RenderDetails( void ){
	int x_pos = 0;
	int y_pos = 0;
	char text[128];
	x_pos = Panel_Left_X;
	y_pos = Panel_Left_Y + (( DISPLAY_ITEM_H + DISPLAY_GAP ) * TimerList_OPTIONS);
	switch( TimerList_Timers[TimerList_Selected].reservationType ){
		case RESERVE_TYPE_Onetime:
			TAP_SPrint( text, "%s, %s on %s, %s ~ %s, %3d mins", Channel_Name(TimerList_Timers[TimerList_Selected].svcType,TimerList_Timers[TimerList_Selected].svcNum), TimerEdit_ReserveType(TimerList_Timers[TimerList_Selected].reservationType), Time_DOWDDMMSpec( TimerList_Timers[TimerList_Selected].startTime ), Time_HHMM(TimerList_Timers[TimerList_Selected].startTime), Endtime_HHMM(TimerList_Timers[TimerList_Selected].startTime,TimerList_Timers[TimerList_Selected].duration), TimerList_Timers[TimerList_Selected].duration);
			break;
		case RESERVE_TYPE_Everyday:
			TAP_SPrint( text, "%s, %s, %s ~ %s, %3d mins", Channel_Name(TimerList_Timers[TimerList_Selected].svcType,TimerList_Timers[TimerList_Selected].svcNum), TimerEdit_ReserveType(TimerList_Timers[TimerList_Selected].reservationType), Time_HHMM(TimerList_Timers[TimerList_Selected].startTime), Endtime_HHMM(TimerList_Timers[TimerList_Selected].startTime,TimerList_Timers[TimerList_Selected].duration), TimerList_Timers[TimerList_Selected].duration);
			break;
		case RESERVE_TYPE_EveryWeekend:
			TAP_SPrint( text, "%s, %s next %s, %s ~ %s, %3d mins", Channel_Name(TimerList_Timers[TimerList_Selected].svcType,TimerList_Timers[TimerList_Selected].svcNum), TimerEdit_ReserveType(TimerList_Timers[TimerList_Selected].reservationType), Time_DOWDDMMSpec( TimerList_Timers[TimerList_Selected].startTime ), Time_HHMM(TimerList_Timers[TimerList_Selected].startTime), Endtime_HHMM(TimerList_Timers[TimerList_Selected].startTime,TimerList_Timers[TimerList_Selected].duration), TimerList_Timers[TimerList_Selected].duration);
			break;
		case RESERVE_TYPE_Weekly:
			TAP_SPrint( text, "%s, %s on %s, %s ~ %s, %3d mins", Channel_Name(TimerList_Timers[TimerList_Selected].svcType,TimerList_Timers[TimerList_Selected].svcNum), TimerEdit_ReserveType(TimerList_Timers[TimerList_Selected].reservationType), Time_DOWDDMMSpec( TimerList_Timers[TimerList_Selected].startTime ), Time_HHMM(TimerList_Timers[TimerList_Selected].startTime), Endtime_HHMM(TimerList_Timers[TimerList_Selected].startTime,TimerList_Timers[TimerList_Selected].duration), TimerList_Timers[TimerList_Selected].duration);
			break;
		case RESERVE_TYPE_WeekDay:
			TAP_SPrint( text, "%s, %s's, %s ~ %s, %3d mins", Channel_Name(TimerList_Timers[TimerList_Selected].svcType,TimerList_Timers[TimerList_Selected].svcNum), TimerEdit_ReserveType(TimerList_Timers[TimerList_Selected].reservationType), Time_HHMM(TimerList_Timers[TimerList_Selected].startTime), Endtime_HHMM(TimerList_Timers[TimerList_Selected].startTime,TimerList_Timers[TimerList_Selected].duration), TimerList_Timers[TimerList_Selected].duration);
			break;
	}	
	TAP_Osd_FillBox( Display_MemRGN, x_pos+DISPLAY_GAP, y_pos + 8, 720-((x_pos+DISPLAY_GAP)*2), 22, DISPLAY_MAIN);
	Font_Osd_PutString1622( Display_MemRGN, x_pos+DISPLAY_GAP, y_pos + 8, 720-((x_pos+DISPLAY_GAP)*1), text, DISPLAY_EVENT, DISPLAY_MAIN );
	TAP_Osd_Copy( Display_MemRGN, Display_RGN, x_pos+DISPLAY_GAP, y_pos + 8, 720-((x_pos+DISPLAY_GAP)*2), 22, x_pos+DISPLAY_GAP, y_pos + 8, FALSE);
}

// Function to render the item in the array on the screen
void TimerList_RenderItem( word rgn, int row, int selected, int prevselected, bool fresh ){
	int x_pos = 0;
	int y_pos = 0;
	int textOffset = 0;
	char text[128];
	int width;
	dword fc = DISPLAY_EVENT;
	dword bc = DISPLAY_ITEM;
	memset( text, '\0', sizeof(text) );
	// Sort out the X position
	x_pos = Panel_Left_X;
	// Sort out the row for the item
	y_pos = Panel_Left_Y + (( DISPLAY_ITEM_H + DISPLAY_GAP ) * (row-TimerList_Min));
	// Sanity Check
	if( y_pos > 575 ) return;
	if( x_pos > 719 ) return;
	if( y_pos < 0 ) return;
	if( x_pos < 0 ) return;
	// ALter the text colour if required
	// Draw background for item it previous or selected otherwise the background on text will do the job
	if( row == selected ){
		if( TimerList_ItemHigh != NULL ){
			TAP_Osd_RestoreBox( rgn, x_pos, y_pos, DISPLAY_ROW_WIDTH, DISPLAY_ITEM_H, TimerList_ItemHigh );
		} else {
			TAP_Osd_FillBox(rgn, x_pos, y_pos, DISPLAY_ROW_WIDTH, DISPLAY_ITEM_H, DISPLAY_ITEMSELECTED);
		}
		TAP_Osd_FillBox(rgn, x_pos+90, y_pos, DISPLAY_GAP, DISPLAY_ITEM_H, DISPLAY_MAIN);
		TAP_Osd_FillBox(rgn, x_pos+230, y_pos, DISPLAY_GAP, DISPLAY_ITEM_H, DISPLAY_MAIN);
		bc = DISPLAY_ITEMSELECTED;
	} else {
		if( row == prevselected ){
			if( TimerList_ItemBase != NULL ){
				TAP_Osd_RestoreBox( rgn, x_pos, y_pos, DISPLAY_ROW_WIDTH, DISPLAY_ITEM_H, TimerList_ItemBase );
			} else {
				TAP_Osd_FillBox(rgn, x_pos, y_pos, DISPLAY_ROW_WIDTH, DISPLAY_ITEM_H, DISPLAY_ITEM);
			}
//			TAP_Osd_FillBox(rgn, x_pos, y_pos, DISPLAY_ROW_WIDTH, DISPLAY_ITEM_H, DISPLAY_ITEM);
			TAP_Osd_FillBox(rgn, x_pos+90, y_pos, DISPLAY_GAP, DISPLAY_ITEM_H, DISPLAY_MAIN);
			TAP_Osd_FillBox(rgn, x_pos+230, y_pos, DISPLAY_GAP, DISPLAY_ITEM_H, DISPLAY_MAIN);
			bc = COLOR_None;
		} else {
			if( fresh ){
				if( TimerList_ItemBase != NULL ){
					TAP_Osd_RestoreBox( rgn, x_pos, y_pos, DISPLAY_ROW_WIDTH, DISPLAY_ITEM_H, TimerList_ItemBase );
				} else {
					TAP_Osd_FillBox(rgn, x_pos, y_pos, DISPLAY_ROW_WIDTH, DISPLAY_ITEM_H, DISPLAY_ITEM);
				}
//				TAP_Osd_FillBox(rgn, x_pos, y_pos, DISPLAY_ROW_WIDTH, DISPLAY_ITEM_H, DISPLAY_ITEM);
				TAP_Osd_FillBox(rgn, x_pos+90, y_pos, DISPLAY_GAP, DISPLAY_ITEM_H, DISPLAY_MAIN);
				TAP_Osd_FillBox(rgn, x_pos+230, y_pos, DISPLAY_GAP, DISPLAY_ITEM_H, DISPLAY_MAIN);
				bc = COLOR_None;
			}
		}
	}
	
	// Lets place the text on the items now align the first 2 columns in the middle of there cells
	width = TAP_Osd_GetW( Time_DOWDDMMSpec(TimerList_Timers[row].startTime), 0, FNT_Size_1622);
	textOffset = (90 - width)/2;	// Cell width - string width / 2
	if( textOffset < 0 ) textOffset =0;
	Font_Osd_PutString1622( rgn, x_pos+textOffset, y_pos + 3, x_pos + 90 - textOffset, Time_DOWDDMMSpec(TimerList_Timers[row].startTime), fc, COLOR_None );
	// Print the timer duration
	sprintf( text, "%s ~ %s\0", Time_HHMM(TimerList_Timers[row].startTime), Endtime_HHMM(TimerList_Timers[row].startTime,TimerList_Timers[row].duration));
	width = TAP_Osd_GetW( text, 0, FNT_Size_1622);
	textOffset = (130 - width)/2;
	if( textOffset < 0 ) textOffset =0;
//	Font_Osd_PutString1622( rgn, x_pos+textOffset+95, y_pos + 3, x_pos + 220 - textOffset, text, fc, COLOR_None );
	Font_Osd_PutString1622( rgn, x_pos+textOffset+95, y_pos + 3, x_pos + 225 - textOffset, text, fc, COLOR_None );
	// The name will be left aligned so no need to get its width
	if( Settings_TimerListLogos ){
		TAP_SPrint( text, "%s\0", TimerList_Timers[row].fileName);
		Font_Osd_PutString1622( rgn, x_pos+DISPLAY_GAP+235, y_pos + 3, 720-(Panel_Left_X+DISPLAY_GAP-DISPLAY_X_OFFSET)-46, text, fc, COLOR_None );
		TAP_Osd_FillBox(rgn, 720-(Panel_Left_X+DISPLAY_GAP-DISPLAY_X_OFFSET)-46+DISPLAY_GAP, y_pos, DISPLAY_GAP, DISPLAY_ITEM_H, DISPLAY_MAIN);
		DisplaySMLogo( rgn, 720-(Panel_Left_X+DISPLAY_GAP-DISPLAY_X_OFFSET)-46+DISPLAY_GAP+DISPLAY_GAP, y_pos, TimerList_Timers[row].svcNum, TimerList_Timers[row].svcType );
	} else {
		TAP_SPrint( text, "%s\0", TimerList_Timers[row].fileName);
		Font_Osd_PutString1622( rgn, x_pos+DISPLAY_GAP+235, y_pos + 3, 720-(Panel_Left_X+DISPLAY_GAP-DISPLAY_X_OFFSET), text, fc, COLOR_None );
	}
}

// HELP PANEL
void TimerList_RenderHelp( void ){
	int x_pos = 0;
	int y_pos = 0;
	int width = 0;
	x_pos = Panel_Left_X;
	y_pos = Panel_Left_Y + (( DISPLAY_ITEM_H + DISPLAY_GAP ) * (TimerList_OPTIONS+1))+10;
	width = DISPLAY_ROW_WIDTH;
	// Fill the background
	TAP_Osd_FillBox(Display_RGN, x_pos, y_pos, width, 30, DISPLAY_MAIN);
	// Draw the keyboard and Timer edit help
	TAP_Osd_PutGd( Display_RGN, x_pos+5, y_pos, &_redGd, TRUE );	// Draw highlighted
	Font_Osd_PutString1419( Display_RGN, x_pos+30, y_pos, x_pos+width, "New Timer", DISPLAY_EVENT, COLOR_None);
	TAP_Osd_PutGd( Display_RGN, x_pos+5+130, y_pos, &_whiteGd, TRUE );	// Draw highlighted
	Font_Osd_PutString1419( Display_RGN, x_pos+30+130, y_pos, x_pos+width, "Delete Timer", DISPLAY_EVENT, COLOR_None);
	TAP_Osd_PutGd( Display_RGN, x_pos+5+270, y_pos, &_okGd, TRUE );	// Draw highlighted
	Font_Osd_PutString1419( Display_RGN, x_pos+30+270, y_pos, x_pos+width, "Edit Timer", DISPLAY_EVENT, COLOR_None);
	TAP_Osd_PutGd( Display_RGN, x_pos+5+395, y_pos, &_voldownGd, TRUE );	// Draw highlighted
	Font_Osd_PutString1419( Display_RGN, x_pos+30+395, y_pos, x_pos+width, "< Day", DISPLAY_EVENT, COLOR_None);
	TAP_Osd_PutGd( Display_RGN, x_pos+5+503, y_pos, &_volupGd, TRUE );	// Draw highlighted
	Font_Osd_PutString1419( Display_RGN, x_pos+35+503, y_pos, x_pos+width, "> Day", DISPLAY_EVENT, COLOR_None);
}

int TimerList_FindFirstOccurance( TYPE_TimerInfo timer ){
	TYPE_TimerInfo	currentTimer;
	int i;
	int retval = -1;
	int totalTimers;
	totalTimers = TAP_Timer_GetTotalNum();
	for ( i = 0; i < totalTimers; i++ ){
		if( TAP_Timer_GetInfo(i, &currentTimer) ){ // Sequentially get timer details and compare with those in the display range
			if( currentTimer.duration == timer.duration ){
				if( currentTimer.svcNum == timer.svcNum ){
					if( currentTimer.reservationType == timer.reservationType ){
						StripFileName(currentTimer.fileName);
						if( strcmp(currentTimer.fileName,timer.fileName) == 0 ){
							// Same element so we are found
							retval = i;
							i = totalTimers;
						}
					}
				}
			}
		}
	}
	return retval;
}

TYPE_TimerInfo TimerList_GetFirstOccurance( TYPE_TimerInfo timer ){
	static TYPE_TimerInfo currentTimer;
	int i;
	int totalTimers;
	totalTimers = TAP_Timer_GetTotalNum();
	for ( i = 0; i < totalTimers; i++ ){
		if( TAP_Timer_GetInfo(i, &currentTimer) ){ // Sequentially get timer details and compare with those in the display range
			if( currentTimer.duration == timer.duration ){
				if( currentTimer.svcNum == timer.svcNum ){
					if( currentTimer.reservationType == timer.reservationType ){
						StripFileName(currentTimer.fileName);
						if( strcmp(currentTimer.fileName,timer.fileName) == 0 ){
							// Same element so we are found
							return currentTimer;
						}
					}
				}
			}
		}
	}
	return currentTimer;
}


void TimerList_GetTimers( void ){
	TYPE_TimerInfo	currentTimer;
	TYPE_TimerInfo	tmp;
	int totalTimers = 0;
	int i, j, x;
	word year;
	byte month, day, weekDay;
	TimerList_Timers_ptr = 0;
	// Manually search for overlap timers. Will only be 2 timers as API would have prevented any more
	totalTimers = TAP_Timer_GetTotalNum();
	for ( i = 0; i < totalTimers; i++ ){
		if( TAP_Timer_GetInfo(i, &currentTimer) ){ // Sequentially get timer details and compare with those in the display range
			if( TimerList_ListingDay != ALL_TIMERS ){
				// Check its the right day of the week
				TAP_ExtractMjd( currentTimer.startTime>>16, &year, &month, &day, &weekDay );
				StripFileName(currentTimer.fileName);
				if( weekDay == TimerList_ListingDay ){	// Event on the day we are listing so add to array
					TimerList_Timers[TimerList_Timers_ptr] = currentTimer;
					TimerList_Timers_ptr++;
					// Check for weekly. If so then we need to add another to help indicate this
					if( currentTimer.reservationType == RESERVE_TYPE_Weekly ){
						for( x = 0; x < 7; x++ ){	// AddTime only handle 1440 max so loop the adding of the time
							currentTimer.startTime = AddTime(currentTimer.startTime, 1440 );
						}
						TimerList_Timers[TimerList_Timers_ptr] = currentTimer;
						TimerList_Timers_ptr++;
					}
				} else {
					if( currentTimer.reservationType == RESERVE_TYPE_Everyday ){
						if( (TimerList_ListingDay-weekDay) > -1 ){	// Must be incremented
							for( x = 0; x < (TimerList_ListingDay-weekDay); x++ ){
								currentTimer.startTime = AddTime( currentTimer.startTime, 1440 );
							}
							TimerList_Timers[TimerList_Timers_ptr] = currentTimer;
							TimerList_Timers_ptr++;
						} else {
							// We need to get the difference between the
							for( x = (TimerList_ListingDay-weekDay); x < 0; x++ ){
								currentTimer.startTime = AddTime( currentTimer.startTime, -1440 );
							}
							for( x = 0; x < 7; x++ ){	// AddTime only handle 1440 max so loop the adding of the time
								currentTimer.startTime = AddTime(currentTimer.startTime, 1440 );
							}
							TimerList_Timers[TimerList_Timers_ptr] = currentTimer;
							TimerList_Timers_ptr++;
						}
					} else {
						// Check for weekday. i.e. Not sat or sunday
						if( TimerList_ListingDay != SAT_TIMERS && TimerList_ListingDay != SUN_TIMERS ){
							if( currentTimer.reservationType == RESERVE_TYPE_WeekDay ){
								if( (TimerList_ListingDay-weekDay) > -1 ){	// Must be incremented
									for( x = 0; x < (TimerList_ListingDay-weekDay); x++ ){
										currentTimer.startTime = AddTime( currentTimer.startTime, 1440 );
									}
									TimerList_Timers[TimerList_Timers_ptr] = currentTimer;
									TimerList_Timers_ptr++;
								} else {
									// We need to get the difference between the
									for( x = (TimerList_ListingDay-weekDay); x < 0; x++ ){
										currentTimer.startTime = AddTime( currentTimer.startTime, -1440 );
									}
									for( x = 0; x < 7; x++ ){	// AddTime only handle 1440 max so loop the adding of the time
										currentTimer.startTime = AddTime(currentTimer.startTime, 1440 );
									}
									TimerList_Timers[TimerList_Timers_ptr] = currentTimer;
									TimerList_Timers_ptr++;
								}
							}
						} else {
							// Must be weekend
							if( currentTimer.reservationType == RESERVE_TYPE_EveryWeekend ){
								if( (TimerList_ListingDay-weekDay) > -1 ){	// Must be incremented
									for( x = 0; x < (TimerList_ListingDay-weekDay); x++ ){
										currentTimer.startTime = AddTime( currentTimer.startTime, 1440 );
									}
									TimerList_Timers[TimerList_Timers_ptr] = currentTimer;
									TimerList_Timers_ptr++;
								} else {
									// We need to get the difference between the
									for( x = (TimerList_ListingDay-weekDay); x < 0; x++ ){
										currentTimer.startTime = AddTime( currentTimer.startTime, -1440 );
									}
									for( x = 0; x < 7; x++ ){	// AddTime only handle 1440 max so loop the adding of the time
										currentTimer.startTime = AddTime(currentTimer.startTime, 1440 );
									}
									TimerList_Timers[TimerList_Timers_ptr] = currentTimer;
									TimerList_Timers_ptr++;
								}
							}
						}
					}
				}
			} else {
				StripFileName(currentTimer.fileName);
				TimerList_Timers[TimerList_Timers_ptr] = currentTimer;
				TimerList_Timers_ptr++;
			}
		}
	}
	// Quick bubble sort the timers by the timer they commence
	for( i = 0; i < TimerList_Timers_ptr; i++ ){
		for( j = 0; j < TimerList_Timers_ptr-1; j++ ){
			if( i != j ){
				if( TimerList_Timers[i].startTime < TimerList_Timers[j].startTime ){
					tmp = TimerList_Timers[i];
					TimerList_Timers[i] = TimerList_Timers[j];
					TimerList_Timers[j] = tmp;
				}
			}
		}
	}
}

void TimerList_Activate( void ){
	word mjd;
	byte hour, min, sec;
	word year;
	byte month, day, weekDay;
	TAP_GetTime( &mjd, &hour, &min, &sec );
	if( TAP_ExtractMjd( mjd, &year, &month, &day, &weekDay ) == 1 ){
		TimerList_ListingDay = weekDay;	
	}
	// If user wants all timers listed on activation then set to all timers
	if( Settings_ListAllTimers ){
		TimerList_ListingDay = ALL_TIMERS;	
	}
	
	TimerList_Min = 0;
	TimerList_Max = TimerList_OPTIONS;
	TimerList_Selected = TimerList_PrevSelected = 0;
	TimerList_GetTimers();
	if( TimerList_Max > TimerList_Timers_ptr -1 ){
		TimerList_Max = TimerList_Timers_ptr;
	}
	TimerList_CreateGradients();
	TimerList_Render();
}


void TimerList_Render( void ){
	int i = 0;
	int c = 0;
	char text[128];
	Display_CreateRegions();
	if( !Guide_Rendered && !TimerList_Active ){
		Screen_Set(Display_RGN);
		// No template so we need to redraw the display from scratch
		Display_DrawBackground();
		Display_DrawTime();
		TimerList_RenderHelp();
		// Draw the new title and flag that it has changed
		switch( TimerList_ListingDay ){
			case MON_TIMERS:
				sprintf(text,"Timers - Monday's (%d)\0", TimerList_Timers_ptr);
				Display_RenderTitle(text);
				break;
			case TUE_TIMERS:
				sprintf(text,"Timers - Tuesday's (%d)\0", TimerList_Timers_ptr);
				Display_RenderTitle(text);
				break;
			case WED_TIMERS:
				sprintf(text,"Timers - Wednesday's (%d)\0", TimerList_Timers_ptr);
				Display_RenderTitle(text);
				break;
			case THU_TIMERS:
				sprintf(text,"Timers - Thursday's (%d)\0", TimerList_Timers_ptr);
				Display_RenderTitle(text);
				break;
			case FRI_TIMERS:
				sprintf(text,"Timers - Friday's (%d)\0", TimerList_Timers_ptr);
				Display_RenderTitle(text);
				break;
			case SAT_TIMERS:
				sprintf(text,"Timers - Saturday's (%d)\0", TimerList_Timers_ptr);
				Display_RenderTitle(text);
				break;
			case SUN_TIMERS:
				sprintf(text,"Timers - Sunday's (%d)\0", TimerList_Timers_ptr);
				Display_RenderTitle(text);
				break;
			case ALL_TIMERS:
				sprintf(text,"Timers - All (%d)\0", TimerList_Timers_ptr);
				Display_RenderTitle(text);
				break;
		}
		c = 0;
		if( TimerList_Timers_ptr > 0 ){	// Only render items if there is some items in the array
			for( i = TimerList_Min; i < TimerList_Max && i < TimerList_Timers_ptr; i++ ){
				TimerList_RenderItem(Display_RGN,i,TimerList_Selected, TimerList_PrevSelected,TRUE);
				c++;
			}
		}
		for( i = c; i < TimerList_OPTIONS; i++ ){
			TimerList_RenderBlankItem( Display_RGN, i );
		}
		if( TimerList_Timers_ptr > 0 ){
			TimerList_RenderDetails();
		}
		// Draw the template on the display by fading in
		Screen_FadeIn(Display_RGN,Setting_FadeIn);
	} else {
		if( TimerList_Redraw ){
			if( !Display_BackgroundDrawn ){
				Display_DrawBackground();
				Display_DrawTime();
			}
			TAP_Osd_Copy( Display_RGN, Display_MemRGN, Panel_Left_X, Panel_Left_Y, DISPLAY_ROW_WIDTH, Panel_Left_Y + (( DISPLAY_ITEM_H + DISPLAY_GAP ) * (TimerList_OPTIONS-1)), Panel_Left_X, Panel_Left_Y, FALSE);
			c = 0;
			if( TimerList_Timers_ptr > 0 ){
				for( i = TimerList_Min; i < TimerList_Max && i < TimerList_Timers_ptr; i++ ){
					TimerList_RenderItem(Display_MemRGN,i,TimerList_Selected, TimerList_PrevSelected,TRUE);
					c++;
				}
			}
			for( i = c; i < TimerList_OPTIONS; i++ ){
				TimerList_RenderBlankItem( Display_MemRGN, i );
			}
			TAP_Osd_Copy( Display_MemRGN, Display_RGN, Panel_Left_X, Panel_Left_Y, DISPLAY_ROW_WIDTH, Panel_Left_Y + (( DISPLAY_ITEM_H + DISPLAY_GAP ) * (TimerList_OPTIONS-1)), Panel_Left_X, Panel_Left_Y, FALSE);
			// Draw the new title and flag that it has changed
			if( !Display_TitleDrawn ){
				switch( TimerList_ListingDay ){
					case MON_TIMERS:
						sprintf(text,"Timers - Monday's (%d)\0", TimerList_Timers_ptr);
						Display_RenderTitle(text);
						break;
					case TUE_TIMERS:
						sprintf(text,"Timers - Tuesday's (%d)\0", TimerList_Timers_ptr);
						Display_RenderTitle(text);
						break;
					case WED_TIMERS:
						sprintf(text,"Timers - Wednesday's (%d)\0", TimerList_Timers_ptr);
						Display_RenderTitle(text);
						break;
					case THU_TIMERS:
						sprintf(text,"Timers - Thursday's (%d)\0", TimerList_Timers_ptr);
						Display_RenderTitle(text);
						break;
					case FRI_TIMERS:
						sprintf(text,"Timers - Friday's (%d)\0", TimerList_Timers_ptr);
						Display_RenderTitle(text);
						break;
					case SAT_TIMERS:
						sprintf(text,"Timers - Saturday's (%d)\0", TimerList_Timers_ptr);
						Display_RenderTitle(text);
						break;
					case SUN_TIMERS:
						sprintf(text,"Timers - Sunday's (%d)\0", TimerList_Timers_ptr);
						Display_RenderTitle(text);
						break;
					case ALL_TIMERS:
						sprintf(text,"Timers - All (%d)\0", TimerList_Timers_ptr);
						Display_RenderTitle(text);
						break;
				}
			}
			if( TimerList_Timers_ptr > 0 ){
				TimerList_RenderDetails();
			}
			TimerList_Redraw = FALSE;
		} else {
			if( TimerList_Timers_ptr > 0 ){
				TimerList_RenderItem(Display_MemRGN,TimerList_Selected,TimerList_Selected, TimerList_PrevSelected,TRUE);
				TimerList_RenderItem(Display_MemRGN,TimerList_PrevSelected,TimerList_Selected, TimerList_PrevSelected,TRUE);
				TAP_Osd_Copy( Display_MemRGN, Display_RGN, Panel_Left_X, Panel_Left_Y + (( DISPLAY_ITEM_H + DISPLAY_GAP ) * (TimerList_PrevSelected-TimerList_Min)), DISPLAY_ROW_WIDTH, (( DISPLAY_ITEM_H )), Panel_Left_X, Panel_Left_Y + (( DISPLAY_ITEM_H + DISPLAY_GAP ) * (TimerList_PrevSelected-TimerList_Min)), FALSE);
				TAP_Osd_Copy( Display_MemRGN, Display_RGN, Panel_Left_X, Panel_Left_Y + (( DISPLAY_ITEM_H + DISPLAY_GAP ) * (TimerList_Selected-TimerList_Min)), DISPLAY_ROW_WIDTH, (( DISPLAY_ITEM_H )), Panel_Left_X, Panel_Left_Y + (( DISPLAY_ITEM_H + DISPLAY_GAP ) * (TimerList_Selected-TimerList_Min)), FALSE);
				TimerList_RenderDetails();
			}
		}
	}
	TimerList_TotalTimers = TAP_Timer_GetTotalNum();	// Store the number of timers when we last rendered
	TimerList_LastRendered = TAP_GetTick();
	TimerList_Active = TRUE;
}

void TimerList_DeleteTimerCallback( bool yes ){
	int timerNo = -1;
	TAP_Osd_Copy( Display_RGN, Display_MemRGN, 0, 0, 720, 576, 0, 0, FALSE);	// Copy the main region to memory to avoid corruption from the previous OSD
	if( TimerList_Timers_ptr < 1 ) return;
	if( yes ){
		// User has elected to delete the timer from the system so find it and remove it.
		if( (timerNo = TimerList_FindFirstOccurance(TimerList_Timers[TimerList_Selected])) > -1 ){
			if( TAP_Timer_Delete( timerNo ) ){
				// Update the display as we have delete the timer
				TimerList_GetTimers();	// Regrab the timers
				if( TimerList_Selected > TimerList_Timers_ptr -1 ) TimerList_Selected = TimerList_Timers_ptr -1;
				if( TimerList_Max > TimerList_Timers_ptr -1 ){
					TimerList_Max = TimerList_Timers_ptr;
				}
				TimerList_Redraw = TRUE;
				Display_TitleDrawn = FALSE;	// Will redraw the title and the number of timers at the top
				TimerList_Render();
			}
		}
	}
}

void TimerList_TimerConflictCallback( bool result ){
	TAP_Osd_Copy(Display_RGN, Display_MemRGN, 0, 0, 720, 576, 0, 0, FALSE); // Copy the main region to memory to avoid corruption from the previous OSD
	TimerList_Redraw = TRUE;
	TimerList_Render();
}

void TimerList_NewTimerCallback( int i, bool cancel, TYPE_TimerInfo newtimer ){
	int totalTimers;
	TAP_Osd_Copy( Display_RGN, Display_MemRGN, 0, 0, 720, 576, 0, 0, FALSE);	// Copy the main region to memory to avoid corruption from the previous OSD
	// Do nothing atm
	if( !cancel ){
		if( i != 0 ){
			// Call the conflict timer module to get it to handle the issues
			TimerConflict_Init( Display_RGN, Display_MemRGN, Panel_Left_X, Panel_Left_Y + (( DISPLAY_ITEM_H + DISPLAY_GAP ) * (0)), DISPLAY_ROW_WIDTH, Panel_Left_Y + (( DISPLAY_ITEM_H + DISPLAY_GAP ) * (TimerList_OPTIONS-2)) );
			TimerConflict_Activate( newtimer, &TimerList_TimerConflictCallback );// ){
			if( !TimerConflict_IsActive() ) { // We didn't activate so check for append
				totalTimers = TAP_Timer_GetTotalNum();
				if( totalTimers > 69 ) {
					Message_Display( "Maximum number of timers are set." );
				} else {
					Message_Display( "ERROR: Unable to resolve issues." );
				}
				TimerList_Redraw = TRUE;
				TimerList_Render();
			}
		} else {
			Message_Display( "Timer successfully set." );
			// Update the display as we have delete the timer
			TimerList_GetTimers();	// Regrab the timers
			TimerList_Redraw = TRUE;
			TimerList_Render();
		}
	}
}

void TimerList_EditTimerCallback( int i, bool cancel ){
	TAP_Osd_Copy( Display_RGN, Display_MemRGN, 0, 0, 720, 576, 0, 0, FALSE);	// Copy the main region to memory to avoid corruption from the previous OSD
	if( TimerList_Timers_ptr < 1 ) return;
	// Do nothing atm
	if( !cancel ){
		if( i != 0 ){
			Message_Display( "Failed to adjust timer" );
		} else {
			Message_Display( "Timer successfully adjusted." );
		}
		// Update the display as we have delete the timer
		TimerList_GetTimers();	// Regrab the timers
		TimerList_Redraw = TRUE;
		TimerList_Render();
	}
}

// Keyhandler for the TimerList
dword TimerList_Keyhandler( dword param1 ){
	if( Keyboard_IsActive() ){	// Handle the keyboard
		return Keyboard_Keyhandler(param1);
	}
	if( YesNo_IsActive() ){
		return YesNo_KeyHandler(param1);
	}
	if( TimerEdit_IsActive() ){
		return TimerEdit_KeyHandler(param1);
	}
	if( TimerEditFull_IsActive() ){
		return TimerEditFull_KeyHandler(param1);
	}
	if( TimerNew_IsActive() ){
		return TimerNew_KeyHandler(param1);
	}		
	if( TimerEditNew_IsActive() ){
		return TimerEditNew_KeyHandler(param1);
	}
	if (TimerConflict_IsActive() ) {
		return TimerConflict_Keyhandler(param1);
	}
	// Either the user has press exit or the activation key. If so close the display
	if( param1 == RKEY_Exit ){
		TimerList_Hide();
		return 0;
	}
	if( param1 == Settings_TimerListKey && Settings_TimerListKey == RKEY_TvRadio ){
		if( TimerList_ListingDay == ALL_TIMERS ){
			TimerList_Hide();
			return 0;
		}
	}
	switch( param1 ){
		case RKEY_Menu:
			TimerList_Hide();
			Menu_Render();
			return 0;
			break;
		case RKEY_ChDown:
			if( TimerList_Timers_ptr < 1 ) return 0;
			TimerList_PrevSelected = TimerList_Selected;
			TimerList_Selected++;
			if( TimerList_Selected > TimerList_Max-1 ){
				TimerList_Min = TimerList_Selected;
				TimerList_Max = TimerList_Min + TimerList_OPTIONS;
				TimerList_Redraw = TRUE;
				if( TimerList_Max > TimerList_Timers_ptr-1 ){
					TimerList_Min = TimerList_Timers_ptr-TimerList_OPTIONS;
					TimerList_Max = TimerList_Timers_ptr;
				}
			}
			if( TimerList_Selected > TimerList_Timers_ptr-1 ){
				TimerList_Selected = 0;
				TimerList_Min = 0;
				TimerList_Max = TimerList_OPTIONS;
				if( TimerList_Max > TimerList_Timers_ptr -1 ){
					TimerList_Max = TimerList_Timers_ptr;
				}
				TimerList_Redraw = TRUE;
			}
			TimerList_Render();
			return 0;
			break;
		case RKEY_ChUp:
			if( TimerList_Timers_ptr < 1 ) return 0;
			TimerList_PrevSelected = TimerList_Selected;
			TimerList_Selected--;
			if( TimerList_Selected < TimerList_Min ){
				TimerList_Min = TimerList_Selected-TimerList_OPTIONS+1;
				TimerList_Max = TimerList_Selected+1;
				TimerList_Redraw = TRUE;
				if( TimerList_Min < 0 ){
					TimerList_Min = 0;
					TimerList_Max = TimerList_OPTIONS;
					if( TimerList_Max > TimerList_Timers_ptr-1 ){
						TimerList_Max = TimerList_Timers_ptr;
					}
				}
			}
			if( TimerList_Selected < 0 ){
				TimerList_Selected = TimerList_Timers_ptr-1;
				TimerList_Max = TimerList_Timers_ptr;
				TimerList_Min = TimerList_Max-TimerList_OPTIONS;
				if( TimerList_Min < 0 ){
					TimerList_Min = 0;
				}
				TimerList_Redraw = TRUE;
			}
			TimerList_Render();
			return 0;
			break;
		case RKEY_VolUp:
			Display_TitleDrawn = FALSE;
			TimerList_PrevSelected = TimerList_Selected = 0;
			TimerList_ListingDay++;
			if( TimerList_ListingDay > ALL_TIMERS ){
				TimerList_ListingDay = MON_TIMERS;
			}
			TimerList_GetTimers();
			TimerList_Min = 0;
			TimerList_Max = TimerList_OPTIONS;
			if( TimerList_Max > TimerList_Timers_ptr -1 ){
				TimerList_Max = TimerList_Timers_ptr;
			}
			TimerList_Redraw = TRUE;
			TimerList_Render();
			return 0;
			break;
		case RKEY_VolDown:
			Display_TitleDrawn = FALSE;
			TimerList_PrevSelected = TimerList_Selected = 0;
			TimerList_ListingDay--;
			if( TimerList_ListingDay < MON_TIMERS ){
				TimerList_ListingDay = ALL_TIMERS;
			}
			TimerList_GetTimers();
			TimerList_Min = 0;
			TimerList_Max = TimerList_OPTIONS;
			if( TimerList_Max > TimerList_Timers_ptr -1 ){
				TimerList_Max = TimerList_Timers_ptr;
			}
			TimerList_Redraw = TRUE;
			TimerList_Render();
			return 0;
			break;
		case RKEY_TvRadio:
			Display_TitleDrawn = FALSE;
			TimerList_PrevSelected = TimerList_Selected = 0;
			TimerList_ListingDay = ALL_TIMERS;
			TimerList_GetTimers();
			TimerList_Min = 0;
			TimerList_Max = TimerList_OPTIONS;
			if( TimerList_Max > TimerList_Timers_ptr -1 ){
				TimerList_Max = TimerList_Timers_ptr;
			}
			TimerList_Redraw = TRUE;
			TimerList_Render();
			return 0;
			break;
		case RKEY_0:
			TimerList_PrevSelected = TimerList_Selected = 0;
			TimerList_Min = 0;
			TimerList_Max = TimerList_OPTIONS;
			if( TimerList_Max > TimerList_Timers_ptr -1 ){
				TimerList_Max = TimerList_Timers_ptr;
			}
			TimerList_Redraw = TRUE;
			TimerList_Render();
			return 0;
			break;
		case RKEY_Ab:
			if( TimerList_Timers_ptr < 1 ) return 0;
			YesNo_Init( Display_RGN, Display_MemRGN,  (720/2)-(250/2)+DISPLAY_X_OFFSET, (576/2)-(150/2)+DISPLAY_Y_OFFSET, 250, 150 );
			YesNo_SetColours( DISPLAY_KEYBOARDITEMTEXT, DISPLAY_ITEM, DISPLAY_KEYBOARDITEMSELECTEDTEXT, DISPLAY_KEYBOARDITEMSELECTED, DISPLAY_MAIN );
			if( !YesNo_Activate("Are you sure you want to remove timer?",&TimerList_DeleteTimerCallback) ){
				Message_Display( "YesNo box failed." );	// Explained that the event has past and we can set because of that
			}
			return 0;
			break;
		case RKEY_Ok:
			if( TimerList_Timers_ptr < 1 ) return 0;
			TimerEditFull_Init( Display_RGN, Display_MemRGN,  (720/2)-(340/2)+DISPLAY_X_OFFSET, (576/2)-(350/2)+DISPLAY_Y_OFFSET-10, 340, 350 );
			TimerEditFull_SetColours( DISPLAY_KEYBOARDITEMTEXT, DISPLAY_ITEM, DISPLAY_KEYBOARDITEMSELECTEDTEXT, DISPLAY_KEYBOARDITEMSELECTED, DISPLAY_MAIN );

//			TimerEdit_Init( Display_RGN, Display_MemRGN,  (720/2)-(270/2)+DISPLAY_X_OFFSET, (576/2)-(250/2)+DISPLAY_Y_OFFSET, 270, 250 );
//			TimerEdit_SetColours( DISPLAY_KEYBOARDITEMTEXT, DISPLAY_ITEM, DISPLAY_KEYBOARDITEMSELECTEDTEXT, DISPLAY_KEYBOARDITEMSELECTED, DISPLAY_MAIN );
			if( !TimerEditFull_ActivateWithTimer(TimerList_GetFirstOccurance(TimerList_Timers[TimerList_Selected]), TimerList_FindFirstOccurance( TimerList_Timers[TimerList_Selected] ),&TimerList_EditTimerCallback) ){
//				char text[128];
//				sprintf(text, "Timer first occurance = %d\0", TimerList_FindFirstOccurance( TimerList_Timers[TimerList_Selected] ));
//				Message_Display( text );	// Explained that the event has past and we can set because of that
//				TAP_Delay(200);
				Message_Display( "TimerEdit failed to launch" );	// Explained that the event has past and we can set because of that
			}
			return 0;
			break;
		case RKEY_NewF1:
			TimerNew_Init( Display_RGN, Display_MemRGN,  (720/2)-(340/2)+DISPLAY_X_OFFSET, (576/2)-(350/2)+DISPLAY_Y_OFFSET-10, 340, 350 );
			TimerNew_SetColours( DISPLAY_KEYBOARDITEMTEXT, DISPLAY_ITEM, DISPLAY_KEYBOARDITEMSELECTEDTEXT, DISPLAY_KEYBOARDITEMSELECTED, DISPLAY_MAIN );
			if( !TimerNew_Activate(&TimerList_NewTimerCallback) ){
//				char text[128];
//				sprintf(text, "Timer first occurance = %d\0", TimerList_FindFirstOccurance( TimerList_Timers[TimerList_Selected] ));
//				Message_Display( text );	// Explained that the event has past and we can set because of that
//				TAP_Delay(200);
				Message_Display( "TimerNew failed to launch. Unknown reason." );	// Explained that the event has past and we can set because of that
			}
			return 0;
			break;
			break;
	}
	if( param1 == Settings_TimerListKey ){
		TimerList_Hide();
		return 0;
	}
	return 0;
}
