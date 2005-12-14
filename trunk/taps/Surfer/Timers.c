/************************************************************
				Part of the UK TAP Project
		This module handles display and setting of  the Timer Indicators

Name	: Timers.c
Author	: kidhazy
Version	: 0.0
For		: Topfield TF5x00 series PVRs
Licence	:
Descr.	:
Usage	:
History	: v0.0 kidhazy: 05-09-05	Inception Date
          v0.1 kidhazy: 09-09-05    Colour changes.
          v0.2 kidhazy: 27-09-05    Set nameFix=1 in the timer definition.

	Last change:  USE   6 Sept 2005
**************************************************************/

#define TIMER_PERC 50 // Percent of an event that a timer must cover before it is indicated.
#define REC_PERC 0 // Percent of an event that a recording must cover before it is indicated.

#include "graphics/RedCircle.GD"
#include "graphics/YellowCircle.GD"
#include "graphics/OrangeCircle.GD"
#include "graphics/PinkCircle.GD"
#include "graphics/GreenCircle.GD"

#include "graphics/solidgreenbar5x39.GD"
#include "graphics/solidredbar5x39.GD"
#include "graphics/solidyellowbar5x39.GD"
#include "graphics/solidorangebar5x39.GD"
#include "graphics/solidpinkbar5x39.GD"



//------------------------------ NumericDateTime --------------------------------------
//
long	NumericDateTime( word Date, dword Hour, dword Min, byte Sec )
{
	word	BaseDate; 
	long	NumericTime;

	BaseDate = TAP_MakeMjd( 2000, 01, 01 );

	NumericTime = ( ( Date - BaseDate) * 86400 );
	NumericTime = NumericTime + ( Hour * 3600 ) + ( Min * 60 ) + Sec;

	return NumericTime;
}

//------------------------------ NumericStartDateTime --------------------------------------
//
long	NumericStartDateTime( dword startTime)
{
	word	Date;
	dword   Hour, Min;

	long	NumericTime;

	Date     = ( startTime>>16)&0xffff;
	Hour     = ((startTime>>8)&0xff);
	Min      =  (startTime&0xff);
     
	NumericTime = NumericDateTime( Date, Hour, Min, 0);

	return NumericTime;
}

//------------------------------ NumericEndDateTime --------------------------------------
//
long	NumericEndDateTime( TYPE_TimerInfo timer)
{
	word	Date;
	dword   Hour, Min, EndMin;

	long	NumericTime;

	Date     = ( timer.startTime>>16)&0xffff;
	Hour     = ((timer.startTime>>8)&0xff);
	Min      =  (timer.startTime&0xff);
	EndMin   = Min + timer.duration;

	NumericTime = NumericDateTime( Date, Hour, EndMin, 0);

	return NumericTime;
}

//------------------------------ CurrentTime --------------------------------------
//
long	CurrentTime()
{
	word	Date; 
	long	NumericTime;
	byte	Hour, Min, Sec;

	TAP_GetTime( &Date, &Hour, &Min, &Sec );
	NumericTime = NumericDateTime( Date, Hour, Min, Sec );

	return NumericTime;
}

//------------------------------ TimersStartTime --------------------------------------
//

bool	TimersStartTime( word myStartTime, int iCheckMin)
{
//	Check if timer starts in the next iCheckMin minutes

	int	    iTimerHour, iTimerMin;
	word	wTimerDate;
	long	lTimerTime;

	wTimerDate = ( myStartTime >> 16 ) &0xffff;
	iTimerHour = ( myStartTime >> 8 & 0xff );
    iTimerMin =  ( myStartTime & 0xff );

	lTimerTime = NumericDateTime( wTimerDate, iTimerHour, iTimerMin, 0 );

	return ( lTimerTime < ( CurrentTime() + ( iCheckMin * 60 ) ) );
}

// ------------------------------------------------------------
  

/******************************************************************************
Indicates the timer specified by the given index if it falls within the bounds of
the displayed event.
idx: index of timer list entry

Displays 3 different colour highlights:
   Red - entire event is covered by timer
   Orange - end of event is covered by timer, but start of the event is not.
   Yellow - start of event is covered by timer, but end of the event is not.   
   Pink - middle of event is covered by timer.
   Green - entire event is covered by a preselect/watching timer.      
******************************************************************************/
void IndicateTimer( int idx, int svcNum, int svcType, word windowRgn, dword *xCoord, int yCoord, TYPE_TapEvent event, int where)
{
	TYPE_TimerInfo timer;
	dword timerStartMin, timerEndMin, eventStartMin, eventEndMin; // in minutes since midnight
	long timerStartTime, timerEndTime, eventStartTime, eventEndTime;  // time in seconds since 01 Jan 2000
	word timerMjd, eventStartMjd, eventEndMjd, currentMjd;
	byte hour, min, sec;
	word year; byte month; byte day; byte currentWeekday; byte eventWeekday;
    int startPercent, endPercent;
    int match = 0;

	if (TAP_Timer_GetInfo(idx, &timer))
	{
		if (timer.svcType != svcType || timer.svcNum != svcNum )  // Is timer for the specified service?
		{
			return;
		}

        TAP_GetTime( &currentMjd, &hour, &min, &sec ); // Get the current time.
        TAP_ExtractMjd(currentMjd, &year, &month, &day, &currentWeekday);   // Grab the current day of week.

		timerMjd       = ( timer.startTime>>16)&0xffff;
		timerStartMin  = (((timer.startTime>>8)&0xff) * 60) + (timer.startTime&0xff); //minutes past midnight
		timerEndMin    = timerStartMin + timer.duration;

		eventStartMjd  = (event.startTime>>16)&0xffff;
		eventEndMjd    = (event.endTime  >>16)&0xffff;
		eventStartMin  = (((event.startTime>>8)&0xff) * 60) + (event.startTime&0xff); //minutes past midnight
		eventEndMin    = (((event.endTime  >>8)&0xff) * 60) + (event.endTime&0xff);   //minutes past midnight
        TAP_ExtractMjd(eventStartMjd, &year, &month, &day, &eventWeekday);   // Grab the event day of week.

        switch ( timer.reservationType )
        {
               case 0: // RESERVE_TYPE_Onetime
                       break;
               case 1: // RESERVE_TYPE_Everyday:
                       timerMjd = eventStartMjd;
                       break;
               case 2: // RESERVE_TYPE_EveryWeekend:
                       if (eventWeekday > 4) timerMjd = eventStartMjd; // Event is on the weekend.
                       break;
               case 3: // RESERVE_TYPE_Weekly:
                       if ((eventStartMjd - timerMjd)%7 == 0) timerMjd = eventStartMjd; // Event is on the same weekday as the timer.
                       break;
               case 4: // RESERVE_TYPE_WeekDay                    
                       if (eventWeekday < 5) timerMjd = eventStartMjd; // Event is on a weekday.
                       break;
        }               

        //Work out the start time of the timer using the modified timer start MJD value.
		timerStartTime = NumericDateTime( timerMjd, 0, timerStartMin, 0); // Time in seconds since 01 Jan 2000
		timerEndTime   = NumericDateTime( timerMjd, 0, timerEndMin, 0);   // Time in seconds since 01 Jan 2000

		eventStartTime = NumericDateTime( eventStartMjd, 0, eventStartMin, 0); // Time in seconds since 01 Jan 2000
		eventEndTime   = NumericDateTime( eventEndMjd, 0, eventEndMin, 0);   // Time in seconds since 01 Jan 2000

        if ( eventEndTime - eventStartTime > 0 )  // Catch any 0 duration events.
        {
           startPercent = 100 * (timerEndTime - eventStartTime) / (eventEndTime - eventStartTime);  // Percentage of start of event covered by end   timer of Timer.
           endPercent   = 100 * (eventEndTime - timerStartTime) / (eventEndTime - eventStartTime);  // Percentage of end   of event covered by start timer of Timer. 
        }
        else
        {
            startPercent = 0;
            endPercent   = 0;
        }

       switch (timer.isRec)
       {
              case 0:   // Check matches for 'watching' timers.
                        if ((eventStartTime >= timerStartTime) && (eventEndTime <= timerEndTime)) // FULL match & Watching event
                           match = 1;
                        break;
                        
              case 1:   // Check matches for recording timers.
                        if      ((eventStartTime >= timerStartTime) && (eventEndTime <= timerEndTime)) // FULL match
                             match = 2;
                        else if ((timerStartTime <= eventStartTime) && (timerEndTime <  eventEndTime) && (timerEndTime   > eventStartTime) && (startPercent > TIMER_PERC)) // START covered
                             match = 3;
                        else if ((timerStartTime >  eventStartTime) && (timerEndTime >= eventEndTime) && (timerStartTime < eventEndTime)   && (endPercent   > TIMER_PERC)) // END covered
                             match = 4;
                        else if ((timerStartTime >  eventStartTime) && (timerEndTime <  eventEndTime)) // Partially covered in MIDDLE
                             match = 5;
                        break;
       }
       
       switch (where)
       {
              case 0:  // Print the timer indicators in the Event Information window, so use round circles.
                     switch (match)
                     {
                            case 0: break;
                            case 1: TAP_Osd_PutGd( windowRgn, *xCoord, yCoord, &_greencircleGd, TRUE );	 // WATCHING event - draw the green dot
                                    TAP_Osd_PutStringAf1622( windowRgn, *xCoord + 7, yCoord+8, 700, "W", MAIN_TEXT_COLOUR, 0 );
                                    break;
                            case 2: TAP_Osd_PutGd( windowRgn, *xCoord, yCoord, &_redcircleGd, TRUE );	 // FULL recording event - draw the red dot
                                    TAP_Osd_PutStringAf1622( windowRgn, *xCoord + 9, yCoord+8, 700, "R", MAIN_TEXT_COLOUR, 0 );
                                    break;
                            case 3: TAP_Osd_PutGd( windowRgn, *xCoord, yCoord, &_yellowcircleGd, TRUE ); // PARTIAL START recording event - draw the yellow dot
                                    TAP_Osd_PutStringAf1622( windowRgn, *xCoord + 9, yCoord+8, 700, "R", COLOR_Black, 0 );
                                    break;
                            case 4: TAP_Osd_PutGd( windowRgn, *xCoord, yCoord, &_orangecircleGd, TRUE ); // PARTIAL END recording event - draw the orange dot
                                    TAP_Osd_PutStringAf1622( windowRgn, *xCoord + 9, yCoord+8, 700, "R", COLOR_Black, 0 );
                                    break;
                            case 5: TAP_Osd_PutGd( windowRgn, *xCoord, yCoord, &_pinkcircleGd, TRUE );	 // PARTIAL MIDDLE recording event - draw the pink dot
                                    TAP_Osd_PutStringAf1622( windowRgn, *xCoord + 9, yCoord+8, 700, "R", COLOR_Black, 0 );
                                    break;
                     }
                     if (match > 0 ) *xCoord = *xCoord + 30;  // Move the x-coordinate past the printed dot.
                     break;
       
              case 1: // Print the timer indicators in the Guide window, so use bars.  (One at the left, the other at the right)
                     switch (match)
                     {
                            case 0: break;
                            case 1: TAP_Osd_PutGd( windowRgn, *xCoord, yCoord, &_solidgreenbar5x39Gd, TRUE );	 // WATCHING event - draw the green dot
                                    TAP_Osd_PutGd( windowRgn, 0,       yCoord, &_solidgreenbar5x39Gd, TRUE );	 // WATCHING event - draw the green dot
                                    break;
                            case 2: TAP_Osd_PutGd( windowRgn, *xCoord, yCoord, &_solidredbar5x39Gd, TRUE );	 // FULL recording event - draw the red dot
                                    TAP_Osd_PutGd( windowRgn, 0,       yCoord, &_solidredbar5x39Gd, TRUE );	 // FULL recording event - draw the red dot
                                    break;
                            case 3: TAP_Osd_PutGd( windowRgn, *xCoord, yCoord, &_solidyellowbar5x39Gd, TRUE ); // PARTIAL START recording event - draw the yellow dot
                                    TAP_Osd_PutGd( windowRgn, 0,       yCoord, &_solidyellowbar5x39Gd, TRUE ); // PARTIAL START recording event - draw the yellow dot
                                    break;
                            case 4: TAP_Osd_PutGd( windowRgn, *xCoord, yCoord, &_solidorangebar5x39Gd, TRUE ); // PARTIAL END recording event - draw the orange dot
                                    TAP_Osd_PutGd( windowRgn, 0,       yCoord, &_solidorangebar5x39Gd, TRUE ); // PARTIAL END recording event - draw the orange dot
                                    break;
                            case 5: TAP_Osd_PutGd( windowRgn, *xCoord, yCoord, &_solidpinkbar5x39Gd, TRUE );	 // PARTIAL MIDDLE recording event - draw the pink dot
                                    TAP_Osd_PutGd( windowRgn, 0,       yCoord, &_solidpinkbar5x39Gd, TRUE );	 // PARTIAL MIDDLE recording event - draw the pink dot
                                    break;
                     }
                     break;
       }
                   
    }
    
}

/******************************************************************************
Indicates the current recordings by the specified slot.
******************************************************************************/
void IndicateRecording( int recSlot, int svcNum, int svcType, word windowRgn, dword *xCoord, int yCoord, TYPE_TapEvent event, int where)
{
	TYPE_RecInfo recInfo;
	dword recStartMin, recEndMin, eventStartMin, eventEndMin; // in minutes since midnight
	long recStartTime, recEndTime, eventStartTime, eventEndTime;  // time in seconds since 01 Jan 2000
	word recMjd, eventStartMjd, eventEndMjd, currentMjd;
	byte hour, min, sec;
	word year; byte month; byte day; byte currentWeekday; byte eventWeekday;
    int startPercent, endPercent;
    int match = 0;
    char str[5];


    if (TAP_Hdd_GetRecInfo(recSlot, &recInfo))
	{
		if (recInfo.svcType != svcType || recInfo.svcNum != svcNum || recInfo.recType !=RECTYPE_Normal)  // Is this a normal recording for the specified service?
		{
			return;
		}

        TAP_GetTime( &currentMjd, &hour, &min, &sec ); // Get the current time.
        TAP_ExtractMjd(currentMjd, &year, &month, &day, &currentWeekday);   // Grab the current day of week.

		recMjd       = (recInfo.startTime>>16)&0xffff;
		recStartMin  = (((recInfo.startTime>>8)&0xff) * 60) + (recInfo.startTime&0xff); //minutes past midnight
		recEndMin    = recStartMin + recInfo.duration;

		eventStartMjd  = (event.startTime>>16)&0xffff;
		eventEndMjd    = (event.endTime>>16)&0xffff;
		eventStartMin  = (((event.startTime>>8)&0xff) * 60) + (event.startTime&0xff); //minutes past midnight
		eventEndMin    = (((event.endTime>>8)&0xff)   * 60) + (event.endTime&0xff);   //minutes past midnight
        TAP_ExtractMjd(eventStartMjd, &year, &month, &day, &eventWeekday);   // Grab the event day of week.


        //Work out the start time of the recording using the modified recording start MJD value.
		recStartTime = NumericDateTime( recMjd, 0, recStartMin, 0); // Time in seconds since 01 Jan 2000
		recEndTime   = NumericDateTime( recMjd, 0, recEndMin, 0);   // Time in seconds since 01 Jan 2000

		eventStartTime = NumericDateTime( eventStartMjd, 0, eventStartMin, 0); // Time in seconds since 01 Jan 2000
		eventEndTime   = NumericDateTime( eventEndMjd, 0, eventEndMin, 0);   // Time in seconds since 01 Jan 2000

        if ( eventEndTime - eventStartTime > 0 )  // Catch any 0 duration events.
        {
           startPercent = 100 * (recEndTime - eventStartTime) / (eventEndTime - eventStartTime);  // Percentage of start of event covered by end   timer of Recording.
           endPercent   = 100 * (eventEndTime - recStartTime) / (eventEndTime - eventStartTime);  // Percentage of end   of event covered by start timer of Recording. 
        }
        else
        {
            startPercent = 0;
            endPercent   = 0;
        }
        
        if    ((eventStartTime >= recStartTime)  && (eventEndTime <= recEndTime)) // FULL match
              match = 2;
        else if ((recStartTime <= eventStartTime) && (recEndTime <  eventEndTime) && (recEndTime   > eventStartTime) && (startPercent > REC_PERC)) // START covered
              match = 3;
        else if ((recStartTime >  eventStartTime) && (recEndTime >= eventEndTime) && (recStartTime < eventEndTime)   && (endPercent   > REC_PERC)) // END covered
              match = 4;
        else if ((recStartTime >  eventStartTime) && (recEndTime <  eventEndTime)) // Partially covered in MIDDLE
              match = 5;

       
       switch (where)
       {
              case 0:  // Print the recording indicators in the Event Information window, so use round circles.
                     switch (match)
                     {
                           case 0: break;
                           case 1: break;	 // WATCHING event - we don't check these for recordings.
                           case 2: TAP_Osd_PutGd( windowRgn, *xCoord, yCoord, &_redcircleGd, TRUE );	 // FULL recording event - draw the red dot
                                   break;
                           case 3: TAP_Osd_PutGd( windowRgn, *xCoord, yCoord, &_yellowcircleGd, TRUE ); // PARTIAL START recording event - draw the yellow dot
                                   break;
                           case 4: TAP_Osd_PutGd( windowRgn, *xCoord, yCoord, &_orangecircleGd, TRUE ); // PARTIAL END recording event - draw the orange dot
                                   break;
                           case 5: TAP_Osd_PutGd( windowRgn, *xCoord, yCoord, &_pinkcircleGd, TRUE );	 // PARTIAL MIDDLE recording event - draw the pink dot
                                   break;
                     }
                     if (match > 0 ) 
                     {
                         sprintf(gstr,"%d",recInfo.tuner+1);
                         if ( match==2 ) TAP_Osd_PutStringAf1622( windowRgn, *xCoord + 11, yCoord+8, 700, gstr, MAIN_TEXT_COLOUR, 0 );  // Red icon so print text in white.
                         else TAP_Osd_PutStringAf1622( windowRgn, *xCoord + 11, yCoord+8, 700, gstr, COLOR_Black, 0 );  // Else print in black.
                         *xCoord = *xCoord + 30;  // Move the x-coordinate past the printed dot.
                     }
                     break;
       
              case 1: // Print the recording indicators in the Guide window, so use bars. (One on the left, another on the right)
                     switch (match)
                     {
                            case 0: break; 
                            case 1: break;	 // WATCHING event - we don't check these for recordings.
                            case 2: TAP_Osd_PutGd( windowRgn, *xCoord, yCoord, &_solidredbar5x39Gd, TRUE );	 // FULL recording event - draw the red dot
                                    TAP_Osd_PutGd( windowRgn, 0,       yCoord, &_solidredbar5x39Gd, TRUE );	 // FULL recording event - draw the red dot
                                    break;
                            case 3: TAP_Osd_PutGd( windowRgn, *xCoord, yCoord, &_solidyellowbar5x39Gd, TRUE ); // PARTIAL START recording event - draw the yellow dot
                                    TAP_Osd_PutGd( windowRgn, 0,       yCoord, &_solidyellowbar5x39Gd, TRUE ); // PARTIAL START recording event - draw the yellow dot
                                    break;
                            case 4: TAP_Osd_PutGd( windowRgn, *xCoord, yCoord, &_solidorangebar5x39Gd, TRUE ); // PARTIAL END recording event - draw the orange dot
                                    TAP_Osd_PutGd( windowRgn, 0,       yCoord, &_solidorangebar5x39Gd, TRUE ); // PARTIAL END recording event - draw the orange dot
                                    break;
                            case 5: TAP_Osd_PutGd( windowRgn, *xCoord, yCoord, &_solidpinkbar5x39Gd, TRUE );	 // PARTIAL MIDDLE recording event - draw the pink dot
                                    TAP_Osd_PutGd( windowRgn, 0,       yCoord, &_solidpinkbar5x39Gd, TRUE );	 // PARTIAL MIDDLE recording event - draw the pink dot
                                    break;
                     }
                     break;
       }
                   
    }
    
}


/******************************************************************************
Indicates all the timers that fall within the bounds of the displayed event.
******************************************************************************/
void IndicateTimers(word windowRgn, int svcNum, int svcType, dword *xCoord, int yCoord, TYPE_TapEvent event, int where)
{
	int i, timerCount;
	int tempXCoord;
	
    tempXCoord = *xCoord; // Store the current xCoord value.

    // First check is either of the tuners is recording this service.
	IndicateRecording(0, svcNum, svcType, windowRgn, xCoord, yCoord, event, where); 
	IndicateRecording(1, svcNum, svcType, windowRgn, xCoord, yCoord, event, where); 

    // If we're not recording this service, see if there is a timer for this service.
    if (*xCoord == tempXCoord)  // We didn't move so there wasn't a recording indicator ... therefore check for timers as well.
	{
                timerCount = TAP_Timer_GetTotalNum();

	            for(i = 0; i < timerCount; i++)
	            {
		              IndicateTimer(i, svcNum, svcType, windowRgn, xCoord, yCoord, event, where); 
                }
    }

}

// ***************************************************************************************************************************


//******************************************************************************
// Creates a NEW timer.
//******************************************************************************
void CreateNewTimer( TYPE_TimerInfo newTimer)
{
    int result;
  
	result = TAP_Timer_Add(&newTimer);
	switch( result )
	{
		case  0 : break;
		case  1 : ShowMessageWin(rgn, "Surfer Timer Error", "Timer Creation Failure:", "Invalid Entry or No resource available.", 550); break;
		case  2 : ShowMessageWin(rgn, "Surfer Timer Error", "Timer Creation Failure:", "Invalid tuner number.", 550); break;
		default : TAP_SPrint(gstr, "Conflict with timer number %d.", (result&0xffff)+1 );
                  ShowMessageWin(rgn, "Surfer Timer Error", "Timer Creation Failure:", gstr, 550); break;
	}
}



//******************************************************************************
// Replaces OLD timer with NEW timer.
//******************************************************************************
void ReplaceTimer( TYPE_TimerInfo newTimer, int oldTimerNumber)
{
    int result;
    
    if (!(TAP_Timer_Delete( oldTimerNumber )))    // Delete the old timer.
    {
        ShowMessageWin(rgn, "Surfer Timer Error", "Timer Deletion Failure:", "Couldn't delete old timer to replace with new timer.", 500);
        return;  
    }
    
	CreateNewTimer(newTimer);
}


//******************************************************************************
// Modify OLD timer.
//******************************************************************************
void ModifyTimer( TYPE_TimerInfo oldTimer, int oldTimerNumber)
{
    int result;
    result = TAP_Timer_Modify( oldTimerNumber, &oldTimer );    // Modify the old timer.
   	switch( result )
	{
		case  0 : break;
		case  1 : ShowMessageWin(rgn, "Surfer Timer Error", "Timer Modification Failure:", "Invalid Entry or No resource available.", 550); break;
		case  2 : ShowMessageWin(rgn, "Surfer Timer Error", "Timer Modification Failure:", "Invalid tuner number.", 550); break;
		default : TAP_SPrint(gstr, "Conflict with timer number %d.", (result&0xffff)+1 );
                  ShowMessageWin(rgn, "Surfer Timer Error", "Timer Modification Failure:", gstr, 550); break;
	}
    
}


//******************************************************************************
// Combines file names of 2 timers.
//******************************************************************************
void CombineFilenames( char	*newFileName, char	*oldFileName)
{
    int result;
    char tempFileName[100];
    
    
    
}


     
//******************************************************************************
// Combines OLD timer and NEW timer to create a NEW timer.
//
//  Assumes oldStart<newStart AND oldEnd>newStart AND oldEnd<newEnd 
//       OR newStart<oldStart AND newEnd>oldStart AND newEnd<oldEnd
//   ie. the timers OVERLAP.
//  |----OLD---------|          OR             |------OLD-------|
//          |-----NEW-----|              |------NEW-----|
//******************************************************************************
void CombineTimer( TYPE_TimerInfo newTimer, TYPE_TimerInfo oldTimer, int oldTimerNumber)
{
    int result;
    long oldStart, oldEnd, newStart, newEnd;
    
    if (!(TAP_Timer_Delete( oldTimerNumber )))    // Delete the old timer.
    {
        ShowMessageWin(rgn, "Surfer Timer Error", "Timer Deletion Failure:", "Couldn't delete old timer to combine with new timer.", 500);
        return;  
    }
    
    // Calculate start and end times in seconds since 1 Jan 2000.  
    oldStart = NumericStartDateTime( oldTimer.startTime );
    oldEnd   = NumericStartDateTime( oldTimer.startTime ) + (oldTimer.duration * 60); 
    newStart = NumericStartDateTime( newTimer.startTime );
    newEnd   = NumericStartDateTime( newTimer.startTime ) + (newTimer.duration * 60);
    
    if ( oldStart < newStart )
    { 
         newTimer.startTime = oldTimer.startTime;        // Make the new timer start the same as the OLD timer.
         newTimer.duration  = (newEnd - oldStart) / 60;  // Duration (in minutes) is from the start of the OLD timer to the end of the NEW timer.
         CombineFilenames( oldTimer.fileName, newTimer.fileName );   // Make the new filename the OLD + NEW
    }
    else
    { 
         newTimer.startTime = newTimer.startTime;        // Make the new timer start the same as the NEW timer.
         newTimer.duration  = (oldEnd - newStart) / 60;  // Duration (in minutes) is from the start of the NEW timer to the end of the OLD timer.
         CombineFilenames( newTimer.fileName, oldTimer.fileName );   // Make the new filename the NEW + OLD
    }
    
    
	CreateNewTimer(newTimer);
}


//******************************************************************************
// Splits OLD timer and NEW timers to create 2 NEW timers.
//
//   ie. the timers OVERLAP with each other and the EVENT.
//  (1) |----OLD-------|              (2)      |------OLD-------|
//           |-----NEW-----|  OR        |------NEW-----|
//             |--EVENT---|               |---EVENT--|
//
//                           OR 
//  (3) |--OLD-|                    (4)            |------OLD-------|
//            |------NEW-----|  OR  |------NEW-------|
//               |--EVENT---|         |---EVENT--|
//******************************************************************************
void SplitTimers( TYPE_TimerInfo newTimer, TYPE_TimerInfo oldTimer, int oldTimerNumber, TYPE_TapEvent event)
{
    int result, durMin, durHour;
    word startMjd; byte startHour, startMin, startSec;
    long oldStart, oldEnd, newStart, newEnd, eventStart, eventEnd;
  
    // Calculate start and end times in seconds since 1 Jan 2000.  
    oldStart   = NumericStartDateTime( oldTimer.startTime );
    oldEnd     = NumericStartDateTime( oldTimer.startTime ) + (oldTimer.duration * 60); 
    newStart   = NumericStartDateTime( newTimer.startTime );
    newEnd     = NumericStartDateTime( newTimer.startTime ) + (newTimer.duration * 60);   
    eventStart = NumericStartDateTime( event.startTime );
    eventEnd   = NumericStartDateTime( event.endTime );
    
    if ((newStart >  oldStart) && (newEnd >= oldEnd))   // Scenario #1 or #3
    {
         if (oldEnd < eventStart)  // Scenario #3
         {
  //          newTimer.startTime = oldTimer.endTime; // ***** NEED TO CALCULATE THE endTime.
  /*
  	startMjd  = ((oldTimer.startTime>>16)&0xffff);
    startHour = ((oldTimer.startTime>>8)&0xff);
	startMin  =  (oldTimer.startTime&0xff); 
	
	if (startMin >= optionStartOffset)  startMin = startMin - optionStartOffset;
	else
	{
        startMin = 60 + startMin - optionStartOffset;
        if (startHour > 0)  // If we've gone back an hour, but not across midnight.
        {
           startHour--;
        }
        else          // We've gone back over midnight.
        {
           startHour = 23;  // Offset will have a max of 60 minutes, so we'll never go back past 11pm.
           startMjd--;
        }
    }
    // Calculate the normal duration (in minutes) of a future event. Event.duration((Hour << 8) | Min)
    durMin = ((currentEvent.duration>>8) & 0xff) * 60 + (currentEvent.duration & 0xff) + optionStartOffset + optionEndOffset;
 */
 
            newTimer.duration  = (newEnd - oldEnd) / 60;
         }
         else  // Scenario #1
         {
            oldTimer.duration  = (eventStart - oldStart) / 60;
            // MODIFY oldTimer
            newTimer.startTime = event.startTime;
            newTimer.duration  = (newStart - eventStart) / 60;
          }
    }
    else
    if ((newStart <= oldStart) && (newEnd >  oldEnd))   // Scenario #2 or #4
    {
         if (oldStart > eventEnd )   // Scenario #4                   
         {
            newTimer.duration = (oldStart - newStart) / 60;
         }
         else   // Scenario #2
         {
            newTimer.duration  = (eventEnd - newStart) / 60;
            oldTimer.startTime =  event.endTime;
            oldTimer.duration  = (oldEnd - eventEnd) / 60; 
            // MODIFY oldTimer
         }
    }


}

/*

//******************************************************************************
// Handles timer conflicts.
//******************************************************************************
void HandleTimerConflict( int svcNum, int svcType, TYPE_TimerInfo newTimer, int oldTimerNumber)
{
     TYPE_TimerInfo oldTimer;
     
     TAP_Timer_GetInfo(oldTimerNumber, &oldTimer);  // Get the details of the timer that we are conflicting with.

	 if (oldTimer.svcType != svcType || oldTimer.svcNum != svcNum )  // If the conflict is from a timer on a different service then
	 {                                                               // we have probably run out of timer slots, rather than a clash,
                                                                     // so we can't solve this apart from deleting old timer, but may still
                                                                     // clash with other timers.
	 }
	 
}

*/

//******************************************************************************
// Adds a timer for the currently selected event.
//******************************************************************************
void SetATimer(byte isRecord )
{
	int result;
    dword startTime; word startMjd; byte startHour, startMin, startSec;
    dword endTime;  dword endMjd; dword endHour, endMin;  byte endSec;
    dword durMin;
	TYPE_TimerInfo timer;

    if (currentIndex <  PiPEventIndex) 
    {
        ShowMessageWin(rgn, "Surfer Timer Error", "Timer Creation Failure:", "Can't create timer for a program that has already been shown.", 500);
        return;  // We can't create a timer for a previous event.
    }
    
    // Calculate the start date/time of the event to be recorded.
	startMjd  = ((currentEvent.startTime>>16)&0xffff);
    startHour = ((currentEvent.startTime>>8)&0xff);
	startMin  =  (currentEvent.startTime&0xff); 
	if (startMin >= optionStartOffset)  startMin = startMin - optionStartOffset;
	else
	{
        startMin = 60 + startMin - optionStartOffset;
        if (startHour > 0)  // If we've gone back an hour, but not across midnight.
        {
           startHour--;
        }
        else          // We've gone back over midnight.
        {
           startHour = 23;  // Offset will have a max of 60 minutes, so we'll never go back past 11pm.
           startMjd--;
        }
    }
    // Calculate the normal duration (in minutes) of a future event. Event.duration((Hour << 8) | Min)
    durMin = ((currentEvent.duration>>8) & 0xff) * 60 + (currentEvent.duration & 0xff) + optionStartOffset + optionEndOffset;
    
    if (currentIndex ==  PiPEventIndex) // We're trying to record the current event.
    {
         TAP_GetTime( &startMjd, &startHour, &startMin, &startSec ); // Get the current time and use that as the starting time.
         // Calculate how long until the end of the event and use that as the duration (plus offset)
         startTime = CurrentTime();  // Curent time in seconds since 01 Jan 2000
	     endMjd    = ((currentEvent.endTime>>16)&0xffff);
         endHour   = ((currentEvent.endTime>>8)&0xff);
	     endMin    =  (currentEvent.endTime&0xff);
         endTime   = NumericDateTime( endMjd, endHour, endMin, 0);  // End time in seconds since 01 Jan 2000
         durMin    = max(((endTime - startTime ) / 60)+1, 0);  // Number of minutes (with 1min round up) between now and end event time with bound check to ensure duration is >=0.
         durMin    = durMin + optionEndOffset;  // Set timer duration in minutes plus offset.
    }

	timer.isRec           = isRecord;
	timer.tuner           = 3;
	timer.svcType         = currentSvcType;
	timer.svcNum          = currentSvcNum;
	timer.reservationType = RESERVE_TYPE_Onetime;
	timer.nameFix         = 1;
    timer.duration        = durMin;
    timer.startTime       = (( startMjd & 0xffff ) << 16) + ( ( startHour & 0xff ) << 8 ) + ( startMin & 0xff );
	strncpy(timer.fileName, currentEvent.eventName,95);
	strcat(timer.fileName, ".rec");

	result = TAP_Timer_Add(&timer);

	switch( result )
	{
		case 0 : break;
		case 1 : ShowMessageWin(rgn, "Surfer Timer Error", "Timer Creation Failure:", "Invalid Entry or No resource available.", 550); break;
		case 2 : ShowMessageWin(rgn, "Surfer Timer Error", "Timer Creation Failure:", "Invalid tuner number.", 550); break;
		default :  TAP_SPrint(gstr, "Conflict with timer number %d.", (result&0xffff)+1 );
                   ShowMessageWin(rgn, "Surfer Timer Error", "Timer Creation Failure:", gstr, 550); break;
	}
	
}

