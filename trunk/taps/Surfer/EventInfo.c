/************************************************************
				Part of the UK TAP Project
		This module handles display of  the channel logos

Name	: EventInfo.c
Author	: Darkmatter and IanP
Version	: 0.0
For		: Topfield TF5x00 series PVRs
Licence	:
Descr.	:
Usage	:
History	: v0.0 Darkmatter: 02-08-05	Inception Date
		  v0.1 Darkmatter: 07-08-05 Modified to handle PIP window next to event info
		  v0.2 Darkmatter: 09-08-05 Now refreshes event info every couple of seconds
		  							Next/Previous event routines now checks start and end times to cope with the background refresh
          v0.3 Kidhazy:    21-08-05 Added handling of Playback Channel event information.
               DB1:                 Updated event duration information.
          v0.4 Kidhazy:    14-09-05 Corrected handling of text in event info window - some text was being chopped off.
          v0.5 Kidhazy:    29-09-05 Extracted out the DEFINE statements to EventInfoConfig.c so they can be referenced elsewhere.

	Last change:  USE   9 Aug 105    5:15 pm
**************************************************************/

// Define the numbers for the different progress bar colour options.
#define PB_MULTI    0
#define PB_SINGLE   1
#define PB_SOLID    2
#define PB_WHITE    3
 
void ShowEventDetails(TYPE_TapEvent event);				// prototype for later
void WrapPutStr(word windowRgn, char *str, int x, int y, int w, int fgCol, int bgCol, int maxRows, byte fntSize, int lineSep);
void PrintEventDescription( TYPE_TapEvent event );
void IndicateTimers(word windowRgn, int svcNum, int svcType, dword *xCoord, int yCoord, TYPE_TapEvent event, int where);

TYPE_TapEvent *events;
byte *extInfo = NULL;   // Pointer for the Extended Event Information
static int _svcType;
static int _svcNum;
static int eventCount;
static int currentIndex;
static TYPE_TapEvent currentEvent;
static bool eventCleared;								// flag to ensure the event description is only clear the once - tied in with delayed draw to the event description
static bool noEvents;									// flag to remember if there was any event data in the event array
int     WrapPutStr_StartPos = 0;  	//KH Starting position to print string from
int     LastWrapPutStr_Y;			//KH Tracks the last coord from the WrapPutStr routine.
int 	LastWrapPutStr_P;  			//KH Tracks the last string position from the WrapPutStr routine.
int 	LastWrapPutStr_Start;       //KH Tracks the start position of the last line from the WrapPutStr routine.
int     ExtInfoRows = 1;  			//KH Keep track of how many lines available for extended info. Initially assume at least 1.;
int     PiPEventIndex;				//KH track of the index number for the event being shown in the PiP Window
char    NowNextTag[10];				//KH Stores the varying Now/Next/Later/Earlier tag to prepend to the Event Title

static int  currentGuideIndex;      // Index number of events being displayed in the Guide Window.

static dword lastTick_EventProc;

//-------------------------------------------
//
/******************************************************************************
Finds the name for a given week day number.
******************************************************************************/
void GetDayName(byte weekDay, char* name)
{
     name[0] = '\0';   
	switch( _language )
	{
		case LAN_French :
			switch( weekDay )
			{
				case 0 : strcpy(name, "Lundi"); break;
				case 1 : strcpy(name, "Mardi"); break;
				case 2 : strcpy(name, "Mercredi"); break;
				case 3 : strcpy(name, "Jeudi"); break;
				case 4 : strcpy(name, "Vendredi"); break;
				case 5 : strcpy(name, "Samedi"); break;
				case 6 : strcpy(name, "Dimanche"); break;
				default : strcpy(name, "Bad day"); break;
			}
			break;
		case LAN_German :
			switch( weekDay )
			{
				case 0 : strcpy(name, "Montag"); break;
				case 1 : strcpy(name, "Dienstag"); break;
				case 2 : strcpy(name, "Mittwoch"); break;
				case 3 : strcpy(name, "Donnerstag"); break;
				case 4 : strcpy(name, "Freitag"); break;
				case 5 : strcpy(name, "Samstag"); break;
				case 6 : strcpy(name, "Sonntag"); break;
				default : strcpy(name, "Bad day"); break;
			}
			break;
		default :
			switch( weekDay )
			{
				case 0 : strcpy(name, "Monday"); break;
				case 1 : strcpy(name, "Tuesday"); break;
				case 2 : strcpy(name, "Wednesday"); break;
				case 3 : strcpy(name, "Thursday"); break;
				case 4 : strcpy(name, "Friday"); break;
				case 5 : strcpy(name, "Saturday"); break;
				case 6 : strcpy(name, "Sunday"); break;
				default : strcpy(name, "Bad day"); break;
			}
	}
}

//-------------------------------------------
//
/******************************************************************************
Converts the Parental Rating from number to textual Australian Based ratings.
******************************************************************************/
void GetOzParentalRating(int parentalRating,char *rating)
{
     // FTA: G=9  PG=11 M=13 MA=15 R=18
     // TED: G=10 PG=12 M=13 MA=15 R=18
     switch ( parentalRating )
     {
            case 9: 
            case 10: strcpy(rating, "G");  rating[1]= '\0'; break;
            case 11: 
            case 12: strcpy(rating, "PG"); rating[2]= '\0'; break;
            case 13: strcpy(rating, "M");  rating[1]= '\0'; break;
            case 15: strcpy(rating, "MA"); rating[2]= '\0'; break;
            case 18: strcpy(rating, "R");  rating[1]= '\0'; break;
            default: strcpy(rating, "");  rating[0]= '\0'; break;
     }
}
     



void DisplayNoInfoMessage( void )
{
	TAP_Osd_PutGd( rgn, EVENT_X, EVENT_Y, &_narrowhighlightGd, TRUE );
	TAP_Osd_PutStringAf1622( rgn, EVENT_X+8, EVENT_Y+8, EVENT_W+EVENT_X, "No Event Information", MAIN_TEXT_COLOUR, 0 );

	if ( eventCleared == FALSE)															// clear the bottom portion of the screen
	{
		TAP_Osd_FillBox( rgn, EVENT_X, EVENT_Y+35, 720-EVENT_X, EVENT_H-35, EVENT_FILL_COLOUR );
		eventCleared = TRUE;
	}
}



void ShowEventTitle(TYPE_TapEvent event)
{
    char	str[256];
	TYPE_TapChInfo chInfo;
	dword startX;     //KH general x co-ordinate variable.
	int startY;     //KH general y co-ordinate variable.
	int textW;      //KH general text width variable.
	
	TAP_Osd_PutGd( rgn, EVENT_X, EVENT_Y, &_narrowhighlightGd, TRUE );	// draw the green bar
    startX = EVENT_X+3;   //KH set the initial x-coord for the Event Name.
	IndicateTimers( rgn , currentSvcNum, currentSvcType, &startX, EVENT_Y-2, currentEvent, 0);  // Display timer indicators for this event in the Event Info window.
    startX = startX + 3;   // Allow some leading space for the Event Name.

    if (Playback_Selected)
       strcpy( NowNextTag, "File:");
    else
    {
       if      (currentIndex == PiPEventIndex)      strcpy( NowNextTag, "Now:"); 
       else if (currentIndex == PiPEventIndex + 1)  strcpy( NowNextTag, "Next:");
       else if (currentIndex >  PiPEventIndex + 1)  strcpy( NowNextTag, "Later:");
       else if (currentIndex <  PiPEventIndex)      strcpy( NowNextTag, "Earlier:");
    }
       
    if (NowNextTagOption || Playback_Selected) //KH If user has selected the Now/Next/Later tag config option print it in front of the event title in a smaller font.
    {                           
       TAP_SPrint( str, "%s", NowNextTag );
       TAP_Osd_PutStringAf1419( rgn, startX, EVENT_Y+4+7, MAX_X, str, MAIN_TEXT_COLOUR, 0 );   // Print the tag in a small font keeping the bottom of the text lined up
	   textW = TAP_Osd_GetW(str,0,FNT_Size_1419);                                            // Calculate the width of the tag
	   startX = startX + textW + 5;                                                                // Reset start position for event title to after the tag plus 1 character.
    }
    
    TAP_SPrint( str, "%s", event.eventName );
	TAP_Osd_PutStringAf1926( rgn, startX, EVENT_Y+4, MAX_X, str, MAIN_TEXT_COLOUR, 0 );

	if ( eventCleared == FALSE)															// To speed up scrolling through the channel logos
	{																					// must only clear the bottom portion of the screen once
		TAP_Osd_FillBox( rgn, EVENT_X, EVENT_Y+35, 720-EVENT_X, EVENT_H-35, EVENT_FILL_COLOUR );
		eventCleared = TRUE;															// will cause the full event details to be displayed
	}																					// a little while after the user stops pushing buttons
}



//-------------------------------------------
//
void LoadEventInfo( int svcType, int svcNum )
{
	_svcType = svcType;
	_svcNum = svcNum;

	if( events )														// release any old memory used to store the events
	{
		TAP_MemFree( events );
		events = 0;
	}
	
    if ((svcNum == Playback_svcnum) && (svcType == SVC_TYPE_Tv))
    {
        //KH load up the events with the event information from the recording.
        //TYPE_Playinfo has a evtInfo 
        events = &Playback_event;  //KH Associate event information with original playback event.
        eventCount = 1;            //KH There is only ever 1 event associated with the playback.
    }
    else
    {	
	    events = TAP_GetEvent( _svcType, _svcNum, &eventCount );			// get all the events for this new channel
    }

}	


//-------------------------------------------
//
void DisplayEventInfo( int svcType, int svcNum )
{
	byte 	hour, min, sec;
    word	mjd;
	dword	currentTime;
	int i;

    LoadEventInfo( svcType, svcNum );  // Load the events into the events array.

 	if( !events )														// check there is at least one event for this channel
	{
		DisplayNoInfoMessage();
		noEvents = TRUE;
		return;
	}
	else noEvents = FALSE;
   	

    TAP_GetTime( &mjd, &hour, &min, &sec );								// now look for the current event
	currentTime = (mjd << 16) | (hour<<8) | min;						// must manually search through the list
																		// comparing start and end times
    for ( i=0; i<eventCount-1; i++ )
    {
        currentEvent = events[i];
        if (( currentTime >= currentEvent.startTime ) && ( currentTime < currentEvent.endTime )) break;
    }

    if (i+currentGuideIndex >= eventCount) currentGuideIndex = (eventCount-1)-i;   /// Boundary check to make sure that we point at the last event.
    if (i+currentGuideIndex < 0) currentGuideIndex = 0-i;   // Boundary check to make sure that we point at the first event.
	currentIndex = i + currentGuideIndex;                   // This is the index of the event we are currently pointing to.
	currentEvent = events[ currentIndex ];                  // This is the event that we are pointing to.
	PiPEventIndex = i;                                      //PiP will always be showing the current event.
	
	ShowEventTitle( currentEvent );							// display the title now, and the full description later
}



//-------------------------------------------
//
void ShowDelayedEvent( void )											// Called once the user stops pressing keys
{																		// Just call the full display routine.
	if ( noEvents == FALSE ) ShowEventDetails( currentEvent );			// Can only display the info if there's valid data.
	eventCleared = FALSE;	
}



void DisplayNextEvent( void )											// Move the display to the next event
{
	dword	currentEndTime;
	int 	i;
	char	str[256];

	if( !events ) { DisplayNoInfoMessage(); return; }					// bounds check
	
	currentEndTime = currentEvent.endTime;								// remember the end time of the currently displayed event

	for ( i=0; i<eventCount; i++ )										// find the 1st event that starts after the last end time
	{
		currentEvent = events[i];
		if ( currentEvent.startTime >= currentEndTime ) break;
	}
    if (i == eventCount) i=eventCount-1;  //We searched through all events and didn't find a later one, so point to the real last one.
    currentIndex = i;													// point to this new event in the array of all events on this channel
	ShowEventDetails( currentEvent );									// call the main display routine
}



void DisplayPreviousEvent( void )
{
	dword	currentStartTime;											// The opposite of the above function
	int		i;															// find the 1st event that ends before the current event's start time

	if( !events ) { DisplayNoInfoMessage(); return; } 					// bounds check

	currentStartTime = currentEvent.startTime;

	for ( i=eventCount; i>0; i-- )										// stepping backwards through the list
	{																	// note: here i is one base, not zero base, to make the comparison easier
		currentEvent = events[i-1];
		if ( currentEvent.endTime <= currentStartTime ) break;
	}
    if (i == 0) i=1;  //We searched through all events and didn't find an earlier one, so point to the real last one.
	currentIndex = i-1;													// remember to subtract the offset
	ShowEventDetails( currentEvent );
}


//-------------------------------------------
//
void EventIdleRoutine( dword currentTickTime )
{
	byte 	hour, min, sec;
    word	mjd;
	dword	currentTime;
	bool	wereThereNoEvents;
	int		i;

	if ( (currentTickTime < (lastTick_EventProc + DWELL_EVENT_PROC)) && (currentTickTime > lastTick_EventProc)) return;
    lastTick_EventProc = currentTickTime;

	if ( !events ) wereThereNoEvents = TRUE;							// refresh the event array
	else
	{
	    wereThereNoEvents = FALSE;
		TAP_MemFree( events );											// release any old memory used to store the events
		events = 0;
	}
    if ( _svcNum != Playback_svcnum )
    {
	   events = TAP_GetEvent( _svcType, _svcNum, &eventCount );			// get all the events for the current channel
    }
    else
    {
        //KH load up the events with the event information from the recording.
        //TYPE_Playinfo has a evtInfo 
        events = &CurrentPlaybackInfo.evtInfo;  //KH Associate event information with original playback event.
        eventCount = 1;   //KH There is only ever 1 event associated with the playback.
    }

	if( !events ) return;						// silently do nothing if no events found
	if ( wereThereNoEvents == FALSE ) return;   // If there was an event being displayed don't do anything.

    TAP_GetTime( &mjd, &hour, &min, &sec );								// now look for the current event
	currentTime = (mjd << 16) | (hour<<8) | min;						// must manually search through the list
																		// comparing start and end times

	for ( i=0; i<eventCount-1; i++ )
	{
		currentEvent = events[i];
		if (( currentTime >= currentEvent.startTime ) && ( currentTime < currentEvent.endTime )) break;
	}

    if (i+currentGuideIndex >= eventCount) currentGuideIndex = (eventCount-1)-i;   /// Boundary check to make sure that we point at the last event.
    if (i+currentGuideIndex < 0) currentGuideIndex = 0-i;   // Boundary check to make sure that we point at the first event.
	currentIndex = i + currentGuideIndex;                   // This is the index of the event we are currently pointing to.
	currentEvent = events[ currentIndex ];                  // This is the event that we are pointing to.
	PiPEventIndex = i;                                      //PiP will always be showing the current event.

//	PiPEventIndex = i;  // Added in to try fix stuck "Later:" tag.
//	currentEvent = events[i];
//	currentIndex = i;
	
	ShowEventDetails( currentEvent );									// Now display the title and full description.
}


void DelayEventRefresh( dword currentTickTime )
{
	lastTick_EventProc = currentTickTime;
}


//-------------------------------------------
//
void InitialiseEvents( void )
{
	eventCleared = FALSE;
	lastTick_EventProc = 0;
}



void TerminateEvents( void )
{
	if( events )														// from Topfields sample EPG TAP
	{																	// TAP_GetEvents appears to allocate memory from the heap
		TAP_MemFree( events );											// must release
		events = 0;
	}
}



/******************************************************************************
Word Wrap string output.

str: The string to be printed.
x, y, w: print area starts at x pixels in, y pixels down and is w pixels wide.
fgCol, bgCol: foreground and background colours.
maxRows: defines the hight of the print area in rows of characters.
fntSize: one of FNT_Size_1419, FNT_Size_1622 and FNT_Size_1926.
lineSep: The extra pixels between rows (can be negative with bgCol = COLOR_None
for overprinting).

Control characters (cariage return, new line etc.) are not supported.
******************************************************************************/
void WrapPutStr(word windowRgn, char *str, int x, int y, int w, int fgCol, int bgCol, int maxRows, byte fntSize, int lineSep)
{
	int row=0,c=0,p=0;
	int done = 0, try = 5;
	int fontH;
	int avgFontW;
	int len;
	int newStrW;
	char newStr[512]; //100 should be more than enough for a line [!!!????]

	switch (fntSize)
	{
		case FNT_Size_1419 : fontH = 19; avgFontW = 6; break;
		case FNT_Size_1622 : fontH = 22; avgFontW = 8; break;
		case FNT_Size_1926 : fontH = 26; avgFontW = 10; break;
		default : fontH = 19; break;
	}

	len = strlen(str);
	
	p = WrapPutStr_StartPos;  //KH Set the starting position to print string from.
	if (len == 0)  
    {
          LastWrapPutStr_P = p;      //KH Keep track of the last string position.
          return;
    }      

	for (row=0; row<maxRows;row++)   
	{
    	LastWrapPutStr_Start = p;  //KH Keep track of the position of the first character on each line.
		c = w/avgFontW;	// guess approx chars in line
		if(p + c > len) c = len - p;
		if(c > 99) c = 99;

		strncpy(newStr,str+p, c); // copy approx chars per line to new string
		newStr[c]= '\0'; //KH clean up the end of the new line.

		while(!done)
		{
			newStrW = TAP_Osd_GetW(newStr,0,fntSize); // see if it fits
			if (newStrW > w && c > 0)  // Too big
			{		 //remove some chars
				if(c > 4) c = c - 4;
				else c--;

				newStr[c]= '\0';
			}
			else
			{
				done = 1; // string short enough
			}
			
		}

		done = 0;

		while (TAP_Osd_GetW(newStr,0,fntSize)<w-avgFontW)    //while the width of the text is less than a lines length
		{
			if (p + c + 1 > len)   //if the counter is larger than the string length of the orig string, we have really finished here 
			{
				done=1;					
				break;
			}
			strncat(newStr,str+p+c,1); //copy 1 char from str at a time.
			c++;                 //get next char
		}


		if (!done ) //KH find wrap position unless finished
		{
			c--; //reduce c by 1 as it is one too much from last while statement.
			while (!isspace(newStr[c])&&!(newStr[c]=='-')&&(c>0))
		 	{   //look for a space or hyphen (or underflow).
				c--;
			}
			c++; //add 1 to keep hyphen on right line
		}
		newStr[c]='\0';   //terminate str (early at wrap point if not done).

		LastWrapPutStr_Y = y+((fontH+lineSep)*row); //KH Keep track of the Y coordinate of where we're printing.
		TAP_Osd_PutStringAf(windowRgn, x, LastWrapPutStr_Y, x+w, newStr, fgCol, bgCol, 0, fntSize, 0);  //put multiple lines down if required.
		p = p + c;   //store current position
		LastWrapPutStr_P = p; //KH Keep track of the last string position.
		newStr[0] = '\0';  //reset string.
    	if (done) //KH if we've printed all the text, break out of loop.
        {
            LastWrapPutStr_P = 0;  //KH Reset to start of string.
            break;
        }
	}    
	LastWrapPutStr_Y = LastWrapPutStr_Y + fontH + lineSep;  //KH Store the last Y position.
	WrapPutStr_StartPos = 0;  //KH Reset starting position to print string from
}



void formatTime(char * when, word diff)
{
	word day, hour, min;
	char buf[32];
	
	day = diff / 1440;
	diff = diff % 1440;
	hour = diff / 60;
	min = diff % 60;

	if ( day )
	{	
		sprintf( buf, "%d day", day);
		strcat(when, buf);
		
		if ( day > 1 )
			strcat(when, "s");
	}
	
	if ( hour )
	{	
		if ( day )
		  	strcat(when, " ");
		sprintf( buf, "%d hour", hour);
		strcat(when, buf);

		if ( hour > 1 )
			strcat(when, "s");
	}

	if ( min )
	{	
		if ( day || hour )
			strcat(when, " ");
		sprintf( buf, "%d minute", min);
		strcat(when, buf);

		if ( min > 1 )
			strcat(when, "s");
	}

	return;
}


/******************************************************************************
Displays the details of the currently selected event below the grid.
******************************************************************************/
void ShowEventDetails(TYPE_TapEvent event)
{
	word startMjd; byte startHour; byte startMin; byte sec; byte startWeekday; //KH
	word endMjd; byte endHour; byte endMin;
	byte durHour; byte durMin;
	word curMjd; byte curHour; byte curMin; byte currWeekday;  
    dword start, end, now, diff;

     
	word startYear; byte startMonth; byte startDay; char DayOfWeek[10];
	dword	duration, runtime;
    int     durPerc;
	dword   startX;     //KH general x co-ordinate variable.
	int     startY;     //KH general y co-ordinate variable.
	int     textW;      //KH general text width variable.
	int     pbWidth;    //Width of progressbar

	TYPE_TapChInfo chInfo;
	char ratingText[2];
	char str[512];
	char when[256];
	
	byte extInfoRowSize, extInfoFontSize;
	bool TF5800ExtInfoFlag;

    word MemRgn;

    void PrintRecorded()
    {
        //
        // Print a "Recorded" title.
        //
        strcpy(str, "Recorded: ");
        textW = TAP_Osd_GetW(str,0,FNT_Size_1622);  // Calculate the width of the weekday.
	    TAP_Osd_PutStringAf(MemRgn, startX, startY, startX+textW, str, MAIN_TEXT_COLOUR, EVENT_FILL_COLOUR, 0, FNT_Size_1622, 0);  //print day of week in standard colour.
        startX = startX + textW;                // Reset X start position.
    }


    void PrintViewedProgress()
    {
        //
        // Print a "Watched so far: " title.
        //
        startX = EVENT_X+8;      // Store that starting position for  info.
        startY = startY + 23;    // Start on a new line.
        strcpy(str, "Watched so far: ");
        textW = TAP_Osd_GetW(str,0,FNT_Size_1622);  // Calculate the width of the weekday.
	    TAP_Osd_PutStringAf(MemRgn, startX, startY, startX+textW, str, MAIN_TEXT_COLOUR, EVENT_FILL_COLOUR, 0, FNT_Size_1622, 0);  //print day of week in standard colour.
        startX = startX + textW;                // Reset X start position.
    }
      
    void PrintDayOfWeek()
    {
        //
        // Print day of week if required.
        //
        //  Calculate the day of week for events that are not today events.
        if (((curMjd != startMjd) && (currentIndex != PiPEventIndex)) ||  // //Ensure the current event never shows a day of week (eg. show started at 11pm-2am and current time is 1am)
             (Playback_Selected))
        {     
           GetDayName(startWeekday, DayOfWeek);    // Get the day of the week if start date is not today.
           TAP_SPrint( str, "%.3s ", DayOfWeek);       // Cut off the first 3 characters of the weekday.
	       textW = TAP_Osd_GetW(str,0,FNT_Size_1622);  // Calculate the width of the weekday.
		   TAP_Osd_PutStringAf(MemRgn, startX, startY, startX+textW, str, MAIN_TEXT_COLOUR, EVENT_FILL_COLOUR, 0, FNT_Size_1622, 0);  //print day of week in standard colour.
	       startX = startX + textW + 0;                // Reset start position for duration to after weekday.
        }   
    }


    void PrintEventStartEnd()
    {
        //
        // Print event start and end times.
        //
     	sprintf( str, "%d:%02d%cm ~ %d:%02d%cm (%d min)",
				(startHour+23)%12 + 1, startMin, startHour >= 12 ? 'p':'a', (endHour+23)%12 + 1, endMin, endHour>= 12 ? 'p':'a', duration);
        textW = TAP_Osd_GetW(str,0,FNT_Size_1622);     // Calculate the width of the start/end time information.				
		TAP_Osd_PutStringAf1622( MemRgn, startX, startY, MAX_X, str, MAIN_TEXT_COLOUR, 0 );
	    startX = startX + textW + 5;    // Calculate where to put the progress bar.
    }


    void PrintProgressBar()
    {
        //
        // Print graphical progress bar.
        //

        if (curHour < startHour) // Catch current shows that cross over midnight
        {
            curHour = curHour + 24;
        }
        runtime  = ((curHour * 60)+curMin)-((startHour * 60)+startMin);  // Calculate how long the show has been running for.
        if (duration <= 0)      duration = 1;          // Boundary checks.
        if (runtime < 0)        runtime  = 0;          // Boundary checks.
        if (runtime > duration) runtime  = duration;   // Boundary checks.
        pbWidth = EVENT_PB_WIDTH;   // Set the standard width for the progress bar.
        if (Playback_Selected)    // Use the file position and file size for the progress bar.
        {
            runtime  = max(0,Playback_SavedPosition);
            duration = max(1,Playback_TotalBlocks);
            pbWidth  = EVENT_PB_WIDTH-10;  // For a recorded playback, we need to make the progress bar width slightly smaller.
        }
        switch (progressBarOption)
        {
               case PB_WHITE:
                           DisplayProgressBar( MemRgn, runtime, duration, startX, startY+3, pbWidth, 18, COLOR_Black,  2, MAIN_TEXT_COLOUR);
                           break;

               case PB_MULTI:
               case PB_SINGLE:
               case PB_SOLID:
                           DisplayProgressBar( MemRgn, runtime, duration, startX, startY+3, pbWidth, 18, COLOR_Black,  2, progressBarOption);
                           break;
                             
               default:
                           DisplayProgressBar( MemRgn, runtime, duration, startX, startY+3, pbWidth, 18, COLOR_Black,  2, 0);
                           break;
                             
        } 

        durPerc = max(0,((runtime*100) / duration));  // Calculate duration as percentage, and catch any negatives if runtime and duration are 0.
        durPerc = min(100, durPerc);                  // 
     	sprintf( str, "%i%%", durPerc);
		TAP_Osd_PutStringAf1622( MemRgn, startX+pbWidth+3, startY+2, MAX_X, str, MAIN_TEXT_COLOUR, 0 );
    }


    void PrintEventDescription()
    {
        //
        // Print event program Rating information if required.
        //
        LastWrapPutStr_Y = startY;      //KH Set Y coord in case there is no event description text printed.
        GetOzParentalRating(event.parentalRating,ratingText);
        if (strcmp(ratingText,"") == 0)
        {
           sprintf(str,"%s",event.description);
        }
        else
        {
           sprintf(str,"%s [%s]",event.description,ratingText);
        }
        if (TF5800ExtInfoFlag)  str[0] ='\0';

		WrapPutStr( MemRgn, str, EVENT_X+8, startY, EVENT_W-8, MAIN_TEXT_COLOUR, EVENT_FILL_COLOUR, 5, FNT_Size_1622, 1); //KH
    }		
    
    
    void DetermineProgressText()    
    {    
	     startMjd  = (event.startTime>>16) & 0xffff;
	     startHour = (event.startTime>>8)  & 0xff;
	     startMin  =  event.startTime      & 0xff;
	     endMjd    = (event.endTime>>16)   & 0xffff;
	     endHour   = (event.endTime>>8)    & 0xff;
	     endMin    =  event.endTime        & 0xff;
	     durHour   = (event.duration>>8)   & 0xff;
	     durMin    =  event.duration       & 0xff;
	     duration  = (durHour * 60) + durMin;

	     TAP_GetTime( &curMjd, &curHour, &curMin, &sec);
	     TAP_ExtractMjd(startMjd, &startYear, &startMonth, &startDay, &startWeekday);   // Grab the day of week of the start

         now   = (((curMjd*24)+curHour)*60)     + curMin;    // Calculate Now   in number of minutes.
         start = (((startMjd*24)+startHour)*60) + startMin;  // Calculate Start in number of minutes.           
         end   = (((endMjd*24)+endHour)*60)     + endMin;    // Calculate End   in number of minutes.          

	     if (start == now)
	     {       // right now	
		    strcpy(when, "Starting now");
         }
	     else if (start > now)
	     {	// future start
		    strcpy(when, "Starts in ");
		    formatTime(when, start - now);
	     }
	     else if ( start < now && end > now && (end - now) > (now - start) )
	     {
		    strcpy( when, "Started ");
		    formatTime(when, now - start);
		    strcat( when, " ago");
	     }
	     else if ( end == now )
	     { 	// right now
		    strcpy(when, "Ending now");
	     }
	     else if (end > now)
	     {	// future end
		    strcpy(when, "Ends in ");
		    formatTime(when, end - now);
	     }
	     else if ( end < now )
	     {	// past end
	        strcpy( when, "Ended ");
            diff = now - end;
		    if ( diff < 1440 )
		    {
		       formatTime(when, diff);
		       strcat( when, " ago");
            }
		    else if ( diff >= 1440 && diff < 2880 )
		    {
		         strcat( when, "Ended Yesterday");
            }
		    else
		    {
		       formatTime(when, (diff / 1440) * 1440);
		       strcat( when, " ago");
		    }
	     }
    }

    
    void PrintProgressInfo()
    {
        //
        // Print textual progress information if required.
        //
        if ((ProgressInfoOption == 1)   &&
           (!Playback_Selected))        //KH If the user has selected the ProgressInfo 1 (TEXT) option print it out.
        {
           TAP_Osd_PutStringAf1622( MemRgn, EVENT_X+8, startY, MAX_X, when, MAIN_TEXT_COLOUR, 0 );
		   startY = startY + 22;    //KH skip over the progress info.
        }
    }

    void PrintNowNextTag()
    {
       if      (currentIndex == PiPEventIndex)      strcpy( NowNextTag, "Now:"); 
       else if (currentIndex == PiPEventIndex + 1)  strcpy( NowNextTag, "Next:");
       else if (currentIndex >  PiPEventIndex + 1)  strcpy( NowNextTag, "Later:");
       else if (currentIndex <  PiPEventIndex)      strcpy( NowNextTag, "Earlier:");
       TAP_SPrint( str, "%s", NowNextTag );
       TAP_Osd_PutStringAf1419( MemRgn, startX, EVENT_Y+4+7, MAX_X, str, MAIN_TEXT_COLOUR, 0 ); // Print the tag in a small font keeping the bottom of the text lined up
	   textW = TAP_Osd_GetW(str,0,FNT_Size_1419);                                             // Calculate the width of the tag
	   startX = startX + textW + 5;    // Reset start position for event title to after the tag plus 1 character.
    }	   


    void PrintFileTag()
    {
       strcpy(str,"File:" );
       TAP_Osd_PutStringAf1419( MemRgn, startX, EVENT_Y+4+7, MAX_X, str, MAIN_TEXT_COLOUR, 0 ); // Print the tag in a small font keeping the bottom of the text lined up
	   textW  = TAP_Osd_GetW(str,0,FNT_Size_1419);                                             // Calculate the width of the tag
	   startX = startX + textW + 5;    // Reset start position for event title to after the tag plus 1 character.
    }	   
    
	MemRgn = TAP_Osd_Create( 0, 0, 720, 576, 0, OSD_Flag_MemRgn );					// In MEMORY define the whole screen for us to draw on
	TAP_Osd_FillBox( MemRgn, 0, 0, 720, 576, 0 );							// clear the memory region

//	TAP_Channel_GetInfo( _svcType, _svcNum, &chInfo );

    //
    // Blank the Event Name  bar.
    // 
	TAP_Osd_PutGd( MemRgn, EVENT_X, EVENT_Y, &_narrowhighlightGd, TRUE );	// draw the green bar
	startX = EVENT_X+3;   // set the initial x-coord for the Event Name.
	
	// Print any timer indicators.
    IndicateTimers( MemRgn, currentSvcNum, currentSvcType, &startX, EVENT_Y-2, currentEvent, 0 );   // Display timer indicators for this event in the Event Info window.
    startX = startX + 3;   // Allow some leading space for the Event Name.

    //
    // Clear the Event Information box.
    // 
	TAP_Osd_FillBox( MemRgn, EVENT_X, EVENT_Y+35, 720-EVENT_X, EVENT_H-35, EVENT_FILL_COLOUR );	// clear the bottom portion of the screen

    //
    // Print the Event Name in the main info bar.
    // 
    // Print the Now/Next/Later Tag if selected.
    if (NowNextTagOption && !Playback_Selected) PrintNowNextTag();
    if (Playback_Selected) PrintFileTag();
    // Print the Event Name.
    TAP_SPrint( str, "%s", event.eventName );
	TAP_Osd_PutStringAf1926( MemRgn, startX, EVENT_Y+4, MAX_X, str, MAIN_TEXT_COLOUR, 0 );

    startX = EVENT_X + 8;   // set the initial x-coord for the Event Information.
    //
    // Determine the text used for the progress information.  Also set the duration fields.
    //
    DetermineProgressText();
    
    // -------------------------------------------------------------------------------------
    // Main print routine for detailed event information.
    // -------------------------------------------------------------------------------------
    LastWrapPutStr_Y = EVENT_Y + 4 + 37;  //Start printing from the top of the information box.

    // Get the extended info.
    extInfo = TAP_EPG_GetExtInfo( &event ); // Get the extended information for the current event.
    // The following is some test Extended Event Information for testing purposes only.
    // extInfo = "This is the Extended Event Information. I have put more information in here to see what that does to the scrolling.  I was hoping that this could get across multiple pages to see what happens after you do multiple Info key presses. You may find that this goes over multiple lines and wrap around at the word boundary.  The quick brown fox jumps over the lazy dog. You will often find multiple pages particularly when you look at the SBS channel.  The ABC is another channel where you have several pages of extended information.  I still need to look at what happens in the routine if we have a zero length string or a really long string that fills the entire line.";
    extInfoRowSize = 19;
    extInfoFontSize = FNT_Size_1419;
    TF5800ExtInfoFlag = FALSE;
    // Test for the circumstances where the TF5800 Description Extender Hack has kicked in.
    // ie.  we have extended information AND the normal description is 127 characters AND
    // the normal description matches the same as the first 127 extended info characters.
    if ((extInfo) && (strncmp(event.description,extInfo,127)==0)) 
    {
        TF5800ExtInfoFlag=TRUE;
        extInfoRowSize = 22;
        extInfoFontSize = FNT_Size_1622;
    }    

    if (!InfoKeyPressed || (LastWrapPutStr_P == 0 && ExtInfoRows !=0)) //KH If we're here because of an event EXCEPT the Info Key or we're back to the start of the Extended Info
    {
        startX = EVENT_X+8;      // Store that starting position for  info.
        startY = EVENT_Y + 37;   // Store that starting position for  info.

        if (Playback_Selected) PrintRecorded();
        PrintDayOfWeek();                  
        PrintEventStartEnd();
        if (Playback_Selected) PrintViewedProgress();
        if (currentIndex == PiPEventIndex) PrintProgressBar();  // If this is the current event display the graphical progress bar.
        
        startY = startY + 23;   // Set starting position for progress info.
        PrintProgressInfo();

        PrintEventDescription();

    	LastWrapPutStr_Start = 0;  //KH Reset pointer to start for Extended Event information
    	ExtInfoRows = 1; //KH Assume there will be at least 1 line available for extended info.
     }

     //
     // Print Extended Event Information.
     //
     ExtInfoRows = ((EVENT_Y + EVENT_H) - LastWrapPutStr_Y ) / extInfoRowSize ;   //KH Calculate how many lines are left available for Extended Info with a font height of 19.
     if( extInfo )
     {
         WrapPutStr_StartPos = LastWrapPutStr_P; //KH Set the start position of the string to where we got up to last time.
         WrapPutStr( MemRgn, extInfo, EVENT_X+8, LastWrapPutStr_Y, EVENT_W-8, MAIN_TEXT_COLOUR, EVENT_FILL_COLOUR, ExtInfoRows, extInfoFontSize, 0);
    	 TAP_MemFree( extInfo );
     }
     else
	 {   if (InfoKeyPressed)  // Don't show the "No info" message unless the Info key was pressed.
	     {
            WrapPutStr( MemRgn, "(No Extended Event Information)", EVENT_X+8, LastWrapPutStr_Y, EVENT_W-8, MAIN_TEXT_COLOUR, EVENT_FILL_COLOUR, ExtInfoRows, FNT_Size_1419, 0);
         }        
	 } 
	 InfoKeyPressed = FALSE;
  
     //
     // Copy the memory region on to the displayed screen region.
     //
     TAP_Osd_Copy( MemRgn, rgn, EVENT_X, EVENT_Y, EVENT_W, EVENT_H, EVENT_X, EVENT_Y, FALSE );
	 TAP_Osd_Delete( MemRgn );
    
     //
     // Print an onscreen 12hour clock in the bottom right hand corner of the main window.
     // Clock is displayed straight to screen region not in memory region as it is outside Event Window.
     UpdateClock( rgn, CLOCK_X, CLOCK_Y );


}
