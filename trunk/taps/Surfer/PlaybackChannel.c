/************************************************************
				Part of the UK TAP Project
		This module handles setup of a virtual Playback Channel

Name	: Picture.c
Author	: kidhazy
Version	: 0.0
For		: Topfield TF5x00 series PVRs
Licence	:
Descr.	:
Usage	:
History	: v0.0 kidhazy: 22-08-05	Inception Date
          v0.1 kidhazy: 14-09-05    Cleaned up some graphics.  Fixed variables for duration/runtime of playback files.
          v0.2 kidhazy: 27-09-05    Fixed variable lengths for eventname combined description for playback channel.
          v0.3 kidhazy: 27-09-05    Added logfile information.


	Last change:  USE   22 Aug 105    9:25 pm
**************************************************************/

#define ApiOverRunArray 128    // Allocate some space to allow for buffer overrun
#define ApiOverRunCheck 1      // Value to check to determine if buffer overrun



static TYPE_PlayInfo CurrentPlaybackInfo;                      // Place to store info about current playback if invoked during playback mode.
static char          sOverRun[ApiOverRunArray];                // Keep some space to protect against buffer overrun.
static int           iOverRunCheck;                            // If buffer overrun is larger than sOverRun iOverRunCheck will be hit.
static bool          Playback_Selected = FALSE;
static bool          InPlaybackMode;
static int           Playback_svcnum;                          // Establish a fake service to represent the playback channel.

TYPE_File            *Playback_Lastfile;
char	             Playback_Lastfilename[ TS_FILE_NAME_SIZE ];
dword                Playback_StartCluster;
char                 NoPlayback_filename[ TS_FILE_NAME_SIZE ];
TYPE_TapEvent        Playback_event;                             // Place to store playback event information.
TYPE_TapEvent        BlankPlayback_event;                        // Create a blank playback event.
long                 Playback_SavedPosition;                     // Save the current block position.
long                 Playback_TotalBlocks;                       // Save the total block size.


typedef struct _DIR_LIST
{
     struct _DIR_LIST* Next;
     char name[TS_FILE_NAME_SIZE];
}
DIR_LIST;

int PlayTs (char* name, dword startCluster)
{
    char str[200];
    
     DIR_LIST* DirList = (DIR_LIST*) NULL;
     TYPE_File file;

     memset (&file, 0, sizeof (file));
     if (TAP_Hdd_FindFirst(&file))
     {
        do
        {
            // have we found the file ?
            
            if ((strcmp (file.name, name) == 0) && (file.startCluster == startCluster))
            {
                 // free up any memory we have allocated
                 while (DirList)
                 {
                      DIR_LIST* temp = DirList;
                      DirList = DirList->Next;
                      TAP_MemFree(temp);
                 }
                 // try and play the file
                 return TAP_Hdd_PlayTs(name);
            }
            // is it a directory, if so save for later
            if (file.attr == ATTR_FOLDER)
            {
                        
                 DIR_LIST* temp = (DIR_LIST*) TAP_MemAlloc (sizeof (DIR_LIST));
                 if (temp)
                 {
                      strcpy (temp->name, file.name);
                      temp->Next = DirList;
                      DirList = temp;
                 }
            }
        }
        while (TAP_Hdd_FindNext (&file));
        // OK we haven't found the file yet, try the subdirectories
        while (DirList)
        {
           DIR_LIST* temp = DirList;
           TAP_Hdd_ChangeDir (DirList->name);   // Move to the subdirectory.
           // Try the subdirectory, stop on success
           if (PlayTs (name, startCluster) == 0)
           {
              // free up any memory we have allocated
              while (DirList)
              {
                 DIR_LIST* temp = DirList;
                 DirList = DirList->Next;
                 TAP_MemFree (temp);
              }
              return 0;
           }
           // not in this one, try the next
           TAP_Hdd_ChangeDir ("..");
           DirList = DirList->Next;
           TAP_MemFree (temp);
        }
     }

     // we've failed totally....
     return -1;
}





void HandlePlaybackPip(char filename[TS_FILE_NAME_SIZE])
{
	TAP_Osd_FillBox( rgn, PIP_X, PIP_Y, PIP_W, PIP_H, COLOR_Black );		// hide the picture & clear any old overlay graphics
    TAP_Osd_PutGd( rgn, PIP_X_GRAPHIC_OFFSET,    PIP_MSG_Y+4,    &_plainredbarGd, TRUE );	// display plain red bar in PiP area
    PrintCenter(   rgn, PIP_X_GRAPHIC_OFFSET+7,  PIP_MSG_Y+4+11, PIP_X_GRAPHIC_OFFSET+204, filename, MAIN_TEXT_COLOUR, 0 , FNT_Size_1419);
	if ( strcmp(filename, NoPlayback_filename) != 0)    // Check if we haven't had a Playback file before
    {
       PrintCenter(  rgn, PIP_X_TEXT_OFFSET, PIP_MSG_Y+50, PIP_W,  "Press OK to play",  MAIN_TEXT_COLOUR, 0 , FNT_Size_1419);
       PrintCenter(  rgn, PIP_X_TEXT_OFFSET, PIP_MSG_Y+70, PIP_W, "in the main window", MAIN_TEXT_COLOUR, 0 , FNT_Size_1419);
    }
}

void CreatePlaybackChannel(int *i)
{
     // Make the last entry in the array the PlayBack service
     // Set a global variable PlayBack_SvcNum to equal the maximum number.
     // Set logoarray for playback to match logo.  eg. chName = PLAYBACKMODE
    strcpy( logoArrayTv[*i].svcName, "Playback Mode");			// and copy the channel's name
    logoArrayTv[*i].svcLCN  = 999;				                // service LCN
	logoArrayTv[*i].svcNum  = *i+1;								// service number
	logoArrayTv[*i].svcType = SVC_TYPE_Tv;						// service type (TV, or Radio)
	logoArrayTv[*i].processedFlag = FALSE;						// no logo cached yet
	logoArrayTv[*i].logo = TAP_MemAlloc( LOGO_DATA_SIZE );		// allocate space for the logo
	CreateFileName( &logoArrayTv[*i] );							// filter out puntuation and generate a file name
    Playback_svcnum = *i; 
    *i = *i + 1;	
}

void CreateBlankPlaybackEvent( TYPE_TapEvent *event )
{
     event->startTime = 0;
     event->endTime   = 0;
     event->duration  = 0;
     strcpy(event->eventName,"No last playback entry.");
     strcpy(event->description,"There is no filename available for playback.  You can play a file if Surfer has previously been started in playback mode.");
     event->parentalRating = 0;
}			

void InitialisePlaybackChannel( void )
{
     Playback_Selected      = FALSE;
     Playback_SavedPosition = 0;
     Playback_TotalBlocks   = 1;  // Assume at least 1 block so that we don't get divide by zero problems.
     InPlaybackMode         = FALSE;
     
     CreateBlankPlaybackEvent( &BlankPlayback_event );
     Playback_event         = BlankPlayback_event;
     
     strcpy(NoPlayback_filename, "No Last Playback File");
     strcpy(Playback_Lastfilename, NoPlayback_filename);
     Playback_StartCluster = 0;
}     


void CheckPlaybackInfo( void )
{    
   char combinedDesc[300];

   word startMjd; byte startHour; byte startMin; 
   word endMjd;   byte endHour;   byte endMin;
   byte durHour;  word durMin;
   dword tempStarttime, tempEndtime;
   int len;

   appendToLogfile("CheckPlaybackInfo: Started.");
   appendToLogfile("CheckPlaybackInfo: About to call TAP_Hdd_GetPlayInfo.");
   // Check for Playbackmode
   iOverRunCheck = ApiOverRunCheck;   // Set flag to catch any buffer overrun from API call.
   TAP_Hdd_GetPlayInfo (&CurrentPlaybackInfo);
   appendToLogfile("CheckPlaybackInfo: After call to TAP_Hdd_GetPlayInfo.");
   if ( iOverRunCheck != ApiOverRunCheck )  // TAP_Hdd_GetPlayInfo V1.22 has a buffer overrun issue, so check if it has overrun by more than 128 bytes.
   {
      appendToLogfile("CheckPlaybackInfo: Buffer overrun on TAP_Hdd_GetPlayInfo.");
      ShowMessageWin( rgn, "Surfer", "Error retrieving Playback Information", "TAP_Hdd_GetPlayInfo buffer overrun in 'CheckPlaybackInfo'.", 500 );
      return;
   }

   // Check the various playmodes to see if we are playing a file back.
   // PLAYMODE_None               Nothing playing
   // PLAYMODE_Playing            Playing a file
   // PLAYMODE_RecPlaying         Playing a currently recording file
   //
   appendIntToLogfile("CheckPlaybackInfo: After call to TAP_Hdd_GetPlayInfo, CurrentPlaybackInfo.playMode=%d",CurrentPlaybackInfo.playMode);
   if (CurrentPlaybackInfo.playMode == PLAYMODE_Playing) InPlaybackMode = TRUE; // Playback happening.
   else InPlaybackMode = FALSE;  // No playback happening.
   appendIntToLogfile("CheckPlaybackInfo: InPlaybackMode set to=%d",InPlaybackMode);
   
   if ( InPlaybackMode )   // We've activated the TAP with a current playing file.
   {                       // Record details of the current playing file
           appendToLogfile("CheckPlaybackInfo: In playbackmode, so setting Playback_event variable.");
           Playback_Lastfile = CurrentPlaybackInfo.file;
           strncpy( Playback_Lastfilename, Playback_Lastfile->name, TS_FILE_NAME_SIZE);
           len = strlen(Playback_Lastfile->name);
           appendStringToLogfile("CheckPlaybackInfo: Playback_Lastfile->name=%s",Playback_Lastfile->name);
           appendIntToLogfile("CheckPlaybackInfo: Playback_Lastfile->name length =%d ",len);
           
           // Store the current cluster of the file so that we can find it later (in case it is in a folder)
           Playback_StartCluster = Playback_Lastfile->startCluster;
           appendLongToLogfile("CheckPlaybackInfo: PlaybackStartCluster =%d ",Playback_StartCluster);
           
           // Associate event information with playback event.
           Playback_event = CurrentPlaybackInfo.evtInfo;  
           appendToLogfile("CheckPlaybackInfo: Playback_event set.");
           
           // Make the short description the Event Name + Description.
           len = strlen(Playback_event.eventName);
           appendIntToLogfile("CheckPlaybackInfo: Playback_event.eventName length =%d",len);
           appendStringToLogfile("CheckPlaybackInfo: Playback_event.eventName =%s<<<",Playback_event.eventName);
           len = strlen(Playback_event.description);
           appendIntToLogfile("CheckPlaybackInfo: Playback_event.description length =%d",len);
           appendStringToLogfile("CheckPlaybackInfo: Playback_event.description =%s<<<",Playback_event.description);
		   sprintf( combinedDesc, "%s %s", Playback_event.eventName, Playback_event.description);
           strncpy(Playback_event.description, combinedDesc, 128);  //**** event.description is 128.
           Playback_event.description[128]='\0';  // Terminate the description field.
           len = strlen(Playback_event.description);
           appendIntToLogfile("CheckPlaybackInfo: COMBINED Playback_event.description length =%d (SHOULD BE MAX OF 128)",len);
           appendStringToLogfile("CheckPlaybackInfo: COMBINED Playback_event.description set to %s<<<",Playback_event.description);


           // Save current position and total block size.
           Playback_TotalBlocks   = CurrentPlaybackInfo.totalBlock;
           Playback_SavedPosition = max(0,CurrentPlaybackInfo.currentBlock - 60) ;   // Save where we're up to with some overlap for restart, but make sure we don't go less than 0.
           appendLongToLogfile("CheckPlaybackInfo: Playback_TotalBlocks=%d",Playback_TotalBlocks);
           appendLongToLogfile("CheckPlaybackInfo: Playback_SavedPosition=%d",Playback_SavedPosition);

           // Make the EventName be the Filename
           strcpy(Playback_event.eventName, Playback_Lastfilename);  //**** eventName is 128.
           len = strlen(Playback_event.eventName);
           appendIntToLogfile("CheckPlaybackInfo: NEW Playback_event.eventName length =%d (SHOULD BE MAX OF 128)",len);
           appendStringToLogfile("CheckPlaybackInfo: NEW Playback_event.eventName =%s<<<",Playback_event.eventName);
           
           // Calculate the start date/time of the recording.
           startMjd  = Playback_Lastfile->mjd;
           startHour = Playback_Lastfile->hour;
           startMin  = Playback_Lastfile->min;
           durMin    = CurrentPlaybackInfo.duration;
           durMin    = CurrentPlaybackInfo.duration % 60;
           durHour   = CurrentPlaybackInfo.duration / 60;
           Playback_event.startTime = (startMjd<<16) | (startHour<<8) | (startMin);
           appendToLogfile("CheckPlaybackInfo: Playback_event.startTime set.");
           appendIntToLogfile("CheckPlaybackInfo: startMjd=%d ",startMjd);
           appendIntToLogfile("CheckPlaybackInfo: startHour=%d ",startHour);
           appendIntToLogfile("CheckPlaybackInfo: startMin=%d ",startMin);
           appendLongToLogfile("CheckPlaybackInfo: durMin=%d ",durMin);
           appendLongToLogfile("CheckPlaybackInfo: Playback_event.startTime=%d ",Playback_event.startTime);
           
           // Calculate the end date/time of the recording.
           endMjd  = startMjd;
           endHour = startHour;
           endMin  = startMin + durMin;

           endHour = endHour + durHour + (endMin / 60);
           endMin  = endMin % 60;

           endMjd  = endMjd + (endHour / 24);
           endHour = endHour % 24;
           
           Playback_event.endTime = (endMjd<<16) | (endHour<<8) | (endMin);
           appendToLogfile("CheckPlaybackInfo: Playback_event.endTime set.");
           appendIntToLogfile("CheckPlaybackInfo: endMjd=%d ",endMjd);
           appendIntToLogfile("CheckPlaybackInfo: endHour=%d ",endHour);
           appendIntToLogfile("CheckPlaybackInfo: endMin=%d ",endMin);
           
           // Store the duration details
           Playback_event.duration = (durHour<<8) | (durMin);
           appendLongToLogfile("CheckPlaybackInfo: Playback_event.duration set to=%d.",Playback_event.duration);
           
    }
   else   
   if ( strcmp(Playback_Lastfilename, NoPlayback_filename) == 0 )  // If we don't have a filename.
   {
          appendToLogfile("CheckPlaybackInfo: NOT in playbackmode and no lastfilename, so setting Playback_event variable to blank event.");
          Playback_event = BlankPlayback_event;         // Blank out the event details.
   }
   else
   {
          // We keep the Playback_event information from last time.
          // Could look at possibly retrieving from 1st file in directory....but that's for later on.
          appendToLogfile("CheckPlaybackInfo: NOT in playbackmode but previous lastfilename, so leave Playback_event variable as is.");
   }

   appendToLogfile("CheckPlaybackInfo: Ended.");
   

}     

void ClosePlaybackChannel( void)
{
    // Make sure that we don't leave pointing to the Playback Channel, so that restart is clean.
     
    if ((firstSvcNum == Playback_svcnum) &&
       (firstSvcType == SVC_TYPE_Tv))  //KH If we exited last time on the Playback Channel decrease pointer by 1.
    {
       firstSvcNum--;
    }       
    if ((currentSvcNum == Playback_svcnum) &&
       (currentSvcType == SVC_TYPE_Tv))  //KH If we are starting on the Playback Channel decrease pointer by 1.
    {
       currentSvcNum--;
    }       

    Playback_Selected = FALSE;
}    

void StartPlayback( char fn[ TS_FILE_NAME_SIZE ] , dword startCluster)
{   
        // Playback Service was selected, so start playback of last saved file.
        int result;
        char str[100];
        int mainType, mainNum;   // Temporary storage for the current service in case we need to switch back after a failed playback.

        static TYPE_PlayInfo PlaybackInfo;                      // Place to store info about current playback if invoked during playback mode.
        static char          sOverRun[ApiOverRunArray];         // Keep some space to protect against buffer overrun.
        static int           iOverRunCheck;                     // If buffer overrun is larger than sOverRun iOverRunCheck will be hit.
        
        if ( strcmp(fn, NoPlayback_filename) == 0)    // Check if we haven't had a Playback file before
        {
             MainChangeOnOK = TRUE; // Reset the exit on double OK flag, so we don't exit if they keep pressing OK after a failed play.
             return;
        }
        
        ChangeDirRoot();                // Change to the root directory.
        TAP_Hdd_ChangeDir("DataFiles");	// Let's go to the data file directory. To start playback.
        TAP_Channel_GetCurrent( &mainType, &mainNum );   // Store the current channel in case we need to switch back after a failed playback.

//        result = TAP_Hdd_PlayTs(fn);
        result = PlayTs(fn, startCluster);

        //Need to check playback success
        if (result != 0)
        {
	       TAP_Osd_FillBox( rgn, PIP_X, PIP_Y, PIP_W, PIP_H, COLOR_Black );		// hide the picture & clear any old overlay graphics
           TAP_Osd_PutGd(   rgn, PIP_X_GRAPHIC_OFFSET,    PIP_MSG_Y+4,    &_plainredbarGd, TRUE );	// display plain red bar in PiP area
           PrintCenter(     rgn, PIP_X_GRAPHIC_OFFSET+7,  PIP_MSG_Y+4+11, PIP_X_GRAPHIC_OFFSET+204, fn, MAIN_TEXT_COLOUR, 0 , FNT_Size_1419); // Print the name of the file

           TAP_SPrint(  str, "Return code = %i", result );
           PrintCenter( rgn, PIP_X_TEXT_OFFSET, PIP_MSG_Y+50, PIP_W,  "Playback failed.",  MAIN_TEXT_COLOUR, 0 , FNT_Size_1419);
           PrintCenter( rgn, PIP_X_TEXT_OFFSET, PIP_MSG_Y+70, PIP_W,   str,                MAIN_TEXT_COLOUR, 0 , FNT_Size_1419);

           TAP_SPrint(  str, "  Switching back to service %i.  ", mainNum );
           TAP_SPrint( gstr, "  Filename: %s  ", fn );
           ShowMessageWin( rgn, "Surfer Playback Failed.", str, gstr, 700 );
           Playback_SavedPosition = 0;  // Since file is invalid, reset Saved Position and TotalBlocks.
           Playback_TotalBlocks   = 1;  // Assume at least 1 block so that we don't get divide by zero problems.
       	   MainChangeOnOK = TRUE;  // Reset the exit on double OK flag, so we don't exit if they keep pressing OK after a failed play.
           InPlaybackMode = FALSE;
           
	       result = TAP_Channel_Start( CHANNEL_Main, mainType, mainNum );
        }
        else
        {
           iOverRunCheck = ApiOverRunCheck;       // Set flag to catch any buffer overrun from API call.
           TAP_Hdd_GetPlayInfo (&PlaybackInfo);   //Get the details of the playback that we just started.
           if ( iOverRunCheck != ApiOverRunCheck )  // TAP_Hdd_GetPlayInfo V1.22 has a buffer overrun issue, so check if it has overrun by more than 128 bytes.
           {
                ShowMessageWin( rgn, "Surfer Error.", "Error retrieving Playback Information", "TAP_Hdd_GetPlayInfo buffer overrun in 'StartPlayback'.", 500 );
                return;
           }
   	       if (( Playback_SavedPosition > PlaybackInfo.totalBlock ) || ( Playback_SavedPosition < 0 )) Playback_SavedPosition = 0; //check boundaries in case file has been cut.
           TAP_Hdd_ChangePlaybackPos( Playback_SavedPosition );
		   //TAP_GenerateEvent( EVT_KEY, RKEY_Play, 0 );
		   InPlaybackMode = TRUE;
	    }
return;
}

void SaveCurrentPlaybackPosition()
{    // Before we change channels, check if we have a currently running playback.
     // If so, then save the current block position so that we can restart at that position.

   // Check for Playbackmode
   iOverRunCheck = ApiOverRunCheck;   // Set flag to catch any buffer overrun from API call.
   TAP_Hdd_GetPlayInfo (&CurrentPlaybackInfo);
   if ( iOverRunCheck != ApiOverRunCheck )  // TAP_Hdd_GetPlayInfo V1.22 has a buffer overrun issue, so check if it has overrun by more than 128 bytes.
   {
      ShowMessageWin( rgn, "Surfer Error.", "Error retrieving Playback Information", "TAP_Hdd_GetPlayInfo buffer overrun in 'SaveCurrentPlaybackPosition'.", 500 );
      return;
   }

   //if ((PlayInfo.playMode == PLAYMODE_Playing)|| (tMyPlayInfo.playMode == PLAYMODE_RecPlaying)) // Playback of recording
   // Check the various playmodes to see if we are playing a file back.
   // PLAYMODE_None               Nothing playing
   // PLAYMODE_Playing            Playing a file
   // PLAYMODE_RecPlaying         Playing a currently recording file
   //
   if (CurrentPlaybackInfo.playMode == PLAYMODE_Playing) // We're in playback mode
   {
       Playback_SavedPosition = max(0,CurrentPlaybackInfo.currentBlock-60);   //Save the current position of playback less 60 blocks so we have some overlap when we restart.  
                                                                              // (Make sure we don't got below 0.)
   }


}
