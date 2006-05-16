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

// This module handles the saving of timers to file, and their restoration


#define MAX_LINE_SIZE_sr	500
#define DATA_BUFFER_SIZE_sr 1024*3
#define Y_TITLE_sr			70
#define Y_COORD_sr 			220


static char *dataBuffer_sr;
static int dataBufferPtr_sr;
static bool SaveRestoreWindowShowing;			// global variable
static int timerCount;



//--------------------------------------------------------------------------------------
//
void DisplaySRwindow( char *titleStr )
{
	word h1, v1, x1, x2, y1, y2;

    SaveRestoreWindowShowing = TRUE;
    TAP_Osd_FillBox( rgn, 0, 0, 720, 576, FILL_COLOUR );				// clear the screen

																		// calculate coords for graphics
	h1 = 627;															// horizontal width
	v1 = 45;															// vertical height
	
	x1 = 44;															// top right
	y1 = Y_COORD_sr-10;

	x2 = x1 + h1;														// bottom left
	y2 = y1 + v1;

	TAP_Osd_PutGd( rgn, x1-1, y1-38, &_menu_titleGd, TRUE );			// draw the graphic
    TAP_Osd_FillBox( rgn, x1+300, y1-8, 100, 8, COLOR_Black );			// must black a bit of the graphics
	
    TAP_Osd_FillBox( rgn, x1+300, y1-6, h1-300+4, 4, TITLE_COLOUR );	// top boarder line
    TAP_Osd_FillBox( rgn, x1, y1, 4, v1, TITLE_COLOUR );				// left boarder line
    TAP_Osd_FillBox( rgn, x2, y1-2, 4, v1+2, TITLE_COLOUR );			// right boarder line
    TAP_Osd_FillBox( rgn, x1, y2, h1+4, 4, TITLE_COLOUR );				// bottom boarder line

	
	PrintCenter( rgn, 0, Y_TITLE_sr, 720, titleStr, MAIN_TEXT_COLOUR, 0, FNT_Size_1926);	// print the title
	timerCount = 1;
}


//----------------
// taken from TimerDisplay.c
//
void DisplayTimerInfo( TYPE_TimerInfo currentTimer )
{
    char	str[80], str2[80], str3[80];
	int 	l;

	byte	hour, min, sec, month, day, weekDay;
	word	year, startTime;
	int		endHour, endMin;
	word 	mjd;
	
	TYPE_TapChInfo	currentChInfo;

	TAP_Osd_PutGd( rgn, 53,  Y_COORD_sr-8, &_rowaGd, FALSE );			// blank line with row background graphics
	
// Record or program
	if ( currentTimer.isRec == 1 )
	{
	    TAP_Osd_PutGd( rgn, 54, Y_COORD_sr-8, &_redcircleGd, TRUE );
		TAP_Osd_PutStringAf1622( rgn, 63, Y_COORD_sr, 90, "R", MAIN_TEXT_COLOUR, 0 );
	}
	else
	{
	    TAP_Osd_PutGd( rgn, 54, Y_COORD_sr-8, &_greencircleGd, TRUE );
		TAP_Osd_PutStringAf1622( rgn, 63, Y_COORD_sr, 90, "P", MAIN_TEXT_COLOUR, 0 );
	}

	
// programme name
	TAP_SPrint(str,"%s", currentTimer.fileName);

	for ( l = strlen(str)-4 ; l < strlen(str) ; l++ )
	{
	    str[l]='\0';
	}
	TAP_Osd_PutStringAf1622( rgn, 93, Y_COORD_sr, 370, str, MAIN_TEXT_COLOUR, 0 );


	
// start and end time
	hour = (currentTimer.startTime&0xff00)>>8;							// extract the time
	min = (currentTimer.startTime&0xff);

	endMin = min + currentTimer.duration;								// add the duration in miutes

	endHour = hour + endMin/60;											// should we have increamented the hour
	if ( endHour >= 24 ) endHour -= 24;									// did the day roll over ?

	endMin = endMin%60;													// Finally remove the hours from the minutes field.
				
	TAP_SPrint(str, "%02d:%02d ~ %02d:%02d", hour, min, endHour, endMin);
	PrintCenter( rgn, 375, Y_COORD_sr, 495, str, MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );

	
	
// repeat type and date
	mjd = currentTimer.startTime>>16;
	TAP_ExtractMjd( mjd, &year, &month, &day, &weekDay) ;

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

	
	switch ( currentTimer.reservationType )
	{
		case RESERVE_TYPE_Onetime: 		TAP_SPrint(str,"Onetime"); TAP_SPrint(str2,"on"); break;
		case RESERVE_TYPE_Everyday: 	TAP_SPrint(str,"Everyday");TAP_SPrint(str2,"from");  break;
		case RESERVE_TYPE_EveryWeekend:	TAP_SPrint(str,"Weekends");TAP_SPrint(str2,"from");  break;
		case RESERVE_TYPE_Weekly:		TAP_SPrint(str,"Weekly");  TAP_SPrint(str2,"from"); break;
		case RESERVE_TYPE_WeekDay:		TAP_SPrint(str,"Weekdays");TAP_SPrint(str2,"from"); break;
		default : break;
	}
	PrintCenter( rgn, 499, Y_COORD_sr-8, 602, str, MAIN_TEXT_COLOUR, 0, FNT_Size_1419 );	//print

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
	PrintCenter( rgn, 499, Y_COORD_sr+11, 602, str, MAIN_TEXT_COLOUR, 0, FNT_Size_1419 );	//print



// channel logo
	DisplayLogo( rgn, 607, Y_COORD_sr-9, currentTimer.svcNum, currentTimer.svcType );


    TAP_Osd_FillBox( rgn, 56, Y_COORD_sr-38, 50, 30, FILL_COLOUR );			// Display the number of timer's processed so far
	TAP_SPrint( str, "%d", timerCount++ );
	TAP_Osd_PutStringAf1926( rgn, 58, Y_COORD_sr-40, 150, str, MAIN_TEXT_COLOUR, 0 );
}



//----------------
//
void WriteTimerFile( TYPE_File *timerFile )
{
	TAP_Hdd_ChangeDir("UK TAP Project");
	if ( TAP_Hdd_Exist( "TimerList.txt" ) ) TAP_Hdd_Delete( "TimerList.txt" );	// Just delete any old copies

	TAP_Hdd_Create( "TimerList.txt", ATTR_PROGRAM );							// Create the file

	timerFile = TAP_Hdd_Fopen( "TimerList.txt" );
	if ( timerFile == NULL ) return; 										// Check we can open it

	TAP_Hdd_Fwrite( dataBuffer_sr, DATA_BUFFER_SIZE_sr, 1, timerFile );		// dump the whole buffer in one hit

	TAP_Hdd_Fclose( timerFile );
	TAP_Hdd_ChangeDir("..");												// return to original directory
}



void WriteStrToBuf( char *str, TYPE_File *timerFile )					// add str to current end of buffer
{																		// and move end out
	int count, i;

    count = strlen(str);
	
	for ( i=0; i<count; i++)
	{
		dataBuffer_sr[dataBufferPtr_sr+i] = str[i];							// copy accross 1 char at a time
	}
	dataBufferPtr_sr += i;
}


//----------------
//
void PrintHeader( TYPE_File *timerFile )
{
	char str[256];
	strcpy( str, "Timer\tMode\tTuner\tType\tLCN\tService Name\tFrequency\tDate\tTime\tDuration\tFile Name\tName Fixed\tDay No\tDay Name\r\n" );
	WriteStrToBuf( str, timerFile );
}



//----------------
//
void PrintTimerLine( int timerIndex, TYPE_File *timerFile )
{
	TYPE_TimerInfo	timerInfo;
	TYPE_TapChInfo	chInfo;
	char str[256];
	byte	month, day, weekDay;
    word	year;
	int 	min, hour, durationMin, durationHour;

	TAP_Timer_GetInfo( timerIndex, &timerInfo );

	TAP_SPrint(str,"%d\t", (timerIndex+1) );							// Timer
	WriteStrToBuf( str, timerFile );


	switch ( timerInfo.isRec  )											// Mode
	{
		case 0 : TAP_SPrint( str, "VCR\t" ); break;
		case 1 : TAP_SPrint( str, "Rec\t" ); break;
		default : TAP_SPrint( str, "bad isRec(%d)\t", timerInfo.isRec ); break;
	}
	WriteStrToBuf( str, timerFile );


	switch ( timerInfo.tuner  )											// Tuner
	{
		case 0 : TAP_SPrint( str, "1\t" ); break;
		case 1 : TAP_SPrint( str, "2\t" ); break;
		case 3 : TAP_SPrint( str, "4\t" ); break;
		default : TAP_SPrint( str, "bad tuner(%d)\t", timerInfo.tuner ); 	break;
	}
	WriteStrToBuf( str, timerFile );


	switch ( timerInfo.svcType  )										// Type
	{
		case 0 : TAP_SPrint( str, "TV\t" ); 	break;
		case 1 : TAP_SPrint( str, "Radio\t" ); break;
		default : TAP_SPrint( str, "bad svcType(%d)\t", timerInfo.svcType ); break;
	}
	WriteStrToBuf( str, timerFile );

																		// LCN and Service Name
	if ( TAP_Channel_GetInfo( timerInfo.svcType, timerInfo.svcNum, &chInfo ) )
		TAP_SPrint(str,"%d\t%s\t", chInfo.logicalChNum, chInfo.chName);
	else
		TAP_SPrint(str,"bad service(%d)\t???\t",  timerInfo.svcNum );
	WriteStrToBuf( str, timerFile );

	
	switch ( timerInfo.reservationType )								// Frequency
	{
		case RESERVE_TYPE_Onetime:		TAP_SPrint( str, "One time\t" ); break;
		case RESERVE_TYPE_Everyday: 	TAP_SPrint( str, "Everyday\t" ); break;
		case RESERVE_TYPE_EveryWeekend:	TAP_SPrint( str, "Every Weekend\t" ); break;
		case RESERVE_TYPE_Weekly:		TAP_SPrint( str, "Weekly\t" ); break;
		case RESERVE_TYPE_WeekDay:		TAP_SPrint( str, "Weekday\t" ); break;
		default : 						TAP_SPrint( str, "bad reservationType(%d)\t", timerInfo.reservationType ); break;
	}
	WriteStrToBuf( str, timerFile );


	hour = (timerInfo.startTime&0xff00)>>8;
	min = (timerInfo.startTime&0xff);
	TAP_ExtractMjd( timerInfo.startTime>>16, &year, &month, &day, &weekDay) ;
	durationMin = timerInfo.duration % 60 ;
	durationHour = timerInfo.duration / 60 ;


	TAP_SPrint(str, "%02d/%02d/%04d\t", day, month, year);				// Date
	WriteStrToBuf( str, timerFile );

				
	TAP_SPrint(str, "%02d:%02d\t", hour, min);							// Time
	WriteStrToBuf( str, timerFile );

	
	TAP_SPrint(str,"%02d:%02d\t", durationHour, durationMin );			// Duration
	WriteStrToBuf( str, timerFile );


	TAP_SPrint(str, "%s\t", timerInfo.fileName);						// File Name
	WriteStrToBuf( str, timerFile );

	
	switch ( timerInfo.nameFix  )										// Name Fixed
	{
		case 0 : TAP_SPrint( str, "0\t" ); 	break;
		case 1 : TAP_SPrint( str, "1\t" ); break;
		default : TAP_SPrint( str, "bad nameFix(%d)\t", timerInfo.nameFix ); break;
	}
	WriteStrToBuf( str, timerFile );


	TAP_SPrint(str,"%d\t", (weekDay+1) );								// Day No
	WriteStrToBuf( str, timerFile );

	
	switch ( weekDay )													// Day Name
	{
		case 0:	TAP_SPrint(str,"Mon"); break;
		case 1:	TAP_SPrint(str,"Tue"); break;
		case 2:	TAP_SPrint(str,"Wed"); break;
		case 3:	TAP_SPrint(str,"Thu"); break;
		case 4:	TAP_SPrint(str,"Fri"); break;
		case 5:	TAP_SPrint(str,"Sat"); break;
		case 6:	TAP_SPrint(str,"Sun"); break;
		default : TAP_SPrint(str,"bad weekday(%d)", weekDay); break;
	}
	WriteStrToBuf( str, timerFile );


	TAP_SPrint(str,"\r\n" );											// end line with carriage return & line feed
	WriteStrToBuf( str, timerFile );


	DisplayTimerInfo( timerInfo );										// Display the timer to the user
}




void ExportTimers( void )
{
	TYPE_File	*timerFile;
	int i;
	int maxTimers;
	char	str[256];

	DisplaySRwindow( "Export Timers" );

	dataBuffer_sr = TAP_MemAlloc( DATA_BUFFER_SIZE_sr );				// Buffer the write data to memory before writing all in one hit
	memset( dataBuffer_sr, '\0', DATA_BUFFER_SIZE_sr );					// set the whole buffer to the string termination character (null)
	dataBufferPtr_sr = 0;

	PrintHeader( timerFile );											// make compatible with PBK
	maxTimers = TAP_Timer_GetTotalNum();								// total number of timers

	for ( i=0; i<maxTimers; i++)										// build an ordered list of timers, eariest=0
	{
		PrintTimerLine( i, timerFile );
		TAP_Delay(50);
	}

	WriteTimerFile( timerFile );										// write all the data in one pass
	TAP_MemFree( dataBuffer_sr );										// must return the memory back to the heap

	TAP_SPrint(str, "Press EXIT to return to main menu" );				// print instructions
	PrintCenter( rgn, 0, 380, 720, str, MAIN_TEXT_COLOUR, 0, FNT_Size_1926);
}




//--------------------------------------------------------------------------------------
//
void SkipHeaderLine( void )												// read a whole line up to the end of line character ("new line")
{
	char readChar;

	while ( dataBufferPtr_sr < DATA_BUFFER_SIZE_sr )					// Bounds check
	{
		readChar = dataBuffer_sr[dataBufferPtr_sr];						// read the file one character at a time
		dataBufferPtr_sr++;

		if ( readChar == '\n' ) break;									// looking for end of line
		if ( readChar == '\0' ) break;									// Null terminator signifies end of file
	}
}



char *ReadTimerField( void )
{
	int i;
	char readChar;
	char *oldPtr;

	oldPtr = (char *) dataBuffer_sr + dataBufferPtr_sr;

	i=0;
	while ( dataBufferPtr_sr < DATA_BUFFER_SIZE_sr )					// Bounds check
	{
		readChar = dataBuffer_sr[dataBufferPtr_sr];						// read the file one character at a time

		if ( readChar == '\t' ) break;									// looking for TAB delimiter,
		if ( readChar == '\n' ) break;									// or end of line.
		if ( readChar == '\r' )
		{																// Carriage return is followed by new line -> discard them both.
			dataBuffer_sr[dataBufferPtr_sr] = '\0';
			dataBufferPtr_sr++;
			break;
		}
		if ( readChar == '\0' ) break;									// Null terminator signifies end of file

		dataBufferPtr_sr++;
	}

	dataBuffer_sr[dataBufferPtr_sr] = '\0';								// add a terminator to the string
	dataBufferPtr_sr++;
	return oldPtr;														// return a pointer to the start of the current field
}


word ExtractServiceID( char *LCN_str, char *serviceName, byte serviceType )
{
	TYPE_TapChInfo	chInfo;
	int countTvSvc, countRadioSvc, count, i, result;
	word extracted_LCN, last_match;

	extracted_LCN = atoi( LCN_str );									// convert the string to an integer (atoi is a built in function)
    last_match = 0;
	
	TAP_Channel_GetTotalNum( &countTvSvc, &countRadioSvc );				// how many channels are installed
	if ( serviceType == 0 ) count = countTvSvc; else count = countRadioSvc;

	for ( i=0 ; i<count ; i++ )											// for all services check the LCN against the value read from file
	{
		result = TAP_Channel_GetInfo( serviceType, i, &chInfo );		// get the LCN info for this service

	    if ( chInfo.logicalChNum == extracted_LCN )						// record any matches
	    {
		    last_match = i;												// (TOPPY may have more than 1 serivce with same LCN)
			if ( strcmp( serviceName, chInfo.chName) == 0) break;		// no need to look further if names match
		}
	}

	return last_match;
}

dword ReadTimerStart( void )
{
    char date_str[256], time_str[256];
	byte	month, day;
    word	year, mjd;
	int 	min, hour;
	dword	startime;
	
	strcpy( date_str, ReadTimerField() );								// Read the Date
	strcpy( time_str, ReadTimerField() );								// Read the Time

	day = ( ((date_str[0] - '0') * 10) + (date_str[1] - '0') );
	month = ( ((date_str[3] - '0') * 10) + (date_str[4] - '0') );
	year = ( ((date_str[6] - '0') * 1000) + ((date_str[7] - '0') * 100) + ((date_str[8] - '0') * 10) + (date_str[9] - '0') );

	hour = ( ((time_str[0] - '0') * 10) + (time_str[1] - '0') );
	min = ( ((time_str[3] - '0') * 10) + (time_str[4] - '0') );
	
	mjd = TAP_MakeMjd( year, month, day );

	startime = (mjd<<16) | (hour<<8) | (min);							// convert the startime to Toppy format
	return startime;
}


word ReadTimerDuration( void )
{
    char 	str[256];
	int 	min, hour;
	word	duration;
	
	strcpy( str, ReadTimerField() );									// Read the Duration

	hour = ( ((str[0] - '0') * 10) + (str[1] - '0') );
	min = ( ((str[3] - '0') * 10) + (str[4] - '0') );
	
	duration = (hour * 60) + min;										// Convert the duration to minutes
	return duration;
}


//strcpy( str, "Timer\tMode\tTuner\tType\tLCN\tService Name\tFrequency\tDate\tTime\tDuration\tFile Name\tName Fixed\tDay No\tDay Name\r\n" );
//----------------
//
bool ProcessTimerLine( void )
{
	TYPE_TimerInfo timerInfo;
	char str[256], tmp[10], svcName[256];
	int result;

//[DEBUG]
	TYPE_TapChInfo	chInfo;
	byte	month, day, weekDay;
    word	year;
	int 	min, hour, durationMin, durationHour;
//----------

	

	
Timer:
	strcpy( str, ReadTimerField() );
	strcpy( tmp, str );
	if ( str[0] == '\0' ) return FALSE;

Mode:
	strcpy( str, ReadTimerField() );
	if ( strcmp( str, "VCR" ) == 0 ) { timerInfo.isRec = 0; goto Tuner; }
	if ( strcmp( str, "Rec" ) == 0 ) { timerInfo.isRec = 1; goto Tuner; }
	timerInfo.isRec = 0;		  										// default = VCR

Tuner:
	strcpy( str, ReadTimerField() );
	if ( strcmp( str, "1" ) == 0 ) { timerInfo.tuner = 0; goto Type; }
	if ( strcmp( str, "2" ) == 0 ) { timerInfo.tuner = 1; goto Type; }
	if ( strcmp( str, "4" ) == 0 ) { timerInfo.tuner = 3; goto Type; }
	timerInfo.tuner = 3;		  										// default = tuner 4
	
Type:
	strcpy( str, ReadTimerField() );
	if ( strcmp( str, "TV" ) == 0 ) { timerInfo.svcType = 0; goto LCN; }
	if ( strcmp( str, "Radio" ) == 0 ) { timerInfo.svcType = 1; goto LCN; }
	timerInfo.svcType = 0;		  										// default = TV

LCN:
	strcpy( str, ReadTimerField() );									// LCN
	strcpy( svcName, ReadTimerField() );									// Service Name
	timerInfo.svcNum = ExtractServiceID( str, svcName, timerInfo.svcType );	// extract service number from above 2 fields


Frequency:
	strcpy( str, ReadTimerField() );
	if ( strcmp( str, "One time" ) == 0 ) { timerInfo.reservationType = RESERVE_TYPE_Onetime; goto StartTime; }
	if ( strcmp( str, "Everyday" ) == 0 ) { timerInfo.reservationType = RESERVE_TYPE_Everyday; goto StartTime; }
	if ( strcmp( str, "Every Weekend" ) == 0 ) { timerInfo.reservationType = RESERVE_TYPE_EveryWeekend; goto StartTime; }
	if ( strcmp( str, "Weekly" ) == 0 ) { timerInfo.reservationType = RESERVE_TYPE_Weekly; goto StartTime; }
	if ( strcmp( str, "Weekday" ) == 0 ) { timerInfo.reservationType = RESERVE_TYPE_WeekDay; goto StartTime; }
	timerInfo.reservationType = RESERVE_TYPE_Onetime;					// default = One time


StartTime:
	timerInfo.startTime = ReadTimerStart();								// Date & Time
	timerInfo.duration = ReadTimerDuration();							// Duration


FileName:
	strcpy ( timerInfo.fileName, ReadTimerField() );					// File Name

	
	strcpy(str, ReadTimerField() );
	if ( strcmp( str, "0" ) == 0 ) { timerInfo.nameFix = 0; goto DayNo; }
	if ( strcmp( str, "1" ) == 0 ) { timerInfo.nameFix = 1; goto DayNo; }
	timerInfo.nameFix = 0;		  										// default = name not fixed


DayNo:
	ReadTimerField();													// Day No
	ReadTimerField();													// Day Name


//[DEBUG] ******** KEEP - will print to file *********
//        ********************************************
//	TAP_Channel_GetInfo( timerInfo.svcType, timerInfo.svcNum, &chInfo );
//
//	hour = (timerInfo.startTime&0xff00)>>8;
//	min = (timerInfo.startTime&0xff);
//	TAP_ExtractMjd( timerInfo.startTime>>16, &year, &month, &day, &weekDay) ;
//	durationMin = timerInfo.duration % 60 ;
//	durationHour = timerInfo.duration / 60 ;
//
//	TAP_SPrint(str,"%s: %s %02d/%02d/%04d %02d:%02d %02d:%02d %s %d %d %d %d %d %d %d", tmp, chInfo.chName,
//									day, month, year, hour, min, durationHour, durationMin,
//									timerInfo.fileName, timerInfo.isRec, timerInfo.tuner, timerInfo.svcType,
//									timerInfo.reserved, timerInfo.svcNum, timerInfo.reservationType, timerInfo.nameFix );
//	if ( currentPrintLine_sr < 500 )
//	{
//		TAP_Osd_PutStringAf1419(rgn, X_COORD_sr, currentPrintLine_sr, X_WIDTH_sr, str, COLOR_Black, COLOR_White);
//		currentPrintLine_sr += LINE_INC_sr;
//	}

	DisplayTimerInfo( timerInfo );										// Display the timer to the user
	result = TAP_Timer_Add( &timerInfo );



	switch ( result )
	{
		case 0 : TAP_SPrint(str, " Success " ); break;
		case 1 : TAP_SPrint(str, " Invalid Entry or No resource available " ); break;
		case 2 : TAP_SPrint(str, " Invalid Tuner " ); break;
		default : TAP_SPrint(str, " Conflict with timer number %d ", (result&0xffff)+1 ); break;
			break;
	}
	

    TAP_Osd_FillBox( rgn, 0, 295, 720, 40, COLOR_Black );				// blank the status line
	PrintCenter( rgn, 0, 300, 720, str, MAIN_TEXT_COLOUR, 0, FNT_Size_1926);

	TAP_Delay(50);

	return TRUE;
}



//----------------
//
void ReadTimerFile( void )
{
	TYPE_File	*timerFile;
	int i;
	dword fileLength;

	TAP_Hdd_ChangeDir("UK TAP Project");
	if ( ! TAP_Hdd_Exist( "TimerList.txt" ) ) return;					// check the timer file exits in the current directory
	
	timerFile = TAP_Hdd_Fopen( "TimerList.txt" );
	if ( timerFile == NULL ) return;									// and we can open it ok

	dataBuffer_sr = TAP_MemAlloc( DATA_BUFFER_SIZE_sr );				// Read the data in one hit (quicker), then process from RAM
	memset( dataBuffer_sr, '\0', DATA_BUFFER_SIZE_sr );					// set the whole buffer to the string termination character (null)
	dataBufferPtr_sr = 0;

	fileLength = TAP_Hdd_Flen( timerFile );								// how big is the file
	if ( fileLength > DATA_BUFFER_SIZE_sr  ) fileLength = DATA_BUFFER_SIZE_sr;	// ensure we don't overflow the buffer
	
	TAP_Hdd_Fread( dataBuffer_sr, fileLength, 1, timerFile );			// grab all the data from the file

	TAP_Hdd_Fclose( timerFile );
	TAP_Hdd_ChangeDir("..");											// return to original directory

	SkipHeaderLine();
	while ( ProcessTimerLine() == TRUE) {}

	TAP_MemFree( dataBuffer_sr );										// must return the memory back to the heap
}


void ImportTimers( void )
{
    char str[256];
	
	DisplaySRwindow( "Import Timers" );
	
	ReadTimerFile();													// Process the timer.dat file

	TAP_SPrint(str, " Press EXIT to return to main menu " );			// print instructions
	PrintCenter( rgn, 0, 380, 720, str, MAIN_TEXT_COLOUR, 0, FNT_Size_1926);
}



//--------------------------------------------------------------------------------------
//
word ReadTimerDurationInMinutes( void )
{
    char 	str[256];
	int i;
	word	duration;

	i = 0;
	duration = 0;
	
	strcpy( str, ReadTimerField() );									// Read the duration in minutes

	while( str[i] != '\0' )
	{
		duration = (duration * 10) + (str[i] - '0');
		i++;
	}

	return duration;
}


bool ProcessDigiGuideImport( void )
{
	TYPE_TimerInfo timerInfo;
	char str[256], tmp[10], svcName[256];
	int result;

//[DEBUG]
	TYPE_TapChInfo	chInfo;
	byte	month, day, weekDay;
    word	year;
	int 	min, hour, durationMin, durationHour;
//----------

	
	timerInfo.isRec = 1;			// Record = on
	timerInfo.tuner = 3;			// tuner 4
	timerInfo.svcType = 0;			// TV
	timerInfo.reservationType = RESERVE_TYPE_Onetime;
	timerInfo.nameFix = 1;			// fixed file name

	strcpy( str, ReadTimerField() );									// LCN
	if ( str[0] == '\0' ) return FALSE;									// simple check for EOF

	strcpy( svcName, ReadTimerField() );									// Service Name
	timerInfo.svcNum = ExtractServiceID( str, svcName, timerInfo.svcType );	// extract service number from above 2 fields

	timerInfo.startTime = ReadTimerStart();								// Date & Time
	timerInfo.duration = ReadTimerDurationInMinutes();					// Duration

	strcpy( str, ReadTimerField() );									// Programme name
	strcat( str, ".rec");												// turn in to a file name
	strcpy( timerInfo.fileName, str );

//	if ( timerInfo.svcNum == 0 ) return FALSE;							// some simple data validation
	if ( timerInfo.startTime == 0) return FALSE;
	if ( timerInfo.duration == 0) return FALSE;
	if ( timerInfo.fileName == 0 ) return FALSE;


	DisplayTimerInfo( timerInfo );										// Display the timer to the user
	result = TAP_Timer_Add( &timerInfo );

	switch ( result )
	{
		case 0 : TAP_SPrint(str, " Success " ); break;
		case 1 : TAP_SPrint(str, " Invalid Entry or No resource available " ); break;
		case 2 : TAP_SPrint(str, " Invalid Tuner " ); break;
		default : TAP_SPrint(str, " Conflict with timer number %d ", (result&0xffff)+1 ); break;
			break;
	}
	

    TAP_Osd_FillBox( rgn, 0, 295, 720, 40, COLOR_Black );				// blank the status line
	PrintCenter( rgn, 0, 300, 720, str, MAIN_TEXT_COLOUR, 0, FNT_Size_1926);

	TAP_Delay(50);

	return TRUE;
}


//----------------
//
void ReadDigiGuideFile( void )
{
	TYPE_File	*timerFile;
	int i;
	dword fileLength;

	TAP_Hdd_ChangeDir("UK TAP Project");
	if ( ! TAP_Hdd_Exist( "ExportList.txt" ) ) return;					// check the timer file exits in the current directory
	
	timerFile = TAP_Hdd_Fopen( "ExportList.txt" );
	if ( timerFile == NULL ) return;									// and we can open it ok

	dataBuffer_sr = TAP_MemAlloc( DATA_BUFFER_SIZE_sr );				// Read the data in one hit (quicker), then process from RAM
	memset( dataBuffer_sr, '\0', DATA_BUFFER_SIZE_sr );					// set the whole buffer to the string termination character (null)
	dataBufferPtr_sr = 0;

	fileLength = TAP_Hdd_Flen( timerFile );								// how big is the file
	if ( fileLength > DATA_BUFFER_SIZE_sr  ) fileLength = DATA_BUFFER_SIZE_sr;	// ensure we don't overflow the buffer
	
	TAP_Hdd_Fread( dataBuffer_sr, fileLength, 1, timerFile );			// grab all the data from the file

	TAP_Hdd_Fclose( timerFile );
	TAP_Hdd_ChangeDir("..");											// return to original directory

	while ( ProcessDigiGuideImport() == TRUE) {}

	TAP_MemFree( dataBuffer_sr );										// must return the memory back to the heap
}


void ImportFromDigiGuide( void )
{
    char str[256];
	
	DisplaySRwindow( "Import from DigiGuide" );
	
	ReadDigiGuideFile();												// Process the timer.dat file

	TAP_SPrint(str, " Press EXIT to return to main menu " );			// print instructions
	PrintCenter( rgn, 0, 380, 720, str, MAIN_TEXT_COLOUR, 0, FNT_Size_1926);
}


//--------------------------------------------------------------------------------------
//
void DeleteAllTimers( void )
{
	TYPE_TimerInfo	timerInfo;
	int maxTimers, i;
	char str[256];

	DisplaySRwindow( "Delete Timers" );
	
	maxTimers = TAP_Timer_GetTotalNum();								// total number of timers

	for ( i=maxTimers-1; i>=0; i--)										// build an ordered list of timers, eariest=0
	{
		TAP_Timer_GetInfo( i, &timerInfo );
		DisplayTimerInfo( timerInfo );

		TAP_Timer_Delete( i );
		TAP_Delay(50);
	}

	TAP_SPrint(str, " Press EXIT to return to main menu " );			// print instructions
	PrintCenter( rgn, 0, 380, 720, str, MAIN_TEXT_COLOUR, 0, FNT_Size_1926);
}



//--------------------------------------------------------------------------------------
//
void InitialiseSaveRestore( void )
{
	SaveRestoreWindowShowing = FALSE;    
}



dword SaveRestoreKeyHandler( dword key )
{
    if ( key == RKEY_Exit )
	{
		SaveRestoreWindowShowing = FALSE;
		returnFromEdit = TRUE;		
	}

	return 0;
}
