/************************************************************
				Part of the UK TAP Project
		This module handles displaying the Now/Next information 
		next to the channel logos.

Name	: Picture.c
Author	: kidhazy
Version	: 0.0
For		: Topfield TF5x00 series PVRs
Licence	:
Descr.	:
Usage	:
History	: v0.0 kidhazy: 26-08-05	Inception Date


	Last change:  USE   22 Aug 105    9:25 pm
**************************************************************/

// Moved the following config information to DisplayLogosConfig.c
//#define GDE_X  (BASE_X + LOGO_W + 10) // Starting x-coordiante for the Guide window.  Calculated from the Logo Window position
//#define GDE_Y  (BASE_Y - HEADER_H)    // Starting y-coordiante for the Guide window.  Calculated from the Logo Window position
//#define GDE_W  170                    // Overall width of the Guide window.
//#define GDE_H  (NUM_LOGO_ON_Y_AXIS * LOGO_SPACE_LINEAR + HEADER_H)   // Overall height of the Guide window.  
//#define PBWIDTH  128    // Width of the progressbar.
//#define PBHEIGHT 19     // Height of the progressbar.
 

#include "graphics/NarrowHighlight170x22.GD"  // Used as the green header bar at the top of the Guide window.
#include "graphics/row170x39.GD"              // Used as the background for each row of the Guide window.



       word            guideRgn;
//static int             currentGuideIndex, GuideEventIndex;
static TYPE_TapEvent   currentGuideEvent;
static TYPE_TapEvent   *GuideEvents[NUM_LOGO_ON_Y_AXIS];
static int             logoSpace = 40;
       int             guideCurIndex[NUM_LOGO_ON_Y_AXIS];
       int             guideMaxIndex[NUM_LOGO_ON_Y_AXIS];
static int             currentGuideIndex;       
       int             guideFormat = 0;   // Display format for the Guide Window 0=Now/Next 1=StartTime
       dword           displayStartTime;  // Start Time for Guide Window Display.
       int             stepMinutes=30;    // Default step interval in minutes for the Guide Window display.
       
//=====================================================================================
//
//  CHECK GUIDE INDEX
//
//=====================================================================================
void CheckGuideEventIndex(void)
{
	// If the Guide Window is NOT displayed, then simply reset the guide index and return.
    if (!guideWindowDisplayed)      
    {
        currentGuideIndex = 0;
        return;
    }
    
    // If the Guide Window is set for timeframe display then make sure that we have a valid Guide index.
    if (guideFormat == 1)
    {
        
    }
}


//=====================================================================================
//
//  SET CURRENT DATE TIME
//
//=====================================================================================
void SetCurrentDateTime(dword *currentTime)
{
	 word mjd; byte hour, min, sec; 
     
     TAP_GetTime( &mjd, &hour, &min, &sec );		// Grab the current date and time.
	 *currentTime = (mjd << 16) | (hour<<8) | min;  // Form it into event date/time format.
       	 
}



//=====================================================================================
//
//  SHOW GUIDE EVENT TITLE
//
//=====================================================================================
void ShowGuideEventTitle(word temprgn, int svcType, int svcNum, int line, int format, dword displayStartTime )
{
    char	str[256];
	TYPE_TapChInfo chInfo;
	int startX;     //KH general x co-ordinate variable.
	int startY;     //KH general y co-ordinate variable.
	int textW;      //KH general text width variable.
	int curIndex;
	word startMjd; byte startHour; byte startMin; byte sec; byte startWeekday;
	word endMjd; byte endHour; byte endMin;
	byte durHour; byte durMin;
	word curMjd; byte curHour; byte curMin; byte currWeekday;  
    dword start, end, now, diff;
	word startYear; byte startMonth; byte startDay; char DayOfWeek[10];
	int	duration, runtime, durPerc, i;
    dword x_coord, y_coord;
    TYPE_TapEvent   event;
	
    // For ease of use, map the current Guide Event depending on the format of the Guide Window.
    switch ( format )
    {
           case 0: // Now/Next Display
                   curIndex = currentGuideIndex + guideCurIndex[line];
                   if ((curIndex > guideMaxIndex[line]) ||
                       (curIndex < 0                  )) return;   //Print a blank event if outside the bounds.

                   if ((guideMaxIndex[line] == 0) && (svcNum != Playback_svcnum)) return;    // There is no events for a normal channel, so leave a blank.
                   break;
           
           default: // StartTime Display Format
                   // Need to find index of LATEST event with startTime <= displayStartTime    
               	   for ( i=guideMaxIndex[line]; i>0; i-- )	// find the latest event that starts before Display Start Time
	               {				         				// note: here i is one base, not zero base, to make the comparison easier
		               event = GuideEvents[line][i-1];
	                   if ( event.startTime <= displayStartTime )  break;
	               }
//TAP_Print("line=%d i=%d max=%d cgi=%d gci=%d ci=%d\r\n",line,i,guideMaxIndex[line],currentGuideIndex,guideCurIndex[line],currentIndex);    
//                   if (i==0) return; //We searched through all events and didn't find a later one, so print a blank event.
//                   if ((i == guideMaxIndex[line]) && (event.endTime <=displayStartTime)) return; // No events that are for this timeframe.
                   if (i==0) //We searched through all events and didn't find one that fitted the timeframe.
                   {
                       if (displayStartTime < event.startTime) i=1; // The displaystarttime is before the first event, so point to the first event.
                       else return;  // There is a gap in the events, so print a blank.
                   }
                   if ((guideMaxIndex[line] == 0) && (svcNum != Playback_svcnum)) return;    // There is no events for a normal channel, so leave a blank.
                   curIndex = i-1;
                   currentGuideIndex = curIndex - guideCurIndex[line];
                   // currentIndex =i-1;
                   break;
    }                
    
    event = GuideEvents[line][curIndex];

	// Get the current time.
	TAP_GetTime( &curMjd, &curHour, &curMin, &sec);
    // Extract the start, end time and duration details for this event.
	startMjd  = (event.startTime>>16) & 0xffff;
	startHour = (event.startTime>>8)  & 0xff;
	startMin  =  event.startTime      & 0xff;
	endMjd    = (event.endTime>>16)   & 0xffff;
	endHour   = (event.endTime>>8)    & 0xff;
	endMin    =  event.endTime        & 0xff;
	durHour   = (event.duration>>8)   & 0xff;
	durMin    =  event.duration       & 0xff;
	duration  = (durHour * 60) + durMin;

	
	TAP_ExtractMjd(startMjd, &startYear, &startMonth, &startDay, &startWeekday);   //Grab the day of week of the start
    DayOfWeek[0] = '\0';   //Start with a blank weekday.  We only include if not today.

    if (curHour < startHour) // Catch current shows that cross over midnight
    {
           curHour = curHour + 24;
    }
    runtime = ((curHour * 60)+curMin)-((startHour * 60)+startMin);  //KH calculate how long the show has been running for.
    if (duration <= 0) duration = 1;
    if (runtime  <  0) runtime  = 0;
    if (runtime  >  duration) runtime = duration;
 
    if (svcNum == Playback_svcnum)    // Use the file position and file size for the progress bar.
    {
         runtime  = Playback_SavedPosition;
         duration = Playback_TotalBlocks;
    }
    LogoPosition( &x_coord, &y_coord, line );
	x_coord = 8;   //KH set the initial x-coord for the Event Name.
	y_coord = y_coord - GDE_Y;  // Offset back to the original guide window.
    if ((displayOption == 0) && ( line == MID_POINT )) y_coord+=4;  // Carousel
    
    TAP_SPrint( str, "%s", event.eventName );
	TAP_Osd_PutStringAf1419( temprgn, x_coord, y_coord, GDE_W, str, MAIN_TEXT_COLOUR, 0 );

    if ((currentGuideIndex == 0) && (format == 0))  // We're pointing to the current event & it's the Now/Next format.
    {
       switch (progressBarOption)
       {
              case PB_WHITE:
                          DisplayProgressBar( temprgn, runtime, duration, x_coord, y_coord+19, PBWIDTH, PBHEIGHT, COLOR_Black,  2, MAIN_TEXT_COLOUR);
                          break;

              case PB_MULTI:
              case PB_SINGLE:
              case PB_SOLID:
                          DisplayProgressBar( temprgn, runtime, duration, x_coord, y_coord+19, PBWIDTH, PBHEIGHT, COLOR_Black,  2, progressBarOption);
                          break;
                             
              default:
                          DisplayProgressBar( temprgn, runtime, duration, x_coord, y_coord+19, PBWIDTH, PBHEIGHT, COLOR_Black,  2, 0);
                          break;
                             
       } 

       durPerc = max(0,((runtime*100) / duration));  // Calculate duration as percentage, and catch any negatives if runtime and duration are 0.
       sprintf( str, "%i%%", durPerc);
       TAP_Osd_PutStringAf1419( temprgn, x_coord+PBWIDTH+3, y_coord+14+6, 666, str, MAIN_TEXT_COLOUR, 0 );
    }
    else
    {
       //KH  Calculate the day of week for events that are not today events.
       if (curMjd != startMjd) GetDayName(startWeekday, DayOfWeek);

       if (DayOfWeek[0] != '\0')   //KH If the weekday was set (ie it's an earlier or later day) and we're not on the current event, then print it out first. (using only first 3 chars)
       {
           TAP_SPrint( str, "%.3s ", DayOfWeek);
	       textW = TAP_Osd_GetW(str,0,FNT_Size_1419);                                          // Calculate the width of the weekday.
	       TAP_Osd_PutStringAf(temprgn, x_coord, y_coord+14+5, x_coord+textW, str, MAIN_TEXT_COLOUR, 0, 0, FNT_Size_1419, 0);  //print day of week in standard colour.
	       x_coord = x_coord + textW;                                                           // Reset start position for duration to after weekday.
       }   

       sprintf( str, "%d:%02d%cm - %d:%02d%cm",
				(startHour+23)%12 + 1, startMin, startHour >= 12 ? 'p':'a', (endHour+23)%12 + 1, endMin, endHour>= 12 ? 'p':'a');
       TAP_Osd_PutStringAf1419( temprgn, x_coord, y_coord+14+5, GDE_W, str, MAIN_TEXT_COLOUR, 0 );
    }

    x_coord=GDE_W-5;  // Print the timer indicators at the far right hand side of the Guide window.
    IndicateTimers( temprgn , svcNum, svcType, &x_coord, y_coord, event, 1);  // Display timer indicators for this event in the Guide window.


}

//=====================================================================================
//
//  LOAD GUIDE EVENT INFO
//
//=====================================================================================
void LoadGuideEventInfo(int svcType, int svcNum)
{
	byte 	hour, min, sec;
    word	mjd;
	dword	currentTime;
	int i, line, index;
    int countTvSvc, countRadioSvc, maxSvc, service;
	word curMjd; byte curHour; byte curMin; byte currWeekday;  
    TYPE_TapEvent   currentGuideEvent;
    
    appendToLogfile("LoadGuideEventInfo: Started.");

	TAP_Channel_GetTotalNum( &countTvSvc, &countRadioSvc );
	countTvSvc++;            //KH Increase TV Service count to accommodate virtual playback channel.
	
	if ( svcType == SVC_TYPE_Tv ) maxSvc = countTvSvc - 1;
	else maxSvc = countRadioSvc - 1;
   
   //  Calculate the first service being displayed in the logo list.   
	switch ( displayOption )
	{																				// Carousel
	    case 0 : 	if ( svcNum >= MID_POINT ) service = svcNum - MID_POINT;		// find 1st channel on the carousel
					else service = maxSvc - ( MID_POINT - svcNum - 1);
					break;
																					// Linear
	    case 1 :	service = svcNum / NUM_LOGO_ON_Y_AXIS;							// find the 1st channel on this page
					service = service * NUM_LOGO_ON_Y_AXIS;
		
		default : break;
	}
	
	// For all of the logos displayed, load the events.
	for ( line=0; line<NUM_LOGO_ON_Y_AXIS; line++ )
	{
	      if( GuideEvents[line] )														// release any old memory used to store the events
	      {
		      TAP_MemFree( GuideEvents[line] );
		      GuideEvents[line] = 0;
	      }
          if ((service == Playback_svcnum) && (svcType == SVC_TYPE_Tv))
          {
              // load up the events with the event information from the recording.
              // TYPE_Playinfo has a evtInfo 
              GuideEvents[line] = &Playback_event;  // Associate event information with original playback event.
              guideMaxIndex[line] = 0;               // There is only ever 1 event associated with the playback.
              guideCurIndex[line] = 0;               // Mark the first playback event as the current event.
          }
          else
          {
              appendIntToLogfile("LoadGuideEventInfo: Getting events via TAP_GetEvent for Service = %d.",service);
	          GuideEvents[line] = TAP_GetEvent( svcType, service, &guideMaxIndex[line] );			// get all the events for this new channel
              appendIntToLogfile("LoadGuideEventInfo: Retrieved %d events via TAP_GetEvent.",guideMaxIndex[line]);
              if( !GuideEvents[line] )
              {
		          // No events
		          guideMaxIndex[line] = 0;               // There is no events
                  guideCurIndex[line] = 0;               // Mark the first playback event as the current event.
              }
              else  // Look for the current event.
              {   
                  TAP_GetTime( &mjd, &hour, &min, &sec );			// now look for the current event
	              currentTime = (mjd << 16) | (hour<<8) | min;		// must manually search through the list
																	// comparing start and end times
                  appendToLogfile("LoadGuideEventInfo: Starting to look for current event.");
                  for ( i=0; i<guideMaxIndex[line]-1; i++ )
	              {
		              currentGuideEvent = GuideEvents[line][i];
	                  if (( currentTime >= currentGuideEvent.startTime ) && ( currentTime < currentGuideEvent.endTime )) break;
                  }
	              guideCurIndex[line] = i;   
                  appendIntToLogfile("LoadGuideEventInfo: Finished looking for current event at index=%d.",i);
              }
          }
	      if ( service < maxSvc ) service++;
	      else
          if (displayOption == 0) service = 0;  // Carousel, so wrap around.
          else break;                           // Linear, so stop after last service
	}
    appendToLogfile("LoadGuideEventInfo: Ended.");
	
}	




//=====================================================================================
//
//  DRAW BLANK GUIDE WINDOW
//
//=====================================================================================
void DrawBlankGuideWindow(word tempRgn)
{
    dword x_coord, y_coord;
    int i;
         
    TAP_Osd_PutGd( tempRgn, 0, 0, &_narrowhighlight170x22Gd, TRUE );	// draw the green bar
         
    for ( i=0; i<NUM_LOGO_ON_Y_AXIS; i++)
    {
	    LogoPosition( &x_coord, &y_coord, i );
	    y_coord = y_coord - GDE_Y;  // Offset back to Guide window original origin
        if ((displayOption == 0) && ( i == MID_POINT )) y_coord+=4;  // Carousel
        TAP_Osd_PutGd( tempRgn, 0, y_coord, &_row170x39Gd, TRUE );
    }
}     



//=====================================================================================
//
//  DISPLAY GUIDE NOW NEXT INDICATOR
//
//=====================================================================================
void DisplayGuideNowNextIndicator(int index, word tempRgn)
{
     char str[10];
     
     if (index ==  0)  strcpy(  str, "Now");
     if (index ==  1)  strcpy(  str, "Next");
     if (index >   1)  sprintf( str, "Next+%d", index-1);
     if (index == -1)  strcpy(  str, "Earlier");
     if (index <  -1)  sprintf( str, "Earlier%d", index+1);

     PrintCenter( tempRgn, 0, 0, GDE_W, str, COLOR_White, 0, FNT_Size_1419);
     
}     


//=====================================================================================
//
//  PRINT GUIDE DATE TIME
//
//=====================================================================================
void PrintGuideDateTime(dword startTime, int stepMinutes, word tempRgn)
{
	 word curMjd;   byte hour,      min,     sec; 
	 word startMjd; byte startHour, startMin;
	 word endMjd;   byte endHour, endMin;
	 word year; byte month, day, weekday; char DayOfWeek[10];
     char str[50], d_str[30];

     str[0] = '\0';  // Blank out the string.
     
     // Get the current mjd.
     TAP_GetTime( &curMjd, &hour, &min, &sec );		// Grab the current date and time.

     // Calculate the MJD of the startTime and get the weekday.
	 startMjd  = (startTime>>16) & 0xffff;
     startHour = (startTime>>8)  & 0xff;
	 startMin  =  startTime      & 0xff;
     // Calculate the end time.
     endMin  = startMin + stepMinutes;
     endHour = startHour + (endMin / 60);
     endMin  = endMin % 60;
     endMjd  = startMjd + (endHour / 24);
     endHour = endHour % 24;

     // Grab the day of week of the start
     TAP_ExtractMjd(startMjd, &year, &month, &day, &weekday);   

     GetDayName(weekday, DayOfWeek);

     //Format the time string eg. "12:15am - 12:45am"
     //     sprintf( d_str, "%d:%02d%cm-%d:%02d%cm",
     //				(startHour+23)%12 + 1, startMin, startHour >= 12 ? 'p':'a', (endHour+23)%12 + 1, endMin, endHour>= 12 ? 'p':'a');
     //Format the time string eg. "12:15am"
     sprintf( d_str, "%.3s %02d. %d:%02d%cm",
				DayOfWeek, day, (startHour+23)%12 + 1, startMin, startHour >= 12 ? 'p':'a');
     strcat(str, d_str);  // Add the timeframe string to overall string.
     PrintCenter( tempRgn, 0, 0, GDE_W, str, COLOR_White, 0, FNT_Size_1419);
       	 
}



//=====================================================================================
//
//  DISPLAY GUIDE WINDOW
//
//=====================================================================================
void DisplayGuideWindow(int svcType, int svcNum)
{
    int  line, service;
    dword x_coord, y_coord;
    int countTvSvc, countRadioSvc, maxSvc;

    word MemGdeRgn;
    
    appendIntToLogfile("DisplayGuideWindow: Started for service %d.",svcNum);

	MemGdeRgn = TAP_Osd_Create( GDE_X, GDE_Y, GDE_W, GDE_H, 0, OSD_Flag_MemRgn );  // In MEMORY define the region for us to draw on
	TAP_Osd_FillBox( MemGdeRgn, 0, 0, GDE_W, GDE_H, COLOR_Black );				   // clear the memory region

    DrawBlankGuideWindow( MemGdeRgn ); 

	TAP_Channel_GetTotalNum( &countTvSvc, &countRadioSvc );
	countTvSvc++;            //KH Increase TV Service count to accommodate virtual playback channel.
	
	if ( svcType == SVC_TYPE_Tv ) maxSvc = countTvSvc    - 1;
	else                          maxSvc = countRadioSvc - 1;
   
	switch ( displayOption )
	{																				// Carousel
	    case 0 : 	if ( svcNum >= MID_POINT ) service = svcNum - MID_POINT;		// find 1st channel on the carousel
					else service = maxSvc - ( MID_POINT - svcNum - 1);
					break;
																					// Linear
	    case 1 :
        default:	service = svcNum  / NUM_LOGO_ON_Y_AXIS;							// find the 1st channel on this page
					service = service * NUM_LOGO_ON_Y_AXIS;
	}
	for ( line=0; line<NUM_LOGO_ON_Y_AXIS; line++ )
	{
         appendIntToLogfile("DisplayGuideWindow: Calling ShowGuideEventTitle routine for line %d.",line);
         ShowGuideEventTitle(MemGdeRgn, svcType, service, line, guideFormat, displayStartTime);
         appendToLogfile("DisplayGuideWindow: Back from ShowGuideEventTitle routine.");
		 if ( service < maxSvc ) service++;
         else 
         if (displayOption == 0) service = 0;  // Carousel
         else break;                           // Linear
	}

    appendToLogfile("DisplayGuideWindow: Calling DisplayGuideNowNextIndicator routine.");
    switch (guideFormat)
    {
       case 0: // Now/Next display
               DisplayGuideNowNextIndicator( currentGuideIndex, MemGdeRgn );  // Display the Now/Next indicator at the top of the window.
               break;
              
       case 1: // Start Time display
               PrintGuideDateTime(displayStartTime, stepMinutes, MemGdeRgn);
               break;
    }             
    appendToLogfile("DisplayGuideWindow: Back from DisplayGuideNowNextIndicator routine.");

    // Copy the memory region on to the displayed screen region.
    TAP_Osd_Copy(   MemGdeRgn, guideRgn, 0, 0, GDE_W, GDE_H, 0, 00, FALSE );
	TAP_Osd_Delete( MemGdeRgn );
    
    appendToLogfile("DisplayGuideWindow: Ended.");

}




//=====================================================================================
//
//  PREVIOUS GUIDE EVENT
//
//=====================================================================================
void PreviousGuideEvent(int svcType, int svcNum)
{
     guideFormat = 0;  // Reset the Guide Display Format to Now/Next Display Format
     if (currentIndex > 0)     currentGuideIndex--;
     else return;
//     SetCurrentDateTime( &displayStartTime );  // Reset display Start Time to current time.
//     if (guideWindowDisplayed) DisplayGuideWindow( svcType, svcNum );
}

//=====================================================================================
//
//  NEXT GUIDE EVENT
//
//=====================================================================================
void NextGuideEvent(int svcType, int svcNum)
{
     guideFormat = 0;  // Reset the Guide Display Format to Now/Next Display Format
     if (currentIndex < eventCount-1)  currentGuideIndex++;
     else return;
//     SetCurrentDateTime( &displayStartTime );  // Reset display Start Time to current time.
//     if (guideWindowDisplayed) DisplayGuideWindow( svcType, svcNum );
}





//=====================================================================================
//
//  CALCULATE NEXT TIMEFRAME
//
//=====================================================================================
void CalculateNextTime(dword *displayStartTime, int stepMinutes)
{
	 dword displayStartMjd; byte displayStartHour; byte displayStartMin; 
     
     if (currentIndex >= eventCount-1)  return;

     // Extract the Display Start Time.
	 displayStartMjd  = (*displayStartTime>>16) & 0xffff;
	 displayStartHour = (*displayStartTime>>8)  & 0xff;
	 displayStartMin  =  *displayStartTime      & 0xff;
     // Calculate the new display start time.
     displayStartMin  = displayStartMin + stepMinutes;

     displayStartHour = displayStartHour + (displayStartMin / 60);
     displayStartMin  = displayStartMin % 60;
     displayStartMjd  = displayStartMjd + (displayStartHour / 24);
     displayStartHour = displayStartHour % 24;
     
     displayStartMin  = (displayStartMin / stepMinutes) * stepMinutes;  // Round down the minutes to the prior stepMinutes.
     
     *displayStartTime = (displayStartMjd<<16) | (displayStartHour<<8) | (displayStartMin);
  	 
}


//=====================================================================================
//
//  CALCULATE PREVIOUS TIMEFRAME
//
//=====================================================================================
void CalculatePreviousTime(dword *displayStartTime, int stepMinutes)
{
	 dword displayStartMjd; byte displayStartHour; byte displayStartMin; 
	 dword oldDisplayStartTime, newDisplayStartTime;
     TYPE_TapEvent firstEvent;
     
//     if (currentIndex <= 0)     return;

     // Store the current start time in case we need to restore back to it.
     oldDisplayStartTime = *displayStartTime;
     
     // Extract the Display Start Time.
	 displayStartMjd  = (*displayStartTime>>16) & 0xffff;
	 displayStartHour = (*displayStartTime>>8)  & 0xff;
	 displayStartMin  =  *displayStartTime      & 0xff;
     // Calculate the new display start time.
	 if ((displayStartMin % stepMinutes) > 0) displayStartMin  = (displayStartMin / stepMinutes) * stepMinutes;  // Round down the minutes to the prior stepMinutes.
     else  // We're already on a stepMinutes boundary, so just subtract the number of minutes.
	 if (displayStartMin >= stepMinutes)  displayStartMin = displayStartMin - stepMinutes;
	 else
	 {
        displayStartMin = 60 + displayStartMin - stepMinutes;
        if (displayStartHour > 0)  // If we've gone back an hour, but not across midnight.
        {
           displayStartHour--;
        }
        else          // We've gone back over midnight.
        {
           displayStartHour = 23;  // stepMinutes will have a max of 60 minutes, so we'll never go back past 11pm.
           displayStartMjd--;
        }
     }
    
     newDisplayStartTime = (displayStartMjd<<16) | (displayStartHour<<8) | (displayStartMin);
     // Make sure the new start time is not before the earliest event that we have.
	 firstEvent = events[0];
     if (newDisplayStartTime >= firstEvent.startTime)
        *displayStartTime = newDisplayStartTime;
     else
        *displayStartTime = oldDisplayStartTime;   // Return to the previous start time.
  	 
}


//=====================================================================================
//
//  DISPLAY TIMEFRAME EVENT TITLE
//
//=====================================================================================

void DisplayTimeframeEventTitle(int svcType, int svcNum, dword displayStartTime )		// Move the display to the event in the current timeframe.
{
	dword	currentEndTime;
	int 	i, line;
	char	str[256];

    LoadEventInfo( svcType, svcNum );  // Load the events into the events array.

 	if( !events )														// check there is at least one event for this channel
	{
		DisplayNoInfoMessage();
		noEvents = TRUE;
		return;
	}
	else noEvents = FALSE;
	
    // Need to find index of LATEST event with startTime <= displayStartTime    
    for ( i=eventCount; i>0; i-- )	// find the latest event that starts before Display Start Time
	{								// note: here i is one base, not zero base, to make the comparison easier
	    currentEvent = events[i-1];
	    if ( currentEvent.startTime <= displayStartTime )  break;
	}
//TAP_Print("i=%d ec=%d ci=%d \r\n",i,eventCount,currentIndex);    
    if (i==0) //We searched through all events and didn't find one that fitted the timeframe.
    {
          if (displayStartTime < currentEvent.startTime) i=1; // The displaystarttime is before the first event, so point to the first event.          
          else  // We didn't find an event, and the displayStartTime is after the first event, so display a blank event.
          {
             currentIndex = 0;
             eventCleared = FALSE;
             noEvents = TRUE;
             DisplayNoInfoMessage();
          }
    }
    if (!i==0) // We found an event.
    {
//          noEvents = FALSE;
          currentIndex =i-1;
//	      ShowEventDetails( currentEvent );									// call the main display routine
//TAP_Print("found i=%d ec=%d ci=%d \r\n",i,eventCount,currentIndex);    
    }
    // Work out which line we're pointing to.
	switch ( displayOption )
	{																				// Carousel
	    case 0 : 	
                    line = 3;  //  Highlighted channel is always the middle.
					break;
	    case 1 :	
                    line = currentSvcNum % NUM_LOGO_ON_Y_AXIS;  //  Highlighted channel is always the middle.
					break;
		default : break;
	}
	// Update the guide index so if we move to the next/previous event then it starts from the right event.
    currentGuideIndex = currentIndex - guideCurIndex[line];
TAP_Print("line=%d cgi=%d gci=%d ci=%d\r\n",line,currentGuideIndex,guideCurIndex[line],currentIndex);    

//    if (i+currentGuideIndex >= eventCount) currentGuideIndex = (eventCount-1)-i;   /// Boundary check to make sure that we point at the last event.
//    if (i+currentGuideIndex < 0) currentGuideIndex = 0-i;   // Boundary check to make sure that we point at the first event.
//	currentIndex = i + currentGuideIndex;                   // This is the index of the event we are currently pointing to.
//	currentEvent = events[ currentIndex ];                  // This is the event that we are pointing to.
	PiPEventIndex = guideCurIndex[line];                    //PiP will always be showing the current event.
	
//	ShowEventTitle( currentEvent );							// display the title now, and the full description later
    ShowEventDetails( currentEvent );	
}




//=====================================================================================
//
//  DISPLAY TIMEFRAME EVENT
//
//=====================================================================================

void DisplayTimeframeEvent( dword displayStartTime )		// Move the display to the event in the current timeframe.
{
	dword	currentEndTime;
	int 	i, line;
	char	str[256];

	if( !events ) { DisplayNoInfoMessage(); return; }					// bounds check
	
    // Need to find index of LATEST event with startTime <= displayStartTime    
    for ( i=eventCount; i>0; i-- )	// find the latest event that starts before Display Start Time
	{								// note: here i is one base, not zero base, to make the comparison easier
	    currentEvent = events[i-1];
	    if ( currentEvent.startTime <= displayStartTime )  break;
	}
//TAP_Print("i=%d ec=%d ci=%d \r\n",i,eventCount,currentIndex);    
    if (i==0) //We searched through all events and didn't find one that fitted the timeframe.
    {
          if (displayStartTime < currentEvent.startTime) i=1; // The displaystarttime is before the first event, so point to the first event.          
          else  // We didn't find an event, and the displayStartTime is after the first event, so display a blank event.
          {
             currentIndex = 0;
             eventCleared = FALSE;
             noEvents = TRUE;
             DisplayNoInfoMessage();
          }
    }
    if (!i==0) // We found an event.
    {
          noEvents = FALSE;
          currentIndex =i-1;
	      ShowEventDetails( currentEvent );									// call the main display routine
//TAP_Print("found i=%d ec=%d ci=%d \r\n",i,eventCount,currentIndex);    
    }
    // Work out which line we're pointing to.
	switch ( displayOption )
	{																				// Carousel
	    case 0 : 	
                    line = 3;  //  Highlighted channel is always the middle.
					break;
	    case 1 :	
                    line = currentSvcNum % NUM_LOGO_ON_Y_AXIS;  //  Highlighted channel is always the middle.
					break;
		default : break;
	}
	// Update the guide index so if we move to the next/previous event then it starts from the right event.
    currentGuideIndex = currentIndex - guideCurIndex[line];
TAP_Print("line=%d cgi=%d gci=%d ci=%d\r\n",line,currentGuideIndex,guideCurIndex[line],currentIndex);    

}





//=====================================================================================
//
//  CLOSE GUIDE WINDOW
//
//=====================================================================================
void CloseGuideWindow()
{
	guideFormat = 0;  // Reset the Guide Display Format to Now/Next Display Format
   TAP_Osd_Delete(guideRgn);
}





//=====================================================================================
//
//  ACTIVATE GUIDE WINDOW
//
//=====================================================================================
void ActivateGuideWindow( void )
{
	guideWindowDisplayed = TRUE;
	guideFormat = 0;  // Reset the Guide Display Format to Now/Next Display Format
    SetCurrentDateTime( &displayStartTime );  // Set display Start Time to current time.
//    currentGuideIndex    = 0;
    guideRgn = TAP_Osd_Create( GDE_X, GDE_Y, GDE_W, GDE_H, 0, FALSE );	// define a section on the main window for the guide.
}





//=====================================================================================
//
//  INITIALISE GUIDE WINDOW
//
//=====================================================================================
void InitialiseGuideWindow( void )
{
     guideWindowDisplayed = FALSE;
     currentGuideIndex    = 0;
}     

