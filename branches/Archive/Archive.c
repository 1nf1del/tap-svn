/************************************************************
				OZ Archive
	Archive Recordings display, and management TAP
              
		    
	This module is the main event handler
 
Name	: OZ Archive.c
Author	: kidhazy
Version	: 0.03
For		: Topfield TF5x00 series PVRs
Licence	:
Descr.	:
Usage	:
History	: v0.01 kidhazy 17-10-05   Inception date.
          v0.02 kidhazy    11-05   TF5800 support
          v0.03 kidhazy    11-05 
          v0.04 kidhazy    11-05
          test

	Last change:  USE   3 Aug 105    0:02 am
**************************************************************/
#ifdef WIN32
#include "c:\TopfieldDisk\DataFiles\SDK\TAP_Emulations.h"
#undef RGB
//#define RGB(r,g,b)		   		 ( (0x8000) | ((r)<<10) | ((g)<<5) | (b) )
#define RGB(r,g,b) ((COLORREF)(((BYTE)(r<<3)|((WORD)((BYTE)(g<<3))<<8))|(((DWORD)(BYTE)(b<<3))<<16)))
#endif           
                    

#define DEBUG 0  // 0 = no debug info, 1 = debug written to logfile,  2 = debug written to screen.
    
#define TAP_NAME "Archive"
#define VERSION "0.05a"       

#include <tap.h>
#include "morekeys.h"
     
//#define ID_UK_Timers 		0x800440FE
//#define ID_UK_Makelogos	0x800440FD
//#define ID_UK_Channels 		0x800440FC
//#define ID_UK_USB			0x800440FB
#define ID_OZ_Archive			0x800440FA
             
TAP_ID( ID_OZ_Archive );
                         
#if DEBUG != 0
   TAP_PROGRAM_NAME(TAP_NAME " v" VERSION " DEBUG ONLY" __TIME__);
#else
   TAP_PROGRAM_NAME(TAP_NAME " v" VERSION);
#endif
               
TAP_AUTHOR_NAME("kidhazy");
TAP_DESCRIPTION("View and monitor recordings.");
TAP_ETCINFO(__DATE__);

char* TAPIniDir;
                        
#include "TSRCommander.inc"
                                                
#include "Common.c"													// Global prototypes, graphics, and global variables
#include "LogFile.c"
#include "ProgressBar.c"
#include "TextTools.c"
#include "YesNoBox.c"
#include "Tools.c"
#include "LoadArchiveInfo.c"
#include "PlaybackDatFile.c"
#include "logo.C"
#include "TimeBar.c"
#include "ArchiveDisplay.c"
#include "ArchiveDelete.c"
#include "ArchiveStop.c"
#include "ArchiveAction.c"
#include "ArchiveRename.c"
#include "ArchiveInfo.c" 
#include "MainMenu.c"
#include "ConfigMenu.c"
#include "IniFile.c"
                     
                   
static dword lastTick;
static byte oldMin;
static byte oldSec;
                               
                                                                   
              
//------------
//
void ActivationRoutine( void )
{
    appendToLogfile("ActivationRoutine: Started.");
  	TAP_ExitNormal();
	rgn     = TAP_Osd_Create( 0, 0, 720, 576, 0, FALSE );
    memRgn  = TAP_Osd_Create( 0, 0, 720, 576, 0, OSD_Flag_MemRgn );	// In MEMORY define the whole screen for us to draw on
    listRgn = TAP_Osd_Create( 0, 0, 720, 576, 0, OSD_Flag_MemRgn );	// In MEMORY define the whole screen for us to draw on

    appendToLogfile("ActivationRoutine: Retrieving current directory.");
    CurrentDir = GetCurrentDir();           // Stores the current directory name.

    appendToLogfile("ActivationRoutine: Checking currentDir to make sure we're in /Datafiles or a subdir.");
    if ((!InDataFilesFolder()) && (!InDataFilesSubFolder()))
    {
        appendToLogfile("ActivationRoutine: Not in Datafiles or subdir, so moving to DataFiles.");
        GotoDataFiles();
        CurrentDir="/DataFiles";            // Set the current directory name.
    }    

    appendToLogfile("ActivationRoutine: Loading archive info.");
    LoadArchiveInfo();

	sortOrder = sortOrderOption;        // Default to default sort order.
	SortList(sortOrder);		
    
    DetermineStartingLine( &chosenLine );       // Determine which line to highlight when we start.

    appendToLogfile("ActivationRoutine: Activating Archive Window.");
	ActivateArchiveWindow();

    appendToLogfile("ActivationRoutine: Finished.");
}
     

void ExitRoutine( void )
{
    CloseArchiveWindow();
    TAP_Osd_Delete( rgn );
	TAP_EnterNormal();
   //TAP_GenerateEvent( EVT_KEY, RKEY_Play, 0 );
	
}

 
//------------
//
void TerminateRoutine( void )											// Performs the clean-up and terminate TAP
{
    SaveDatToFile();
	TerminateMenu();
	TerminateConfigMenu();
	ReleaseLogoMemory();												// clean-up logo routines
	ExitRoutine();
	closeLogfile();   // Close logfile if we were in debug mode.
	TAP_Exit();															// exit
}

    
//------------
//  
dword My_KeyHandler(dword key, dword param2)
{
	dword state, subState ;
 

	if ( yesnoWindowShowing ) { return( YesNoKeyHandler( key ) ); }
																			
	if ( configWindowShowing ) { return( ConfigKeyHandler( key ) ); }
																			
	if ( creditsShowing ) { return( CreditsKeyHandler( key ) ); }
	 																		
	if ( infoWindowShowing ) { return( ArchiveInfoKeyHandler( key) );; }
	    
	if ( menuShowing ) { return(MainMenuKeyHandler( key )); }				// Menu
 
	if ( windowShowing ) { return( ArchiveWindowKeyHandler( key ) ); }		// archive display
 

    TAP_GetState(&state, &subState);
	if ((state != STATE_Normal) || (subState != SUBSTATE_Normal)) return key;	// otherwise just return if another menu is shown

	if (TAP_Channel_IsStarted( 0 )) return key;							// check for PIP

    
	if ( key == mainActivationKey )										// only enter our code from the normal state
	{
		// If our activation key is the PlayList key, make sure we aren't here from the PlayList key that this TAP generated
		// when PlayList was pressed within the TAP.
        if ((mainActivationKey == RKEY_PlayList) && (generatedPlayList == TRUE))
		{
           generatedPlayList = FALSE;
		   return key;  // Don't activate this time around, and pass the key through.
        }   
        ActivationRoutine();
		return 0;
	}

	return  key;														// default
}
          
 
//--------------------------
//
void CheckFlags( void )
{
	if ( exitFlag == TRUE ) { ExitRoutine(); exitFlag = FALSE; return; }	// close window and enter normal state
	if ( terminateFlag == TRUE ) { TerminateRoutine(); return; }			// clean-up and terminate TAP

 
	if ( returnFromInfo == TRUE )											// Handle returning from the info window.
	{																		// redraw the underlying window if it's changed.
	    returnFromInfo = FALSE;
		if ( fileRenamed )        // If the file/folder was renamed, refresh the list.
        {
             fileRenamed = FALSE; 
             RefreshArchiveList( FALSE );
        }
		if ( fileReset )        // If the file playback info was reset, refresh the list.
        {
             fileReset = FALSE; 
             RefreshArchiveList( FALSE );
        }
	}

	if ( returnFromDelete == TRUE )											// Handle returning from delete.
	{																		// redraw the underlying window if it's changed.
	    returnFromDelete = FALSE;
		if ( fileDeleted )        // If the file/folder was deleted, reload the file/folder data and refresh the list.
        {
             if (infoWindowShowing) CloseArchiveInfoWindow();  // If we deleted the file from the info window, then close the info window.
             DeleteProgressInfo(FALSE);                        // Delete any associated playback progress info (no message window)
             LoadArchiveInfo();                                // Reload the archive list 
             fileDeleted = FALSE; 
             RefreshArchiveList(FALSE);                        // Redisplay the entire list.
        }
	}
   
	if ( returnFromStop == TRUE )											// Handle returning from delete.
	{																		// redraw the underlying window if it's changed.
	    returnFromStop = FALSE;
		if ( fileStopped )        // If the file/folder was stopped, reload the file/folder data and refresh the list.
        {
             LoadArchiveInfo();                                // Reload the archive list 
             fileStopped = FALSE; 
             RefreshArchiveList(FALSE);                        // Redisplay the entire list.
        }
	}
    
	if ( returnFromRename == TRUE )									// Handle returning from rename.
	{																// redraw the underlying window
	    returnFromRename = FALSE;
	}

	if ( returnFromMenu == TRUE )									// Handle returning from main menu.
	{																// redraw the underlying window
	    returnFromMenu = FALSE;
	    RefreshArchiveWindow();                                     // Redraw the entire archive window.
	}

	if ( returnFromConfig == TRUE )									// Handle returning from config window.
	{																// redraw the underlying window
	    returnFromConfig = FALSE;
	    RedrawMainMenu();                                           // Redraw the entire main menu.
	}
 
	if ( returnFromCredits == TRUE )								// Handle returning from credits window.
	{																// redraw the underlying window
	    returnFromCredits = FALSE;
	    RedrawMainMenu();                                           // Redraw the entire main menu.
	}
 
}

//--------------------------
//
dword My_IdleHandler(void)
{
    dword currentTickTime;
	byte 	hour, min, sec;
	word 	mjd;	

#ifdef WIN32
#else
    CheckPlaybackStatus();   // Check for any active playbacks so that we can update the progress information.
#endif
	currentTickTime = TAP_GetTick();									// only get the current tick time once
    if ((currentTickTime - playbackInfoTick > PLAYBACK_INFO_WRITE_INTERVAL) && (playinfoChanged))
    {
         playbackInfoTick = currentTickTime;
         SaveDatToFile();
         playinfoChanged = FALSE;
    }
 
 
	if ( !windowShowing ) return;										// don't show the clock unless main window is active
	if ( menuShowing ) return;						                    // but, not interested if these windows are showing


	// If the keyboard is showing, and NOT the keyboard help, then blink the keyboard cursor.
    if (( keyboardWindowShowing ) && (!keyboardHelpWindowShowing)) KeyboardCursorBlink( currentTickTime );

   	TAP_GetTime( &mjd, &hour, &min, &sec);

	if ( min != oldMin )  // Update the clock and any recording/playback entries that are displayed, every minute.
	{
	    oldMin = min;
		UpdateListClock();
	    
	    // If our pop-up windows are showing, then exit, so that we don't overwrite them with any updated info.
        if ( infoWindowShowing ) return;						                    // Don't update recording/playback info if these windows are showing
	    if ( deleteWindowShowing ) return;						                    // Don't update recording/playback info if these windows are showing
	    if ( stopWindowShowing ) return;						                    // Don't update recording/playback info if these windows are showing
	    if ( archiveHelpWindowShowing ) return;					                    // Don't update disk/recording info if these windows are showing
		
		// Check if there is an active playback displayed on the screen.  If so, update it's progress information.
		if ((inPlaybackMode) && (playbackOnScreenEntry>0))
		{
            SetPlaybackStatus(playbackOnScreenEntry);                                 // Load the latest information from the playback file.
            DisplayArchiveLine( playbackOnScreenEntry, playbackOnScreenLine );        // Update the playback line.
        }
        
		// Check if there is are active recording(s) displayed on the screen.  If so, update their progress information.
		if ((recordingOnScreenEntry1 + recordingOnScreenEntry2)>0)                    // There's at least 1 active recording on the screen.
		{
            GetRecordingInfo();                                                       // Get the latest information from the recordings.
            if (recordingOnScreenEntry1 > 0)                                          // Is it the 1st recording?
            {
               LoadRecordingInfo( recordingOnScreenEntry1 );                          // Load the latest information into the list.
               DisplayArchiveLine( recordingOnScreenEntry1, recordingOnScreenLine1 ); // Update the playback line.
            } 
            if (recordingOnScreenEntry2 > 0)                                          // Is it the 2nd recording?
            {
               LoadRecordingInfo( recordingOnScreenEntry2 );                          // Load the latest information into the list.
               DisplayArchiveLine( recordingOnScreenEntry2, recordingOnScreenLine2 ); // Update the playback line.
            } 
        }
	}

	if ( infoWindowShowing ) return;						                    // Don't update disk/recording info if these windows are showing
	if ( deleteWindowShowing ) return;						                    // Don't update disk/recording info if these windows are showing
	if ( stopWindowShowing ) return;						                    // Don't update disk/recording info if these windows are showing
	if ( archiveHelpWindowShowing ) return;					                    // Don't update disk/recording info if these windows are showing
   
    if (sec != oldSec)
    {
        oldSec = sec;
        
        // If I've highlighted an active recording, then update it's recording information every second.    
        if (myfiles[chosenLine].isRecording) // Update the recording information very second.
        {
           GetRecordingInfo();               // Update the recording information.
           LoadRecordingInfo( chosenLine );  // Update the myfiles array with the updated recording information.
        }   
        UpdateSelectionNumber();             // Update diskspace and recording info every second.
    }      
}


//-----------------------------------------------------------------------
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

		case EVT_KEY :	returnKey = My_KeyHandler(param1, param2);
						CheckFlags();
						break;

		default : 		break;
	}
	return (returnKey);
}
           
void TSRCommanderConfigDialog()
{
    windowShowing = FALSE;
	menuShowing = FALSE;
	configWindowShowing = TRUE;

	TAP_ExitNormal();
    CalledByTSRCommander=TRUE;
    DisplayConfigWindow();
	return;
}
   
bool TSRCommanderExitTAP (void)
{	

	terminateFlag = TRUE;	// Will cause TAP to terminate and unload.

	return TRUE;
}

        
int TAP_Main (void)
{
    openLogfile();                   // Opens the logfile in the current directory, if we are in debug mode.
    
    appendToLogfile("Archive TAP Started.");

    if (GetModel() == TF5800t) 
    {
       unitModelType=TF5800t;
       appendToLogfile("TAP_Main: Detected model TF5800.");
       headerOffset=4;        // The TF5800 has some fields out 4 bytes compared to the TF500.
    }
    else
    {
       appendToLogfile("TAP_Main: Detected model is NOT TF5800.");
       unitModelType=TF5000t;
       headerOffset=0;        // Do not apply any offset when reading the header.
    }
     
    TSRCommanderInit( 0, TRUE );    // Include the TSR Commander function.  

	TAP_Hdd_ChangeDir(PROJECT_DIRECTORY);  // Change to the UK TAP Project SubDirectory.
    TAPIniDir = GetCurrentDir();           // Store the directory location of the INI file.	

    appendToLogfile("TAP_Main: Loading configuration data.");
	LoadConfiguration();
	sortOrder = sortOrderOption; // Default to sort order specified in the config file. 
    ResetScreenColumns();        // Set column widths according to column options.
    
    appendToLogfile("TAP_Main: Loading Play Data.");
	LoadPlayData();  // Load up the playback status information from the dat file.

    appendToLogfile("TAP_Main: Caching logos.");
	CacheLogos();

    appendToLogfile("TAP_Main: Starting initialisation routines.");
    appendToLogfile("TAP_Main: Initialising ArchiveWindow.");
	initialiseArchiveWindow();

    appendToLogfile("TAP_Main: Initialising ArchiveDelete.");
    initialiseArchiveDelete();

    appendToLogfile("TAP_Main: Initialising ArchiveRename.");
    initialiseArchiveRename();

    appendToLogfile("TAP_Main: Initialising Menu.");
	initialiseMenu();

    appendToLogfile("TAP_Main: Initialising ConfigRoutines.");
	InitialiseConfigRoutines();

	oldMin = 100;
	oldSec = 100;
	exitFlag = FALSE;
	terminateFlag = FALSE;
	generatedPlayList = FALSE;

    appendToLogfile("TAP_Main: Finished initial TAP_Main routine.");
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
      