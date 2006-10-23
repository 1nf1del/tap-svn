/************************************************************
		 		OZ Archive
	Archive Recordings display, and management TAP
                               

	This module is the main event handler
  
Name	: OZ Archive.c
Author	: kidhazy
Version	: 0.08
For		: Topfield TF5x00 series PVRs
Licence	:  
Descr.	:
Usage	:
History	: v0.01 kidhazy 17-10-05   Inception date.
          v0.02 kidhazy    11-05   TF5800 support
          v0.03 kidhazy    11-05 
          v0.04 kidhazy    11-05
          V0.05 kidhazy    12-05   Now uses arrays to hold recordings and directories.
          V0.06 kidhazy 20-12-05   Fixes and added line number display.  
          V0.07 kidhazy 23-12-05
          V0.08 kidhazy 
  
	Last change:  USE   3 Aug 105    0:02 am
**************************************************************/
#ifdef WIN32
#include "c:\TopfieldDisk\DataFiles\SDK\TAP_Emulations.h"
#undef RGB
#define RGB(r,g,b)		   		 ( (0x8000) | ((r)<<10) | ((g)<<5) | (b) )
//#define RGB(r,g,b) ((COLORREF)(((BYTE)(r<<3)|((WORD)((BYTE)(g<<3))<<8))|(((DWORD)(BYTE)(b<<3))<<16)))
#endif           
                      

#define DEBUG   0      // 0 = no debug info, 1 = debug written to logfile,  2 = debug written to screen, 3 = TAP_Print output, 4 = Message Box

// Define the error levels 
#define ERR     1
#define WARNING 2
#define INFO    3

// Define the level of info or error message to be displayed.
#define LOGLEVEL ERR   // 1 = errors         2 = warnings      3 = information
    
#define TAP_NAME "Archive"
#define VERSION "0.08l"       

#include "tap.h"

#ifdef WIN32
#include "TAPExtensions.c"
#include "FirmwareCalls.c"
#else
#include "TAPExtensions.h"
#include "FirmwareCalls.h"
#include "model.c"
#endif      

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

#include "TSRCommander.inc"
 
char* TAPIniDir;
                                
#include "morekeys.h"
  
                                                                                    
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
//#include "ArchiveDelete.c"
#include "ArchiveStop.c"
#include "ArchiveAction.c"
#include "ArchiveRename.c"
#include "ArchiveRecycle.c" 
#include "ArchiveDelete.c"
#include "ArchiveMove.c" 
#include "ArchiveInfo.c" 
#include "MainMenu.c"
#include "ConfigMenu.c"
#include "GmtOffset.c"
#include "IniFile.c"
      
                              
static dword lastTick;
static byte oldHour;
static byte oldMin;
static byte oldSec;
                                      
                                                                      
                                         
//------------  
//                   
void ActivationRoutine( void )
{         
    appendToLogfile("ActivationRoutine: Started.", INFO);
  	TAP_ExitNormal();

    tapStartup = FALSE;              // clear flag to indicate that TAP has finished startup and this is a reactivation.
  	
  	// Define on screen region to draw on.
	rgn      = TAP_Osd_Create( 0, 0, 720, 576, 0, FALSE );
	// Define a copy of the screen region in memory.
    memRgn   = TAP_Osd_Create( 0, 0, 720, 576, 0, OSD_Flag_MemRgn );	// In MEMORY define the whole screen for us to draw on
    listRgn  = TAP_Osd_Create( 0, 0, 720, 576, 0, OSD_Flag_MemRgn );	// In MEMORY define the whole screen for us to draw on
    clockRgn = TAP_Osd_Create( 0, 0, 720, TOP_H, 0, OSD_Flag_MemRgn );	// In MEMORY define the a region for us to draw on

    // Default to standard view mode.
	recycleWindowMode = FALSE;  
    strcpy( tagStr, REC_STRING);   // Set the tag at the end of the filenames to ".rec"
    tagLength = strlen( tagStr );  // Calculate the length of the tag.  

    // Initial loading of info may take some time due to the recursive calls.  Users can therefore select the splashScreenOption in the config menu.
    // So display a "Please wait..."  message.
    if (splashScreenOption == 1)
        TAP_Osd_PutStringAf1926( rgn, 58, 40, 390, "             LOADING...            ", TITLE_COLOUR, COLOR_Black );
//      ShowMessageBox( rgn, "Archive Starting", "Loading file information.", "Please wait ...");
        
	if (PBKgmtOffsetOption == 0)  // Check for PBK GMT_Offset.ini file in case daylight savings has kicked in.
	{
        ReadGmtOffset();          // Read the GMT Offset and daylight savings options from GMT_Offset.ini if available.
        ChangeDirRoot();          // Jump back to the root directory.
        GotoPath(CurrentDir);     // Return to the previous data directory.
    }    

    // Check that we are in the /DataFiles directory, or a subdirectory.  If not, change to the /DataFiles directory.   
    appendToLogfile("ActivationRoutine: Checking currentDir to make sure we're in /Datafiles or a subdir.", INFO);
    if ((!InDataFilesFolder( CurrentDir )) && (!InDataFilesSubFolder( CurrentDir )))
    {
        appendToLogfile("ActivationRoutine: Not in Datafiles or subdir, so moving to DataFiles.", WARNING);
        GotoDataFiles();
        strcpy(CurrentDir,"/DataFiles");            // Set the current directory name.
    }    


    // Load information about the archive files.
    loadSubsequentArchiveInfo(TRUE, 1);    
    DetermineStartingLine( &chosenLine );           // Determine which line to highlight when we start.

    appendToLogfile("ActivationRoutine: Activating Archive Window.", INFO);
	ActivateArchiveWindow();
 
    appendToLogfile("ActivationRoutine: Finished.", INFO);
}
   
 
void ExitRoutine( void )
{
    CloseArchiveWindow();
    
    if ( recycleWindowMode )     // Reset to standard view mode, and reload the data so that it is ready for next activation.
    {
	    recycleWindowMode = FALSE;  
        strcpy( tagStr, REC_STRING);   // Set the tag at the end of the filenames to ".rec"
        tagLength = strlen( tagStr );  // Calculate the length of the tag.  
        loadInitialArchiveInfo(FALSE, 99); // Load all the files for the new view, but don't delete any progress info.
    }    

	TAP_EnterNormal();
}          


//------------
//
void TerminateRoutine( void )											// Performs the clean-up and terminate TAP
{
    int dirNumber, fileIndex;
    
    SaveDatToFile();
	TerminateMenu();
	TerminateConfigMenu();
	ReleaseLogoMemory();												// clean-up logo routines
	ExitRoutine();

    // Free up any memory that may have been allocated during TAP.
    for (dirNumber=0; dirNumber<=MAX_DIRS; dirNumber++)
    {
        for (fileIndex=1; fileIndex <= MAX_FILES; fileIndex++)
        {
            if (myfiles[dirNumber][fileIndex] != NULL) TAP_MemFree(myfiles[dirNumber][fileIndex]);
        }
        if (myfolders[dirNumber] != NULL) TAP_MemFree(myfolders[dirNumber]);
    }
    if (TAPIniDir != NULL) TAP_MemFree( TAPIniDir );
              
    // Free up any memory that may have been allocated during TAP.
    for (fileIndex=0; fileIndex <= MAX_FILES; fileIndex++)
    {
        if (playedFiles[fileIndex] != NULL) TAP_MemFree(playedFiles[fileIndex]);
    }

	closeLogfile();   // Close logfile if we were in debug mode.
    TAP_Exit();															// exit
}
    

//------------
//  
dword My_KeyHandler(dword key, dword param2)
{ 
	dword state, subState ;
	
	if ( yesnoWindowShowing )  { return( YesNoKeyHandler( key ) ); }
																			
// 	if ( msgWindowShowing )    { return( MsgWindowKeyHandler( key) ); }
	    
	if ( configWindowShowing ) { return( ConfigKeyHandler( key ) ); }
																			
	if ( creditsShowing )      { return( CreditsKeyHandler( key ) ); }
	 																		
	if ( moveWindowShowing )   { return( ArchiveMoveKeyHandler( key) ); }
	    
	if ( infoWindowShowing )   { return( ArchiveInfoKeyHandler( key) ); }
	    
	if ( menuShowing )         { return( MainMenuKeyHandler( key ) ); }				// Menu
	
	if ( recycleWindowMode )   { return( RecycleBinWindowKeyHandler( key ) ); }		// recycle bin display

	if ( windowShowing )       { return( ArchiveWindowKeyHandler( key ) ); }		// archive display
 

    TAP_GetState(&state, &subState);
	if ((state != STATE_Normal) || (subState != SUBSTATE_Normal)) return key;	// otherwise just return if another menu is shown

	if (TAP_Channel_IsStarted( 0 )) return key;							// check for PIP

    if ( OsdActive(50, 10, 200, 40) || OsdActive(50, 150, 200, 300)) return key;  //Check if there's anything on the screen between 50,10 and 200,40 or 50,150 and 200,300.
                                                                                 //Catches the manual channel entry numbers, bug ignores the mute/volume graphics.
    if ( (mainActivationKey == RKEY_Ok) && (OsdActive(150, 390, 400, 510)) ) return key;  //If OK is the activation key and there is something on the bottom middle of the screen (eg. Info box or Improbox) then don't activate.
    
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
		if ( listMoved )        // If the list was changed during info, refresh the list.
        {
             listMoved = FALSE; 
             RefreshArchiveList( FALSE );   // Redraw and reposition as we may have moved off the page.
        }
		if ( fileMoved )        // If the file was moved during info, refresh the list.
        { 
             fileMoved = FALSE; 
             RefreshArchiveList( FALSE );   // Redraw and reposition as we may have moved off the page.
        }     
	} 
  
	if ( returnFromMove == TRUE )											// Handle returning from the Move window.
	{																		// redraw the underlying window if it's changed.
	    returnFromMove = FALSE;
		if ( fileMoved )        // If the file/folder was renamed, refresh the list.
        {
             if (infoWindowShowing) CloseArchiveInfoWindow();  // If we moved the file from the info window, then close the info window.
             if (myfiles[CurrentDirNumber][chosenLine]->attr == ATTR_FOLDER)  // Delete any subfolders first.
                   DeleteMyfilesFolderEntry( myfiles[CurrentDirNumber][chosenLine]->directoryNumber);
             DeleteMyfilesEntry(CurrentDirNumber, chosenLine);
             fileMoved = FALSE; 
             RefreshArchiveList( FALSE );
        }
    }
          
	if ( returnFromDelete == TRUE )								// Handle returning from delete.
	{	   														// redraw the underlying window if it's changed.
	    returnFromDelete = FALSE;
		if ( fileDeleted )        // If the file/folder was deleted, reload the file/folder data and refresh the list.
        {
             if (infoWindowShowing) CloseArchiveInfoWindow();   // If we deleted the file from the info window, then close the info window.
             if (myfiles[CurrentDirNumber][chosenLine]->attr == ATTR_FOLDER)  // Delete any subfolders first.
                   DeleteMyfilesFolderEntry( myfiles[CurrentDirNumber][chosenLine]->directoryNumber);
             DeleteMyfilesEntry(CurrentDirNumber, chosenLine);
             fileDeleted = FALSE; 
             RefreshArchiveList(FALSE);                         // Redisplay the entire list.
        }
	}
   
	if ( returnFromStop == TRUE )								// Handle returning from stop.
	{															// redraw the underlying window if it's changed.
	    returnFromStop = FALSE;
		if ( fileStopped )                                      // If the file was stopped, reload the file/folder data and refresh the list.
        {
             GetRecordingInfo();                                // Get the information about the current active recordings.
             LoadRecordingInfo(CurrentDirNumber, chosenLine);   // Assign any current recording info to the current line.
             GetPlaybackInfo();                                 // Get the information about the current active playbacks.
             LoadPlaybackInfo(CurrentDirNumber, chosenLine);    // Assign any current playback info to the current line.
             fileStopped = FALSE; 
             RefreshArchiveList(FALSE);                         // Redisplay the entire list.
        }
	}
    
	if ( returnFromRename == TRUE )								// Handle returning from rename.
	{															// redraw the underlying window
	    returnFromRename = FALSE;
	} 

    if ( returnFromRecycleBinWindowEmpty == TRUE )              // Handle returning from the Recycle Bin window after deleting recycled files.
    {
        returnFromRecycleBinWindowEmpty = FALSE;
        TAP_Osd_PutStringAf1926( rgn, 58, 40, 390, "             LOADING...            ", TITLE_COLOUR, COLOR_Black );
        loadInitialArchiveInfo(FALSE, 99); // Load all the files for the new view, but don't delete any progress info.
        RefreshArchiveList(TRUE);      // Redraw the contents of the screen.
    }

	if ( returnFromMenu == TRUE )								// Handle returning from main menu.
	{															// redraw the underlying window
	    returnFromMenu = FALSE;
        if ( returnFromRecycleBinEmpty == TRUE )                                // Handle returning from emptying the Recycle Bin.
        {
            returnFromRecycleBinEmpty = FALSE;
            if (recycleWindowMode)  // We need to refresh all of the recycle bin information if we are currently in Recycle Bin view mode.
            {
                TAP_Osd_PutStringAf1926( rgn, 58, 40, 390, "             LOADING...            ", TITLE_COLOUR, COLOR_Black );
                loadInitialArchiveInfo(FALSE, 99); // Load all the files for the new view, but don't delete any progress info.
//   		        RefreshArchiveList(TRUE);      // Redraw the contents of the screen.
            }    
        }     
	    RefreshArchiveWindow();                                 // Redraw the entire archive window.
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
 
    // Get the current time.
   	TAP_GetTime( &mjd, &hour, &min, &sec);
 
    // If we've set the automatic Recycle Bin cleanout for 3AM check if it has just clocked over to 3AM.  If so, delete the recycle bin.
	if ((recycleBinCleanoutOption==2) && (hour == 3) && (hour != oldHour ))
	{
        oldHour = hour;       // Save the current hour for checking again next time through.                       
        if ( !windowShowing ) 						          // If the main window isn't active, then
	       rgn = TAP_Osd_Create( 0, 0, 720, 576, 0, FALSE );  // Define on screen region to draw on.
        ReturnFromAllRecycleBinYesNo( TRUE );                 // Call routine to delete ALL recycled files.	
        if ( !windowShowing ) 						          // If the main window isn't active, then
        {
           returnFromRecycleBinWindowEmpty = FALSE;           // Clear any flags as we're not actually in normal display mode.
	       TAP_Osd_Delete( rgn );                             // Delete region we just used.
        }
    }               
    oldHour = hour;       // Save the current hour for checking again next time through.                       
 
	if ( !windowShowing ) return;										// don't show the clock unless main window is active
	if (  menuShowing )   return;						                // but, not interested if these windows are showing

//    if ((msgWindowShowing) && (currentTickTime - msgWindowTickTime > msgWindowDelay))
//        returnFromMsgWindow = TRUE;


	// If the keyboard is showing, and NOT the keyboard help, then blink the keyboard cursor.
    if (( keyboardWindowShowing ) && (!keyboardHelpWindowShowing)) KeyboardCursorBlink( currentTickTime );


	if ( min != oldMin )  // Update the clock and any recording/playback entries that are displayed, every minute.
	{
	    if ( archiveHelpWindowShowing ) return;					                    // Don't update disk/recording info if these windows are showing
	    if ( keyboardHelpWindowShowing ) return;					                    // Don't update disk/recording info if these windows are showing

	    oldMin = min;
		UpdateListClock();
	    
	    // If our pop-up windows are showing, then exit, so that we don't overwrite them with any updated info.
        if ( infoWindowShowing )   return;						                    // Don't update recording/playback info if these windows are showing
        if ( moveWindowShowing )   return;						                    // Don't update recording/playback info if these windows are showing
	    if ( deleteWindowShowing ) return;						                    // Don't update recording/playback info if these windows are showing
	    if ( stopWindowShowing )   return;						                    // Don't update recording/playback info if these windows are showing
	    if ( msgWindowShowing )    return;						                    // Don't update recording/playback info if these windows are showing
		
		// Check if there is an active playback displayed on the screen.  If so, update it's progress information.
		if ((inPlaybackMode) && (playbackOnScreenEntry>0))
		{
            SetPlaybackStatus(CurrentDirNumber, playbackOnScreenEntry);               // Load the latest information from the playback file.
            DisplayArchiveLine( playbackOnScreenEntry, playbackOnScreenLine );        // Update the playback line.
        }
         
		// Check if there is are active recording(s) displayed on the screen.  If so, update their progress information.
		if ((recordingOnScreenEntry1 + recordingOnScreenEntry2)>0)                    // There's at least 1 active recording on the screen.
		{
            GetRecordingInfo();                                                       // Get the latest information from the recordings.
            if (recordingOnScreenEntry1 > 0)                                          // Is it the 1st recording?
            {
               LoadRecordingInfo( CurrentDirNumber, recordingOnScreenEntry1 );        // Load the latest information into the list.
               DisplayArchiveLine( recordingOnScreenEntry1, recordingOnScreenLine1 ); // Update the playback line.
            } 
            if (recordingOnScreenEntry2 > 0)                                          // Is it the 2nd recording?
            {
               LoadRecordingInfo( CurrentDirNumber, recordingOnScreenEntry2 );        // Load the latest information into the list.
               DisplayArchiveLine( recordingOnScreenEntry2, recordingOnScreenLine2 ); // Update the playback line.
            } 
        }
	}
   
	if ( infoWindowShowing )        return;	// Don't update disk/recording info if these windows are showing
    if ( moveWindowShowing )        return;	// Don't update disk/recording info if these windows are showing
	if ( deleteWindowShowing )      return;	// Don't update disk/recording info if these windows are showing
	if ( stopWindowShowing )        return;	// Don't update disk/recording info if these windows are showing
	if ( archiveHelpWindowShowing ) return;	// Don't update disk/recording info if these windows are showing
    if ( msgWindowShowing )         return;	// Don't update recording/playback info if these windows are showing
    
    if (sec != oldSec)
    {
        oldSec = sec;
        // If I've highlighted an active recording, then update it's recording information every second.    
        if (myfiles[CurrentDirNumber][chosenLine]->isRecording) // Update the recording information very second.
        {
           GetRecordingInfo();               // Update the recording information.
           LoadRecordingInfo( CurrentDirNumber, chosenLine );  // Update the myfiles array with the updated recording information.
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
    windowShowing       = FALSE;
	menuShowing         = FALSE;
	configWindowShowing = TRUE;

	TAP_ExitNormal();
    CalledByTSRCommander=TRUE;
	rgn      = TAP_Osd_Create( 0, 0, 720, 576, 0, FALSE );
    clockRgn = TAP_Osd_Create( 0, 0, 720, TOP_H, 0, OSD_Flag_MemRgn );	// In MEMORY define the a region for us to draw on
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
    int i,d, dirNumber, fileIndex;

    TSRCommanderInit( 0, TRUE );    // Include the TSR Commander function.  

    tapStartup = TRUE;              // set flag to indicate that we're starting the TAP for the first time.
    
    // Create a full screen region in case there are any messages to be displayed during the initial startup.
	rgn     = TAP_Osd_Create( 0, 0, 720, 576, 0, FALSE );
	
    openLogfile();                   // Opens the logfile in the current directory, if we are in debug mode.
 
    appendToLogfile("Archive TAP Started.", INFO);
 
    #ifdef WIN32
    #else
         StartTAPExtensions();
    #endif
//	TAP_Print("%d\n", CallFirmware( 0,0,0,0, 0x80003a58, tapProcess, currentTAPIndex ));

    #ifdef WIN32
       appendToLogfile("TAP_Main: Detected model is NOT TF5800.", WARNING);
       unitModelType=TF5000t;
       headerOffset=0;        // Do not apply any offset when reading the header.
    #else
    if (GetModel() == TF5800t) 
    {
       unitModelType=TF5800t;
       appendToLogfile("TAP_Main: Detected model TF5800.", WARNING);
       headerOffset=4;        // The TF5800 has some fields out 4 bytes compared to the TF500.
    }
    else
    {
       appendToLogfile("TAP_Main: Detected model is NOT TF5800.", WARNING);
       unitModelType=TF5000t;
       headerOffset=0;        // Do not apply any offset when reading the header.
    }
    #endif
    
	TAP_Hdd_ChangeDir(PROJECT_DIRECTORY);  // Change to the UK TAP Project SubDirectory.
    TAPIniDir = GetCurrentDir();           // Store the directory location of the INI file.	
    strcpy(CurrentDir,"/DataFiles");
    
    appendToLogfile("TAP_Main: Loading configuration data.", INFO);
	LoadConfiguration();
	
	if (PBKgmtOffsetOption == 0)       // If option has been set to read the GMT_Offset.ini file, read it.
    {
        ReadGmtOffset();               // Read the GMT Offset and daylight savings options from GMT_Offset.ini if available.
    }   

    appendIntToLogfile("Config Info: infoLineOption=%d",infoLineOption, WARNING);
    appendIntToLogfile("Config Info: sortOrderOption=%d",sortOrderOption, WARNING);
    appendIntToLogfile("Config Info: column1Option=%d",column1Option, WARNING);
    appendIntToLogfile("Config Info: column2Option=%d",column2Option, WARNING);
    appendIntToLogfile("Config Info: column3Option=%d",column3Option, WARNING);
    appendIntToLogfile("Config Info: column4Option=%d",column4Option, WARNING);
    appendIntToLogfile("Config Info: column5Option=%d",column5Option, WARNING);
    appendIntToLogfile("Config Info: numberLinesOption=%d",numberLinesOption, WARNING);
    appendIntToLogfile("Config Info: borderOption=%d",borderOption, WARNING);
    appendIntToLogfile("Config Info: recCheckOption=%d",recCheckOption, WARNING);
    appendIntToLogfile("Config Info: progressBarOption=%d",progressBarOption, WARNING);
	
	sortOrder = sortOrderOption; // Default to sort order specified in the config file. 
    ResetScreenColumns();        // Set column widths according to column options.

    appendToLogfile("TAP_Main: Loading Play Data.", INFO);
	// Loop through and set all file pointers to NULL.
    for (fileIndex=0; fileIndex <= MAX_FILES; fileIndex++)
    {
        playedFiles[fileIndex] = NULL;
    }
	LoadPlayData();  // Load up the playback status information from the dat file.

    appendToLogfile("TAP_Main: Caching logos.", INFO);
	CacheLogos();    
  
    appendToLogfile("TAP_Main: Starting initialisation routines.", INFO);
    appendToLogfile("TAP_Main: Initialising ArchiveRecycle.", INFO);
    initialiseArchiveRecycle();            

    appendToLogfile("TAP_Main: Initialising ArchiveWindow.", INFO);
	initialiseArchiveWindow();
     
    appendToLogfile("TAP_Main: Initialising ArchiveDelete.", INFO);
    initialiseArchiveDelete();

    appendToLogfile("TAP_Main: Initialising ArchiveRename.", INFO);
    initialiseArchiveRename();

    appendToLogfile("TAP_Main: Initialising Menu.", INFO);
	initialiseMenu();

    appendToLogfile("TAP_Main: Initialising ConfigRoutines.", INFO);
	InitialiseConfigRoutines();
 
	numberOfFiles   = 0;
	numberOfFolders = 0;
	// Loop through and set all folder and file pointers to NULL.
    for (dirNumber=0; dirNumber<=MAX_DIRS; dirNumber++)
    {
        myfolders[dirNumber] = NULL;
        for (fileIndex=1; fileIndex <= MAX_FILES; fileIndex++)
        {
            myfiles[dirNumber][fileIndex] = NULL;
        }   
    }
 	
    CreateBlankFile();
     
  
    // Blank out initial folder & file variable space.
    myfolders[0] = TAP_MemAlloc( sizeof  currentFolder); 
    memset(myfolders[0],0,sizeof (*myfolders[0]));
    myfiles[0][0] = TAP_MemAlloc(sizeof (*myfiles[0][0]));
    memset(myfiles[0][0],0,sizeof (*myfiles[0][0]));
    myfiles[0][1] = TAP_MemAlloc(sizeof (*myfiles[0][1]));
    memset(myfiles[0][1],0,sizeof (*myfiles[0][1]));
 
    ChangeDirRoot();
    GotoDataFiles();
    strcpy(CurrentDir,"/DataFiles");
    CurrentDirNumber = 0;      // Start off in the DataFiles directory which is number 0 in our array.
    GetRecordingInfo();

    SetAllFilesToNotPresent();
    strcpy(myfolders[0]->name,"/DataFiles");
	LoadArchiveInfo("/DataFiles", 0, 0, 99);  // Do a full recursive load of the Archive files/folders.

    numberOfFiles = myfolders[CurrentDirNumber]->numberOfFiles;
    maxShown      = numberOfFiles;
 
    GetPlaybackInfo();  // Get info about any active playback.
    LoadPlaybackStatusInfo();  

    numberOfFiles = myfolders[CurrentDirNumber]->numberOfFiles;
    maxShown      = numberOfFiles;

    // Set 'old' times to invalid number so they'll be reset.
    oldHour = 100;
	oldMin  = 100;
	oldSec  = 100;
	exitFlag = FALSE;
	terminateFlag = FALSE;
	generatedPlayList = FALSE;

    appendToLogfile("TAP_Main: Finished initial TAP_Main routine.", INFO);
  
    TAP_Osd_Delete( rgn );

    tapStartup = FALSE;              // clear flag to indicate that TAP has finished startup.

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
      
