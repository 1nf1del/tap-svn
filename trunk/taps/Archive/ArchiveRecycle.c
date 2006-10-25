/************************************************************
			Part of the ukEPG project
		This module deletes the archive entries

Name	: ArchiveRecycle.c
Author	: kidhazy
Version	: 0.1
For		: Topfield TF5x00 series PVRs
Licence	:
Descr.	:
Usage	:
History	: v0.0 kidhazy 14-07-06  Inception date

**************************************************************/

static bool  returnFromRecycleWindow;       // Flag to indicate when we have returned from displaying the Recycle window.                 
static bool  returnFromDelete;              // Flag to indicate when we have returned from deleting a file.                
static bool  fileDeleted;                   // Flag to indicate when we have deleted or recycled a file.
static bool  fileRestored;                  // Flag to indicate when we restored a file.
static char  recycleCommandOption;          // Index of the option selected on the Recycle window.
static byte* recycleBinWindowCopy;          // Pointer to in memory copy of the Recycle window.


// Position of the Recycle Bin Screen
#define RECYCLE_WINDOW_W 476
#define RECYCLE_WINDOW_H 416
#define RECYCLE_WINDOW_X ((MAX_SCREEN_X-RECYCLE_WINDOW_W)/2)      // Centre the recycle window
#define RECYCLE_WINDOW_Y ((MAX_SCREEN_Y-RECYCLE_WINDOW_H)/2-10)   // Centre the recycle window

#define RECYCLE_OPTION_W        82                                // Width  of recycle Options Buttons
#define RECYCLE_OPTION_H        43                                // Height of recycle Options Buttons
#define RECYCLE_OPTION_X        (RECYCLE_WINDOW_X + 30)           // Starting x-position for first recycle button  82
#define RECYCLE_OPTION_Y        (RECYCLE_WINDOW_Y + RECYCLE_WINDOW_H - RECYCLE_OPTION_H - 15)             // Starting y-position for Option buttons.
#define RECYCLE_OPTION_X_SPACE  (RECYCLE_OPTION_W+3)              // Space between options on recycle window.

#define RECYCLE_COMMAND_OPTIONS 5  // Number of command buttons in the recycle window.
#define RECYCLE_OK_OPTION 1        // Define the option number that is the OK option.

//------------------------------------------------------------------
//
void DisplayArchiveRecycleHelp( void )
{
    char str[200];
    char key[20][15], help[20][30];
    
    int i, l;

	archiveHelpWindowShowing = TRUE;
    
	// Store the currently displayed screen area where we're about to put our pop-up window on.
    archiveHelpWindowCopy = TAP_Osd_SaveBox(rgn, ARCHIVE_HELP_BASE_X, ARCHIVE_HELP_BASE_Y, ARCHIVE_HELP_WIDTH, ARCHIVE_HELP_HEIGHT);

    // Display the pop-up window.
    if ( unitModelType==TF5800t) // Display the UK style remote
       TAP_Osd_PutGd( rgn, ARCHIVE_HELP_BASE_X, ARCHIVE_HELP_BASE_Y, &_archive_recycle_help_ukGd, TRUE );
    else  
       TAP_Osd_PutGd( rgn, ARCHIVE_HELP_BASE_X, ARCHIVE_HELP_BASE_Y, &_archive_recycle_help_ozGd, TRUE );

}




void DeleteDirRecycleFiles(char* directory, bool recursive)
{
    // Deletes all Recycled Files in the current directory, and if set, does a recursive call to delete files in the subdirectories
    int cnt, i, numberOfDirFolders;
    TYPE_Dir_List subfolders[MAX_DIRS];
    char subdir[1024];
    
    cnt = TAP_Hdd_FindFirst(&file); 

    appendStringToLogfile("DeleteDirRecycleFiles: Started for directory=%s<.", directory, WARNING);
    appendIntToLogfile("DeleteDirRecycleFiles: Count=%d",cnt, WARNING);
    
    numberOfDirFolders = 0;
    
    for ( i=1; i <= cnt ; i += 1 )
	{
          if (IsFileRecycledRec(file.name, file.attr))
          {
               TAP_Hdd_Delete(file.name);
               // Check if the delete was successful.
               #ifdef WIN32  // If testing on Windows platform, assume success rather than physically deleting file.
               if (FALSE)
               #else  
               if (TAP_Hdd_Exist(file.name))  // Delete didn't work
               #endif
               {
                     ShowMessageWin( rgn, "Archive Recycle Bin", "Recycle Bin Cleanout Failed to Delete:", file.name, 400 );
               }
          }            
          //
          // Sub FOLDER
          // We need to keep track of subdirectories so we cna do a recursive delete if we're asked to.
          //
          if (file.attr == ATTR_FOLDER)    // It's a subfolder, so store it's name for checking later.
          {
             numberOfDirFolders++;         // Increase local directory count for the number of subfolders (count starts at 0).
             // Add the name of this folder to the list of files to recursively check at the end.  Index 1 is the first subfolder to check.
             strcpy( subfolders[numberOfDirFolders].name, file.name );
          }
             
          TAP_Hdd_FindNext (&file);
    }
   
    if (recursive)       // If we have asked to recursively load files, call the additional subfolders.
    { 
	   for ( i=1; i<= numberOfDirFolders; i++)
	   {
          if (strcmp(subfolders[i].name, "..")==0) break;   // Don't do a recursive on a parent directory.
          // Create the full directory name of the subfolder.
          subdir[0]='\0';                         // Blank out existing subdir.
          strcpy(subdir, directory);              // Add in our starting directory.
          strcat(subdir,"/");                     // Add a slash.
          strcat(subdir, subfolders[i].name);     // Add the subfolder name.
          // Change directory to the subfolder.
          TAP_Hdd_ChangeDir(subfolders[i].name);  // Change to the sub directory.
          // Recursively call the LoadArchiveInfo to read/load the information for te subfolder.
          DeleteDirRecycleFiles(subdir, TRUE);          
          // Go back to our starting directory, ready for anymore sub directories.
          GotoPath(directory);              
       }
    }   

    appendToLogfile("DeleteDirRecycleFiles: Finished.", INFO);
}


//-----------------------------------------------------------------------
//
void ReturnFromEmptyBinYesNo( bool result)
{
    // Routine if invoked upon return from the "Delete File" confirmation prompt.
     
    char str1[200], str2[200];

    // Check the result of the confirmation panel to decide what to do.
	switch (result)
    {
           case TRUE:  // YES
                       ShowMessageWinTemp( rgn, "Archive Recycle Bin", "Emptying Recycle Bin", "Please wait ...");           
                       DeleteDirRecycleFiles(CurrentDir, TRUE);
                       RestoreMessageWinTemp(rgn);
                       ShowMessageWin( rgn, "Archive Recycle Bin", "Recycle Bin Emptied.", " ", 300 );
                       returnFromRecycleBinEmpty = TRUE;         // Set flag to cause reload and redraw of Recycle Bin info after we leave the main menu.
                       break;
                      
	       case FALSE: // NO
                       break;	
    }   
}


//-----------------------------------------------------------------------
//
void ReturnFromDirRecycleBinYesNo( bool result)
{
    // Routine if invoked upon return from the "Delete Directory Recycled Files" confirmation prompt.
     
    char str1[200], str2[200];

    // Check the result of the confirmation panel to decide what to do.
	switch (result)
    {
           case TRUE:  // YES
                       DeleteDirRecycleFiles(CurrentDir, FALSE); // Delete only current directory files
                       ShowMessageWin( rgn, "Archive Recycle Bin", "Directory Recycle Bin Emptied.", " ", 300 );
                       returnFromRecycleBinWindowEmpty = TRUE;         // Set flag to cause reload and redraw of Recycle Bin.
                       break;
                      
	       case FALSE: // NO
                       break;	
    }   
}


//-----------------------------------------------------------------------
//
void ReturnFromAllRecycleBinYesNo( bool result)
{
    // Routine if invoked upon return from the "Delete All Recycled Files" confirmation prompt.
     
    char str1[200], str2[200];

    // Check the result of the confirmation panel to decide what to do.
	switch (result)
    {
           case TRUE:  // YES
                       ShowMessageWinTemp( rgn, "Archive Recycle Bin", "Emptying Recycle Bin", "Please wait ...");           
                       GotoDataFiles();                                // Change directory to the root /DataFiles directory.
                       DeleteDirRecycleFiles("/DataFiles", TRUE);   // Recursively delete all files.
                       RestoreMessageWinTemp(rgn);
                       GotoPath(CurrentDir);                           // Change directory back to the directory that we were last in.
                       ShowMessageWin( rgn, "Archive Recycle Bin", "Recycle Bin Emptied.", " ", 300 );
                       returnFromRecycleBinWindowEmpty = TRUE;         // Set flag to cause reload and redraw of Recycle Bin.
                       break;
                      
	       case FALSE: // NO
                       break;	
    }   
}



//------------
//
void EmptyRecycleBin(void)
{
    // Routine to scan entire harddrive and delete all of the recycle bin files.
    returnFromRecycleBinWindowEmpty = FALSE;         // Clear flag to cause reload and redraw of Recycle Bin.
    
    DisplayYesNoWindow("Empty Recycle Bin", "This will delete ALL files in the Recycle Bin", "Are you sure?", "Yes", "No", 1, &ReturnFromEmptyBinYesNo );
    
}


//------------
//
void DeleteDirRecycleBinWindow(void)
{
    // Routine to delete all of the recycle bin files from the CURRENT directory
    returnFromRecycleBinWindowEmpty = FALSE;         // Clear flag to cause reload and redraw of Recycle Bin.
    
    DisplayYesNoWindow("Empty Recycle Bin", "Empty the Recycle Bin for THIS directory.", "Are you sure?", "Yes", "No", 1, &ReturnFromDirRecycleBinYesNo );
    
}


//------------
//
void DeleteAllRecycleBinWindow(void)
{
    // Routine to delete all of the recycle bin files from the hard drive
    returnFromRecycleBinWindowEmpty = FALSE;         // Clear flag to cause reload and redraw of Recycle Bin.
    
    DisplayYesNoWindow("Empty Recycle Bin", "Delete ALL files in the Recycle Bin", "for ALL directories.  Are you sure?", "Yes", "No", 1, &ReturnFromAllRecycleBinYesNo );
    
}


//------------
//
void RecycleAction(void)
{
    // A file is placed in the "recycle bin" by appending the ".del" to the ".rec"
     
	char currentFileName[256], baseFileName[256], str1[256], str2[256], fileName[256], counter[6];
	bool fileAlreadyExists;   // Flag to indicate a file with the same name already exists.
	int  fileIncrement;       // A counter used in case we need to append a number to an already existing file name. 
    int  result;
	
	// If we try to recycle a file and there is already an existing file in the recycle-bin
	// with the same name, we will append an incrementing counter to the file name. (eg. news-1.rec.del  news-23.rec.del)
	fileAlreadyExists = FALSE;          // Flag to indicate that the renamed file already exists.
	fileIncrement     = 0;              // Counter in case we need to append a number to an already existing file name.
   
    strncpy( currentFileName, currentFile.name, 256 );   // Copy the current filename
    strncpy( fileName,        currentFile.name, 256 );   // Copy the current filename
    strncpy( baseFileName,    currentFile.name, 256 );   // Copy the current filename
	StripFileName( baseFileName );                       // Strip off the ".rec"
    strncpy( fileName, baseFileName, 256 );              // Copy the stripped off filename in case we need to rename on a move.
	strcat( fileName, RECYCLED_STRING );	             // Append ".rec.del" to the base filename

    // Check if the file already exists.
    fileAlreadyExists = TAP_Hdd_Exist(fileName);
    
    // If the 'renamed' recycle-bin file already exists, we'll add a counter to the end
    // of the file name and try again.
    while (fileAlreadyExists)
    {
         fileIncrement++;         // Increase the counter for the number to append to the filename.
         if (fileIncrement > 99999)   // If there are more than 99999 files in the recycle bin with the same name, abort the delete.
         {
              sprintf(str1,"More than 99999 files in the bin called:");
              ShowMessageWin( rgn, "Recyle Bin Failure.", str1, str2, 500 ); 
              fileDeleted = FALSE;
              return;   // Jump out of recycle option.
         }   
         sprintf(counter, "-%d", fileIncrement);        // Create the counter text
         strncpy( fileName, baseFileName, 256);         // Copy back the original filename
         strcat(  fileName, counter);                   // Append the counter text to the filename
	     strcat(  fileName, RECYCLED_STRING );	        // Append the ".rec.del" to the end.
         fileAlreadyExists = TAP_Hdd_Exist(fileName);   // Check if the file exists.
    }
         
    // Rename the file.
    #ifdef WIN32  // If testing on WIN32 platform don't do rename as not yet implemented in the TAP SDK.
         result=0;
    #else
         result=TAP_Hdd_Rename(currentFile.name,fileName);
    #endif   
    if (result==0)    // If the result of the operation was zero we assume the file was renamed (and therefore recycled) correctly.
    {
         fileDeleted = TRUE;    // Indicate the the file has been deleted (recycled).
    }                           
    else // There was an error during the rename process, so display an error message.
    {
         sprintf(str1,"Error renaming: '%s'", currentFile.name);
         sprintf(str2,"to: '%s'", fileName);
         ShowMessageWin( rgn, "Recyle Bin Failure.", str1, str2, 500 ); 
         fileDeleted = FALSE;
    }   

}
       

void RecycleRestoreAction(void)
{
    // A file is restored from the "recycle bin" by removing the ".del" from the end of the filename
	char currentFileName[256], baseFileName[256], str1[256], str2[256], fileName[256], counter[6];
	bool fileAlreadyExists;   // Flag to indicate a file with the same name already exists.
	int  fileIncrement;       // A counter used in case we need to append a number to an already existing file name. 
    int  result;
	
	// If we try to restore a file and there is already an existing file in the normal view
	// with the same name, we will append an incrementing counter to the file name. (eg. news-1.rec  news-23.rec)
	fileAlreadyExists = FALSE;          // Flag to indicate that the renamed file already exists.
	fileIncrement     = 0;              // Counter in case we need to append a number to an already existing file name.
   
    strncpy( currentFileName, currentFile.name, 256 );   // Copy the current filename
    strncpy( baseFileName, currentFile.name, 256 );      // Copy the current filename
	StripFileName( baseFileName );                       // Strip off the ".rec.del"
    strncpy( fileName, baseFileName, 256 );              // Copy the stripped off filename in case we need to rename on a move.
	strcat( fileName, REC_STRING );	             // Append ".rec" to the base filename

    // Check if the file already exists.
    fileAlreadyExists = TAP_Hdd_Exist(fileName);
    
    // If the 'renamed' recycle-bin file already exists, we'll add a counter to the end
    // of the file name and try again.
    while (fileAlreadyExists)
    {
         fileIncrement++;         // Increase the counter for the number to append to the filename.
         if (fileIncrement > 2)   // If there are more than 99999 files in the recycle bin with the same name, abort the delete.
         {
              sprintf(str1,"More than 99999 files in directory called:");
              ShowMessageWin( rgn, "Recyle Bin Failure.", str1, str2, 500 ); 
              fileDeleted = FALSE;
              return;   // Jump out of recycle option.
         }   
         sprintf(counter, "-%d", fileIncrement);           // Create the counter text
         strncpy( fileName, baseFileName, 256);            // Copy back the original filename
         strcat(  fileName, counter);                      // Append the counter text to the filename
	     strcat(  fileName, REC_STRING );	               // Append the ".del" to the end.
         fileAlreadyExists = TAP_Hdd_Exist(fileName);      // Check if the file exists.
    }
         
    // Rename the file.
    #ifdef WIN32  // If testing on WIN32 platform don't do rename as not yet implemented in the TAP SDK.
         result=0;
    #else
         result=TAP_Hdd_Rename(currentFile.name,fileName);
    #endif   
    if (result==0)    // If the result of the operation was zero we assume the file was renamed (and therefore restored) correctly.
    {
         fileDeleted      = TRUE;    // Indicate the the file has been deleted (restored).
         returnFromDelete = TRUE;	 //  Set flag that will cause a redraw of file list.
         
    }                           
    else // There was an error during the rename process, so display an error message.
    {
         sprintf(str1,"Error renaming: '%s'", currentFile.name);
         sprintf(str2,"to: '%s'", fileName);
         ShowMessageWin( rgn, "Recyle Bin Failure.", str1, str2, 500 ); 
         fileDeleted = FALSE;
    }   
}     



//------------
//
void DeleteRecycledFile(void)
{
     char str[TS_FILE_NAME_SIZE];
 
     DeleteAction(1);                   // Delete the File
     returnFromDelete    = TRUE;		// Set flag that will cause a redraw of file list.
     deleteWindowShowing = FALSE;       // Clear the flag that show the delete file is showing.

}
                                           
                                           


//------------
//
void initialiseArchiveRecycle(void)
{
    deleteWindowShowing = FALSE;
	returnFromDelete    = FALSE;
    fileDeleted         = FALSE;
    returnFromRecycleBinEmpty = FALSE;
    returnFromRecycleBinWindowEmpty = FALSE;
	// Initialise flag to indicate we're in normal view.
	recycleWindowMode = FALSE;  
    strcpy( tagStr, REC_STRING);   // Set the tag at the end of the filenames to ".rec"
    tagLength = strlen( tagStr );  // Calculate the length of the tag.  
             
}


//------------
//
dword RecycleBinWindowKeyHandler(dword key)

{
	int		oldPage, oldChosenLine, oldPrintLine, lineOffset;
	oldPage = page;
	oldChosenLine = chosenLine;
	oldPrintLine  = printLine;
	lineOffset    = 0;
	
    if ( archiveHelpWindowShowing ) { ArchiveHelpKeyHandler( key ); return; }				// handle help screen

	switch ( key )
	{
		case RKEY_Exit :	exitFlag = TRUE;										// signal exit to top level - will clean up, close window,
							break;													// and enter the normal state
							
	    case RKEY_PlayList:	// Handle the FileList Key
	                        switch (fileListKeyOption)
	                        {
                                   case 0: // Invoke Toppy standard archive
                                           exitFlag = TRUE;										// signal exit to top level - will clean up, close window,
                                           appendToLogfile("RecycleBinWindowKeyHandler: Playlist pressed.", WARNING);
                                           generatedPlayList = TRUE;
	                                       TAP_GenerateEvent( EVT_KEY, RKEY_PlayList, 0 );
							               break;													// and enter the normal state

                                   case 1: // Ignore key
                                           break;

                                   case 2: // Exit Archive.
                                           exitFlag = TRUE;
                                           break;
                            }													// and enter the normal state

		case RKEY_Slow :	if (mainActivationKey == RKEY_Slow) exitFlag = TRUE;  // Close on a 2nd press of the activation key.
							break;

		case RKEY_ChDown :	if ( chosenLine < maxShown ) 
                            {
                                 chosenLine++;         // 0=hidden - can't hide once cursor moved
                                 printLine++;
                                 if (printLine > NUMBER_OF_LINES) printLine=1;
                            }     
                            else 
                            {    // Roll past bottom to start again.
                                 chosenLine=1;	
                                 printLine=1;
                            }
							if ( maxShown == 0 ) chosenLine = 0;					// don't move the cursor if no files shown
							
							page = (chosenLine-1) / NUMBER_OF_LINES;
                            
							//if ( page == oldPage )									// only redraw what's nessesary
							if (printLine!=1)  // We're still on the same page.
							{
								if ( oldChosenLine > 0 ) DisplayArchiveLine(oldChosenLine,oldPrintLine);
								DisplayArchiveLine(chosenLine,printLine);
							}
							else DrawArchiveList();
                            UpdateListClock();
							UpdateSelectionNumber();
							break;

		case RKEY_ChUp :	if ( chosenLine > 1 ) 
                            {
                                 chosenLine--; 
                                 printLine--;
                                 if (printLine == 0) printLine = NUMBER_OF_LINES;
                            }     
                            else 
                            {    // Roll past top to start again.
                                 chosenLine = maxShown;
                                 printLine  = NUMBER_OF_LINES;
                            }
							if ( maxShown == 0 ) chosenLine = 0;					// not strictly needed - included in above logic
							
							page = (chosenLine-1) / NUMBER_OF_LINES;
                            if (printLine == NUMBER_OF_LINES)    // We've scrolled off the top of the page, so determine where to start next.
                            {
                                if (chosenLine < NUMBER_OF_LINES) // But if we're right near the top with only a small number of entries, only
                                {                                 // show the small list.
                                     printLine = chosenLine;
                                }
                                DrawArchiveList();    // Since this is a page change, redraw the entire list. 
                            }    
                            else                        // Just unhighlight the old line, and redraw the new selection.
							{
								if ( oldChosenLine > 0 ) DisplayArchiveLine(oldChosenLine, oldPrintLine);
					      		DisplayArchiveLine(chosenLine, printLine);
							}

                            UpdateListClock();
							UpdateSelectionNumber();
							break;

		case RKEY_Forward:
        case RKEY_VolUp :   
                            if ((printLine < NUMBER_OF_LINES) && (chosenLine < maxShown)) // If we're not already pointing to the last line on the page.
		                    {
                                printLine = NUMBER_OF_LINES;
                                chosenLine = chosenLine + (printLine - oldPrintLine); 
                            }    
                            else
                            if (chosenLine == maxShown) // If we're pointing to the end of the list, then wrap around to the start.
                            {
                               chosenLine = 1;
                               printLine = 1;
                            }   
                            else
                            {   // Page forward, and point at the last line.
                                chosenLine = chosenLine + NUMBER_OF_LINES;
                                printLine  = NUMBER_OF_LINES;
                            }

							if (chosenLine > maxShown) // Make sure we haven't gone too far.
							{
                                 chosenLine = maxShown;  // Select the last entry.
                                 if (maxShown < NUMBER_OF_LINES) printLine = chosenLine;
                                 else printLine = NUMBER_OF_LINES;
                            }      
                            
                            UpdateListClock();
                            DrawArchiveList();
							UpdateSelectionNumber();
							break;

		case RKEY_Rewind:
        case RKEY_VolDown :	
                            if (printLine > 1) // If we're not already pointing to the first line on the page.
		                    {
                                printLine = 1;
                                chosenLine = chosenLine - (oldPrintLine - printLine); ;
                            }    
                            else
                            if (chosenLine == 1) // If we're pointing to the start of the list, then wrap around to the end.
                            {
                               chosenLine = maxShown;
                               if (maxShown >= NUMBER_OF_LINES) printLine = NUMBER_OF_LINES;
                               else printLine = maxShown;
                            }   
                            else
                            {   // Page backward, and point at the first line.
                                if (chosenLine > NUMBER_OF_LINES) chosenLine = chosenLine - NUMBER_OF_LINES;
                                else chosenLine = 1;
                                printLine  = 1;
                            }
                            // Unhighlight the old selection.  Otherwise you may see 2 highlighted selections as the new page redraws from top to bottom.
                            if ( oldChosenLine > 0 ) DisplayArchiveLine(oldChosenLine,oldPrintLine);
                            UpdateListClock();
							DrawArchiveList();
							UpdateSelectionNumber();
							break;

							
		case RKEY_1 :
		case RKEY_2 :
		case RKEY_3 :
		case RKEY_4 :
		case RKEY_5 :
		case RKEY_6 :
		case RKEY_7 :
		case RKEY_8 :
		case RKEY_9 :		// Direct keyboard line selection
                            lineOffset = ((page * NUMBER_OF_LINES) + printLine) - chosenLine;   // Determine if we are out of sync with chosenline and printline (eg. if we scroll backwards to bottom of list.
		                    //  Determine if this line is available on the screen.
							if ( (key - RKEY_0) + (page * NUMBER_OF_LINES) -lineOffset <= maxShown ) 
                            {
                                 chosenLine = (key - RKEY_0) + (page * NUMBER_OF_LINES) - lineOffset;	
							     page = (chosenLine-1) / NUMBER_OF_LINES;
							     DisplayArchiveLine( oldChosenLine, oldPrintLine );
							     printLine = (key - RKEY_0);
							     DisplayArchiveLine( chosenLine, printLine );
							     UpdateSelectionNumber();							
                            }			     
							break;
							
		case RKEY_0 :       // Direct keyboard line selection
                            lineOffset = ((page * NUMBER_OF_LINES) + printLine) - chosenLine;   // Determine if we are out of sync with chosenline and printline (eg. if we scroll backwards to bottom of list.
		                    //  Determine if this line is available on the screen.
                            if (NUMBER_OF_LINES < 10) break;                              // If we're showing less than 10 lines, ignore 0.
							if ( ((10) + (page * NUMBER_OF_LINES)) -lineOffset  <= maxShown ) 
							{
							     chosenLine = (10) + (page * NUMBER_OF_LINES) -lineOffset ;			  // make "0" select the 10th (last) line
							     page = (chosenLine-1) / NUMBER_OF_LINES;
							     DisplayArchiveLine( oldChosenLine, oldPrintLine );
							     printLine = 10;
							     DisplayArchiveLine( chosenLine, printLine );
							     UpdateSelectionNumber();							
                            }
							break;

		case RKEY_Blue :	// Restore a Recycled File.
                            if (chosenLine == 0) break;   // Don't allow restore if there is nothing displayed.
                            if (IsFileRec(myfiles[CurrentDirNumber][chosenLine]->name, myfiles[CurrentDirNumber][chosenLine]->attr))  // We can only restore files.
                            {
                                 currentFile   = *myfiles[CurrentDirNumber][chosenLine];
                                 currentFolder = *myfolders[myfiles[CurrentDirNumber][chosenLine]->directoryNumber];
                                 RecycleRestoreAction();
                            }    
                            break;
							
		case RKEY_Ok :		// Handle opening of subfolders.
		                    if (!IsFileRec(myfiles[CurrentDirNumber][chosenLine]->name, myfiles[CurrentDirNumber][chosenLine]->attr))  // We can only open folders.
                                ArchiveAction(chosenLine);   // Normal "OK" action - open folder
                            break;
							
		case RKEY_Play :	break;  // Playback not allowed from Recycle Bin window.

		case RKEY_Pause:    // Cycle through the sort order (date, name, channel, size)
                            if (sortOrder < SORT_SIZE_OPTION) sortOrder += 1; 
		                    else sortOrder = SORT_DATE_OPTION;
		                    SortList(sortOrder);            // Resort the list with the new sort order.
		                    DrawBackground();               // Update the title.
		                    RefreshArchiveList(TRUE);       // Redraw the contents of the screen.
		                    break;
							
		case RKEY_Info :	// Display Info window
                            if (( chosenLine > 0 ) && (myfiles[CurrentDirNumber][chosenLine]->attr != PARENT_DIR_ATTR) && (!myfiles[CurrentDirNumber][chosenLine]->isRecording) )
                            { 
                                 currentFile   = *myfiles[CurrentDirNumber][chosenLine];
                                 currentFolder = *myfolders[myfiles[CurrentDirNumber][chosenLine]->directoryNumber];
                                 ActivateInfoWindow();
                            }     
							break;

        case RKEY_Green:    // Delete ALL Recycled Files
                            DeleteAllRecycleBinWindow();
                            break;
                                    
        case RKEY_Yellow:   // Delete ALL Recycled Files in THIS Directory.
                            DeleteDirRecycleBinWindow();
                            break;
                            
        case RKEY_Red:      DisplayArchiveRecycleHelp();
                            break;
                            
        case RKEY_Recall:   //   Toggle between standard file view and view of the recycle bin files.
                            if (recycleWindowMode == TRUE) 
                            {
                                recycleWindowMode = FALSE;
                                strcpy( tagStr, REC_STRING);   // Set the tag at the end of the filenames to ".rec"
                            }
                            else
                            {
                                recycleWindowMode = TRUE;
                                strcpy( tagStr, RECYCLED_STRING);   // Set the tag at the end of the filenames to ".rec.del"
                            }
                            tagLength = strlen( tagStr );  // Calculate the length of the tag.  
                            TAP_Osd_PutStringAf1926( rgn, 58, 40, 390, "       LOADING...      ", TITLE_COLOUR, COLOR_Black );
                            loadInitialArchiveInfo(FALSE, 99); // Load all the files for the new view, but don't delete any progress info.
   		                    RefreshArchiveList(TRUE);      // Redraw the contents of the screen.
                            break;                            

                            							
		case RKEY_Menu :	ActivateMenu();
						    break;

		case RKEY_Mute :	// Pass thru Mute key
                            return key;


        case RKEY_Stop:     break;  // Playback not allowed from Recycle Bin window.
    
        #ifdef WIN32
        case RKEY_Fav   :   // If testing on WIN32 SDK also allow the Favourite key 
        #endif
        case RKEY_White :   // Delete
                            if (chosenLine == 0) break;   // Don't allow delete if there is nothing displayed.
                            if (IsFileRec(myfiles[CurrentDirNumber][chosenLine]->name, myfiles[CurrentDirNumber][chosenLine]->attr))  // We can only delete files.
                            {
                                 DeleteRecycledFile();
                            }    
                            break;
							
		default :			break;
	}
	return 0;
}    
