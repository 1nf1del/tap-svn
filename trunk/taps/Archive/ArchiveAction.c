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
    appendToLogfile("DeletePlayedFileEntry: Started.", INFO);
    appendIntToLogfile("DeletePlayedFileEntry: At start, entry=%d",entry, INFO);
    appendIntToLogfile("DeletePlayedFileEntry: At start, numberOfPlayedFiles=%d",numberOfPlayedFiles, INFO);

    TAP_MemFree( playedFiles[i] );   // Clear the allocated memory for this entry as we will reassign it in the shuffle.
    for (i=entry; i <= numberOfPlayedFiles-1; i += 1)
    {
        playedFiles[i] = playedFiles[i+1];  // Shuffle everything down one in the list.
    }    
    
    numberOfPlayedFiles--;                                                             // Decrease the count of the entries.
    
    appendIntToLogfile("DeletePlayedFileEntry: At end, numberOfPlayedFiles=%d",numberOfPlayedFiles, INFO);
    appendToLogfile("DeletePlayedFileEntry: Finished.", INFO);
}
 

//------------
//
void DeleteProgressInfo(int dirNumbr, int index, bool message)
{
    int i;
    bool match;
    char str[TS_FILE_NAME_SIZE];
    
    appendToLogfile("DeleteProgressInfo: Started.", INFO);
    
    match = FALSE;

    // Check if the deleted file was set as the last playback file.  If so, remove the last playback information.
    if ((!message) && (myfiles[dirNumbr][index]->startCluster == playedFiles[0]->startCluster) && (strcmp(myfiles[dirNumbr][index]->name,playedFiles[0]->name)==0))
    {
        appendToLogfile("DeleteProgressInfo: Last playback info matched.  Now deleting.", WARNING);
        memset(playedFiles[0],0,sizeof(*playedFiles[0]));   // Clear out the lastplayback entry.
        playedFiles[0]->startCluster = 0;
        playedFiles[0]->totalBlock   = 1;  // Set to 1 to ensure no division by 0 errors.
        match = TRUE;
    }
        
                                  
    
    for (i=1; i <= numberOfPlayedFiles; i += 1)
    {
        // If the file has the same filename and the same disk startcluster we consider it a match.
        if ((myfiles[dirNumbr][index]->startCluster == playedFiles[i]->startCluster) && (strcmp(myfiles[dirNumbr][index]->name,playedFiles[i]->name)==0))
        {
              myfiles[dirNumbr][index]->hasPlayed = FALSE;  // Reset indicator to show that file has not been played.
              myfiles[dirNumbr][index]->currentBlock = 0;
              myfiles[dirNumbr][index]->totalBlock   = 1;   // Set to 1 to ensure no division by 0 errors.
              appendIntToLogfile("DeleteProgressInfo: Found match at i==%d",i, WARNING);
              DeletePlayedFileEntry(i);
              match = TRUE;
              break; // We've found a match, so don't bother checking other playback entries.
        }              
    }   

    TAP_SPrint(str,myfiles[dirNumbr][index]->name);
    if (match)
    {
        SaveDatToFile();   // Write the updated data to disk.
        if (message) ShowMessageWin( rgn, "Progress Info Cleared.", "Removed playback info for:", str, 400 );
    }
    else
        if (message) ShowMessageWin( rgn, "No Progress Info to Clear.", "No playback info found for:", str, 400 );

            
    appendToLogfile("DeleteProgressInfo: Finished.", INFO);
}


//------------
//
void CreateNewFolder(void)
{
     int grp, result;
     char grpName[12];
     char newDir[ MAX_FULL_DIR_NAME_LENGTH ];
     
     for (grp = 1; grp <= 99; grp += 1)
     {
        TAP_SPrint(grpName,"FOLDER %02d",grp);
        if (!TAP_Hdd_Exist(grpName)) break;
     }   
     
     if (grp == 100) // Then we couldn't find any spare "GROUP xx" folder names.
     {
         ShowMessageWin( rgn, "Folder Create Failed.", "All 'FOLDER xx' folders between", "'FOLDER 01' and 'FOLDER 99' already exist.", 400 );
         return;
     }    
     TAP_Hdd_Create(grpName, ATTR_FOLDER);
     strcpy(blankFile.name, grpName);
     blankFile.attr = ATTR_FOLDER;
     myfolders[CurrentDirNumber]->numberOfFiles++;    // Increase local file/folder count for the number of subfolders (count starts at 0).
     myfolders[CurrentDirNumber]->numberOfFolders++;  // Increase local folder count for the number of subfolders (count starts at 0).
     numberOfFolders++;                               // Increase global count for number of folders (/Datafiles is #0).
     
     AddNewFolder(CurrentDir, CurrentDirNumber, myfolders[CurrentDirNumber]->numberOfFiles, blankFile, numberOfFolders);     // Create subfolder entry in the "myfiles" array.

     // Create new parent directory entry.
     strcpy(newDir, CurrentDir);
     strcat(newDir, "/");
     strcat(newDir, grpName);

     AddNewParentFolder(newDir, numberOfFolders, 1, CurrentDirNumber); 
     
     // Save information on file and sub-folder counts for new directory.
     myfolders[numberOfFolders]->numberOfFiles      = 1;
     myfolders[numberOfFolders]->numberOfFolders    = 0;
     myfolders[numberOfFolders]->numberOfRecordings = 0;
     myfolders[numberOfFolders]->parentDirNumber    = CurrentDirNumber;
     myfolders[numberOfFolders]->directoryNumber    = numberOfFolders;
     
     maxShown         = myfolders[CurrentDirNumber]->numberOfFiles;
     numberOfFiles    = myfolders[CurrentDirNumber]->numberOfFiles;
}
             

//------------
//
void ChangeToParentDir()
{
     char subFolder[TS_FILE_NAME_SIZE];
     int i;
     
     appendToLogfile("ChangeToParentDir: Started.", INFO);

     strcpy(subFolder,myfolders[CurrentDirNumber]->name);  // Extract the current qunquailified directory name from the fully qualified directory name.
     TAP_Hdd_ChangeDir("..");                    // Switch to the parent directory.

     CurrentDirNumber = myfolders[CurrentDirNumber]->parentDirNumber;  // Retrieve the new directory number
     maxShown         = myfolders[CurrentDirNumber]->numberOfFiles;
     numberOfFiles    = myfolders[CurrentDirNumber]->numberOfFiles;

     if (CurrentDirNumber == 0) strcpy(CurrentDir, "/DataFiles");     // We're pointing at the DataFiles directory
     else strcpy(CurrentDir, myfiles[CurrentDirNumber][1]->directory); // else get the full directory name from the first file/folder.

/*
     if (recursiveLoadOption)
     {     
        SetDirFilesToNotPresent(CurrentDirNumber);                      // Flag all of the files/folders in our myfiles list as not present.
 	    LoadArchiveInfo(CurrentDir, CurrentDirNumber, myfolders[CurrentDirNumber]->parentDirNumber, FALSE);            // Check all of the files/folders again to see if there are any new files/folders.
        DeleteDirFilesNotPresent(CurrentDirNumber);     // Delete any of the files/folders that are no longer on the disk.
        LoadPlaybackStatusInfo();  // Update 'myfiles' entries with latest playback information.
     }   

        SetDirFilesToNotPresent(CurrentDirNumber);                      // Flag all of the files/folders in our myfiles list as not present.
 	    LoadArchiveInfo(CurrentDir, CurrentDirNumber, myfolders[CurrentDirNumber]->parentDirNumber, 1);    // Check all of the files/folders again to see if there are any new files/folders.
        DeleteDirFilesNotPresent(CurrentDirNumber);     // Delete any of the files/folders that are no longer on the disk.
        LoadPlaybackStatusInfo();  // Update 'myfiles' entries with latest playback information.
 */    
     loadSubsequentArchiveInfo(FALSE, 0);    
 
     chosenLine = 1;                             // By default select the first line to highlight.
     // Scan through the FOLDERS in the current directory to see if we have a match to the folder that we came from.
     for (i=1; i<=numberOfFiles; i++)
     {
          if ((myfiles[CurrentDirNumber][i]->attr == ATTR_FOLDER) && (strncmp(myfiles[CurrentDirNumber][i]->name,subFolder, TS_FILE_NAME_SIZE)==0))
          {   // We've found a match, so allocate line number.
              chosenLine = i;
              break;
          }
     }
     DeterminePrintingLine(chosenLine);     // Determine where we will start printing this line.
	 RefreshArchiveList( TRUE );
     appendToLogfile("ChangeToParentDir: Finished.", INFO);
}	 


//------------
//
void JumpToLastPosition(dword currentBlock, dword totalBlock)
{
     signed int lastPos;
     signed int curPercent;
     
     appendIntToLogfile("JumpToLastPosition: currentBlock=%d", currentBlock, WARNING);
     appendIntToLogfile("JumpToLastPosition: totalBlock=%d", totalBlock, WARNING);

     if (currentBlock > totalBlock) currentBlock = 0;  // Check there's no invalid block data.
     if (currentBlock < 0)          currentBlock = 0;  // Check there's no invalid block data.
     if (totalBlock < 1)            totalBlock   = 1;  // Check there's no invalid block data.
     
     curPercent  = (( currentBlock * 100) / totalBlock );

     if (curPercent < 95) // If we haven't watched more than 95%, jump to the last position.
     {     
           // Jump back a further 60 blocks so we have some overlap from where we left off.
           lastPos = currentBlock-60;
           lastPos = max(0, lastPos);
           
     }
     else  // Start from the start.
           lastPos = 0;
     
     appendIntToLogfile("JumpToLastPosition: jumping to lastPos=%d", lastPos, WARNING);
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
                 if (inPlaybackMode) // Stop the current playback to allow us to start a new playback file.
                      TAP_Hdd_StopTs();  
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
     
     ChangeDirRoot();                // Change to the root directory.
     TAP_Hdd_ChangeDir("DataFiles");	// Let's go to the data file directory. To start playback.
     TAP_Channel_GetCurrent( &mainType, &mainNum );   // Store the current channel in case we need to switch back after a failed playback.

     result = PlayTs(playedFiles[0]->name, playedFiles[0]->startCluster);

     if (result != 0)
     { 
        sprintf(str,"Return code #%d when trying to play",result);
        sprintf(str2,"file: %s",playedFiles[0]->name);
        ShowMessageWin( rgn, "Restarting Last Playback Failed.", str, str2, 400 );
        GotoPath( CurrentDir );  // Return to the current directory.
        return result;
     }
  
     // Jump back to the last watched position - or to the start if >95% watched.
     JumpToLastPosition(playedFiles[0]->currentBlock, playedFiles[0]->totalBlock);

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
  
     if ((jump == 1) && (myfiles[CurrentDirNumber][chosenLine]->hasPlayed))  // If we want to resume playback, and the file has been previously played.
     {
        JumpToLastPosition(myfiles[CurrentDirNumber][chosenLine]->currentBlock, myfiles[CurrentDirNumber][chosenLine]->totalBlock);
     }
     return 0;
}     


//------------
//
void RestartPlayback(int line, int jump)
{
    if (myfiles[CurrentDirNumber][line]->isRecording) // If this is an active recording, then change to the channel first and jump back to start.
    {
        TAP_Channel_Start(1, myfiles[CurrentDirNumber][line]->svcType, myfiles[CurrentDirNumber][line]->svcNum);
        TAP_Hdd_ChangePlaybackPos( 0 );                      
	    exitFlag = TRUE;						// signal exit to top level - will clean up, close window,                      
    }
    else
    {
        if (StartPlayback(myfiles[CurrentDirNumber][line]->name, jump)==0) // If playback starts OK.
	       exitFlag = TRUE;						// signal exit to top level - will clean up, close window,                      
    }
}



//------------
//
void ArchiveAction (int line)
{
    char newDir[ MAX_FULL_DIR_NAME_LENGTH ];
    
    switch (myfiles[CurrentDirNumber][line]->attr)
    {
           case 250:
           case 240:  // Parent directory.
          	          ChangeToParentDir();
          	          break;
          	          
           case ATTR_FOLDER:  // Sub folder
                      appendStringToLogfile("ArchiveAction: Calling TAP_Hdd_ChangeDir to %s<.",myfiles[CurrentDirNumber][line]->name, WARNING);
          	          TAP_Hdd_ChangeDir(myfiles[CurrentDirNumber][line]->name);
                      TAP_SPrint(newDir, "%s/%s",CurrentDir,myfiles[CurrentDirNumber][line]->name);
                      
                      appendStringToLogfile("ArchiveAction: Copying newDir.  newDir=%s<.",newDir, WARNING);
                      strcpy(CurrentDir, newDir);
                      
                      appendStringToLogfile("ArchiveAction: Copied to CurrentDir.  CurrentDir=%s<.",CurrentDir, WARNING);
                      // Change the CurrentDirNumber to be the new directory number.
                      CurrentDirNumber = myfiles[CurrentDirNumber][line]->directoryNumber;
                      
	                  maxShown         = myfolders[CurrentDirNumber]->numberOfFiles;
	                  numberOfFiles    = myfolders[CurrentDirNumber]->numberOfFiles;

                      appendIntToLogfile("ArchiveAction: CurrentDirNumber=%d<.",CurrentDirNumber, WARNING);
                      appendIntToLogfile("ArchiveAction: numberOfFiles=%d<.",numberOfFiles, WARNING);

                      appendIntToLogfile("ArchiveAction: Calling SetDirFilesToNotPresent CurrentDirNumber=%d",CurrentDirNumber, WARNING);
                      SetDirFilesToNotPresent(CurrentDirNumber);                      // Flag all of the files/folders in our myfiles list as not present.
                         
                      appendIntToLogfile("ArchiveAction: Calling LoadArchiveInfo parentDirNumber=%d.",myfolders[CurrentDirNumber]->parentDirNumber, WARNING);
 	                  LoadArchiveInfo(CurrentDir, CurrentDirNumber, myfolders[CurrentDirNumber]->parentDirNumber, 1);            // Check all of the files/folders again to see if there are any new files/folders.
                         
                      appendIntToLogfile("ArchiveAction: Calling DeleteDirFilesNotPresent with CurrentDirNumber=%d<.",CurrentDirNumber, WARNING);
                      DeleteDirFilesNotPresent(CurrentDirNumber);     // Delete any of the files/folders that are no longer on the disk.
                      // Reload the numberOfFiles/maxshown variables incase any files/folders were deleted.
   	                  maxShown         = myfolders[CurrentDirNumber]->numberOfFiles;
	                  numberOfFiles    = myfolders[CurrentDirNumber]->numberOfFiles;
                      appendIntToLogfile("ArchiveAction: numberOfFiles=%d<.",numberOfFiles, WARNING);

                      appendToLogfile("ArchiveAction: Calling LoadPlaybackStatusInfo.", WARNING);
	                  LoadPlaybackStatusInfo();  // Update 'myfiles' entries with latest playback information.
	                  chosenLine = 1;
	                  printLine = 1;

                      appendToLogfile("ArchiveAction: Calling RefreshArchiveList.", WARNING);
                      RefreshArchiveList(TRUE);
          	          break;
          	          
           case ATTR_TS:  // Recorded program - resume at last location.
                      RestartPlayback( line, 1); 
                      break;
    }

}




