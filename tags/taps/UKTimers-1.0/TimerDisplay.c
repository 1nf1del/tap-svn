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
// Doesn't deal with timers firing in the background - timer list can change at any point!!!


#define MAX_TIMERS 255
#define MAX_START_TIME 0xffffffff
#define DEFAULT_DAY 7
#define NUMBER_OF_LINES 10
#define Y1_OFFSET 36
#define Y1_STEP 42					// was 44


static TYPE_Window Win;


static bool windowShowing;
static int order[MAX_TIMERS];
static char chosenDay;
static int chosenLine;
static int page;
static int maxShown;
static int timerCount;


static byte rowSelection;			// test code



//------------
//
void PrintCenter( word windowRgn, dword x, dword y, dword maxX, const char *str, dword fcolor, dword bcolor, byte fntSize)
{
	dword width, offset;

	width = TAP_Osd_GetW( str, 0, fntSize );

	if ( width <= (maxX-x) ) offset = (maxX - x - width)/2;				// centralise text
	else offset = 5;													// too wide - fill width

	switch ( fntSize )
	{
		case FNT_Size_1419 : if ( bcolor > 0 ) TAP_Osd_FillBox( windowRgn, x, y-1, maxX-x, 21, bcolor );
							 TAP_Osd_PutStringAf1419( windowRgn, x+offset, y, maxX, str, fcolor, bcolor);
							 break;
							
		case FNT_Size_1622 : if ( bcolor > 0 ) TAP_Osd_FillBox( windowRgn, x, y-1, maxX-x, 24, bcolor );
							 TAP_Osd_PutStringAf1622( windowRgn, x+offset, y, maxX, str, fcolor, bcolor);
							 break;
							 
		case FNT_Size_1926 : if ( bcolor > 0 ) TAP_Osd_FillBox( windowRgn, x, y-1, maxX-x, 28, bcolor );
							 TAP_Osd_PutStringAf1926( windowRgn, x+offset, y, maxX, str, fcolor, bcolor);
							 break;
							 
		default : break;
	}

	
}


//------------
//
void SortOrder(void)
{
	int 	i, l, count, firstTimer;
	dword 	firstStartTime, startTime, mjd;
	
	byte 	weekDay, month, day, hour, min, sec;
	word	year;
	
	byte 	currentWeekDay, currentMonth, currentDay, currentHour, currentMin, currentSec;
	word 	currentYear, currentMJD;
	
	word 	chosenMJD;

	bool 	seenTimer[MAX_TIMERS];
	TYPE_TimerInfo	currentTimer;
	bool 	flag, showFlag;

	TAP_GetTime( &currentMJD, &currentHour, &currentMin, &currentSec );
	TAP_ExtractMjd( currentMJD, &currentYear, &currentMonth, &currentDay, &currentWeekDay) ;

	if ( chosenDay >= currentWeekDay ) chosenMJD = currentMJD + (chosenDay - currentWeekDay);
	else chosenMJD = currentMJD + (chosenDay - currentWeekDay) + 7;		// calculate date on selected day - this week, or next


	for ( i=0; i<MAX_TIMERS ; i++)
	{
		order[i] = 0;													// initialise the order array
		seenTimer[i] = FALSE;
	}
		
	timerCount = TAP_Timer_GetTotalNum();
	count = 1;

	for ( l=0; l<timerCount; l++)										// build an ordered list of timers, eariest=0
	{
		firstStartTime = MAX_START_TIME;
		flag = FALSE;

		for ( i=0; i<timerCount ; i++)
		{
			TAP_Timer_GetInfo(i, &currentTimer);						// Get this timer's info
			mjd = currentTimer.startTime>>16;
			TAP_ExtractMjd( mjd, &year, &month, &day, &weekDay) ;			

			if ( chosenDay == 7 )										// ALL listing -> no need to process the start time
			{
				if ( (currentTimer.startTime < firstStartTime) && (seenTimer[i] == FALSE) )	// check we haven't used this timer before
				{
					firstStartTime = currentTimer.startTime;			// this timer is earlier than current eariest
					firstTimer = i;										// remember it's number
					flag = TRUE;
				}
			}
			
			else														// must take a different approach if the list is filtered
			{
                showFlag = FALSE;

				switch (currentTimer.reservationType)					// manipulate repeat timers, so they appear correctly in the filtered list
				{
					case RESERVE_TYPE_Onetime:		if (weekDay == chosenDay) showFlag = TRUE;
													break;
					
					case RESERVE_TYPE_Everyday: 	if (mjd < chosenMJD) mjd = chosenMJD;
													showFlag = TRUE;
													break;
													
					case RESERVE_TYPE_EveryWeekend:	if (chosenDay >= 5)
					    							{
													    if (mjd < chosenMJD) mjd = chosenMJD;
														showFlag = TRUE;
													}
													break;
													
					case RESERVE_TYPE_Weekly:		if (weekDay == chosenDay) showFlag = TRUE;
													break;
					
					case RESERVE_TYPE_WeekDay:		if (chosenDay <= 4 )
					    							{
													    if (mjd < chosenMJD) mjd = chosenMJD;
														showFlag = TRUE;
													}
													break;
					default : break;
				}

				startTime = (mjd<<16) | (currentTimer.startTime&0xffff);
				TAP_ExtractMjd( mjd, &year, &month, &day, &weekDay) ;
				 															// check this timer activated today, and we haven't used this timer before
				if ( (showFlag == TRUE) && (startTime < firstStartTime) && (seenTimer[i] == FALSE) )
				{
					firstStartTime = startTime;								// this timer is earlier than current eariest
					firstTimer = i;											// remember it's number
					flag = TRUE;
				}

			}

		}
		order[l]=firstTimer;												// create the order list, 1 timers at a time.
		seenTimer[firstTimer] = TRUE;										// cross the timers off the list, as we go

		if (flag == TRUE) count++;
	}
	maxShown = count - 1;
}



//------------
//
void DrawGraphicBoarders(void)
{
    TAP_Osd_FillBox( rgn, 0, 0, 720, 576, FILL_COLOUR );				// clear the screen
    TAP_Osd_PutGd( rgn, 0, 0, &_topGd, TRUE );							// draw top graphics
    TAP_Osd_PutGd( rgn, 0, 0, &_sideGd, TRUE );							// draw left side graphics
	TAP_Osd_PutGd( rgn, 672, 0, &_sideGd, TRUE );						// draw right side graphics
}


	
//------------
//
void CreateTimerWindow(void)
{
	byte 	currentWeekDay, currentMonth, currentDay, currentHour, currentMin, currentSec;
	word 	currentYear, currentMJD;
	char	str[80];

	windowShowing = TRUE;
	DrawGraphicBoarders();

	TAP_GetTime( &currentMJD, &currentHour, &currentMin, &currentSec );
	TAP_ExtractMjd( currentMJD, &currentYear, &currentMonth, &currentDay, &currentWeekDay) ;
	
	chosenDay = currentWeekDay;											// default: show only today's timers
	chosenLine = 0;														// default: highlight the 1st timer
	page = 0;
}


void CloseTimerWindow( void )
{
	windowShowing = FALSE;
}



//------------
//
void DrawBackground(void)
{
	int		i;
	char	str[80];

	switch( chosenDay )
	{
	    case 0 : TAP_SPrint( str, "Monday's Recording Schedule" ); break;
	    case 1 : TAP_SPrint( str, "Tuesday's Recording Schedule" ); break;
	    case 2 : TAP_SPrint( str, "Wednesday's Recording Schedule" ); break;
	    case 3 : TAP_SPrint( str, "Thursday's Recording Schedule" ); break;
	    case 4 : TAP_SPrint( str, "Friday's Recording Schedule" ); break;
	    case 5 : TAP_SPrint( str, "Saturday's Recording Schedule" ); break;
	    case 6 : TAP_SPrint( str, "Sunday's Recording Schedule" ); break;
	    case 7 : TAP_SPrint( str, "Recording Schedule : All events" ); break;
	}
	TAP_Osd_PutStringAf1926( rgn, 58, 40, 390, str, TITLE_COLOUR, COLOR_Black );
}



//------------
//
void DisplayTimerText(int line, int i)
{
    char	str[80], str2[80], str3[80];
	int 	l;
	bool	delayedEvent;
    
	byte	hour, min, sec, month, day, weekDay;
	word	year, startTime;
	int		endHour, endMin;

	byte 	currentWeekDay, currentMonth, currentDay, currentHour, currentMin, currentSec;
	word 	currentYear, currentMJD, currentTime;
	
	word 	chosenMJD, mjd;
	
	TYPE_TimerInfo	currentTimer;
	TYPE_TapChInfo	currentChInfo;

	if ( line == 0 ) return;											// bounds check
    
	TAP_Timer_GetInfo(order[line-1], &currentTimer); 					// use the order array - eariest timer will be first


// Record or program
	if ( currentTimer.isRec == 1 )
	{
	    TAP_Osd_PutGd( rgn, 54, i*Y1_STEP+Y1_OFFSET-8, &_redcircleGd, TRUE );
		TAP_Osd_PutStringAf1622( rgn, 63, i*Y1_STEP+Y1_OFFSET, 90, "R", MAIN_TEXT_COLOUR, 0 );
	}
	else
	{
	    TAP_Osd_PutGd( rgn, 54, i*Y1_STEP+Y1_OFFSET-8, &_greencircleGd, TRUE );
		TAP_Osd_PutStringAf1622( rgn, 63-2, i*Y1_STEP+Y1_OFFSET+1, 90, "W", MAIN_TEXT_COLOUR, 0 );
	}

	
// programme name
	TAP_SPrint(str,"%s", currentTimer.fileName);

	for ( l = strlen(str)-4 ; l < strlen(str) ; l++ )
	{
	    str[l]='\0';
	}
	TAP_Osd_PutStringAf1622( rgn, 93, i*Y1_STEP+Y1_OFFSET, 370, str, MAIN_TEXT_COLOUR, 0 );


	
// start and end time
	hour = (currentTimer.startTime&0xff00)>>8;							// extract the time
	min = (currentTimer.startTime&0xff);

	endMin = min + currentTimer.duration;								// add the duration in miutes

	endHour = hour + endMin/60;											// should we have increamented the hour
	if ( endHour >= 24 ) endHour -= 24;									// did the day roll over ?

	endMin = endMin%60;													// Finally remove the hours from the minutes field.
				
	TAP_SPrint(str, "%02d:%02d ~ %02d:%02d", hour, min, endHour, endMin);
	PrintCenter( rgn, 375, i*Y1_STEP+Y1_OFFSET, 495, str, MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );

	
	
// repeat type and date
	mjd = currentTimer.startTime>>16;
	TAP_ExtractMjd( mjd, &year, &month, &day, &weekDay) ;
	
	TAP_GetTime( &currentMJD, &currentHour, &currentMin, &currentSec );
	TAP_ExtractMjd( currentMJD, &currentYear, &currentMonth, &currentDay, &currentWeekDay) ;
	currentTime = (currentHour<<8) | currentMin;
	startTime = currentTimer.startTime & 0xffff;

	if ( chosenDay >= currentWeekDay ) chosenMJD = currentMJD + (chosenDay - currentWeekDay);
	else chosenMJD = currentMJD + (chosenDay - currentWeekDay) + 7;		// calculate date on selected day - this week, or next	

	delayedEvent = TRUE;
	
	switch ( currentTimer.reservationType )
	{
		case RESERVE_TYPE_Onetime:		if (( chosenDay != 7 ) && ( mjd <= chosenMJD )) delayedEvent = FALSE;
										if (( chosenDay == 7 ) && ( mjd < currentMJD+7 )) delayedEvent = FALSE;
										if (( chosenDay == 7 ) && ( mjd == currentMJD+7 ) && ( startTime <= currentTime )) delayedEvent = FALSE;
										break;

		case RESERVE_TYPE_Everyday:		if ( mjd <= currentMJD ) delayedEvent = FALSE;
										if (( startTime <= currentTime ) && ( mjd <= currentMJD+1 )) delayedEvent = FALSE;
										break;
		
		case RESERVE_TYPE_EveryWeekend:	if ( chosenDay != 7 )
		    							{
                                            if (  mjd <= chosenMJD ) delayedEvent = FALSE;
											if (( startTime <= currentTime ) && ( mjd <= chosenMJD+1 )) delayedEvent = FALSE;

										}
										else
										{
											if ( currentWeekDay < 5 )		// today is a weekday
											{								// check event is valid for this Saturday
	                                            if (  mjd <= (currentMJD+(5-currentWeekDay)) ) delayedEvent = FALSE;
											}

											if ( currentWeekDay == 5 )		// today is a Saturday
											{								// check event is valid today
	                                            if (  mjd <= currentMJD ) delayedEvent = FALSE;
												if (( startTime <= currentTime ) && ( mjd <= currentMJD+1 )) delayedEvent = FALSE;
											}

											if ( currentWeekDay > 5 )		// today is a Sunday
											{								// check event is valid today, or next Saturday
	                                            if (  mjd <= currentMJD ) delayedEvent = FALSE;
												if (( startTime <= currentTime ) && ( mjd <= currentMJD+6 )) delayedEvent = FALSE;
											}
										}
										break;

		case RESERVE_TYPE_Weekly:		if ( chosenDay != 7 )
		    							{
											if (( startTime <= currentTime ) && ( mjd <= currentMJD+7 )) delayedEvent = FALSE;
											if ( !( startTime <= currentTime ) && ( mjd <= chosenMJD )) delayedEvent = FALSE;
										}
										else
										{
											if ( mjd <= currentMJD+7 ) delayedEvent = FALSE;
										}
										break;
										
		case RESERVE_TYPE_WeekDay:		if ( chosenDay != 7 )
		    							{
											if ( mjd <= chosenMJD ) delayedEvent = FALSE;
											if (( startTime <= currentTime ) && ( mjd <= currentMJD+1 )) delayedEvent = FALSE;
										}
										else
										{
											if ( currentWeekDay < 4 )		// today is a weekday (Mon-Thu)
											{								// check event is valid for today
	                                            if (  mjd <= currentMJD ) delayedEvent = FALSE;
												if (( startTime <= currentTime ) && ( mjd <= currentMJD+1 )) delayedEvent = FALSE;
											}

											if ( currentWeekDay == 4 )		// today is a Friday
											{								// check event is valid for today, or Monday
	                                            if (  mjd <= currentMJD ) delayedEvent = FALSE;
												if (( startTime <= currentTime ) && ( mjd <= currentMJD+3 )) delayedEvent = FALSE;
											}

											if ( currentWeekDay > 4 )		// today is a weekend
											{								// check event is valid Monday
	                                            if (  mjd <= (currentMJD+(7-currentWeekDay)) ) delayedEvent = FALSE;
											}
										}
										break;
		default : break;
	}

	switch ( weekDay )													// set up some display strings
	{
		case 0:	TAP_SPrint(str2,"Mon"); break;
		case 1:	TAP_SPrint(str2,"Tue"); break;
		case 2:	TAP_SPrint(str2,"Wed"); break;
		case 3:	TAP_SPrint(str2,"Thu"); break;
		case 4:	TAP_SPrint(str2,"Fri"); break;
		case 5:	TAP_SPrint(str2,"Sat"); break;
		case 6:	TAP_SPrint(str2,"Sun"); break;
		default : TAP_SPrint(str2,"OT BAD"); break;
	}

	switch ( month )
	{
		case 1:	TAP_SPrint(str3,"Jan"); break;
		case 2:	TAP_SPrint(str3,"Feb"); break;
		case 3:	TAP_SPrint(str3,"Mar"); break;
		case 4:	TAP_SPrint(str3,"Apr"); break;
		case 5:	TAP_SPrint(str3,"May"); break;
		case 6:	TAP_SPrint(str3,"Jun"); break;
		case 7:	TAP_SPrint(str3,"Jul"); break;
		case 8:	TAP_SPrint(str3,"Aug"); break;
		case 9:	TAP_SPrint(str3,"Sep"); break;
		case 10:TAP_SPrint(str3,"Oct"); break;
		case 11:TAP_SPrint(str3,"Nov"); break;
	   	case 12:TAP_SPrint(str3,"Dec"); break;
		default : TAP_SPrint(str3,"BAD"); break;
	}

	
	if ( !delayedEvent )												// normal event, and NOT selected "display all events"
	{
		switch ( currentTimer.reservationType )
		{
			case RESERVE_TYPE_Onetime:      if ( chosenDay !=7 ) TAP_SPrint(str,"%d %s", day, str3);
											else TAP_SPrint(str,"%s %d OT", str2, day, str3);
											break;
											

			case RESERVE_TYPE_Everyday: 	if ( chosenDay !=7 ) TAP_SPrint(str,"Everyday");
											else TAP_SPrint(str,"%s %d ED", str2, day, str3);
											break;
											
			
			case RESERVE_TYPE_EveryWeekend:	if ( chosenDay !=7 ) TAP_SPrint(str,"Sat & Sun");
											else TAP_SPrint(str,"%s %d WE", str2, day, str3);
											break;
											
			
			case RESERVE_TYPE_Weekly:		if ( chosenDay !=7 )
											{
												switch ( weekDay )
												{
													case 0:	TAP_SPrint(str,"Mondays"); break;
													case 1:	TAP_SPrint(str,"Tuesdays"); break;
													case 2:	TAP_SPrint(str,"Wednesdays"); break;
													case 3:	TAP_SPrint(str,"Thursdays"); break;
													case 4:	TAP_SPrint(str,"Fridays"); break;
													case 5:	TAP_SPrint(str,"Saturdays"); break;
													case 6:	TAP_SPrint(str,"Sundays"); break;
													default : TAP_SPrint(str,"WL BAD"); break;
												}
											}
											else TAP_SPrint(str,"%s %d WL", str2, day, str3);
											break;

											
			case RESERVE_TYPE_WeekDay:		if ( chosenDay !=7 ) TAP_SPrint(str,"Mon - Fri");
											else TAP_SPrint(str,"%s %d WD", str2, day, str3);
											break;
			default : break;
		}
		PrintCenter( rgn, 499, i*Y1_STEP+Y1_OFFSET, 602, str, MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );		//print
	}
	else															 // delayed start
	{
		switch ( currentTimer.reservationType )
		{
			case RESERVE_TYPE_Onetime: 		TAP_SPrint(str,"Onetime"); TAP_SPrint(str2,"on"); break;
			case RESERVE_TYPE_Everyday: 	TAP_SPrint(str,"Everyday");TAP_SPrint(str2,"from");  break;
			case RESERVE_TYPE_EveryWeekend:	TAP_SPrint(str,"Weekends");TAP_SPrint(str2,"from");  break;
			case RESERVE_TYPE_Weekly:		TAP_SPrint(str,"Weekly");  TAP_SPrint(str2,"from"); break;
			case RESERVE_TYPE_WeekDay:		TAP_SPrint(str,"Weekdays");TAP_SPrint(str2,"from"); break;
			default : break;
		}
		PrintCenter( rgn, 499, i*Y1_STEP+Y1_OFFSET-8, 602, str, MAIN_TEXT_COLOUR, 0, FNT_Size_1419 );	//print

		switch ( weekDay )
		{
			case 0:	TAP_SPrint(str,"%s Mon %d", str2, day); break;
			case 1:	TAP_SPrint(str,"%s Tue %d", str2, day); break;
			case 2:	TAP_SPrint(str,"%s Wed %d", str2, day); break;
			case 3:	TAP_SPrint(str,"%s Thu %d", str2, day); break;
			case 4:	TAP_SPrint(str,"%s Fri %d", str2, day); break;
			case 5:	TAP_SPrint(str,"%s Sat %d", str2, day); break;
			case 6:	TAP_SPrint(str,"%s Sun %d", str2, day); break;
			default : TAP_SPrint(str,"BAD %d", day); break;
		}
		PrintCenter( rgn, 499, i*Y1_STEP+Y1_OFFSET+11, 602, str, MAIN_TEXT_COLOUR, 0, FNT_Size_1419 );	//print
	}



// channel logo
	DisplayLogo( rgn, 607, i*Y1_STEP+Y1_OFFSET-8, currentTimer.svcNum, currentTimer.svcType );
}



//------------
//
void DisplayTimerLine(int line)
{
	int i;

	if ( line == 0 ) return;											// bounds check
	
	i = ((line-1) % NUMBER_OF_LINES) + 1;								// calculate position on page

	if ( chosenLine == line )											// highlight the current cursor line
    {
//	    TAP_Osd_PutGd( rgn, 53, i*Y1_STEP+Y1_OFFSET-8, &_highlightGd, FALSE );

        switch (rowSelection)											// test code
		{
			case 0 :
			case 1 : TAP_Osd_PutGd( rgn, 53, i*Y1_STEP+Y1_OFFSET-8, &_highlightGd, FALSE ); break;
			case 2 : TAP_Osd_PutGd( rgn, 53, i*Y1_STEP+Y1_OFFSET-8, &_rowbGd, FALSE ); break;
		}
	}
	else
	{
//	    TAP_Osd_PutGd( rgn, 53, i*Y1_STEP+Y1_OFFSET-8, &_rowGd, FALSE );

        switch (rowSelection)											// test code
		{
			case 0 : TAP_Osd_PutGd( rgn, 53, i*Y1_STEP+Y1_OFFSET-8, &_rowaGd, FALSE ); break;
			case 1 : TAP_Osd_PutGd( rgn, 53, i*Y1_STEP+Y1_OFFSET-8, &_rowbGd, FALSE ); break;
			case 2 : TAP_Osd_PutGd( rgn, 53, i*Y1_STEP+Y1_OFFSET-8, &_highlightGd, FALSE ); break;
		}
	}

	if ( line <= maxShown )	DisplayTimerText(line, i);								// anything to display ?

    TAP_Osd_FillBox( rgn, 85, i*Y1_STEP+Y1_OFFSET-8, 3, Y1_STEP, FILL_COLOUR );		// draw the column seperators
    TAP_Osd_FillBox( rgn, 372, i*Y1_STEP+Y1_OFFSET-8, 3, Y1_STEP, FILL_COLOUR );	// temporary code - will eventually place in graphics
    TAP_Osd_FillBox( rgn, 496, i*Y1_STEP+Y1_OFFSET-8, 3, Y1_STEP, FILL_COLOUR );	// once positions fixed.
    TAP_Osd_FillBox( rgn, 604, i*Y1_STEP+Y1_OFFSET-8, 3, Y1_STEP, FILL_COLOUR );
}



//------------
//
void DrawTimerList(void)
{
	int		i, start, end;

	DrawBackground();

	start = (page * NUMBER_OF_LINES)+1;

	for ( i=start; i<start+NUMBER_OF_LINES ; i++)
	{
		DisplayTimerLine(i);
	}
}


//------------
//
void UpdateSelectionNumber(void)
{
    char	str[80], str2[80], str3[80], str4[80], str5[80];
	int 	l;

	byte	hour, min, sec, month, day, weekDay;
	word	year, mjd;
	int		endHour, endMin;

	TYPE_TimerInfo	currentTimer;
	TYPE_TapChInfo	currentChInfo;

	dword textColour;


    TAP_Osd_FillBox( rgn, 53, 490, 614, 86, FILL_COLOUR );		// clear the bottom portion

	if ( chosenLine > 0 )												// update, or blank the last line
	{
		TAP_Timer_GetInfo(order[chosenLine-1], &currentTimer); 			// use the order array - eariest timer will be first
		mjd = currentTimer.startTime>>16;
		TAP_ExtractMjd( mjd, &year, &month, &day, &weekDay) ;

// file name
		TAP_SPrint(str2,"%s", currentTimer.fileName);
		for ( l = strlen(str2)-4 ; l < strlen(str2) ; l++ )
		{
		    str2[l]='\0';
		}
	    TAP_SPrint(str,"Event %d of %d : %s", chosenLine, maxShown, str2 );
		TAP_Osd_PutStringAf1622( rgn, 58, 493, 666, str, INFO_COLOUR, FILL_COLOUR );

// start date
		switch ( weekDay )
		{
			case 0:	TAP_SPrint(str2,"Mon %d/%d/%d", day, month, year); TAP_SPrint(str3,"Monday"); break;
			case 1:	TAP_SPrint(str2,"Tue %d/%d/%d", day, month, year); TAP_SPrint(str3,"Tuesday"); break;
			case 2:	TAP_SPrint(str2,"Wed %d/%d/%d", day, month, year); TAP_SPrint(str3,"Wednesday"); break;
			case 3:	TAP_SPrint(str2,"Thu %d/%d/%d", day, month, year); TAP_SPrint(str3,"Thursday"); break;
			case 4:	TAP_SPrint(str2,"Fri %d/%d/%d", day, month, year); TAP_SPrint(str3,"Friday"); break;
			case 5:	TAP_SPrint(str2,"Sat %d/%d/%d", day, month, year); TAP_SPrint(str3,"Saturday"); break;
			case 6:	TAP_SPrint(str2,"Sun %d/%d/%d", day, month, year); TAP_SPrint(str3,"Sunday"); break;
			default : TAP_SPrint(str2,"BAD %d/%d/%d", day, month, year);  TAP_SPrint(str3,"BAD");break;
		}

// start time
		hour = (currentTimer.startTime&0xff00)>>8;						// extract the time
		min = (currentTimer.startTime&0xff);

		endMin = min + currentTimer.duration;							// add the duration in miutes

		endHour = hour + endMin/60;										// should we have incremented the hour
		if ( endHour >= 24 ) endHour -= 24;								// did the day roll over ?

		endMin = endMin%60;												// Finally remove the hours from the minutes field.
				
		TAP_SPrint(str4, "%02d:%02d (%d mins)", hour, min, currentTimer.duration);

// channel		
		if ( TAP_Channel_GetInfo( currentTimer.svcType, currentTimer.svcNum, &currentChInfo ) )
			TAP_SPrint(str5,"%s", currentChInfo.chName);
		else
		    TAP_SPrint(str5,"BAD ch", currentChInfo.chName);


// repeat type - and format whole string
		switch ( currentTimer.reservationType )
		{
			case RESERVE_TYPE_Onetime: 		TAP_SPrint(str,"Onetime on %s %d/%d/%d at %s on %s", str3, day, month, year, str4, str5); break;
			case RESERVE_TYPE_Everyday: 	TAP_SPrint(str,"Everyday at %s from %s on %s", str4, str2, str5); break;
			case RESERVE_TYPE_EveryWeekend:	TAP_SPrint(str,"Weekends at %s from %s on %s", str4, str2, str5); break;
			case RESERVE_TYPE_Weekly:		TAP_SPrint(str,"Weekly on %ss at %s from %d/%d/%d on %s", str3, str4, day, month, year, str5); break;
			case RESERVE_TYPE_WeekDay:		TAP_SPrint(str,"Weekdays at %s from %s on %s", str4, str2, str5); break;
			default : 						TAP_SPrint(str,"Undefined at %s from %s on %s", str4, str5, str5); break;
		}
		TAP_Osd_PutStringAf1622( rgn, 58, 518, 666, str, INFO_COLOUR, FILL_COLOUR );	// print the complete second line in one go


		switch( currentTimer.tuner)										// sneek in the tuner number for now
		{
		    case 0 : TAP_SPrint(str,"tuner 1");
            		 textColour = COLOR_DarkRed;
					 break;
					 
		    case 1 : TAP_SPrint(str,"tuner 2");
            		 textColour = COLOR_DarkRed;
					 break;

		    case 3 : TAP_SPrint(str,"tuner 4");
            		 textColour = COLOR_DarkGray;
					 break;
			
		    default : TAP_SPrint(str,"Bad %d", currentTimer.tuner + 1);
            		 textColour = COLOR_Red;
					 break;			
		}
		TAP_Osd_PutStringAf1419( rgn, 606, 496, 666, str, textColour, FILL_COLOUR );

	}
	else
	{
		switch ( chosenDay )
		{
			case 0:	TAP_SPrint(str2,"Monday"); break;
			case 1:	TAP_SPrint(str2,"Tuesday"); break;
			case 2:	TAP_SPrint(str2,"Wednesday"); break;
			case 3:	TAP_SPrint(str2,"Thursday"); break;
			case 4:	TAP_SPrint(str2,"Friday"); break;
			case 5:	TAP_SPrint(str2,"Saturday"); break;
			case 6:	TAP_SPrint(str2,"Sunday"); break;
			default : TAP_SPrint(str2,"BAD"); break;
		}

		if ( chosenDay < 7 ) TAP_SPrint(str,"%d Events Scheduled for %s", maxShown, str2 );
		else TAP_SPrint(str,"%d Scheduled Events", maxShown);
		
		TAP_Osd_PutStringAf1622( rgn, 58, 509, 614, str, INFO_COLOUR, FILL_COLOUR );
	}
}



//------------
//
void TimerWindowKeyHandler(dword key)
{
	byte 	hour, min, sec, month, day, weekDay;
	word 	year, mjd;

	int		oldPage, oldChosenLine;
	oldPage = page;
	oldChosenLine = chosenLine;
	
	switch ( key )
	{
		case RKEY_Exit :	exitFlag = TRUE;										// signal exit to top level - will clean up, close window,
							break;													// and enter the normal state
							
		case RKEY_White :	if ( mainActivationKey == RKEY_White ) exitFlag = TRUE;	// only use WHITE to close if it's also set to open
							break;


		case RKEY_VolUp :
		case RKEY_Yellow :	if ( chosenDay < 7 ) chosenDay++; else chosenDay=0;
							chosenLine = 0;
							page = 0;
							SortOrder();											// change the sort order based on the chosen day
							DrawTimerList();
							UpdateSelectionNumber();
						    break;

		case RKEY_VolDown :
		case RKEY_Green :	if ( chosenDay > 0 ) chosenDay--; else chosenDay=7;
							chosenLine = 0;
							page = 0;
							SortOrder();											// change the sort order based on the chosen day
							DrawTimerList();
							UpdateSelectionNumber();
						    break;

		case RKEY_PlayList :
		case RKEY_TvRadio :	chosenDay=7;											// shortcut key - display all timers
							chosenLine = 0;
							page = 0;
							SortOrder();
							DrawTimerList();
							UpdateSelectionNumber();
						    break;
							
		case RKEY_ChDown :	if ( chosenLine < maxShown ) chosenLine++; else chosenLine=1;	// 0=hidden - can't hide once cursor moved
							if ( maxShown == 0 ) chosenLine = 0;					// don't move the cursor if not timers shown
							
							page = (chosenLine-1) / NUMBER_OF_LINES;

							if ( page == oldPage )									// only redraw what's nessesary
							{
								if ( oldChosenLine > 0 ) DisplayTimerLine(oldChosenLine);
								DisplayTimerLine(chosenLine);
							}
							else DrawTimerList();

							UpdateSelectionNumber();
							break;

		case RKEY_ChUp :	if ( chosenLine > 1 ) chosenLine--; else chosenLine = maxShown;
							if ( maxShown == 0 ) chosenLine = 0;					// not strictly needed - included in above logic
							
							page = (chosenLine-1) / NUMBER_OF_LINES;

							if ( page == oldPage )									// only redraw what's nessesary
							{
								if ( oldChosenLine > 0 ) DisplayTimerLine(oldChosenLine);
								DisplayTimerLine(chosenLine);
							}
							else DrawTimerList();

							UpdateSelectionNumber();
							break;

		case RKEY_Forward : if ( page == ((maxShown-1) / NUMBER_OF_LINES) )			// page down
							{
								chosenLine = maxShown;
							}
							else
							{
							    page = page + 1;
							    chosenLine = (page * NUMBER_OF_LINES) + 1;			// will land only on top of page
							}
							DrawTimerList();
							UpdateSelectionNumber();
							break;

		case RKEY_Rewind :	if ( page > 0 )											// page up
							{
							    page = page - 1;
							    chosenLine = (page * NUMBER_OF_LINES) + 1;			// will land only on bottom of page
							}
							else
							{
							    page = 0;
								chosenLine = 1;
							}
							DrawTimerList();
							UpdateSelectionNumber();
							break;

							
		case RKEY_1 :
		case RKEY_2 :
		case RKEY_3 :
		case RKEY_4 :
		case RKEY_5 :
		case RKEY_6 :
		case RKEY_7 :
		case RKEY_8 :
		case RKEY_9 :		chosenLine = (key - RKEY_0) + (page * NUMBER_OF_LINES);		// direct keyboard selection of any line
							if ( chosenLine > maxShown ) chosenLine = maxShown;

							DisplayTimerLine( oldChosenLine );
							DisplayTimerLine( chosenLine );
							UpdateSelectionNumber();							
							break;
							
		case RKEY_0 :
							chosenLine = (10) + (page * NUMBER_OF_LINES);				// make "0" select the 10th (last) line
							if ( chosenLine > maxShown ) chosenLine = maxShown;

							DisplayTimerLine( oldChosenLine );
							DisplayTimerLine( chosenLine );
							UpdateSelectionNumber();							
							break;


		case RKEY_Ok :		if ( chosenLine > 0 ) TimerEdit(chosenLine);
							break;
							

		case RKEY_Recall :	TAP_GetTime( &mjd, &hour, &min, &sec);						// shortcut key - display today's timers
							TAP_ExtractMjd( mjd, &year, &month, &day, &weekDay) ;

							chosenDay = weekDay;
							chosenLine = 0;
							page = 0;
							SortOrder();
							DrawTimerList();
							UpdateSelectionNumber();
						    break;

							
		case RKEY_Red :		MakeNewTimer( chosenDay );									// Create a new timer
							SortOrder();												// currently no way to directly jump to edit
							DrawTimerList();											// so just refresh the display
							UpdateSelectionNumber();									// user must manually edit
		
							break;

							
		case RKEY_Menu :	ActivateMenu();
						    break;
							
		default :			break;
	}
}



//---------------------------
//
void ActivateTimerWindow( void )
{
    CreateTimerWindow();
	SortOrder();														// sort the timers in order of start time
	
	DrawTimerList();
	UpdateSelectionNumber();
	UpdateListClock();
}



void initialiseTimerWindow( void )
{
    windowShowing = FALSE;
	chosenDay = DEFAULT_DAY;
	chosenLine = 0;
	page = 0;
	rowSelection = 0;
}



void RefreshTimerWindow( void )
{
	SortOrder();														// sort the timers in order of start time
	while ( chosenLine > maxShown ) chosenLine--;						// cater for delete of the last timer - move up one, or clear hightlight if = 0
	page = (chosenLine-1) / NUMBER_OF_LINES;
							
	DrawGraphicBoarders();
	DrawTimerList();
	UpdateSelectionNumber();
	UpdateListClock();
}
