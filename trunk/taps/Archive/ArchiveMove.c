/************************************************************
			Part of the ukEPG project
		This module displays move options for the archive entries

Name	: ArchiveMove.c
Author	: kidhazy
Version	: 0.1
For		: Topfield TF5x00 series PVRs
Licence	:
Descr.	:
Usage	:
History	: v0.0 kidhazy 07-07-06  Inception date


**************************************************************/

static bool  returnFromMove;                // Flag to indicate when we have returned from displaying the Move window.                 
static bool  listMoved;                     // Flag to indicate when the list in the Move window has been moved.
static byte* moveWindowCopy;                // Pointer to a copy in memory of the Move Window.
static bool  fileMoved;                     // Flag to indicate when we have moved a file/folder.
static int   chosenFolderLine;              // Index to which folder in the list has been chosen as the target foler.
static TYPE_My_Files *moveFiles[MAX_DIRS];  // Array of pointers pointing to folders for destination list.   
static int   numberOfDestinationFolders;    // Number of directories that are in the destination window.
static char  *currentMoveDestination;
static int   printMoveFolderLine;
static char  GlbSourceDir[256], GlbTargetDir[256], GlbSourceFile[256], GlbRenamedFile[256];

static word  moveRgn;						// a memory region used for the move window to display in.


#define MAX_MOVE_FOLDERS_SHOWN 9
#define NUMBER_OF_MOVE_FOLDER_LINES 10


// Position of the Information Screen
#define MOVE_WINDOW_W 476
#define MOVE_WINDOW_H 416
#define MOVE_WINDOW_X ((MAX_SCREEN_X-MOVE_WINDOW_W)/2)    // Centre the move window
#define MOVE_WINDOW_Y ((MAX_SCREEN_Y-MOVE_WINDOW_H)/2-10) // Centre the move window

#define MOVE_OPTION_W 82                                  // Width  of Info Options Buttons
#define MOVE_OPTION_H 43                                  // Height of Info Options Buttons
#define MOVE_OPTION_X  (MOVE_WINDOW_X + 154)              // Starting x-position for first Option button  82
#define MOVE_OPTION_Y  (MOVE_WINDOW_Y + MOVE_WINDOW_H - MOVE_OPTION_H - 15)             // Starting y-position for Option buttons.
#define MOVE_OPTION_X_SPACE   (MOVE_OPTION_W+3)           // Space between options on Move window.

#define MOVE_COMMAND_OPTIONS 2  // Number of command buttons in the move window.

static int  moveCommandOption;

//------------
//
void populateMoveFileList(void)
{
     int  index, numberOfEntries;

     appendToLogfile("populateMoveFileList: Started.", WARNING);

     numberOfEntries = myfolders[CurrentDirNumber]->numberOfFiles;
 	 
 	 numberOfDestinationFolders = 0;
     for ( index=1; index<=numberOfEntries ; index++)
	 {
        switch (myfiles[CurrentDirNumber][index]->attr)
        {
           case PARENT_DIR_ATTR:
           case 240:          // Parent Directory  ".."
           case ATTR_FOLDER:  // Normal Folder
                              // Make sure we don't include the current folder that we may be trying to move.
                              if (index == chosenLine) break;
                              
                              numberOfDestinationFolders++;
                              if (moveFiles[numberOfDestinationFolders] == NULL)
                              {
                                  moveFiles[numberOfDestinationFolders] = TAP_MemAlloc( sizeof  (*moveFiles[numberOfDestinationFolders])); 
                              }    
                              memset(moveFiles[numberOfDestinationFolders],0,sizeof (*moveFiles[numberOfDestinationFolders]));
                              if (myfiles[CurrentDirNumber][index]->attr == ATTR_FOLDER)  // Normal folder so use the folder name.
	                              strcat(moveFiles[numberOfDestinationFolders]->name, myfiles[CurrentDirNumber][index]->name);
	                          else // Parent Directory, so use ".."    
	                              strcat(moveFiles[numberOfDestinationFolders]->name, "..");
	                          break;
           case ATTR_TS:
           default:
		                      break;
        }		

	 }
     appendToLogfile("populateMoveFileList: Finished.", WARNING);
}

     

//-----------------------------------------------------------------------
//
void PerformMove(void)
{
     bool moveres;
          
     appendToLogfile("PerformMove: Started.", INFO);
     
     // Call the move routine and track the result code.
     #ifdef WIN32
            moveres = TRUE;
     #else
            appendToLogfile("PerformMove: Calling TAP_Hdd_Move", WARNING);
            appendStringToLogfile("PerformMove: Calling TAP_Hdd_Move GlbSourceDir: %s",GlbSourceDir, WARNING);
            appendStringToLogfile("PerformMove: Calling TAP_Hdd_Move GlbTargetDir: %s",GlbTargetDir, WARNING);
            appendStringToLogfile("PerformMove: Calling TAP_Hdd_Move GlbSourceFile: %s",GlbSourceFile, WARNING);
            moveres = TAP_Hdd_Move( GlbSourceDir, GlbTargetDir, GlbSourceFile);
            appendToLogfile("PerformMove: TAP_Hdd_Move finished.", WARNING);
     #endif


     // Check and handle the result of the move routine.
     // Check the file actually moved via file exists.
     #ifdef WIN32  // If testing on Windows platform, assume success rather than physically deleting file.
     if (FALSE)
     #else  
     appendToLogfile("PerformMove: Calling TAP_Hdd_Exist", WARNING);
     if (TAP_Hdd_Exist(GlbSourceFile))  // If the file/folder still exists in the current directory, then the move didn't work.
     #endif
     {
         ShowMessageWin( rgn, "File/Folder Move Failed.", "Failed to move file/folder:", GlbSourceFile, 400 );
         fileMoved = FALSE;
     }
     else
     {
         fileMoved = TRUE;
         myfiles[CurrentDirNumber][chosenLine]->isNew = FALSE;   // Clear the 'new' flag for this file.
     }    
     returnFromMove = TRUE;   // Force a closer of the Move window, and refresh the list.
     
/*     
     ShowMessageWin( rgn, "Move Test #1.", "Moving Test.rec", "to MINE folder", 400 );
     moveres = TAP_Hdd_Move( "DataFiles", "DataFiles/Mine", "Test.rec");
   
     ShowMessageWin( rgn, "Move Test #2.", "Moving MINE folder", "to MINE2 folder", 400 );
     moveres = TAP_Hdd_Move( "DataFiles", "DataFiles/Mine2", "Mine");
     
     ShowMessageWin( rgn, "Move Test #3.", "Moving Test2.rec", "to .. folder", 400 );
     moveres = TAP_Hdd_Move( "DataFiles/Mine2", "DataFiles/Mine2/..", "Test2.rec");
*/   
     appendToLogfile("PerformMove: Finished.", WARNING);
     
}     


//-----------------------------------------------------------------------
//
void ReturnFromMoveRenameYesNo( bool result)
{
    // Routine if invoked upon return from the "Automatic Rename on Move" confirmation prompt.
     
    char str1[200], str2[200];
    int  renameResult;

    appendToLogfile("ReturnFromMoveRenameYesNo: Started.", INFO);

    // Check the result of the confirmation panel to decide what to do.
	switch (result)
    {
           case TRUE:  // YES
                       appendStringToLogfile("ReturnFromMoveRenameYesNo: Rename from %s", GlbSourceFile, WARNING);
                       appendStringToLogfile("ReturnFromMoveRenameYesNo: to %s", GlbRenamedFile, WARNING);
                       #ifdef WIN32  // If testing on WIN32 platform don't do rename as not yet implemented in the TAP SDK.
                       renameResult=0;
                       #else
                  	   renameResult=TAP_Hdd_Rename(GlbSourceFile,GlbRenamedFile);
                       #endif          
                  	   if (renameResult!=0)
                       {
                          TAP_SPrint(str1,"Error renaming: '%s'", GlbSourceFile);
                          TAP_SPrint(str2,"to: '%s'", GlbRenamedFile);
                          fileMoved      = FALSE;
                          returnFromMove = FALSE;   // Don't close the Move window to give user a chance to choose a different folder.
                          ShowMessageWin( rgn, "Rename On Move Failed.", str1, str2, 500 ); 
                          break;
                       }   
                       strncpy( GlbSourceFile, GlbRenamedFile, 256 );   // Copy the renamed filename into the Global variable so that we move the renamed file.
                       PerformMove();
                       switch (RenameOnMoveOption)
                       {
                              case 0:   // "Rename and confirm"
                                        // Close the Move Window after this, otherwise it is closed as part of the other routines.
                                        CloseArchiveMoveWindow();			    // Close the move window
                                        break;
                                        
                              case 2:   // "Rename automatically (with message) "
                                        TAP_SPrint(str1, "%s/%s", GlbTargetDir, GlbRenamedFile);  
                                        ShowMessageWin( rgn, "File Move Successful.", "Moved and renamed file to:", str1, 400 ); 
                                        break;
                                        
                              default:  // "Rename automatically (no message) "
                                        break;
                       }
                       break;
                      
	       case FALSE: // NO
                       fileMoved      = FALSE;
                       returnFromMove = FALSE;   // Don't close the Move window to give user a chance to choose a different folder.
                       break;	
    }   
    appendToLogfile("ReturnFromMoveRenameYesNo: Finished.", INFO);
}


//------------
//
void MoveAFile(char* sourceDir, char* destDir, char* sourceFile, dword attr)
{
     bool fileAlreadyExists;
     char targetDir[500];
	 char currentFileName[256], baseFileName[256], title1[50], title2[50], str1[256], str2[256], fileName[256], counter[6];
	 int  fileIncrement;       // A counter used in case we need to append a number to an already existing file name. 

     

     appendToLogfile("MoveAFile: Started.", WARNING);
 	 
      // If we try to move a file and there is already an existing file in the target directory
	 // with the same name, we will append an incrementing counter to the file name. (eg. news-1.rec  news-23.rec)
	 fileAlreadyExists = FALSE;          // Flag to indicate that the renamed file already exists.
	 fileIncrement     = 0;              // Counter in case we need to append a number to an already existing file name.

     if (attr != ATTR_FOLDER)
     {
         strncpy( currentFileName, sourceFile, 256 );   // Copy the current filename
         strncpy( fileName,        sourceFile, 256 );   // Copy the current filename
         strncpy( baseFileName,    sourceFile, 256 );   // Copy the current filename
    	 StripFileName( baseFileName );                       // Strip off the ".rec"
         strncpy( fileName, baseFileName, 256 );              // Copy the stripped off filename in case we need to rename on a move.
    	 strcat( fileName, REC_STRING );	                  // Append ".rec" to the base filename
     }

     // Perform check that the Move routine is available for this firmware.
     #ifdef WIN32
     #else
     if (!TAP_Hdd_Move_Available())
     {
         ShowMessageWin( rgn, "Move NOT available.", "This firmware does not support", "the move function via Archive.", 400 );
         returnFromMove = TRUE;   // Force a closer of the Move window, and refresh the list.
         return;
     }
     #endif
    
     appendToLogfile("MoveAFile: Creating target filename.", WARNING);
     
     // Create the fully qualified target directory name.  
     //
     // Start with the Source Directory 
     strcpy(targetDir, sourceDir);
     // Add the intermediate "/"
     strcat(targetDir, "/" );
     // Add the target directory.
     strcat(targetDir, destDir);

     // Check in the target directory to make sure a file with the same name doesn't already exists.
     //
     // Change to the target directory.
     appendStringToLogfile("MoveAFile: Changing to targetdir: %s.",targetDir, WARNING);
     GotoPath( targetDir );
     // Check for the same file.
     appendStringToLogfile("MoveAFile: Calling TAP_Hdd_Exist for:%s",sourceFile, WARNING);
     fileAlreadyExists = TAP_Hdd_Exist(sourceFile);

     // If we're moving a file and the target already exists, try to rename it - if the config option #28 has been set to allow rename..
     while ((fileAlreadyExists) && (fileIncrement < 99999) && (attr != ATTR_FOLDER) && (RenameOnMoveOption!=3))
     {
         fileIncrement++;         // Increase the counter for the number to append to the filename.
         appendIntToLogfile("MoveAFile: fileAlreadyExists loop:%d",fileIncrement, WARNING);
         sprintf(counter, "-%d", fileIncrement);        // Create the counter text
         strncpy( fileName, baseFileName, 256);         // Copy back the original filename
         strcat(  fileName, counter);                   // Append the counter text to the filename
 	     strcat(  fileName, REC_STRING );	            // Append the ".rec" to the end.
         appendStringToLogfile("MoveAFile: fileAlreadyExists loop calling TAP_Hdd_Exist for: %s", fileName, WARNING);
         fileAlreadyExists = TAP_Hdd_Exist(fileName);   // Check if the file exists.
         appendToLogfile("MoveAFile: fileAlreadyExists loop TAP_Hdd_Exist finished", WARNING);
     }

     // Return to the original directory.
     appendStringToLogfile("MoveAFile: Changing to sourceDir: %s.",sourceDir, WARNING);
     GotoPath( sourceDir );
     
    // Set up appropriate text strings for message windows, depending on whether it's a file or folder. 
     switch (attr)
     {
           case ATTR_FOLDER: TAP_SPrint(title1, "Folder Move Failed.");
                             TAP_SPrint(title2, "");
                             TAP_SPrint(str1,   "Same folder already exists:");
                             TAP_SPrint(str2,   "");
                             break;
                
           default:          TAP_SPrint(title1, "File Move Failed.");
                             TAP_SPrint(title2, "File Move Conflict.");
                             TAP_SPrint(str1,   "Same file already exists.");
                             TAP_SPrint(str2,   "Same file already exists.  Rename to:");
                             break;
     }
     
     // Copy source and destination information to Global variables so that they are available for the actual move routine.
     strncpy( GlbSourceFile, sourceFile, 256 );   // Copy the source filename
     strncpy( GlbSourceDir,  sourceDir,  256 );   // Copy the source directory
     strncpy( GlbTargetDir,  targetDir,  256 );   // Copy the target directory
     
     // If the target file already exists in the target directory we can't move.
     if (fileAlreadyExists)
     {
         ShowMessageWin( rgn, title1, str1, sourceFile, 400 );
         fileMoved      = FALSE;
         returnFromMove = FALSE;   // Don't close the Move window to give user a chance to choose a different folder.
         return;                   // Jump out of this routine as we can't do the move.
     }

     // Check if we needed to rename the file in the target directory.
     if (fileIncrement > 0) 
     {
         strncpy( GlbRenamedFile, fileName, 256 );   // Copy the renamed filename into the Global variable
         // Check the config option #28 to see if the Rename On Move requires a confirmation or not.
         if (RenameOnMoveOption == 0)   // Need to display a confirmation panel.
         {
                  DisplayYesNoWindow(title2, str2, fileName, "Yes", "No", 1, &ReturnFromMoveRenameYesNo );
                  fileMoved      = FALSE;
                  returnFromMove = FALSE;   // Don't close the Move window until after the Yes/No window.
                  return;                   // Jump out of this routine as Move will be done after Yes/No window.
         }
         else   // Otherwise call routine as if we said "Yes" on the confirmation window.
         {
                  ReturnFromMoveRenameYesNo( TRUE );
                  return;                    // Jump out of this routine as we've already done the move in the ReturnFromMoveRenameYesNo routine.
         }
         
     }
     
     // Call the routine to perform the actual move.
     PerformMove();

     appendToLogfile("MoveAFile: Finished.", WARNING);
   
}



//------------------------------------------------------------------
//
void DisplayMoveLine(void)
{
	TAP_Osd_PutGd( rgn, MOVE_OPTION_X+(0*MOVE_OPTION_X_SPACE), MOVE_OPTION_Y,   &_keyblue82x43Gd, FALSE );
    PrintCenter(rgn,    MOVE_OPTION_X+(0*MOVE_OPTION_X_SPACE), MOVE_OPTION_Y+9, MOVE_OPTION_X+(0*MOVE_OPTION_X_SPACE)+MOVE_OPTION_W, "Move", MAIN_TEXT_COLOUR, 0, FNT_Size_1926 );

	TAP_Osd_PutGd( rgn, MOVE_OPTION_X+(1*MOVE_OPTION_X_SPACE), MOVE_OPTION_Y,   &_keyblue82x43Gd, FALSE );
	PrintCenter(rgn,    MOVE_OPTION_X+(1*MOVE_OPTION_X_SPACE), MOVE_OPTION_Y+9, MOVE_OPTION_X+(1*MOVE_OPTION_X_SPACE)+MOVE_OPTION_W, "Cancel", MAIN_TEXT_COLOUR, 0, FNT_Size_1926 );

	switch ( moveCommandOption )
	{
		case 0 :
					TAP_Osd_PutGd( rgn, MOVE_OPTION_X+(0*MOVE_OPTION_X_SPACE), MOVE_OPTION_Y,   &_keygreen82x43Gd, FALSE );
				    PrintCenter(rgn,    MOVE_OPTION_X+(0*MOVE_OPTION_X_SPACE), MOVE_OPTION_Y+9, MOVE_OPTION_X+(0*MOVE_OPTION_X_SPACE)+MOVE_OPTION_W, "Move", COLOR_Yellow, 0, FNT_Size_1926 );
                    break;
					
		case 1 :
					TAP_Osd_PutGd( rgn, MOVE_OPTION_X+(1*MOVE_OPTION_X_SPACE), MOVE_OPTION_Y,   &_keygreen82x43Gd, FALSE );
					PrintCenter(rgn,    MOVE_OPTION_X+(1*MOVE_OPTION_X_SPACE), MOVE_OPTION_Y+9, MOVE_OPTION_X+(1*MOVE_OPTION_X_SPACE)+MOVE_OPTION_W, "Cancel", COLOR_Yellow, 0, FNT_Size_1926 );
                    break;
	}
}


//-----------------------------------------------------------------------
//
void DisplayArchiveMoveFolderList()
{
    int printLine, index, lineHeight;

    appendToLogfile("DisplayArchiveMoveFolderList: Started.", WARNING);

    // Vertical spacing for each of the rows to display the list of target folders.
    lineHeight = 30;

    // Where in the list should we start the printing from.
    index     = printMoveFolderLine;
    
    // Start the printing on the first row.
    printLine = 1;
    
    // Build the folder list in a memory screen region.
    TAP_Osd_PutGd( moveRgn, MOVE_WINDOW_X, MOVE_WINDOW_Y, &_popup476x416Gd, TRUE );             // Redisplay blank popup window to clear out old list.
    while ((index<=numberOfDestinationFolders) && (printLine<=MAX_MOVE_FOLDERS_SHOWN))
	{
         if (index == chosenFolderLine)  // If we're about to print the selected folder, print it with a different background.
              PrintLeft (moveRgn, MOVE_WINDOW_X+20, MOVE_WINDOW_Y+ 50 + (printLine * lineHeight), MOVE_WINDOW_X+MOVE_WINDOW_W-8, moveFiles[index]->name, MAIN_TEXT_COLOUR, HEADING_TEXT_COLOUR, FNT_Size_1926 );
         else                            // If we're about to print a normal folder, print it with a normal background
              PrintLeft (moveRgn, MOVE_WINDOW_X+20, MOVE_WINDOW_Y+ 50 + (printLine * lineHeight), MOVE_WINDOW_X+MOVE_WINDOW_W-8, moveFiles[index]->name, MAIN_TEXT_COLOUR, 0, FNT_Size_1926 );
         index++;      // Increment the counter for the list.
         printLine++;  // Increment the row counter.
    }     
    
    // Print "+" indicators and top and/or bottom of the list to indicate if there are more files above/below in the list.
    // Check if there are some folders not shown at TOP of the list.
    if (printMoveFolderLine > 1)  
        PrintLeft (moveRgn, MOVE_WINDOW_X+MOVE_WINDOW_W-30, MOVE_WINDOW_Y+ 50 + ((1) * lineHeight),           MOVE_WINDOW_X+MOVE_WINDOW_W-8, "+", MAIN_TEXT_COLOUR, 0, FNT_Size_1926 );
    // Check if there are some folders not shown at BOTTOM of the list.
    if (index <= numberOfDestinationFolders)  // We've have't printed all of our list entries.
        PrintLeft (moveRgn, MOVE_WINDOW_X+MOVE_WINDOW_W-30, MOVE_WINDOW_Y+ 50 + ((printLine-1) * lineHeight), MOVE_WINDOW_X+MOVE_WINDOW_W-8, "+", MAIN_TEXT_COLOUR, 0, FNT_Size_1926 );
    
    // Copy the screen region from memory to the real screen.  This reduces any flicker when moving between rows.
    TAP_Osd_Copy( moveRgn, rgn, MOVE_WINDOW_X+20, MOVE_WINDOW_Y+50+(1*lineHeight), MOVE_WINDOW_W-20-8, (MAX_MOVE_FOLDERS_SHOWN*lineHeight), MOVE_WINDOW_X+20, MOVE_WINDOW_Y+50+(1*lineHeight), FALSE );
    
    appendToLogfile("DisplayArchiveMoveFolderList: Finised.", WARNING);

}


//-----------------------------------------------------------------------
//
void DisplayArchiveMoveWindow()
{
    char title[50];

    appendToLogfile("DisplayArchiveMoveWindow: Started.", INFO);
 
    // Display the pop-up window - will also clear any old text if we are doing a refresh.
    TAP_Osd_PutGd( rgn, MOVE_WINDOW_X, MOVE_WINDOW_Y, &_popup476x416Gd, TRUE );
//	TAP_Osd_FillBox( moveRgn, 0, 0, 720, 576, POPUP_FILL_COLOUR );		// clear the memory region with FILL COLOUR

    // Format and display the title for the Move Window.
    TAP_SPrint(title, "Select Destination Folder");
    PrintCenter( rgn, MOVE_WINDOW_X, MOVE_WINDOW_Y +  14, MOVE_WINDOW_X+MOVE_WINDOW_W, title, MAIN_TEXT_COLOUR, 0, FNT_Size_1926 );
	
    DisplayMoveLine();
    appendToLogfile("DisplayArchiveMoveWindow: Finised.", INFO);
}



//-----------------------------------------------------------------------
//
void ActivateMoveWindow()
{
	moveWindowShowing   = TRUE;
	moveCommandOption   = 0;       // Default to the "OK" option.
	fileMoved           = FALSE;
    listMoved           = FALSE;
    chosenFolderLine    = 1;       // Initialise to point at the first destination folder.
    printMoveFolderLine = 1;

    moveRgn  = TAP_Osd_Create( 0, 0, 720, 576, 0, OSD_Flag_MemRgn );	// In MEMORY define the whole screen for us to draw on
    
	// Store the currently displayed screen area where we're about to put our pop-up window on.
    moveWindowCopy = TAP_Osd_SaveBox(rgn, MOVE_WINDOW_X, MOVE_WINDOW_Y, MOVE_WINDOW_W, MOVE_WINDOW_H);

    DisplayArchiveMoveWindow();
}


//-----------------------------------------------------------------------
//
void CloseArchiveMoveWindow(void)
{
    int index;

    appendToLogfile("CloseArchiveMoveWindow: Started.", INFO);
     
	moveWindowShowing = FALSE;
	TAP_Osd_RestoreBox(rgn, MOVE_WINDOW_X, MOVE_WINDOW_Y, MOVE_WINDOW_W, MOVE_WINDOW_H, moveWindowCopy);
	TAP_MemFree(moveWindowCopy);           // Free memory allocated to storing old screen area.
	TAP_Osd_Delete(moveRgn);               // Free memory allocated to virtual screen area.
	// Release any memory allocated to the "moveFiles" array.   
    for ( index=1; index<=numberOfDestinationFolders ; index++)
	{
       if (moveFiles[index] != NULL) 
       {
           TAP_MemFree(moveFiles[index]);  // Free any previously allocated memory
           moveFiles[index] = NULL;        // Set the pointer to NULL so that it's all cleared.                   
       }
    }  
    appendToLogfile("CloseArchiveMoveWindow: Finished.", INFO);
}


//------------
//
dword ArchiveMoveKeyHandler(dword key)
{

	switch ( key )
	{
		case RKEY_ChDown :	if ( chosenFolderLine < numberOfDestinationFolders ) 
                            {
                                 chosenFolderLine++;         // 0=hidden - can't hide once cursor moved
                                 if (chosenFolderLine >= NUMBER_OF_MOVE_FOLDER_LINES) printMoveFolderLine++;
                                 listMoved   = TRUE;
                            }     
                            else 
                            {    // Roll past bottom to start again.
                                 chosenFolderLine=1;	
                                 printMoveFolderLine=1;
                                 listMoved   = TRUE;
                            }
							if ( numberOfDestinationFolders == 0 ) chosenFolderLine = 0;					// don't move the cursor if no files shown

                            currentMoveDestination = moveFiles[chosenFolderLine]->name;
							DisplayArchiveMoveFolderList();

/*	
						page = (chosenFolderLine-1) / NUMBER_OF_LINES;
                            
		
            				if (printMoveFolderLine!=1)  // We're still on the same page.
							{
								if ( oldChosenLine > 0 ) DisplayArchiveLine(oldChosenLine,oldprintMoveFolderLine);
								DisplayArchiveLine(chosenFolderLine,printMoveFolderLine);
							}
							else DrawArchiveList();
                            UpdateListClock();
							UpdateSelectionNumber();
*/
							break;

		case RKEY_ChUp :	if ( chosenFolderLine > 1 ) 
                            {
                                 chosenFolderLine--; 
                                 if (chosenFolderLine < printMoveFolderLine) printMoveFolderLine--;
                                 listMoved   = TRUE;
                            }     
                            else 
                            {    // Roll past top to start again.
                                 chosenFolderLine = numberOfDestinationFolders;
                                 if (chosenFolderLine < NUMBER_OF_MOVE_FOLDER_LINES)
                                     printMoveFolderLine = 1;
                                 else
                                     printMoveFolderLine = numberOfDestinationFolders - NUMBER_OF_MOVE_FOLDER_LINES + 2;
                                     
                                 listMoved   = TRUE;
                            }
							if ( numberOfDestinationFolders == 0 ) chosenFolderLine = 0;					// not strictly needed - included in above logic

                            currentMoveDestination = moveFiles[chosenFolderLine]->name;
							DisplayArchiveMoveFolderList();
/*							
							page = (chosenFolderLine-1) / NUMBER_OF_LINES;
                            if (printMoveFolderLine == NUMBER_OF_LINES)    // We've scrolled off the top of the page, so determine where to start next.
                            {
                                if (chosenFolderLine < NUMBER_OF_LINES) // But if we're right near the top with only a small number of entries, only
                                {                                 // show the small list.
                                     printMoveFolderLine = chosenFolderLine;
                                }
                                DrawArchiveList();    // Since this is a page change, redraw the entire list. 
                            }    
                            else                        // Just unhighlight the old line, and redraw the new selection.
							{
								if ( oldChosenLine > 0 ) DisplayArchiveLine(oldChosenLine, oldprintMoveFolderLine);
					      		DisplayArchiveLine(chosenFolderLine, printMoveFolderLine);
							}

                            UpdateListClock();
							UpdateSelectionNumber();
*/
							break;

		case RKEY_VolUp:	if ( moveCommandOption < (MOVE_COMMAND_OPTIONS-1) ) moveCommandOption++;
							else moveCommandOption = 0;
							DisplayMoveLine();
		     				break;

		case RKEY_VolDown:	if ( moveCommandOption > 0 ) moveCommandOption--;
							else moveCommandOption = (MOVE_COMMAND_OPTIONS-1);
							DisplayMoveLine();
							break;


		case RKEY_Ok :		switch ( moveCommandOption )
							{
								case 0 :   // Move
								           MoveAFile( myfiles[CurrentDirNumber][chosenLine]->directory,  moveFiles[chosenFolderLine]->name, myfiles[CurrentDirNumber][chosenLine]->name, myfiles[CurrentDirNumber][chosenLine]->attr);
								           // If the Move was successful, or had a fatal error, close the Move window.
								           if (returnFromMove)
							                  CloseArchiveMoveWindow();	// Close the move window
                                           break;	

								case 1 :   // Cancel
							               CloseArchiveMoveWindow();	// Close the move window
							               fileMoved      = FALSE;      // Clear flag to indicate that file wasn't moved.
                                           returnFromMove = TRUE;		// will cause a redraw of file list
                                           break;	

								default :  
                                           break;
							}

							break;


		case RKEY_Exit : 	
                            CloseArchiveMoveWindow();			    // Close the move window
							returnFromMove = TRUE;					// will cause a redraw of file list
							break;
							
		case RKEY_Mute :	return key;


		default :
			break;
	}
	return 0;
}



//------------
//
void initialiseArchiveMove(void)
{
    moveWindowShowing = FALSE;
	moveCommandOption = 0;  // Default to the "Move" option.
	returnFromMove    = FALSE;
	fileReset         = FALSE;
	chosenFolderLine  = 0;

}



