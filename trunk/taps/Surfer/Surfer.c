/************************************************************
	  			UK OZ Surfer
           
			Part of the UK TAP Project
		This module is the main event handler
     
Name	: UkChannelSurfer.c
Author	: Darkmatter
Version	: 0.11
For		: Topfield TF5x00 series PVRs
Licence	:
Descr.	:  
Usage	: 
History	: v0.0 Darkmatter: 02-08-05 	Inception date
		  v0.1 Darkmatter: 03-08-05		Added loading of configuration from file
		  v0.2 Darkmatter: 03-08-05		Completed full port of config load/save/modify
		  v0.3 Darkmatter: 03-08-05		TAP name changed to UkChannelSurfer
 									 	More display options - higlight bar & logo selection
		  								Added PIP window to display hightlighted channel
		  v0.4 Darkmatter: 09-08-05		Periodic refresh of event information
		  v0.5 Darmkatter: 11-08-05		Speeded things up by adding an immediate exit from main TAP event handler is key=0
		  v0.6 Kidhazy:    17-08-05     Added extended info handling, Now/Next/Later tags, option to hide progress info, 
                                        ability to exit with a double OK press, ues Vol -/+ to select prev/next event.  
                                        Display day of week before start/end time if not today.
          v0.7 Kidhazy:    23-08-05     Added ability to track last playback file and swap between channels and playback file.   
          v0.8 Kidhazy:    02-09-05     Added guide overview window.
          v0.9 Kidhazy:    06-09-05     Added coloured timer indicators. 
          v0.10 Kidhazy:   10-09-05     New functionality for channel name and channel entry.   
          v0.11 Kidhazy:   11-10-05     Screen rezising and repositioing capabilities. 
          v0.12 Kidhazy:   24-11-05     TF5800 Description Extender TAP support and progress bar options.                                                                         

	Last change:  USE  13 Aug 105   10:05 pm
**************************************************************/
          
#ifdef WIN32
#include "c:\TopfieldDisk\DataFiles\SDK\TAP_Emulations.h"
#undef RGB
//#define RGB(r,g,b)		   		 ( (0x8000) | ((r)<<10) | ((g)<<5) | (b) )
#define RGB(r,g,b) ((COLORREF)(((BYTE)(r<<3)|((WORD)((BYTE)(g<<3))<<8))|(((DWORD)(BYTE)(b<<3))<<16)))
#endif    
                   
#define TAP_NAME "Surfer"
#define VERSION "0.12a"       

#define DEBUG 0  // 0 = no debug info, 1 = debug written to logfile,  2 = debug written to screen.  
            
#include "tap.h"
#include "morekeys.h"

  
//#define ID_UK_Timers 		0x800440FE
//#define ID_UK_Makelogos	0x800440FD
#define ID_UK_Channels 		0x800440FC
//#define ID_UK_USB			0x800440FB
 
TAP_ID( ID_UK_Channels );

#if DEBUG == 1
   TAP_PROGRAM_NAME(TAP_NAME " v" VERSION " DEBUG ONLY" __TIME__);
#else
   TAP_PROGRAM_NAME(TAP_NAME " v" VERSION);
#endif

TAP_AUTHOR_NAME("Darkmatter");
TAP_DESCRIPTION("Surf the other channels.");
TAP_ETCINFO(__DATE__);
   
#include "TSRCommander.inc"

static bool ourWindowDisplayed;
static bool closeFlag;
static bool terminateFlag;
static bool redrawWindow;
static bool generateListKey;
static word rgn;														// one region used for all our graphics
static dword lastTickTime;
static bool inhibitOurCode;
static byte generateStage;
static bool MainChangeOnOK;
static bool InfoKeyPressed;
static bool guideWindowDisplayed;
 
static byte TvRatioOption;  // Moved from IniFile.c  
   
int _language;
                       
static int currentSvcType;
static int currentSvcNum;
static int firstSvcType;
static int firstSvcNum;
static int tempSvcType;
static int tempSvcNum;
 
static byte oldMin;
static char gstr[100];  //General Message string.
char* TAPIniDir;
                                 
                                          
#include "UkGraphics.c"
#include "EventInfoConfig.c"
#include "DisplayLogosConfig.c"
#include "RemoteKeys.inc"
#include "GlobalScreenSizes.c"     
#include "ScreenControls.c"
#include "Tools.c"     
#include "LogFile.c"     
#include "IniFile.c"
#include "DisplayLogos.c"
#include "ProgressBar.c"
#include "Clock.c"
#include "EventInfo.c"
#include "MainMenu.c"
#include "Picture.c"
#include "NowNextDisplay.c"
#include "Timers.c"
#include "ManualChannels.c"
                     
                                             
                           
                       
//----------------------------------------
// Open our window and display the content
//
void ActivationRoutine( void )
{
    appendToLogfile("ActivationRoutine: started.");

    ourWindowDisplayed = TRUE;
	MainChangeOnOK     = TRUE;                                              //KH Initialise so OK will change main channel.
    currentGuideIndex  = 0;
	ScreenOn           = TRUE;

	TAP_ExitNormal();
	rgn = TAP_Osd_Create( 0, 0, 720, 576, 0, FALSE );					// define the whole screen for us to draw on
	TAP_Osd_FillBox( rgn, 0, 0, 720, 576, 0 );							// clear the screen

    appendToLogfile("ActivationRoutine: calling ActivatePicture routine.");

    SwitchScreenToNormal();
    ActivatePicture();													// set size & aspect ratio for main window & PIP

    appendToLogfile("ActivationRoutine: calling TAP_Channel_GetCurrent.");
    TAP_Channel_GetCurrent( &firstSvcType, &firstSvcNum );

    appendToLogfile("ActivationRoutine: calling CheckPlaybackInfo routine.");
    CheckPlaybackInfo();
    appendIntToLogfile("ActivationRoutine: after CheckPlaybackInfo routine InPlaybackMode=%d.",InPlaybackMode);
                        
    if ( !InPlaybackMode ) // No playback happening.
    {
        appendIntToLogfile("ActivationRoutine: calling ChangeMainChannel routine for service=%d.",firstSvcNum);
    	ChangeMainChannel( firstSvcType, firstSvcNum );
    }	
    else
    { 
        appendToLogfile("ActivationRoutine: calling ShrinkPictureWindows routine.");
        // Don't need to activate any channel on the main picture, but shrink the current playing file.
        ShrinkPictureWindows();	
    }    
 
    if (StartChannelOption == 1) // Start with current channel highlighted.
    {
           currentSvcNum  = firstSvcNum;
           currentSvcType = firstSvcType;
           if (!InPlaybackMode)   MainChangeOnOK = FALSE;  // We've started with the current channel, so allow OK to simply exit again.
    } 
    appendIntToLogfile("ActivationRoutine: calling ChangePipChannel routine for currentSvcNum=%d.",currentSvcNum);
	ChangePipChannel( currentSvcType, currentSvcNum );					// Now, restore the same PIP channel that was displayed
    appendIntToLogfile("ActivationRoutine: calling ActivateLogos routine for currentSvcNum=%d.",currentSvcNum);
	ActivateLogos( currentSvcType, currentSvcNum );						// when we closed, and select it's logo, and current event.
    appendIntToLogfile("ActivationRoutine: calling DisplayEventInfo routine for currentSvcNum=%d.",currentSvcNum);
	DisplayEventInfo( currentSvcType, currentSvcNum );
    appendToLogfile("ActivationRoutine: after DisplayEventInfo routine was called.");

	switch (GuideStartOption)
	{
           case 0 : guideWindowDisplayed = FALSE; break;
           case 1 : guideWindowDisplayed = TRUE;  break;
           case 3 : break;  // Take the last setting.
           default: break;
    }
	if (guideWindowDisplayed) 
    { 
        appendToLogfile("ActivationRoutine: calling ActivateGuideWindow routine.");
        if (GuideWindowOption == 1) 
        {
             SwitchScreenToGuideWindow();  // Shrink Main Window if option is set.
             ShrinkPictureWindows();
        }
		ActivateGuideWindow();
        appendIntToLogfile("ActivationRoutine: calling LoadGuideEventInfo routine for currentSvcNum=%d.",currentSvcNum);
	    LoadGuideEventInfo( currentSvcType, currentSvcNum );
        appendIntToLogfile("ActivationRoutine: calling DisplayGuideWindow routine for currentSvcNum=%d.",currentSvcNum);
        DisplayGuideWindow( currentSvcType, currentSvcNum );
        appendToLogfile("ActivationRoutine: after DisplayGuideWindow routine was called.");
    }

    appendToLogfile("ActivationRoutine: ended.");

}

    

//----------------------------------------
// Close our window and restore the TV picture
//
void CloseRoutine( void )
{
    closeFlag = FALSE;
    ourWindowDisplayed = FALSE;	

    SwitchScreenToNormal();  // Reset the SCREEN_* settings to normal.
	ClosePicture();
	ClosePlaybackChannel();

    TAP_Osd_Delete( rgn );
    TAP_Osd_Delete( guideRgn );

	TAP_EnterNormal();
	
}



//----------------------------------------
// Terminate and unload the TAP
//
void TerminateRoutine( void )											// Performs the clean-up and terminate TAP
{
	CloseRoutine();
	TerminateConfigMenu();
	TerminateLogos();
	TerminateEvents();
	closeLogfile();   // Close logfile if we were in debug mode.
	TAP_Exit();															// exit
}



//----------------------------------------
// Redraw our window
//
void RefreshOurWindow( void )
{ 
	TAP_Osd_FillBox( rgn, 0, 0, 720, 576, 0 );							// clear the screen
	RedrawAllLogos( currentSvcType, currentSvcNum, TRUE );
    selectionChanged = TRUE;  // Flag it so the event information is redrawn.
    if (guideWindowDisplayed) DisplayGuideWindow( currentSvcType, currentSvcNum );
}
 
  

//----------------------------------------
// Handles moving up levels, by redrawing whatever window should be currently displayed
//
void CheckFlags( void )
{
	if ( closeFlag == TRUE ) { CloseRoutine(); return; }				// close window and enter normal state
	if ( terminateFlag == TRUE ) { TerminateRoutine(); return; }		// clean-up and terminate TAP
	if ( redrawWindow == TRUE )											// Handle navigation up level
	{																	// redraw the underlying window
	    redrawWindow = FALSE;
		if ( menuShowing ) { RedrawMainMenu(); return; }				// Menu
		if ( ourWindowDisplayed ) { RefreshOurWindow(); return; }		// Channel display
	}
}


     

 
//----------------------------------------
// Handles Keys at this level - [top level]
//               
dword ListKeyHandler( dword key )
{
 
	switch ( key )
	{
		case RKEY_Menu :	ActivateMenu();
							break;

		case RKEY_TvRadio :	SwapTvRadio( &currentSvcType, &currentSvcNum );
							ChangePipChannel( currentSvcType, currentSvcNum );
							DisplayEventInfo( currentSvcType, currentSvcNum );
							MainChangeOnOK = TRUE;
	                        currentGuideIndex = 0;
							if (guideWindowDisplayed) 
                            {
						       LoadGuideEventInfo( currentSvcType, currentSvcNum );
                               DisplayGuideWindow( currentSvcType, currentSvcNum );
   	                           if (guideFormat == 1) DisplayTimeframeEventTitle( currentSvcType, currentSvcNum, displayStartTime );
                            }
							break;
         
		case RKEY_ChUp :	SelectPreviousLogo( &currentSvcType, &currentSvcNum );
							ChangePipChannel( currentSvcType, currentSvcNum );
	                        CheckGuideEventIndex();
							if (guideFormat == 0) DisplayEventInfo( currentSvcType, currentSvcNum );
							MainChangeOnOK = TRUE;
                            //currentGuideIndex = 0;
							if (guideWindowDisplayed) 
                            {
						       LoadGuideEventInfo( currentSvcType, currentSvcNum );
                               DisplayGuideWindow( currentSvcType, currentSvcNum );
   	                           if (guideFormat == 1) DisplayTimeframeEventTitle(currentSvcType, currentSvcNum, displayStartTime );
                            }
							break;
        
		case RKEY_ChDown :	SelectNextLogo( &currentSvcType, &currentSvcNum );
							ChangePipChannel( currentSvcType, currentSvcNum );			// change the PIP window
	                        CheckGuideEventIndex();
                            if (guideFormat == 0) DisplayEventInfo( currentSvcType, currentSvcNum );
							MainChangeOnOK = TRUE;
                            //currentGuideIndex = 0;
							if (guideWindowDisplayed) 
                            {
						       LoadGuideEventInfo( currentSvcType, currentSvcNum );
                               DisplayGuideWindow( currentSvcType, currentSvcNum );
   	                           if (guideFormat == 1) DisplayTimeframeEventTitle( currentSvcType, currentSvcNum, displayStartTime );
                            }
							break;
     
		case RKEY_Green :	SelectPreviousLogo( &currentSvcType, &currentSvcNum );
							DisplayEventInfo( currentSvcType, currentSvcNum );
							ChangeMainChannel( currentSvcType, currentSvcNum );
							MainChangeOnOK = TRUE;
	                        currentGuideIndex = 0;
							if (guideWindowDisplayed) 
                            {
						       LoadGuideEventInfo( currentSvcType, currentSvcNum );
                               DisplayGuideWindow( currentSvcType, currentSvcNum );
   	                           if (guideFormat == 1) DisplayTimeframeEventTitle( currentSvcType, currentSvcNum, displayStartTime );
                            }
	 						break;
 
		case RKEY_Yellow :	SelectNextLogo( &currentSvcType, &currentSvcNum );
							DisplayEventInfo( currentSvcType, currentSvcNum );
							ChangeMainChannel( currentSvcType, currentSvcNum );
							MainChangeOnOK = TRUE;
	                        currentGuideIndex = 0;
							if (guideWindowDisplayed) 
                            {
						       LoadGuideEventInfo( currentSvcType, currentSvcNum );
                               DisplayGuideWindow( currentSvcType, currentSvcNum );
   	                           if (guideFormat == 1) DisplayTimeframeEventTitle( currentSvcType, currentSvcNum, displayStartTime );
                            }
							break;
		case RKEY_Forward :	
                            SelectNextLogoPage( &currentSvcType, &currentSvcNum );
							ChangePipChannel( currentSvcType, currentSvcNum );			// change the PIP window
	                        CheckGuideEventIndex();
							DisplayEventInfo( currentSvcType, currentSvcNum );
							MainChangeOnOK = TRUE;
                            //currentGuideIndex = 0;
							if (guideWindowDisplayed) 
                            {
						       LoadGuideEventInfo( currentSvcType, currentSvcNum );
                               DisplayGuideWindow( currentSvcType, currentSvcNum );
   	                           if (guideFormat == 1) DisplayTimeframeEventTitle( currentSvcType, currentSvcNum, displayStartTime );
                            }
							break;
        
		case RKEY_Rewind :
	                        SelectPreviousLogoPage( &currentSvcType, &currentSvcNum );
							ChangePipChannel( currentSvcType, currentSvcNum );			// change the PIP window
	                        CheckGuideEventIndex();
							DisplayEventInfo( currentSvcType, currentSvcNum );
							MainChangeOnOK = TRUE;
                            //currentGuideIndex = 0;
							if (guideWindowDisplayed) 
                            {
						       LoadGuideEventInfo( currentSvcType, currentSvcNum );
                               DisplayGuideWindow( currentSvcType, currentSvcNum );
   	                           if (guideFormat == 1) DisplayTimeframeEventTitle( currentSvcType, currentSvcNum, displayStartTime );
                            }
							break;

		case RKEY_VolUp :	NextGuideEvent( currentSvcType, currentSvcNum );
							if (!Playback_Selected) 
                            {
                                DisplayNextEvent();
                                displayStartTime = currentEvent.startTime;  // Set the Display Time frame to start from this event.
                            }
                            else
                                 SetCurrentDateTime( &displayStartTime );  // Reset display Start Time to current time.
							if (guideWindowDisplayed) DisplayGuideWindow( currentSvcType, currentSvcNum  );
                            break;

		case RKEY_VolDown :	PreviousGuideEvent( currentSvcType, currentSvcNum );
							if (!Playback_Selected) 
                            {
                                DisplayPreviousEvent();
                                displayStartTime = currentEvent.startTime;  // Set the Display Time frame to start from this event.
                            }
                            else
                                 SetCurrentDateTime( &displayStartTime );  // Reset display Start Time to current time.
							if (guideWindowDisplayed) DisplayGuideWindow( currentSvcType, currentSvcNum  );
                            break;
           
		case RKEY_Prev :	
                            if ((guideWindowDisplayed))
                            {
                               CalculatePreviousTime(&displayStartTime,stepMinutes);
                               guideFormat = 1;
                               DisplayGuideWindow( currentSvcType, currentSvcNum );
							   if (!Playback_Selected) DisplayTimeframeEventTitle( currentSvcType, currentSvcNum, displayStartTime );
                            }
                            break;
                               
		case RKEY_Next :	    
                            if ((guideWindowDisplayed) && (currentIndex < eventCount-1))
                            {
                               CalculateNextTime(&displayStartTime,stepMinutes);
                               guideFormat = 1;
                               DisplayGuideWindow( currentSvcType, currentSvcNum );
							   if (!Playback_Selected) DisplayTimeframeEventTitle( currentSvcType, currentSvcNum, displayStartTime );
                            }
                            break;
                             
        case RKEY_Pause:
                            // stepMinutes: 15 30 60
                            if (stepMinutes < 60) stepMinutes = stepMinutes * 2; // Double interval each press
                            else stepMinutes = 15;  // Wrap back to 15 minutes.
                            break;   
							
 						
 		case RKEY_Sat:  				
		case RKEY_Recall:	if (RecallKeyOption == 0 )   // Option to change to first channel
                            {
                                currentSvcType = firstSvcType;							// Load whatever channel was set when we first
							    currentSvcNum = firstSvcNum;							// open our window
                            }
                            else
                            {
                                if (!InPlaybackMode) 
                                {     
                                      TAP_Channel_GetCurrent( &tempSvcType, &tempSvcNum );   // Save what's on Main Window now.
						              ChangeMainChannel( currentSvcType, currentSvcNum );    // Switch the PiP to the Main Window.
                                      currentSvcType = tempSvcType;                          // Set the PiP to the previous Main Window channel.
                                      currentSvcNum  = tempSvcNum;
                                      Playback_Selected = FALSE;                              // Flag the Playback channel is NOT selected.    
                                }
                                else
                                {     
						              ChangeMainChannel( currentSvcType, currentSvcNum );    // Switch the PiP to the Main Window.
                                      currentSvcType = SVC_TYPE_Tv;                          // Set the PiP to the Playback Channel.
                                      currentSvcNum  = Playback_svcnum;                      // Set the PiP to the Playback Channel.
                                      Playback_Selected = TRUE;                              // Flag the Playback channel is selected.    
                                }
                            }   
                            ChangePipChannel( currentSvcType, currentSvcNum );			// change the PIP window
	                        currentGuideIndex = 0;
                            if (guideWindowDisplayed)  // Redraw Guide Window in case new PiP channel was on another page.
                            {
						        LoadGuideEventInfo( currentSvcType, currentSvcNum );
                                DisplayGuideWindow( currentSvcType, currentSvcNum );
                                if (guideFormat == 1) DisplayTimeframeEventTitle( currentSvcType, currentSvcNum, displayStartTime );
                            }
							RedrawAllLogos( currentSvcType, currentSvcNum, FALSE );
							DisplayEventInfo( currentSvcType, currentSvcNum );
							MainChangeOnOK = TRUE;
							break;
     

		case RKEY_Ok :	
                    if (manualChannelEntered) 
                    {
                         ChangeToManualChannel( enteredChannelNumber );
		                 MainChangeOnOK = TRUE;
                    }
                    else 
                    {	
					  if (MainChangeOnOK)
					  {
						 MainChangeOnOK = FALSE;
						 ChangeMainChannel( currentSvcType, currentSvcNum );
					  }
					  else
					  {
						  CloseRoutine();
					  }
                    }
				    break;
				 			

		case RKEY_Info :	InfoKeyPressed = TRUE;
     						ShowDelayedEvent();  
	       					break;

      

        case RKEY_Guide:    
                    
                    if (!guideWindowDisplayed)
					{
						if (GuideWindowOption == 1) 
                        { 
                            SwitchScreenToGuideWindow();  // Shrink Main Window if option is set.
                            ShrinkPictureWindows();
                        }
                        ActivateGuideWindow();
						LoadGuideEventInfo(currentSvcType, currentSvcNum);
						DisplayGuideWindow(currentSvcType, currentSvcNum);
					}
					else
					{
						if (GuideWindowOption == 1) 
                        { 
                            SwitchScreenToNormal();  // Reset Main Window if we previously shrunk it.
                            ShrinkPictureWindows();
                        }
                        CloseGuideWindow();
						guideWindowDisplayed = FALSE;
					}
                            break;							
  
		case RKEY_Mute :  return key;
		
		case RKEY_Record :	// Create record timer
                    if (!noEvents)  // Only create timers when we have event information.
                    {
                         SetATimer(RECTYPE_Normal);
                         selectionChanged = TRUE;   // Refresh the screen to show the timer indicators.
                    }
                    else
                         ShowMessageWin(rgn, "Surfer Error", "Timer Creation Failure:", "Can't create a timer with no event information.", 500);
			        break;
 
		case RKEY_Blue :	if (screenFadeOption>0) HideScreen();  // Only allow manual screen hiding if not "Off" option.
							break;
		   	
        case RKEY_0:  
                      if (!manualChannelEntered)   // If the 0 was pressed, and it is not part of the Manual Channel number entry.
                      {                            // Reset all of the event pointers to point to the NOW event.
                          currentGuideIndex = 0;   // Reset the Guide window to point to the current event.
                          guideFormat = 1;         // Set back to the Now/Next Guide display format.
                          SetCurrentDateTime( &displayStartTime );  // Set display Start Time to current time.
                          selectionChanged = TRUE;   // Refresh the screen to updated events.
                          if (guideWindowDisplayed)  // Redraw Guide Window in case new PiP channel was on another page.
                          {
                                DisplayGuideWindow( currentSvcType, currentSvcNum );
                                if (guideFormat == 1) DisplayTimeframeEventTitle( currentSvcType, currentSvcNum, displayStartTime );
                          }
   						  DisplayEventInfo( currentSvcType, currentSvcNum );
   						  ShowDelayedEvent();
                          break;                     
                      }
                      else  // Otherwise do the SelectManualChannel section.
                      {
                          SelectManualChannel( key );
                          break;
                      }
                      
		case RKEY_1:
        case RKEY_2: 
		case RKEY_3:
        case RKEY_4:
		case RKEY_5:
        case RKEY_6: 
		case RKEY_7:
        case RKEY_8:
		case RKEY_9:
                      SelectManualChannel( key );
                      break;

		case RKEY_Exit :	CloseRoutine();
							break;
/*
// test code
		case RKEY_Red :	
                       // InitialiseOverScanSizes();
                        InitialiseScreenConfigWindow(7);

							break;
//[end] test code
*/    

		default :			break;
	}
	return 0;
}
             
        

//--------------------------
// can only do cetain things from the idle state
//
void My_IdleHandler( void )
{  
    dword currentTickTime;
	byte 	hour, min, sec;
	int     oldCurrentIndex;
	word 	mjd;	

#ifdef WIN32
#else
	CheckPlaybackInfo();  // Check if we're in playback mode and update the playback information.
#endif
	
	if ( !ourWindowDisplayed ) return;									// only interested if we are doing something

	TAP_GetTime( &mjd, &hour, &min, &sec);   // Update the onscreen clock each minute.
	if (( min != oldMin )  && ( !configWindowShowing )  && ( !menuShowing ) && ( !screenConfigShowing ))   //Update the clock each minute, on the main display only.
	{
	    oldMin = min;
		UpdateClock( rgn, CLOCK_X, CLOCK_Y);
		if (InPlaybackMode) SaveCurrentPlaybackPosition(); // If a playback is running, update the saved position info to display the new progress.
        oldCurrentIndex = currentIndex;                    // Temporarily save the current event index.      
        DisplayEventInfo(currentSvcType, currentSvcNum);   // Redisplay the events and check if the current event has changed.
        ShowDelayedEvent();                                // Redisplay the full event info to update progress information.
        if (guideWindowDisplayed) 
        {
            if (currentIndex != oldCurrentIndex) LoadGuideEventInfo( currentSvcType, currentSvcNum );  // Current event changed, so reload.
            DisplayGuideWindow( currentSvcType, currentSvcNum );		
        }
	}
  
	currentTickTime = TAP_GetTick();
	
	if ((manualChannelEntered) && (currentTickTime - manualChannelTick > manualChannelWait))  // If a manual channel number was entered
    {
          ChangeToManualChannel( enteredChannelNumber );
          MainChangeOnOK = TRUE;
    }
    
    if ((ScreenOn) && (currentTickTime - screenFadeTick > screenFadeOption*100) && (screenFadeOption>=2) && (!configWindowShowing)  && (!menuShowing) && (!screenConfigShowing))  HideScreen();  // Fade the screen out if we're in automatic fade mode, and the time has passed.
	  
	DelayedUpdate( currentTickTime );
	EventIdleRoutine( currentTickTime );
	     
 
}
  
      
//----------------------------------------
// General key router - passes on keys to the appropriate level
//
dword My_KeyHandler(dword key, dword param2)
{
    dword currentTickTime;
	dword state, subState ;
 
    if ( inhibitOurCode ) return key;									// check we are allowed to process the key

    screenFadeTick = TAP_GetTick();                                     // A key was pressed, so reset the wait timer for screen fade.

	if ( screenConfigShowing ) { ScreenConfigKeyHandler( key );   return 0; }
	if ( configWindowShowing ) { ConfigKeyHandler( key );         return 0; }
	if ( menuShowing )         { MainMenuKeyHandler( key );	      return 0; }

    if ((!ScreenOn) && (screenFadeOption >=2) && (key != RKEY_Blue) && (key != RKEY_Exit) && (key != RKEY_Recall) && (key != RKEY_Menu) && (!configWindowShowing) && (!menuShowing) )  HideScreen();           // Turns the screen back on if it is off and we're in automatic mode.
	currentTickTime = TAP_GetTick();									// Starting a new channel introduces a delay, so hold off until
    DelayPipChange( currentTickTime );													// user had stopped pressing buttons.
	DelayEventRefresh( currentTickTime );
	  
	if ( ourWindowDisplayed ) { return ( ListKeyHandler( key ) ); }

    TAP_GetState(&state, &subState);
	if ((state != STATE_Normal) || (subState != SUBSTATE_Normal)) return key;	// otherwise just return if another menu is shown

	if (TAP_Channel_IsStarted( 0 )) return key;							// check for PIP
 
    if ( OsdActive(50, 10, 200, 40) || OsdActive(50, 150, 200, 300)) return key;  //Check if there's anything on the screen between 50,10 and 200,40 or 50,150 and 200,300.
                                                                                 //Catches the manual channel entry numbers, bug ignores the mute/volume graphics.
    if ( (mainActivationKey == RKEY_Ok) && (OsdActive(150, 390, 400, 510)) ) return key;  //If OK is the activation key and there is something on the bottom middle of the screen (eg. Info box or Improbox) then don't activate.

	if ( key == mainActivationKey )										// only enter our code from the normal state
	{
		ActivationRoutine();
		return 0;
	} 

	return  key;														// default
}
  
     

//----------------------------------------
// Standard event handler
//
dword TAP_EventHandler( word event, dword param1, dword param2 )
{
	dword returnKey;
	returnKey = param1;

	switch(event)
	{
		case EVT_IDLE :	My_IdleHandler();
		                TSRCommanderWork();
						break;

		case EVT_KEY :	if ( param1 == 0 ) break;						// means another TAP as exited the normal state
						returnKey = My_KeyHandler(param1, param2);
		 				CheckFlags();
						break;
 
		default : 		break;
	}
	return (returnKey);
}
          
void TSRCommanderConfigDialog()
{
    ourWindowDisplayed = FALSE;
	menuShowing = FALSE;
	configWindowShowing = TRUE;

	TAP_ExitNormal();
    CalledByTSRCommander=TRUE;
    DisplayConfigWindow();
	return;
}
  
bool TSRCommanderExitTAP (void)
{	

    closeFlag = FALSE;
    ourWindowDisplayed = FALSE;	
	menuShowing = FALSE;

    TAP_Osd_Delete( rgn );
    TAP_Osd_Delete( guideRgn );

	TAP_EnterNormal();
	return TRUE;
}
     
                          
   
//----------------------------------------
// Startup routine. Called only once - when the TAP first loads
//    
int TAP_Main (void)
{
     char *cDir;
     char oldFile[128], newFile[128];
     word retcode;


    openLogfile();                   // Opens the logfile in the current directory, if we are in debug mode.
    
    appendToLogfile("Surfer TAP Started.");
    TSRCommanderInit( 0, TRUE );    // Include the TSR Commander function.  

	TAP_Hdd_ChangeDir(PROJECT_DIRECTORY);  // Change to the UK TAP Project SubDirectory.
    TAPIniDir = GetCurrentDir();           // Store the directory location of the INI file.	
    
    // Surfer V0.11a changed the name of the ini file from UKChannels.ini to Surfer.ini, so rename any old INI files if
    // a new one doesn't already exist.
    if ((TAP_Hdd_Exist( OLD_OPTIONS_FILENAME )) && (!TAP_Hdd_Exist( OPTIONS_FILENAME )) ) 
    { 
#ifdef WIN32  // If testing on WIN32 platform don't do rename as not yet implemented in the TAP SDK.
#else
       TAP_Hdd_Rename(OLD_OPTIONS_FILENAME, OPTIONS_FILENAME);
#endif          
    }

	LoadConfiguration();

   
	InitialiseScreenSizes();               // Ensure that screen position and size variables have been set.	
	SaveNormalScreenSettings();            // Save the Normal screen settings.
    
	_language = TAP_GetSystemVar(SYSVAR_OsdLan);                        //KH Grab the system language setting.
   
	InitialiseLogos();
	initialiseMenu();
	InitialiseConfigRoutines();
	InitialiseEvents();
	InitialisePicture();
	InitialisePlaybackChannel();
	InitialiseGuideWindow();

    ourWindowDisplayed   = FALSE;
	redrawWindow         = FALSE;
	closeFlag            = FALSE;
	terminateFlag        = FALSE;
	generateListKey      = FALSE;
	guideWindowDisplayed = FALSE;
	ScreenOn             = TRUE;
	screenConfigShowing  = FALSE; 

	inhibitOurCode = FALSE; 
	lastTickTime   = 0;
	generateStage  = 0;   

	MainChangeOnOK = TRUE; 
	InfoKeyPressed = FALSE;

    return 1;
}     
     
#ifdef WIN32
int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
   return InitializeTAP_Environment(hInstance, nCmdShow, __tap_program_name__, TAP_Main, TAP_EventHandler);
} 
#endif
      
