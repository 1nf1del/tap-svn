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


#include "graphics/Calendar.gd"
//#include "graphics/CalendarDayBar.gd"
#include "graphics/DateHighlight.gd"
#include "graphics/DateOverStamp.gd"

#define MONTH_BACKGROUND_COLOUR RGB8888(90,104,212)

#define CAL_BASE_X	140
#define CAL_BASE_Y	70
#define CAL_STEP_X	60
#define CAL_STEP_Y	51



typedef enum
{
	Cal_Background,
	Cal_Month,
	Cal_Year,
	Cal_Day,
	Cal_Date,
	Cal_DateBar,
	Cal_Highlight
} TYPE_CalendarPosition;

static byte *savedRegion;

//------------------------------------------------------------------
//
void CalcCalendarPosition( dword *x, dword *y, TYPE_CalendarPosition control, byte row, byte column)
{
    switch( control )
	{

		case Cal_Background :	*x = CAL_BASE_X;
						 		*y = CAL_BASE_Y;
						 		break;
	
	    case Cal_Month :		*x = CAL_BASE_X + 30;
								*y = CAL_BASE_Y + 15;
								break;

	    case Cal_Year :			*x = (CAL_BASE_X + 25 + (6 * CAL_STEP_X));
								*y = CAL_BASE_Y + 12;
								break;

						
		case Cal_Day :			*x = CAL_BASE_X + 15 + (column * CAL_STEP_X);
								*y = CAL_BASE_Y + 58;
								break;

		case Cal_Date :			*x = CAL_BASE_X + 20 + (column * CAL_STEP_X);
								*y = CAL_BASE_Y + 55 + (row * CAL_STEP_Y);
								break;

		case Cal_DateBar :		*x = CAL_BASE_X - 2;
								*y = CAL_BASE_Y + 48;
								break;

		case Cal_Highlight : 	*x = CAL_BASE_X + 22 + (column * CAL_STEP_X);
						 		*y = CAL_BASE_Y + 45 + (row * CAL_STEP_Y);
						 		break;
						 
//		case Cal_TopLeft :		*x = CAL_BASE_X - 10;
//								*y = CAL_BASE_Y - 15;
//								break;
//
//		case Cal_BottomRight : 	*x = CAL_BASE_X - 10 + 50 + (CAL_STEP_X * 7);
//						   		*y = CAL_BASE_Y - 15 + 20 + (CAL_STEP_Y * 7);
//						   		break;

		default :				*x = CAL_BASE_X;		// something sensible in case it goes wrong!
								*y = CAL_BASE_Y;
		break;
	}
}



byte CalcFirstWeekDayOfMonth( word year, byte month )
{
	byte	tmpMonth, tmpDay, firstWeekDayOfMonth;
    word	tmpYear, firstOfMonth;


	firstOfMonth = TAP_MakeMjd( year, month, 1 );						// use built in function to calculate which day falls on the 1st of the month
	TAP_ExtractMjd( firstOfMonth, &tmpYear, &tmpMonth, &tmpDay, &firstWeekDayOfMonth );

	return firstWeekDayOfMonth;
}


byte CalcLastDayOfMonth( word year, byte month )
{
	byte	lastDayOfMonth;
    
	switch ( month )													// how many days does this month have
	{
		case 1:
		case 3:
		case 5:
		case 7:
		case 8:
		case 10:
		case 12: lastDayOfMonth = 31; break;

		case 4:
		case 6:
		case 9:
		case 11: lastDayOfMonth = 30; break;

		case 2: if ( (year % 4) == 0 ) lastDayOfMonth = 29;
				else lastDayOfMonth = 28;
				
				break;

		default : break;
	}

	return lastDayOfMonth;
}



void PrintCalendarDate( word year, byte month, byte day, dword fcolor, dword bcolor, byte fntSize )
{
    dword 	x_coord, y_coord;
	dword 	column, row;
	byte	firstWeekDayOfMonth;
	char	str[256];

	firstWeekDayOfMonth = CalcFirstWeekDayOfMonth( year, month );
	
	column = (firstWeekDayOfMonth + day - 1) % 7;
	row = ( (firstWeekDayOfMonth + day - 1) / 7 ) + 1;

	CalcCalendarPosition( &x_coord, &y_coord, Cal_Date, row, column);

	TAP_SPrint( str, "%d", day );
	PrintCenter( rgn, x_coord, y_coord, (x_coord + CAL_STEP_X), str, fcolor, bcolor, fntSize );
}



void HighlightCalendarDate( word year, byte month, byte day, bool highlight)
{
    dword 	x, y, h, v;
	dword 	column, row;
	byte	firstWeekDayOfMonth;
	dword	fillColour1 , fillColour2, fillColour3;

	firstWeekDayOfMonth = CalcFirstWeekDayOfMonth( year, month );

	column = (firstWeekDayOfMonth + day - 1) % 7;
	row = ( (firstWeekDayOfMonth + day - 1) / 7 ) + 1;

	CalcCalendarPosition( &x, &y, Cal_Highlight, row, column);
	h = 66;
	v = 60;

	if ( highlight )
	{
//        savedRegion = TAP_Osd_SaveBox( rgn, x, y, h, v );				// save the background behind the highlight
	    TAP_Osd_PutGd( rgn, x, y, &_datehighlightGd, TRUE );			// display the highlight

		PrintCalendarDate( year, month, day, COLOR_Yellow, 0, FNT_Size_1926 );		// re-write the date text
	}
	else
	{
//		TAP_Osd_RestoreBox( rgn, x, y, h, v, savedRegion );				// just restore what was there before
//		TAP_MemFree( savedRegion );										// a bit crappy - must release the memory

	    TAP_Osd_PutGd( rgn, x, y, &_dateoverstampGd, TRUE );
		PrintCalendarDate( year, month, day, MAIN_TEXT_COLOUR, 0, FNT_Size_1926 );
	}
}




void DisplayMonth( word year, byte month )
{
    dword 	x, y, h, v, x_coord, y_coord;
	byte	tmpMonth, tmpDay, firstWeekDayOfMonth, lastDayOfMonth;
    word	tmpYear, firstOfMonth;
	byte 	delta;
	int		row = 0, column = 0, i;
	char	str[256];

	CalcCalendarPosition( &x, &y, Cal_Background, row, column);
    TAP_Osd_PutGd( rgn, x, y, &_calendarGd, TRUE );
//    TAP_Osd_PutGd( rgn, x, y, &_calendardaybarGd, TRUE );


	switch ( month )													// Print the month
	{
		case 1:	TAP_SPrint(str,"January"); break;
		case 2:	TAP_SPrint(str,"February"); break;
		case 3:	TAP_SPrint(str,"March"); break;
		case 4:	TAP_SPrint(str,"April"); break;
		case 5:	TAP_SPrint(str,"May"); break;
		case 6:	TAP_SPrint(str,"June"); break;
		case 7:	TAP_SPrint(str,"July"); break;
		case 8:	TAP_SPrint(str,"August"); break;
		case 9:	TAP_SPrint(str,"September"); break;
		case 10:TAP_SPrint(str,"October"); break;
		case 11:TAP_SPrint(str,"November"); break;
		case 12:TAP_SPrint(str,"December"); break;
		default : TAP_SPrint(str,"BAD"); break;
	}
	CalcCalendarPosition( &x_coord, &y_coord, Cal_Month, row, column);
	TAP_Osd_PutStringAf1926( rgn, x_coord, y_coord, x_coord + 110, str, MAIN_TEXT_COLOUR, 0 );


	CalcCalendarPosition( &x_coord, &y_coord, Cal_Year, row, column);
	TAP_SPrint(str,"%d", year);											// Print the year
	TAP_Osd_PutStringAf1926( rgn, x_coord , y_coord, x_coord+100, str, MAIN_TEXT_COLOUR, 0 );


	for ( i=0; i<=6; i++)												// Print the day names
	{
		column = i;

		switch ( i )
		{
			case 0:	TAP_SPrint(str,"Mon"); break;
			case 1:	TAP_SPrint(str,"Tue"); break;
			case 2:	TAP_SPrint(str,"Wed"); break;
			case 3:	TAP_SPrint(str,"Thu"); break;
			case 4:	TAP_SPrint(str,"Fri"); break;
			case 5:	TAP_SPrint(str,"Sat"); break;
			case 6:	TAP_SPrint(str,"Sun"); break;
			default : TAP_SPrint(str,"BAD"); break;
		}
		CalcCalendarPosition( &x_coord, &y_coord, Cal_Day, 0, column);
		x = x_coord + 5;
		y = y_coord - 5;
		h = 55;
		v = 28;
		

//	    TAP_Osd_FillBox( rgn, x+1, y+1, h, v, COLOR_DarkGreen );		// background
		
//	    TAP_Osd_FillBox( rgn, x, y, h, 2, COLOR_White );				// top
//	    TAP_Osd_FillBox( rgn, x, y, 1, v+2, COLOR_White );				// left

//	    TAP_Osd_FillBox( rgn, x+h, y, 3, v, COLOR_Black );				// right
//	    TAP_Osd_FillBox( rgn, x+2, y+v, h+1, 3, COLOR_Black );			// bottom
		
		PrintCenter( rgn, x_coord, y_coord, (x + CAL_STEP_X), str, CALENDAR_DAY_TEXT, 0, FNT_Size_1622 );
	}


	firstWeekDayOfMonth = CalcFirstWeekDayOfMonth( year, month );
	lastDayOfMonth = CalcLastDayOfMonth( year, month );


	for ( i=1; i<=lastDayOfMonth; i++)								// for all days in this month
	{
//		PrintCalendarDate( year, month, i, MAIN_TEXT_COLOUR, 0, FNT_Size_1926 );
		HighlightCalendarDate( year, month, i, FALSE);
	}
	
}



void DisplayCalendarWindow( void )
{
    byte	month, day, weekDay;
	word	year;
	dword	h1, v1, x1, x2, y1, y2;

	calendarWindowShowing = TRUE;
    TAP_Osd_FillBox( rgn, 0, 0, 720, 576, FILL_COLOUR );				// Clear the screen
	
//	CalcCalendarPosition( &x1, &y1, TopLeft, 0, 0);
//	CalcCalendarPosition( &x2, &y2, BottomRight, 0, 0);
//
//	h1 = x2 - x1;														// horizontal width
//	v1 = y2 - y1;														// vertical height
//
//    TAP_Osd_PutGd( rgn, (x1 - 1), (y1 - 38), &_menu_titleGd, TRUE );
//    TAP_Osd_FillBox( rgn, x1+300, y1-8, 100, 8, COLOR_Black );			// must black a bit of the graphics
//
//    TAP_Osd_FillBox( rgn, x1+300, y1-6, h1-300+4, 4, TITLE_COLOUR );	// top boarder line
//    TAP_Osd_FillBox( rgn, x1, y1, 4, v1, TITLE_COLOUR );				// left boarder line
//    TAP_Osd_FillBox( rgn, x2, y1-2, 4, v1+2, TITLE_COLOUR );			// right boarder line
//    TAP_Osd_FillBox( rgn, x1, y2, h1+4, 4, TITLE_COLOUR );				// bottom boarder line

	TAP_ExtractMjd( currentTimer.startTime>>16, &year, &month, &day, &weekDay );
    selectedMonth = month;
	selectedYear = year;
	selectedDay = day;
	
	DisplayMonth( selectedYear, selectedMonth );
	HighlightCalendarDate( selectedYear, selectedMonth, selectedDay, TRUE);
}


void CloseCalendarWindow( void )
{
	calendarWindowShowing = FALSE;
	returnFromEdit = TRUE;												// will cause a redraw of timer edit window
}


void CalendarKeyHandler( dword key )
{

    byte	month, day, weekDay, lastDayOfMonth, oldDay, oldMonth;
	word	year, oldYear;

	byte	hour, min, sec;
	word	mjd;
	dword	selectedMJD, timeOfDay;

	lastDayOfMonth = CalcLastDayOfMonth( selectedYear, selectedMonth );
	oldYear = selectedYear;
	oldMonth = selectedMonth;
	oldDay = selectedDay;


	
	switch ( key )
	{

		case RKEY_VolDown:	if ( selectedDay > 1 )
							{
							    selectedDay--;
								HighlightCalendarDate( oldYear, oldMonth, oldDay, FALSE);
								HighlightCalendarDate( selectedYear, selectedMonth, selectedDay, TRUE);
								}
							else
							{
								if ( selectedMonth > 1 ) selectedMonth--;
								else { selectedMonth = 12; selectedYear--; }
							    
								lastDayOfMonth = CalcLastDayOfMonth( selectedYear, selectedMonth );
								selectedDay = lastDayOfMonth;

								DisplayMonth( selectedYear, selectedMonth );
								HighlightCalendarDate( selectedYear, selectedMonth, selectedDay, TRUE);
							}
							break;

		
		case RKEY_VolUp:	if ( selectedDay < lastDayOfMonth )
							{
							    selectedDay++;
								HighlightCalendarDate( oldYear, oldMonth, oldDay, FALSE);
								HighlightCalendarDate( selectedYear, selectedMonth, selectedDay, TRUE);
							}
							else
							{
								if ( selectedMonth < 12 ) selectedMonth++;
								else { selectedMonth = 1; selectedYear++; }
							    
								lastDayOfMonth = CalcLastDayOfMonth( selectedYear, selectedMonth );
								selectedDay = 1;

								DisplayMonth( selectedYear, selectedMonth );
								HighlightCalendarDate( selectedYear, selectedMonth, selectedDay, TRUE);
							}
							break;

							
		case RKEY_ChUp:		if ( selectedDay > 7 )
							{
						    	selectedDay -= 7;
							}
							else
							{
							    while ( selectedDay <= (lastDayOfMonth-7) ) selectedDay += 7;
							}
								
							HighlightCalendarDate( oldYear, oldMonth, oldDay, FALSE);
							HighlightCalendarDate( selectedYear, selectedMonth, selectedDay, TRUE);
							break;

								
		case RKEY_ChDown:	if ( selectedDay <= (lastDayOfMonth-7) )
							{
								selectedDay += 7;
							}
        					else
							{
							    while ( selectedDay > 7 ) selectedDay -= 7;
							}

							HighlightCalendarDate( oldYear, oldMonth, oldDay, FALSE);
							HighlightCalendarDate( selectedYear, selectedMonth, selectedDay, TRUE);
							break;

		case RKEY_Blue :
	    case RKEY_Forward :	if ( selectedMonth < 12 ) selectedMonth++;
							else { selectedMonth = 1; selectedYear++; }

							lastDayOfMonth = CalcLastDayOfMonth( selectedYear, selectedMonth );		// check highlight is valid
							if ( selectedDay > lastDayOfMonth ) selectedDay = lastDayOfMonth;
							
							DisplayMonth( selectedYear, selectedMonth );
							HighlightCalendarDate( selectedYear, selectedMonth, selectedDay, TRUE);
							break;
							
		case RKEY_Red :
		case RKEY_Rewind :	if ( selectedMonth > 1 ) selectedMonth--;
							else { selectedMonth = 12; selectedYear--; }

							lastDayOfMonth = CalcLastDayOfMonth( selectedYear, selectedMonth );		// check highlight is valid
							if ( selectedDay > lastDayOfMonth ) selectedDay = lastDayOfMonth;

							DisplayMonth( selectedYear, selectedMonth );
							HighlightCalendarDate( selectedYear, selectedMonth, selectedDay, TRUE);
							break;

							
		case RKEY_Yellow :	selectedYear++;

							lastDayOfMonth = CalcLastDayOfMonth( selectedYear, selectedMonth );		// check highlight is valid
							if ( selectedDay > lastDayOfMonth ) selectedDay = lastDayOfMonth;

							DisplayMonth( selectedYear, selectedMonth );
							HighlightCalendarDate( selectedYear, selectedMonth, selectedDay, TRUE);
						    break;

							
		case RKEY_Green :	selectedYear--;

							lastDayOfMonth = CalcLastDayOfMonth( selectedYear, selectedMonth );		// check highlight is valid
							if ( selectedDay > lastDayOfMonth ) selectedDay = lastDayOfMonth;

							DisplayMonth( selectedYear, selectedMonth );
							HighlightCalendarDate( selectedYear, selectedMonth, selectedDay, TRUE);
						    break;
							

		case RKEY_Recall :	TAP_ExtractMjd( currentTimer.startTime>>16, &selectedYear, &selectedMonth, &selectedDay, &weekDay );
                                                                                                    // reload the original data
							DisplayMonth( selectedYear, selectedMonth );
							HighlightCalendarDate( selectedYear, selectedMonth, selectedDay, TRUE);
							break;

							
		case RKEY_0 :		TAP_GetTime( &mjd, &hour, &min, &sec );									// select the current date
							TAP_ExtractMjd( mjd, &selectedYear, &selectedMonth, &selectedDay, &weekDay );

							DisplayMonth( selectedYear, selectedMonth );
							HighlightCalendarDate( selectedYear, selectedMonth, selectedDay, TRUE);
							break;

		case RKEY_Record :							
		case RKEY_Ok :		timeOfDay = currentTimer.startTime & 0xffff;
							selectedMJD = TAP_MakeMjd( selectedYear, selectedMonth, selectedDay );
							currentTimer.startTime = (selectedMJD<<16) | timeOfDay;

							CloseCalendarWindow();
							break;
							
		case RKEY_Exit :	CloseCalendarWindow();
							break;

		default :			break;
	}
}

