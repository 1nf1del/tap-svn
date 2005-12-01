/************************************************************
			Part of the ukEPG project
		This module performs actions against the archive list

Name	: ArchiveAction.c
Author	: kidhazy
Version	: 0.1
For		: Topfield TF5x00 series PVRs
Licence	:
Descr.	:
Usage	:
History	: v0.0 

	Last change:  USE   1 Aug 105    8:34 pm
**************************************************************/


#define X1	130
#define X2	300
#define E0	113
#define E1	275
#define E2	627
#define Y	57
#define Y2	514
#define Y_STEP 30

static bool  fileReset;

typedef struct _DIR_LIST
{
     struct _DIR_LIST* Next;
     char name[TS_FILE_NAME_SIZE];
}
DIR_LIST;


//------------
//
void DeletePlayedFileEntry(int entry)
{
    int i;
    appendToLogfile("DeletePlayedFileEntry: Started.");
    appendIntToLogfile("DeletePlayedFileEntry: At start, entry=%d",entry);
    appendIntToLogfile("DeletePlayedFileEntry: At start, numberOfPlayedFiles=%d",numberOfPlayedFiles);

    for (i=entry; i <= numberOfPlayedFiles-1; i += 1)
    {
        playedFiles[i] = playedFiles[i+1];  // Shuffle everything down one in the list.
    }    
    
    memset(&playedFiles[numberOfPlayedFiles],0,sizeof(playedFiles[numberOfPlayedFiles]));   // Clear out the last entry.
    numberOfPlayedFiles--;                                                             // Decrease the count of the entries.
    
    appendIntToLogfile("DeletePlayedFileEntry: At end, numberOfPlayedFiles=%d",numberOfPlayedFiles);
    appendToLogfile("DeletePlayedFileEntry: Finished.");
}
 

//------------
//
void DeleteProgressInfo(bool message)
{
    int i;
    bool match;
    char str[TS_FILE_NAME_SIZE];
    
    appendToLogfile("DeleteProgressInfo: Started.");
    
    match = FALSE;

    // Check if the deleted file was set as the last playback file.  If so, remove the last playback information.
    if ((!message) && (myfiles[0][chosenLine].startCluster == playedFiles[0].startCluster) && (strcmp(myfiles[CurrentDirNumber][chosenLine].name,playedFiles[0].name)==0))
    {
        appendToLogfile("DeleteProgressInfo: Last playback info matched.  Now deleting.");
        memset(&playedFiles[0],0,sizeof(playedFiles[0]));   // Clear out the lastplayback entry.
        playedFiles[0].startCluster = 0;
        match = TRUE;
    }
        
                                  
    
    for (i=1; i <= numberOfPlayedFiles; i += 1)
    {
        // If the file has the same filename and the same disk startcluster we consider it a match.
        if ((myfiles[CurrentDirNumber][chosenLine].startCluster == playedFiles[i].startCluster) && (strcmp(myfiles[CurrentDirNumber][chosenLine].name,playedFiles[i].name)==0))
        {
              myfiles[CurrentDirNumber][chosenLine].hasPlayed = FALSE;  // Reset indicator to show that file has not been played.
              myfiles[CurrentDirNumber][chosenLine].currentBlock = 0;
              myfiles[CurrentDirNumber][chosenLine].totalBlock = 0;
              appendIntToLogfile("DeleteProgressInfo: Found match at i==%d",i);
              DeletePlayedFileEntry(i);
              match = TRUE;
              break; // We've found a match, so don't bother checking other playback entries.
        }              
    }   

    TAP_SPrint(str,myfiles[CurrentDirNumber][chosenLine].name);
    if (match)
    {
        SaveDatToFile();   // Write the updated data to disk.
        if (message) ShowMessageWin( rgn, "Progress Info Cleared.", "Removed playback info for:", str, 400 );
    }
    else
        if (message) ShowMessageWin( rgn, "No Progress Info to Clear.", "No playback info found for:", str, 400 );

            
    appendToLogfile("DeleteProgressInfo: Finished.");
}




//------------
//
void CreateNewFolder(void)
{
     int grp, result;
     char grpName[10];
     
     for (grp = 1; grp <= 99; grp += 1)
     {
        TAP_SPrint(grpName,"FOLDER %02d",grp);
        result = TAP_Hdd_Exist(grpName);
        if (!TAP_Hdd_Exist(grpName)) break;
     }   
     
     if (grp == 100) // Then we couldn't find any spare "GROUP xx" folder names.
     {
         ShowMessageWin( rgn, "Folder Create Failed.", "All 'FOLDER xx' folders between", "'FOLDER 01' and 'FOLDER 99' already exist.", 400 );
         return;
     }    
     
     TAP_Hdd_Create(grpName, ATTR_FOLDER);
}
             

//------------
//
void ChangeToParentDir()
{
     char subFolder[TS_FILE_NAME_SIZE];
     int i;
     
     appendToLogfile("ChangeToParentDir: Started.");

     ExtractLastField ( myfolders[CurrentDirNumber].name, subFolder);  // Extract the current qunquailified directory name from the fully qualified directory name.
     TAP_Hdd_ChangeDir("..");                    // Switch to the parent directory.
     CurrentDir = GetCurrentDir();               // Retrieve the new current directory name.
TAP_Print("before cd=%d\r\n",CurrentDirNumber);
     CurrentDirNumber = myfolders[CurrentDirNumber].parentDirNumber;
TAP_Print("after cd=%d\r\n",CurrentDirNumber);
     maxShown         = myfolders[CurrentDirNumber].numberOfFiles;
     numberOfFiles    = myfolders[CurrentDirNumber].numberOfFiles;
//     LoadArchiveInfo();                          // Load the file/folder info for the new directory.
	 SortList(sortOrder);		                 // Sort the list.
	 
     chosenLine = 1;                             // By default select the first line to highlight.
     // Scan through the FOLDERS in the current directory to see if we have a match to the folder that we came from.
     for (i=1; i<=numberOfFiles; i++)
     {
TAP_Print("i=%d n=%s< sf=%s< \r\n", i, myfiles[CurrentDirNumber][i].name, subFolder);
          if ((myfiles[CurrentDirNumber][i].attr == ATTR_FOLDER) && (strncmp(myfiles[CurrentDirNumber][i].name,subFolder, TS_FILE_NAME_SIZE)==0))
          {   // We've found a match, so allocate line number.
              chosenLine = i;
              break;
          }
     }
TAP_Print("i=%d\r\n",i);     
     DeterminePrintingLine(chosenLine);     // Determine where we will start printing this line.
     
	 RefreshArchiveList( TRUE );
     appendToLogfile("ChangeToParentDir: Finished.");
}	 


//------------
//
void JumpToLastPosition(dword currentBlock, dword totalBlock)
{
     dword lastPos;
     dword   curPercent;

     if (currentBlock > totalBlock) currentBlock = 0;  // Check there's no invalid block data.
     
     curPercent  = (( max(0,currentBlock) * 100) / max(1,totalBlock) );
     
     if (curPercent < 95) // If we haven't watched more than 95%, jump to the last position.
     {     
           // Jump back a further 60 blocks so we have some overlap from where we left off.
           lastPos = max(0, currentBlock-60);
     }
     else  // Start from the start.
           lastPos = 0;
     
     TAP_Hdd_ChangePlaybackPos( lastPos );
}



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



//------------
//
int RestartLastPlayback(void)
{
     int result;
     char str[80], str2[TS_FILE_NAME_SIZE + 10];
     int mainType, mainNum;   // Temporary storage for the current service in case we need to switch back after a failed playback.
     dword lastPos;
     
     CurrentDir = GetCurrentDir();
     
     ChangeDirRoot();                // Change to the root directory.
     TAP_Hdd_ChangeDir("DataFiles");	// Let's go to the data file directory. To start playback.
     TAP_Channel_GetCurrent( &mainType, &mainNum );   // Store the current channel in case we need to switch back after a failed playback.

     result = PlayTs(playedFiles[0].name, playedFiles[0].startCluster);

     if (result != 0)
     { 
        sprintf(str,"Return code #%d when trying to play",result);
        sprintf(str2,"file: %s",playedFiles[0].name);
        ShowMessageWin( rgn, "Restarting Last Playback Failed.", str, str2, 400 );
        GotoPath( CurrentDir );  // Return to the current directory.
//      TAP_Channel_Start(1, mainType, mainNum);  // Switch back to current channel.
        return result;
     }
     else  // Set the current directory to where the playback file was.
        CurrentDir = GetCurrentDir();
  
     // Jump back to the last watched position - or to the start if >95% watched.
     JumpToLastPosition(playedFiles[0].currentBlock, playedFiles[0].totalBlock);

     return 0;
}     


//------------
//
int StartPlayback(char filename[TS_FILE_NAME_SIZE], int jump)
{
     int result;
     char str[80], str2[300];
     
     result = TAP_Hdd_PlayTs(filename);
     if (result != 0)
     { 
        sprintf(str,"Return code #%d when trying to play",result);
        sprintf(str2,"file: %s",filename);
        ShowMessageWin( rgn, "Starting Playback Failed.", str, str2, 400 );
        return result;
     }
  
     if ((jump == 1) && (myfiles[CurrentDirNumber][chosenLine].hasPlayed))  // If we want to resume playback, and the file has been previously played.
        JumpToLastPosition(myfiles[CurrentDirNumber][chosenLine].currentBlock, myfiles[CurrentDirNumber][chosenLine].totalBlock);
     
     return 0;
}     


//------------
//
void RestartPlayback(int line, int jump)
{
    if (myfiles[CurrentDirNumber][line].isRecording) // If this is an active recording, then change to the channel first and jump back to start.
    {
        TAP_Channel_Start(1, myfiles[CurrentDirNumber][line].svcType, myfiles[CurrentDirNumber][line].svcNum);
        // GotoDataFiles();
        TAP_Hdd_ChangePlaybackPos( 0 );                      
	    exitFlag = TRUE;						// signal exit to top level - will clean up, close window,                      
    }
    else
    {
        if (StartPlayback(myfiles[CurrentDirNumber][line].name, jump)==0) // If playback starts OK.
	       exitFlag = TRUE;						// signal exit to top level - will clean up, close window,                      
    }
}



//------------
//
void ArchiveAction (int line)
{

    switch (myfiles[CurrentDirNumber][line].attr)
    {
           case 250:
           case 240:  // Parent directory.
          	          ChangeToParentDir();
          	          break;
          	          
           case ATTR_FOLDER:  // Sub folder
          	          TAP_Hdd_ChangeDir(myfiles[CurrentDirNumber][line].name);
                      CurrentDir = GetCurrentDir();           // Stores the current directory name.
                      CurrentDirNumber = myfiles[CurrentDirNumber][line].directoryNumber;
//                      LoadArchiveInfo();
	                  SortList(sortOrder);		
	                  maxShown      = myfolders[CurrentDirNumber].numberOfFiles;
	                  numberOfFiles = myfolders[CurrentDirNumber].numberOfFiles;
	                  chosenLine = 1;
	                  printLine = 1;
                      RefreshArchiveList(TRUE);
          	          break;
          	          
           case ATTR_TS:  // Recorded program - resume at last location.
                      RestartPlayback( line, 1); 
                      break;
    }

}




