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
}

//------------
//
void MoveAFile(char* sourceDir, char* destDir, char* sourceFile)
{
     bool moveres, fileExistRes;
     char targetDir[500];


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
     GotoPath( targetDir );
     // Check for the same file.
     fileExistRes = TAP_Hdd_Exist(sourceFile);
     // Return to the original directory.
     GotoPath( sourceDir );
     // If the same file exists in the targetdir then exit.
     if (fileExistRes)
     {
         ShowMessageWin( rgn, "File/Folder Move Failed.", "Same file/folder already exists", sourceFile, 400 );
         fileMoved = FALSE;
         returnFromMove = FALSE;   // Don't close the Move window.
         return;
     }
     
     
     // Call the move routine and track the result code.
     #ifdef WIN32
            moveres = TRUE;
     #else
            moveres = TAP_Hdd_Move( sourceDir, targetDir, sourceFile);
     #endif


     // Check and handle the result of the move routine.
     // Check the file actually moved via file exists.
     #ifdef WIN32  // If testing on Windows platform, assume success rather than physically deleting file.
     if (FALSE)
     #else  
     if (TAP_Hdd_Exist(sourceFile))  // If the file/folder still exists in the current directory, then the move didn't work.
     #endif
     {
         ShowMessageWin( rgn, "File/Folder Move Failed.", "Failed to move file/folder:", sourceFile, 400 );
         fileMoved = FALSE;
     }
     else
         fileMoved = TRUE;
     
     returnFromMove = TRUE;   // Force a closer of the Move window, and refresh the list.
     
/*     
          

     ShowMessageWin( rgn, "Move Test #1.", "Moving Test.rec", "to MINE folder", 400 );
     moveres = TAP_Hdd_Move( "DataFiles", "DataFiles/Mine", "Test.rec");
   
     ShowMessageWin( rgn, "Move Test #2.", "Moving MINE folder", "to MINE2 folder", 400 );
     moveres = TAP_Hdd_Move( "DataFiles", "DataFiles/Mine2", "Mine");
     
     ShowMessageWin( rgn, "Move Test #3.", "Moving Test2.rec", "to .. folder", 400 );
     moveres = TAP_Hdd_Move( "DataFiles/Mine2", "DataFiles/Mine2/..", "Test2.rec");
*/   
   
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

    // Vertical spacing for each of the rows to display the list of target folders.
    lineHeight = 30;

    // Where in the list should we start the printing from.
    index     = printMoveFolderLine;
    
    // Start the printing on the first row.
    printLine = 1;
    
    while ((index<=numberOfDestinationFolders) && (printLine<=MAX_MOVE_FOLDERS_SHOWN))
	{
         if (index == chosenFolderLine)  // If we're about to print the selected folder, print it with a different background.
              PrintLeft (rgn, MOVE_WINDOW_X+20, MOVE_WINDOW_Y+ 50 + (printLine * lineHeight), MOVE_WINDOW_X+MOVE_WINDOW_W-8, moveFiles[index]->name, MAIN_TEXT_COLOUR, HEADING_TEXT_COLOUR, FNT_Size_1926 );
         else                            // If we're about to print a normal folder, print it with a normal background
              PrintLeft (rgn, MOVE_WINDOW_X+20, MOVE_WINDOW_Y+ 50 + (printLine * lineHeight), MOVE_WINDOW_X+MOVE_WINDOW_W-8, moveFiles[index]->name, MAIN_TEXT_COLOUR, 0, FNT_Size_1926 );
         index++;      // Increment the counter for the list.
         printLine++;  // Increment the row counter.
    }     
    
    // Print "+" indicators and top and/or bottom of the list to indicate if there are more files above/below in the list.
    // Check if there are some folders not shown at TOP of the list.
    if (printMoveFolderLine > 1)  
        PrintLeft (rgn, MOVE_WINDOW_X+MOVE_WINDOW_W-30, MOVE_WINDOW_Y+ 50 + ((1) * lineHeight),           MOVE_WINDOW_X+MOVE_WINDOW_W-8, "+", MAIN_TEXT_COLOUR, 0, FNT_Size_1926 );
    // Check if there are some folders not shown at BOTTOM of the list.
    if (index <= numberOfDestinationFolders)  // We've have't printed all of our list entries.
        PrintLeft (rgn, MOVE_WINDOW_X+MOVE_WINDOW_W-30, MOVE_WINDOW_Y+ 50 + ((printLine-1) * lineHeight), MOVE_WINDOW_X+MOVE_WINDOW_W-8, "+", MAIN_TEXT_COLOUR, 0, FNT_Size_1926 );
    

}


//-----------------------------------------------------------------------
//
void DisplayArchiveMoveWindow()
{
    char title[50];
    
    // Display the pop-up window - will also clear any old text if we are doing a refresh.
    TAP_Osd_PutGd( rgn, MOVE_WINDOW_X, MOVE_WINDOW_Y, &_popup476x416Gd, TRUE );

    // Format and display the title for the Move Window.
    TAP_SPrint(title, "Select Destination Folder");
    PrintCenter( rgn, MOVE_WINDOW_X, MOVE_WINDOW_Y +  14, MOVE_WINDOW_X+MOVE_WINDOW_W, title, MAIN_TEXT_COLOUR, 0, FNT_Size_1926 );
	
    DisplayArchiveMoveFolderList();
    DisplayMoveLine();
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

	// Store the currently displayed screen area where we're about to put our pop-up window on.
    moveWindowCopy = TAP_Osd_SaveBox(rgn, MOVE_WINDOW_X, MOVE_WINDOW_Y, MOVE_WINDOW_W, MOVE_WINDOW_H);

    DisplayArchiveMoveWindow();
}


//-----------------------------------------------------------------------
//
void CloseArchiveMoveWindow(void)
{
    int index;
     
	moveWindowShowing = FALSE;
	TAP_Osd_RestoreBox(rgn, MOVE_WINDOW_X, MOVE_WINDOW_Y, MOVE_WINDOW_W, MOVE_WINDOW_H, moveWindowCopy);
	TAP_MemFree(moveWindowCopy);
	// Release any memory allocated to the "moveFiles" array.   
    for ( index=1; index<=numberOfDestinationFolders ; index++)
	{
       if (moveFiles[index] != NULL) 
       {
           TAP_MemFree(moveFiles[index]);  // Free any previously allocated memory
           moveFiles[index] = NULL;        // Set the pointer to NULL so that it's all cleared.                   
       }
    }  
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
							DisplayArchiveMoveWindow();

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
							DisplayArchiveMoveWindow();
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
								           MoveAFile( myfiles[CurrentDirNumber][chosenLine]->directory,  moveFiles[chosenFolderLine]->name, myfiles[CurrentDirNumber][chosenLine]->name);
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



