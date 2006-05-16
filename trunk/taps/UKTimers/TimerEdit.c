/*
	Copyright (C) 2005-2006 Darkmatter

	This file is part of the TAPs for Topfield PVRs project.
		http://tap.berlios.de/

	This is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	The software is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this software; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

// This module displays the timer list


static TYPE_Window editWin;
static int chosenEditLine;
static int timerLine;
static char commandOption;
static byte currentExitOption;

static TYPE_TimerInfo currentTimer;
static bool returnFromEdit;

static int selectedLogo;
static byte selectedSvc;

static byte selectedDay;
static byte selectedMonth;
static word selectedYear;

#define X1	130
#define X2	300
#define E0	113
#define E1	275
#define E2	627
#define Y	57
#define Y2	514
#define Y_STEP 30
#define TIMER_LINES 10

#define DEFUALT_EXIT_OPTION 0

#include "Calendar.c"
#include "ChannelSelection.c"
#include "Keyboard.c"

void CloseTimerEditWindow(void);

//------------------------------------------------------------------
//

//--------------------------------------------------------
// removes the ".rec" from the end of a file name
void StripFileName( char *name )
{
	int 	i, k, len;
	bool 	noREC;
	char	str[10];

	noREC = FALSE;
	len = strlen( name );
	memset( str, '\0', sizeof(str) );									// will catch any duff processing

	if ( len < 4 ) return;												// can't process if string too short
	else
	{
	    k=0;
		
		for ( i = len-4 ; i < len ; i++ )
		{
		    str[k] = name[i];
			k++;
		}

		if ( strcmp( str, ".rec" ) != 0 ) return;						// can't process if the string doesn't end in ".rec"
	}

	for ( i = len-4 ; i < len ; i++ )									// overwrite the ".rec"
	{
	    name[i]='\0';
	}
}


//-----------------------------------------------------------------------
//
void CreateTimerEditWindow(void)
{
	editWindowShowing = TRUE;
	DrawGraphicBoarders();
	TAP_Osd_PutStringAf1926( rgn, 58, 40, 390, "Timer Edit", TITLE_COLOUR, COLOR_Black );
}


void CloseTimerEditWindow(void)
{
	editWindowShowing = FALSE;
}



//------------
//
void DisplayLine(char lineNumber)
{
    char	str[80], str2[80];
	byte	month, day, weekDay;
    word	year;
	int 	min, hour, endMin, endHour;
	dword	textColour, backgroundColour, textColour2;
	int l;

	TYPE_TapChInfo	currentChInfo;

	if (( lineNumber < 1 ) || ( lineNumber > TIMER_LINES )) return;		// bound check
	
	if (( chosenEditLine == lineNumber ) && ( lineNumber != 10 ))		// highlight the current cursor line
    {																	// save, cancel, delete looks after itself
		TAP_Osd_PutGd( rgn, 53, lineNumber*Y1_STEP+Y1_OFFSET-8, &_highlightGd, FALSE );
	}
	else
	{
		TAP_Osd_PutGd( rgn, 53, lineNumber * Y1_STEP + Y1_OFFSET - 8, &_rowaGd, FALSE );
	}


	switch ( lineNumber )
	{
		case 1 :														// programme name
				PrintCenter(rgn, E0, (lineNumber * Y1_STEP + Y1_OFFSET), E1, "File Name", MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );

				strcpy( str, currentTimer.fileName );
				StripFileName( str );									// strips the ".rec" of the end (if any found)

				TAP_Osd_PutStringAf1622(rgn, X2, (lineNumber * Y1_STEP + Y1_OFFSET), E2, str, MAIN_TEXT_COLOUR, 0 );

				break;


		case 2 :														// Record or program
				PrintCenter(rgn, E0, (lineNumber * Y1_STEP + Y1_OFFSET), E1, "Record Mode", MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );

				if ( currentTimer.isRec == 1 )
				{
				    TAP_Osd_PutGd( rgn, 69, lineNumber*Y1_STEP+Y1_OFFSET-8, &_redcircleGd, TRUE );
					TAP_Osd_PutStringAf1622( rgn, 78, lineNumber*Y1_STEP+Y1_OFFSET, E0, "R", MAIN_TEXT_COLOUR, 0 );
					TAP_SPrint(str, "Record");
				}
				else
				{
				    TAP_Osd_PutGd( rgn, 69, lineNumber*Y1_STEP+Y1_OFFSET-8, &_greencircleGd, TRUE );
					TAP_Osd_PutStringAf1622( rgn, 78-2, lineNumber*Y1_STEP+Y1_OFFSET+1, E0, "W", MAIN_TEXT_COLOUR, 0 );
					TAP_SPrint(str, "Watch");
				}

				TAP_Osd_PutStringAf1622(rgn, X2, (lineNumber * Y1_STEP + Y1_OFFSET), E2, str, MAIN_TEXT_COLOUR, 0 );

		    break;

				
		case 3 :														// channel name
				PrintCenter(rgn, E0, (lineNumber * Y1_STEP + Y1_OFFSET), E1,  "Channel", MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );

				if ( TAP_Channel_GetInfo( currentTimer.svcType, currentTimer.svcNum, &currentChInfo ) )
				{
	             	TAP_SPrint(str,"%s", currentChInfo.chName);
					DisplayLogo( rgn, 53, lineNumber*Y1_STEP+Y1_OFFSET-8, currentTimer.svcNum, currentTimer.svcType );
				}
				else
	               TAP_SPrint(str,"Bad ch", currentChInfo.chName);

				TAP_Osd_PutStringAf1622(rgn, X2, (lineNumber * Y1_STEP + Y1_OFFSET), E2, str, MAIN_TEXT_COLOUR, 0 );

			    break;


		case 4 :														// date
				PrintCenter(rgn, E0, (lineNumber * Y1_STEP + Y1_OFFSET), E1, "Date", MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );

				TAP_ExtractMjd( currentTimer.startTime>>16, &year, &month, &day, &weekDay) ;
				
				switch ( weekDay )
				{
					case 0:	TAP_SPrint(str2,"Mon"); break;
					case 1:	TAP_SPrint(str2,"Tue"); break;
					case 2:	TAP_SPrint(str2,"Wed"); break;
					case 3:	TAP_SPrint(str2,"Thu"); break;
					case 4:	TAP_SPrint(str2,"Fri"); break;
					case 5:	TAP_SPrint(str2,"Sat"); break;
					case 6:	TAP_SPrint(str2,"Sun"); break;
					default : TAP_SPrint(str2,"BAD"); break;
				}
				TAP_SPrint(str, "%3s %d-%d-%d", str2, day, month, year);
				TAP_Osd_PutStringAf1622(rgn, X2, (lineNumber * Y1_STEP + Y1_OFFSET), E2, str, MAIN_TEXT_COLOUR, 0 );

			    break;

				
		case 5 :														// start time
				PrintCenter(rgn, E0, (lineNumber * Y1_STEP + Y1_OFFSET), E1, "Start Time", MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );

				hour = (currentTimer.startTime&0xff00)>>8;				// extract the time
				min = (currentTimer.startTime&0xff);

				TAP_SPrint(str, "%02d:%02d", hour, min);
				TAP_Osd_PutStringAf1622(rgn, X2, (lineNumber * Y1_STEP + Y1_OFFSET), E2, str, MAIN_TEXT_COLOUR, 0 );

			    break;


		case 6 :														// end time
				PrintCenter(rgn, E0, (lineNumber * Y1_STEP + Y1_OFFSET), E1, "End Time", MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );

				hour = (currentTimer.startTime&0xff00)>>8;				// extract the time
				min = (currentTimer.startTime&0xff);

				endMin = min + currentTimer.duration;					// add the duration in miutes

				endHour = hour + endMin/60;								// should we have increamented the hour
				if ( endHour >= 24 ) endHour -= 24;						// did the day roll over ?

				endMin = endMin%60;										// Finally remove the hours from the minutes field.
				
				TAP_SPrint(str, "%02d:%02d", endHour, endMin);
				TAP_Osd_PutStringAf1622(rgn, X2, (lineNumber * Y1_STEP + Y1_OFFSET), E2, str, MAIN_TEXT_COLOUR, 0 );

			    break;


		case 7 :														// duration
				PrintCenter(rgn, E0, (lineNumber * Y1_STEP + Y1_OFFSET), E1, "Duration", MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );

				TAP_SPrint(str, "%d min", currentTimer.duration);
				TAP_Osd_PutStringAf1622(rgn, X2, (lineNumber * Y1_STEP + Y1_OFFSET), E2, str, MAIN_TEXT_COLOUR, 0 );

				break;


		case 8 :														// repeat type
				PrintCenter(rgn, E0, (lineNumber * Y1_STEP + Y1_OFFSET), E1, "Repeat Type", MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );

				switch ( currentTimer.reservationType )
				{
					case RESERVE_TYPE_Onetime: 		TAP_SPrint(str,"One Time"); break;
					case RESERVE_TYPE_Everyday: 	TAP_SPrint(str,"Every Day"); break;
					case RESERVE_TYPE_EveryWeekend:	TAP_SPrint(str,"Weekends"); break;
					case RESERVE_TYPE_Weekly:		TAP_SPrint(str,"Weekly"); break;
					case RESERVE_TYPE_WeekDay:		TAP_SPrint(str,"Week Days"); break;
					default : break;
				}
				TAP_Osd_PutStringAf1622(rgn, X2, (lineNumber * Y1_STEP + Y1_OFFSET), E2, str, MAIN_TEXT_COLOUR, 0 );

				break;
				
		case 10 :
				TAP_Osd_PutGd( rgn, 53, lineNumber * Y1_STEP + Y1_OFFSET - 8, &_rowaGd, FALSE );		// No highlight for us
				
				if (( commandOption == 0 ) && ( chosenEditLine == 10 ))
				{
					TAP_Osd_PutGd( rgn, 116, lineNumber * Y1_STEP + Y1_OFFSET - 8, &_smallgreenbarGd, FALSE );
				    PrintCenter(rgn, 144, lineNumber * Y1_STEP + Y1_OFFSET, 244, "Save", MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );
				}
				else PrintCenter(rgn, 144, lineNumber * Y1_STEP + Y1_OFFSET, 244, "Save", MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );

					
				if (( commandOption == 1 ) && ( chosenEditLine == 10 ))
				{
					TAP_Osd_PutGd( rgn, 278, lineNumber * Y1_STEP + Y1_OFFSET - 8, &_smallgreenbarGd, FALSE );
					PrintCenter(rgn, 306, lineNumber * Y1_STEP + Y1_OFFSET, 406, "Cancel", MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );
				}
				else PrintCenter(rgn, 306, lineNumber * Y1_STEP + Y1_OFFSET, 406, "Cancel", MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );

				  
				if (( commandOption == 2 ) && ( chosenEditLine == 10 ))
				{
					TAP_Osd_PutGd( rgn, 440, lineNumber * Y1_STEP + Y1_OFFSET - 8, &_smallgreenbarGd, FALSE );
					PrintCenter(rgn, 468, lineNumber * Y1_STEP + Y1_OFFSET, 568, "Delete", MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );
				}
				else PrintCenter(rgn, 468, lineNumber * Y1_STEP + Y1_OFFSET, 568, "Delete", MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );

				
			    TAP_Osd_FillBox( rgn, 437, lineNumber*Y1_STEP+Y1_OFFSET-8, 3, Y1_STEP, FILL_COLOUR );		// two extra column seperators for us
			    TAP_Osd_FillBox( rgn, 599, lineNumber*Y1_STEP+Y1_OFFSET-8, 3, Y1_STEP, FILL_COLOUR );
				break;


		default :
			break;
	}


    TAP_Osd_FillBox( rgn, E0, lineNumber*Y1_STEP+Y1_OFFSET-8, 3, Y1_STEP, FILL_COLOUR );		// draw the column seperators
    TAP_Osd_FillBox( rgn, E1, lineNumber*Y1_STEP+Y1_OFFSET-8, 3, Y1_STEP, FILL_COLOUR );

}


void PrintTimerExtraInfo( void )
{
    char	str[256];

	TAP_Osd_PutStringAf1419( rgn, 58, 503, 666, "Debug:", COLOR_DarkGray, 0 );
	
	TAP_SPrint(str, "Tuner : %d      NameFix : %d       Reserved : %d", currentTimer.tuner, currentTimer.nameFix, currentTimer.reserved );
	TAP_Osd_PutStringAf1419( rgn, 58, 528, 666, str, COLOR_DarkGray, 0 );
}



//------------
//
void CopyTimerFields(int timerIndex)
{
	TYPE_TimerInfo timer;
	TAP_Timer_GetInfo(timerIndex, &timer);

    currentTimer.isRec = timer.isRec;
    currentTimer.tuner = timer.tuner;
    currentTimer.svcType = timer.svcType;
    currentTimer.reserved = timer.reserved;
    currentTimer.svcNum = timer.svcNum;
    currentTimer.reservationType = timer.reservationType;
    currentTimer.nameFix = timer.nameFix;
    currentTimer.duration = timer.duration;
    currentTimer.startTime = timer.startTime;
    strcpy(currentTimer.fileName, timer.fileName);
}



void RedrawTimerEdit( void )
{
	int i;
	
	CreateTimerEditWindow();
	UpdateListClock();

	for ( i=1; i<=TIMER_LINES ; i++)
	{
	    DisplayLine(i);
	}

	PrintTimerExtraInfo();
}


//------------
//
void ReturnFromKeyboard( char *str, bool success)
{
	if ( success == TRUE)
	{
	    strcat( str, ".rec" );											// ensure the file name is of the correct format
		strcpy( currentTimer.fileName, str );
	    currentTimer.nameFix = 1;										// file name becomes fixed onced edited
	}
}																		// no need to update display as keyboard.c has scheduled a global redisplay


void EditFileName( void )
{
	char	str[256];

	strcpy( str, currentTimer.fileName );
	StripFileName( str );
	ActivateKeyboard( str, &ReturnFromKeyboard );
}



//------------
//
void EditLineKeyHandler(dword key)
{

	int		maxTvChannels, maxRadioChannels, max;
	int		newSvcNum;
	int		count, result;
	TYPE_TapChInfo	currentChInfo;
	word 	mjd;
	int 	min, hour, duration, startTime;

	switch ( chosenEditLine )
	{																	// file name 	[Programme name]
		case 1 :	if ( key == RKEY_Ok ) EditFileName();
					break;

		case 2 :	switch ( key )										// Record mode 	[Record, or Watch]

					{
						case RKEY_VolUp:								// toggle
						case RKEY_VolDown:	if (currentTimer.isRec == 1) currentTimer.isRec = 0;
											else currentTimer.isRec = 1;
										    break;
						default :			break;
					}

					DisplayLine( chosenEditLine );
					break;

					
		case 3 :														// channel
					newSvcNum = currentTimer.svcNum;
					TAP_Channel_GetTotalNum( &maxTvChannels, &maxRadioChannels );

					if ( currentTimer.svcType == SVC_TYPE_Tv) max = maxTvChannels;
					else max = maxRadioChannels;

					count = 0;

					switch ( key )
					{

						case RKEY_VolUp:								// Channel up
										do
										{
										    if ( newSvcNum < max ) newSvcNum++;		// skip to the next channel
											else newSvcNum = 1;
											count++;								// if the channel is not selectable, go round again
											result = TAP_Channel_GetInfo( currentTimer.svcType, newSvcNum, &currentChInfo);
										}
										while ((result == 0) && (count < 3000));	// until either we find a channel that is ok,
																					// or we've tried 3000 times.
										if (result == 0) break;

										currentTimer.svcNum = newSvcNum;
										DisplayLine(chosenEditLine);
										break;

						case RKEY_VolDown:								// Channel down
										do
										{
										    if ( newSvcNum > 0 ) newSvcNum--;
											else newSvcNum = max;
											count++;
											result = TAP_Channel_GetInfo( currentTimer.svcType, newSvcNum, &currentChInfo);
										}
										while ((result == 0) && (count < 3000));
						
										if (result == 0) break;

										currentTimer.svcNum = newSvcNum;
										DisplayLine(chosenEditLine);
										break;

						case RKEY_Ok :	DisplayChannelListWindow();
										return;


						case RKEY_TvRadio :								// swap between TV and Radio
										if ( currentTimer.svcType == 0 )
										{
											currentTimer.svcType = 1;
											if ( currentTimer.svcNum >= maxRadioChannels ) currentTimer.svcNum = 0;

										}
										else
										{
										    currentTimer.svcType = 0;
											if ( currentTimer.svcNum >= maxTvChannels ) currentTimer.svcNum = 0;

										}
										DisplayLine(chosenEditLine);
										result = 0;
										
										break;
						
										
						default :		result = 0;
							break;
					}
					if (result == 0) break;

					currentTimer.svcNum = newSvcNum;
					DisplayLine( chosenEditLine );
					break;
					
		case 4 :														// date
	        			startTime = currentTimer.startTime&0xffff;		// extract the time
						mjd = currentTimer.startTime>>16;

						switch ( key )
						{
							case RKEY_VolUp:	mjd++;	break;			// Date up
							case RKEY_VolDown:	mjd--;	break;			// Date down
							case RKEY_Ok :		DisplayCalendarWindow(); return;
							default :			break;
						}

						currentTimer.startTime = (mjd<<16) | startTime;
						
						DisplayLine( chosenEditLine );
					    break;


					
		case 5 :														// start time
					hour = (currentTimer.startTime&0xff00)>>8;			// extract the time
					min = (currentTimer.startTime&0xff);
					mjd = currentTimer.startTime>>16;

					switch ( key )
					{
						case RKEY_VolUp:								// Time up
										if ( min < 59 )	min++;
										else
										{
										    if ( hour < 23 )
											{
											    min = 0;
												hour++;
											}
											else
											{
											    min = 0;
												hour = 0;
												mjd++;
											}
										}
										break;

						case RKEY_VolDown:								// Time down
										if ( min > 0 )	min--;
										else
										{
										    if ( hour > 0 )
											{
											    min = 59;
												hour--;
											}
											else
											{
											    min = 59;
												hour = 23;
												mjd--;
											}
										}
										break;

						default :
										break;
						}

						currentTimer.startTime = (mjd<<16) | ((hour&0xff)<<8) | (min&0xff);
						DisplayLine( chosenEditLine );			// start time
						DisplayLine( 6 ); 						// end time
					    break;


		case 6 :														// end time
						switch ( key )									// [same as changing the duration]
						{
							case RKEY_VolUp:							// Time up
										currentTimer.duration++;
										break;

							case RKEY_VolDown:							// Time down
										if ( currentTimer.duration > 0 ) currentTimer.duration--;
										break;
							default :
										break;
						}

						DisplayLine( 7 );								// Duration
						DisplayLine( chosenEditLine );					// End time
					    break;

		case 7 :														// duration
						switch ( key )
						{
							case RKEY_VolUp:							// Time up
										currentTimer.duration++;
										break;

							case RKEY_VolDown:							// Time down
										if ( currentTimer.duration > 0 ) currentTimer.duration--;
										break;
							default :
										break;
						}

						DisplayLine( chosenEditLine );					// Duration
						DisplayLine( 6 );								// End time
					    break;

				
		case 8 :														// repeat type
						switch ( key )
						{
							case RKEY_VolUp:							// Time up
										switch ( currentTimer.reservationType )
										{
											case RESERVE_TYPE_Onetime: 		currentTimer.reservationType = RESERVE_TYPE_Everyday; break;
											case RESERVE_TYPE_Everyday: 	currentTimer.reservationType = RESERVE_TYPE_EveryWeekend; break;
											case RESERVE_TYPE_EveryWeekend: currentTimer.reservationType = RESERVE_TYPE_Weekly; break;
											case RESERVE_TYPE_Weekly: 		currentTimer.reservationType = RESERVE_TYPE_WeekDay; break;
											case RESERVE_TYPE_WeekDay: 		currentTimer.reservationType = RESERVE_TYPE_Onetime; break;
											default : break;
										}
										break;

							case RKEY_VolDown:									// Time down
										switch ( currentTimer.reservationType )
										{
											case RESERVE_TYPE_Onetime: 		currentTimer.reservationType = RESERVE_TYPE_WeekDay; break;
											case RESERVE_TYPE_Everyday: 	currentTimer.reservationType = RESERVE_TYPE_Onetime; break;
											case RESERVE_TYPE_EveryWeekend: currentTimer.reservationType = RESERVE_TYPE_Everyday; break;
											case RESERVE_TYPE_Weekly: 		currentTimer.reservationType = RESERVE_TYPE_EveryWeekend; break;
											case RESERVE_TYPE_WeekDay: 		currentTimer.reservationType = RESERVE_TYPE_Weekly; break;
											default : break;
										}
										break;
							default :
										break;
						}

						DisplayLine( chosenEditLine );
						break;
				
				




		case 10 :														// Record or program
						switch ( key )
						{
							case RKEY_VolUp:	if ( commandOption < 2 ) commandOption++;
												else commandOption = 0;
												break;

							case RKEY_VolDown:	if ( commandOption > 0 ) commandOption--;
												else commandOption = 2;
												break;


							case RKEY_Ok :		switch ( commandOption )
												{
													case 0 :	TAP_Timer_Modify( order[timerLine-1], &currentTimer ); break;	// save
													case 1 :	break;															// cancel
													case 2 :	TAP_Timer_Delete( order[timerLine-1] );	break;					// delete
													default :	break;
												}

												CloseTimerEditWindow();		// Close the edit window
												returnFromEdit = TRUE;		// will cause a redraw of timer list
												break;

							default :			break;
						}

						DisplayLine( chosenEditLine );
					    break;
						
		default :		break;
	}
}


//------------
//
void TimerEditKeyHandler(dword key)
{
	int i, oldEditLine;

	oldEditLine = chosenEditLine;

    if ( channelListWindowShowing ) { ChannelListKeyHandler( key ); return; }		// handle channel list edit
    if ( calendarWindowShowing ) { CalendarKeyHandler( key ); return; }				// handle calendar
    if ( keyboardWindowShowing ) { KeyboardKeyHandler( key ); return; }				// handle calendar
	

	switch ( key )
	{
		case RKEY_ChDown :	commandOption = 0;
							if ( chosenEditLine < TIMER_LINES )
		    				{
							    chosenEditLine++;
								DisplayLine(chosenEditLine-1);
								DisplayLine(chosenEditLine);
							}
							else
							{
							    chosenEditLine=1;						// 0=hidden - can't hide once cursor moved
								DisplayLine(TIMER_LINES);
								DisplayLine(chosenEditLine);
							}
							break;

							
		case RKEY_ChUp :	commandOption = 0;
							if ( chosenEditLine > 1 )					// 0=hidden - can't hide once cursor moved
		    				{
							    chosenEditLine--;
								DisplayLine(chosenEditLine+1);
								DisplayLine(chosenEditLine);
							}
							else
							{
							    chosenEditLine = TIMER_LINES;
								DisplayLine(1);
								DisplayLine(chosenEditLine);
							}
								
							break;

							
		case RKEY_Forward :	if ( timerLine < maxShown )					// load next timer
		    				{
								timerLine++;
								CopyTimerFields(order[timerLine-1]);

								for ( i=1; i<=TIMER_LINES ; i++)		// redraw all fields
								{
								    DisplayLine(i);
								}
      						}
							break;

							
		case RKEY_Rewind :	if ( timerLine > 1 )						// load previous timer
		    				{
								timerLine--;
								CopyTimerFields(order[timerLine-1]);

								for ( i=1; i<=TIMER_LINES ; i++)		// redraw all fields
								{
								    DisplayLine(i);
								}
							}
							break;

		case RKEY_1 :
		case RKEY_2 :
		case RKEY_3 :
		case RKEY_4 :
		case RKEY_5 :
		case RKEY_6 :
		case RKEY_7 :
		case RKEY_8 :
		case RKEY_9 :		chosenEditLine = key - RKEY_0;				// direct keyboard selection of any line
							DisplayLine( oldEditLine );
							DisplayLine( chosenEditLine );
							commandOption = 0;							// will move the cursor to "Save" when last line selected
							break;


		case RKEY_0 :		chosenEditLine = 10;						// make "0" select the last line
							DisplayLine( oldEditLine );
							DisplayLine( chosenEditLine );
							break;

							
		case RKEY_Ok :
		case RKEY_TvRadio :
		case RKEY_VolUp :
		case RKEY_VolDown : EditLineKeyHandler(key);
							break;


		case RKEY_Recall :	CopyTimerFields( order[timerLine-1] );
							RedrawTimerEdit();
							break;
							

		case RKEY_Record :	TAP_Timer_Modify( order[timerLine-1], &currentTimer );
							CloseTimerEditWindow();					// Close the edit window
							returnFromEdit = TRUE;					// will cause a redraw of timer list
							break;


		case RKEY_Exit : 	CloseTimerEditWindow();					// Close the edit window
							returnFromEdit = TRUE;					// will cause a redraw of timer list
							break;
							

		default :
			break;
	}
}



//--------------------------------------------------
//
void MakeNewTimer( byte chosenWeekDay )
{
	byte 	hour, min, sec, month, day, weekDay;
	word 	year, mjd, currentMJD;
	int currentSvcType, currentSvcNum;
	int count;

	TAP_GetTime( &currentMJD, &hour, &min, &sec);
	TAP_ExtractMjd( currentMJD, &year, &month, &day, &weekDay) ;
																		// work out which date this timer should be added on
	if ( chosenWeekDay == 7 ) mjd = currentMJD;									// all day -> select today
	ef ( chosenWeekDay >= weekDay) mjd = currentMJD + chosenWeekDay-weekDay;	// later this week
	else mjd = currentMJD + chosenWeekDay+7-weekDay;							// next week

	min += 10;															// must set the timer in to the future, or it will get deleted
	if ( min >= 60 ) { min -= 60; hour++; }								// 10 minutes should do the trick
	if ( hour >= 24 ) { hour -= 24; mjd++; }
	
	
    TAP_Channel_GetCurrent( &currentSvcType, &currentSvcNum );			// default to whatever the current station is

    currentTimer.isRec = 1;
    currentTimer.tuner = 3;
    currentTimer.svcType = currentSvcType;
    currentTimer.reserved = 0;											// name not fixed until edited
    currentTimer.svcNum = currentSvcNum;
    currentTimer.reservationType = RESERVE_TYPE_Onetime;
    currentTimer.nameFix = 0;
    currentTimer.duration = 30;
    currentTimer.startTime = (mjd << 16) | (hour << 8) | min;
    strcpy(currentTimer.fileName, "New Timer.rec");

	count=0;
	while ( (TAP_Timer_Add( &currentTimer ) != 0) && (count<256) )		// repeat until timer add succeeds
	{																	// if a conflict is detected
		hour++;															// just add 1 to the hour
		if ( hour >= 24 ) { hour = 0; mjd++; }							// and try again
		currentTimer.startTime = (mjd << 16) | (hour << 8) | min;
//		TAP_Delay( 50 );
		count++;
	}
}



bool TimerEdit (int line)
{
	int i;

	timerLine = line;
	CopyTimerFields(order[timerLine-1]);
	
	CreateTimerEditWindow();
	UpdateListClock();
	
	chosenEditLine = 10;		// select the command options
	commandOption = 1;			// and default to "cancel"

	for ( i=1; i<=TIMER_LINES ; i++)
	{
	    DisplayLine(i);
	}

	PrintTimerExtraInfo();
}

void initialiseTimerEdit(void)
{
    editWindowShowing = FALSE;
	timeEditExitWindowShowing = FALSE;
	
	channelListWindowShowing = FALSE;
	calendarWindowShowing = FALSE;

	chosenEditLine = 0;
	commandOption = 0;
	returnFromEdit = FALSE;

	InitialiseKeyboard();
}



