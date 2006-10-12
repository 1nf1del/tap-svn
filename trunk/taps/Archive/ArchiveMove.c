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

static bool  returnFromMove;
static bool  listMoved;
static byte* moveWindowCopy;

#define MAX_MOVE_FOLDERS_SHOWN 10


// Position of the Information Screen
#define MOVE_WINDOW_W 476
#define MOVE_WINDOW_H 416
#define MOVE_WINDOW_X ((MAX_SCREEN_X-MOVE_WINDOW_W)/2)   // Centre the move window
#define MOVE_WINDOW_Y ((MAX_SCREEN_Y-MOVE_WINDOW_H)/2-10)   // Centre the move window

#define MOVE_OPTION_W 82                                  // Width  of Info Options Buttons
#define MOVE_OPTION_H 43                                   // Height of Info Options Buttons
#define MOVE_OPTION_X  (MOVE_WINDOW_X + 30)              // Starting x-position for first Option button  82
#define MOVE_OPTION_Y  (MOVE_WINDOW_Y + MOVE_WINDOW_H - MOVE_OPTION_H - 15)             // Starting y-position for Option buttons.
#define MOVE_OPTION_X_SPACE   (MOVE_OPTION_W+3)         // Space between options on Move window.

#define MOVE_COMMAND_OPTIONS 5  // Number of command buttons in the move window.



//------------------------------------------------------------------
//
void DisplayMoveLine(void)
{
	TAP_Osd_PutGd( rgn, MOVE_OPTION_X+(0*MOVE_OPTION_X_SPACE), MOVE_OPTION_Y, &_keyblue82x43Gd, FALSE );
    PrintCenter(rgn, MOVE_OPTION_X+(0*MOVE_OPTION_X_SPACE), MOVE_OPTION_Y+9, MOVE_OPTION_X+(0*MOVE_OPTION_X_SPACE)+MOVE_OPTION_W, "Move", MAIN_TEXT_COLOUR, 0, FNT_Size_1926 );

	TAP_Osd_PutGd( rgn, MOVE_OPTION_X+(1*MOVE_OPTION_X_SPACE), MOVE_OPTION_Y, &_keyblue82x43Gd, FALSE );
	PrintCenter(rgn, MOVE_OPTION_X+(1*MOVE_OPTION_X_SPACE), MOVE_OPTION_Y+9, MOVE_OPTION_X+(1*MOVE_OPTION_X_SPACE)+MOVE_OPTION_W, "Cancel", MAIN_TEXT_COLOUR, 0, FNT_Size_1926 );

	TAP_Osd_PutGd( rgn, MOVE_OPTION_X+(2*MOVE_OPTION_X_SPACE), MOVE_OPTION_Y, &_keyblue82x43Gd, FALSE );
	PrintCenter(rgn, MOVE_OPTION_X+(2*MOVE_OPTION_X_SPACE), MOVE_OPTION_Y+9, MOVE_OPTION_X+(2*MOVE_OPTION_X_SPACE)+MOVE_OPTION_W, "Rename", MAIN_TEXT_COLOUR, 0, FNT_Size_1926 );

	TAP_Osd_PutGd( rgn, MOVE_OPTION_X+(3*MOVE_OPTION_X_SPACE), MOVE_OPTION_Y, &_keyblue82x43Gd, FALSE );
	PrintCenter(rgn, MOVE_OPTION_X+(3*MOVE_OPTION_X_SPACE), MOVE_OPTION_Y+9, MOVE_OPTION_X+(3*MOVE_OPTION_X_SPACE)+MOVE_OPTION_W, "Move", MAIN_TEXT_COLOUR, 0, FNT_Size_1926 );

	TAP_Osd_PutGd( rgn, MOVE_OPTION_X+(4*MOVE_OPTION_X_SPACE), MOVE_OPTION_Y, &_keyblue82x43Gd, FALSE );
	PrintCenter(rgn, MOVE_OPTION_X+(4*MOVE_OPTION_X_SPACE), MOVE_OPTION_Y+0, MOVE_OPTION_X+(4*MOVE_OPTION_X_SPACE)+MOVE_OPTION_W, "Reset", MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );
	PrintCenter(rgn, MOVE_OPTION_X+(4*MOVE_OPTION_X_SPACE), MOVE_OPTION_Y+19, MOVE_OPTION_X+(4*MOVE_OPTION_X_SPACE)+MOVE_OPTION_W, "Progress", MAIN_TEXT_COLOUR, 0, FNT_Size_1622);


	switch ( moveCommandOption )
	{

		case 0 :
					TAP_Osd_PutGd( rgn, MOVE_OPTION_X+(0*MOVE_OPTION_X_SPACE), MOVE_OPTION_Y, &_keygreen82x43Gd, FALSE );
				    PrintCenter(rgn, MOVE_OPTION_X+(0*MOVE_OPTION_X_SPACE), MOVE_OPTION_Y+9, MOVE_OPTION_X+(0*MOVE_OPTION_X_SPACE)+MOVE_OPTION_W, "Move", COLOR_Yellow, 0, FNT_Size_1926 );
                    break;
					
		case 1 :
					TAP_Osd_PutGd( rgn, MOVE_OPTION_X+(1*MOVE_OPTION_X_SPACE), MOVE_OPTION_Y, &_keygreen82x43Gd, FALSE );
					PrintCenter(rgn, MOVE_OPTION_X+(1*MOVE_OPTION_X_SPACE), MOVE_OPTION_Y+9, MOVE_OPTION_X+(1*MOVE_OPTION_X_SPACE)+MOVE_OPTION_W, "Cancel", COLOR_Yellow, 0, FNT_Size_1926 );
                    break;
					
		case 2 :
					TAP_Osd_PutGd( rgn, MOVE_OPTION_X+(2*MOVE_OPTION_X_SPACE), MOVE_OPTION_Y, &_keygreen82x43Gd, FALSE );
					PrintCenter(rgn, MOVE_OPTION_X+(2*MOVE_OPTION_X_SPACE), MOVE_OPTION_Y+9, MOVE_OPTION_X+(2*MOVE_OPTION_X_SPACE)+MOVE_OPTION_W, "Rename", COLOR_Yellow, 0, FNT_Size_1926 );
                    break;
				
		case 3 :
					TAP_Osd_PutGd( rgn, MOVE_OPTION_X+(3*MOVE_OPTION_X_SPACE), MOVE_OPTION_Y, &_keygreen82x43Gd, FALSE );
					PrintCenter(rgn, MOVE_OPTION_X+(3*MOVE_OPTION_X_SPACE), MOVE_OPTION_Y+9, MOVE_OPTION_X+(3*MOVE_OPTION_X_SPACE)+MOVE_OPTION_W, "Move", COLOR_Yellow, 0, FNT_Size_1926 );
                    break;
				
		case 4 :
					TAP_Osd_PutGd( rgn, MOVE_OPTION_X+(4*MOVE_OPTION_X_SPACE), MOVE_OPTION_Y, &_keygreen82x43Gd, FALSE );
					PrintCenter(rgn, MOVE_OPTION_X+(4*MOVE_OPTION_X_SPACE), MOVE_OPTION_Y+0, MOVE_OPTION_X+(4*MOVE_OPTION_X_SPACE)+MOVE_OPTION_W, "Reset", COLOR_Yellow, 0, FNT_Size_1622 );
					PrintCenter(rgn, MOVE_OPTION_X+(4*MOVE_OPTION_X_SPACE), MOVE_OPTION_Y+19, MOVE_OPTION_X+(4*MOVE_OPTION_X_SPACE)+MOVE_OPTION_W, "Progress", COLOR_Yellow, 0, FNT_Size_1622 );
                    break;
				
	}
}



//-----------------------------------------------------------------------
//
void DisplayArchiveMoveWindow()
{
    char title[50], str1[200], str2[200], str3[200];
    int  fontHeight, fontSize;
//    byte* keyboardWindowCopy;
    
    
    void DisplayFolderList()
    {
	     char   filesString[10], foldersString[10];
	     
	     // Format the number of files and subfolders in this folder.
	     TAP_SPrint(filesString,   "%d", currentFolder.numberOfRecordings);
	     TAP_SPrint(foldersString, "%d", currentFolder.numberOfFolders);
	     
         TAP_SPrint(title, "Folder Details");
	     PrintCenter( rgn, MOVE_WINDOW_X, MOVE_WINDOW_Y +  14, MOVE_WINDOW_X+MOVE_WINDOW_W, title, MAIN_TEXT_COLOUR, 0, FNT_Size_1926 );
	     
         PrintRight( rgn, MOVE_WINDOW_X+10,  MOVE_WINDOW_Y+ 100, MOVE_WINDOW_X+180, "Folder Name:", HEADING_TEXT_COLOUR, 0, FNT_Size_1926 );
         PrintLeft ( rgn, MOVE_WINDOW_X+190, MOVE_WINDOW_Y+ 100, MOVE_WINDOW_X+MOVE_WINDOW_W-8, currentFolder.name, MAIN_TEXT_COLOUR, 0, FNT_Size_1926 );
         
	     PrintRight( rgn, MOVE_WINDOW_X+10,  MOVE_WINDOW_Y+ 150, MOVE_WINDOW_X+180, "Number of Files:", HEADING_TEXT_COLOUR, 0, FNT_Size_1926 );
         PrintLeft ( rgn, MOVE_WINDOW_X+190, MOVE_WINDOW_Y+ 150, MOVE_WINDOW_X+MOVE_WINDOW_W-8, filesString, MAIN_TEXT_COLOUR, 0, FNT_Size_1926 );

	     PrintRight( rgn, MOVE_WINDOW_X+10,  MOVE_WINDOW_Y+ 200, MOVE_WINDOW_X+180, "Subfolders:", HEADING_TEXT_COLOUR, 0, FNT_Size_1926 );
         PrintLeft ( rgn, MOVE_WINDOW_X+190, MOVE_WINDOW_Y+ 200, MOVE_WINDOW_X+MOVE_WINDOW_W-8, foldersString, MAIN_TEXT_COLOUR, 0, FNT_Size_1926 );

    }

    // Display the pop-up window - will also clear any old text if we are doing a refresh.
    TAP_Osd_PutGd( rgn, MOVE_WINDOW_X, MOVE_WINDOW_Y, &_popup476x416Gd, TRUE );
	
	
    // Display the appropriate info text.
    switch (currentFile.attr)
    {
           case ATTR_FOLDER: DisplayFolderInfo();
                             break;
                
           default:          DisplayFileInfo();
                             break;
    }
    
    DisplayInfoLine();
}



//-----------------------------------------------------------------------
//
void ActivateMoveWindow()
{
	moveWindowShowing = TRUE;
	moveCommandOption = 0; // Default to the "OK" option.
	fileMoved = FALSE;
    listMoved   = FALSE;

	// Store the currently displayed screen area where we're about to put our pop-up window on.
    moveWindowCopy = TAP_Osd_SaveBox(rgn, MOVE_WINDOW_X, MOVE_WINDOW_Y, MOVE_WINDOW_W, MOVE_WINDOW_H);

    DisplayArchiveMoveWindow();
}


//-----------------------------------------------------------------------
//
void CloseArchiveInfoWindow(void)
{
	moveWindowShowing = FALSE;
	TAP_Osd_RestoreBox(rgn, MOVE_WINDOW_X, MOVE_WINDOW_Y, MOVE_WINDOW_W, MOVE_WINDOW_H, moveWindowCopy);
	TAP_MemFree(moveWindowCopy);
}


//------------
//
dword ArchiveInfoKeyHandler(dword key)
{
    if ( keyboardWindowShowing ) { KeyboardKeyHandler( key ); return; }				// handle rename keyboard

	switch ( key )
	{
		case RKEY_ChDown :	if ( chosenFolderLine < MAX_MOVE_FOLDERS_SHOWN ) 
                            {
                                 chosenFolderLine++;         // 0=hidden - can't hide once cursor moved
                                 printLine++;
                                 if (printLine > NUMBER_OF_LINES) printLine=1;
                                 listMoved   = TRUE;
                            }     
                            else 
                            {    // Roll past bottom to start again.
                                 chosenFolderLine=1;	
                                 printLine=1;
                                 listMoved   = TRUE;
                            }
							if ( MAX_MOVE_FOLDERS_SHOWN == 0 ) chosenFolderLine = 0;					// don't move the cursor if no files shown
                            // Make sure we're not pointing to the parent directory, or a recording file entry.
                            while ((myfiles[CurrentDirNumber][chosenFolderLine]->attr == PARENT_DIR_ATTR) || (myfiles[CurrentDirNumber][chosenFolderLine]->isRecording))
                            {
                                 // If we are, then keep moving forward until we're not.                             
                                 if ( chosenFolderLine < MAX_MOVE_FOLDERS_SHOWN ) 
                                 {
                                      chosenFolderLine++;         // 0=hidden - can't hide once cursor moved
                                      printLine++;
                                      if (printLine == 0) printLine = NUMBER_OF_LINES;
                                 }     
                                 else 
                                 {    // Roll past bottom to start again.
                                      chosenFolderLine=1;	
                                      printLine=1;
                                 }
                            }     
                            currentFile   = *myfiles[CurrentDirNumber][chosenFolderLine];
                            currentFolder = *myfolders[myfiles[CurrentDirNumber][chosenFolderLine]->directoryNumber];
							DisplayArchiveInfoWindow();

/*	
						page = (chosenFolderLine-1) / NUMBER_OF_LINES;
                            
		
            				if (printLine!=1)  // We're still on the same page.
							{
								if ( oldChosenLine > 0 ) DisplayArchiveLine(oldChosenLine,oldPrintLine);
								DisplayArchiveLine(chosenFolderLine,printLine);
							}
							else DrawArchiveList();
                            UpdateListClock();
							UpdateSelectionNumber();
*/
							break;

		case RKEY_ChUp :	if ( chosenFolderLine > 1 ) 
                            {
                                 chosenFolderLine--; 
                                 printLine--;
                                 if (printLine == 0) printLine = NUMBER_OF_LINES;
                                 listMoved   = TRUE;
                            }     
                            else 
                            {    // Roll past top to start again.
                                 chosenFolderLine = MAX_MOVE_FOLDERS_SHOWN;
                                 printLine  = NUMBER_OF_LINES;
                                 listMoved   = TRUE;
                            }
							if ( MAX_MOVE_FOLDERS_SHOWN == 0 ) chosenFolderLine = 0;					// not strictly needed - included in above logic

                            // Make sure we're not pointing to the parent directory, or a recording file entry.
                            while ((myfiles[CurrentDirNumber][chosenFolderLine]->attr == PARENT_DIR_ATTR) || (myfiles[CurrentDirNumber][chosenFolderLine]->isRecording))
                            {
                                 // If we are, then keep moving back until we're not.                             
                                 if ( chosenFolderLine > 1 ) 
                                 {
                                     chosenFolderLine--; 
                                     printLine--;
                                     if (printLine == 0) printLine = NUMBER_OF_LINES;
                                 }     
                                 else 
                                 {    // Roll past top to start again.
                                     chosenFolderLine = MAX_MOVE_FOLDERS_SHOWN;
                                     printLine  = NUMBER_OF_LINES;
                                 }
                            }     
                            currentFile   = *myfiles[CurrentDirNumber][chosenFolderLine];
                            currentFolder = *myfolders[myfiles[CurrentDirNumber][chosenFolderLine]->directoryNumber];

							DisplayArchiveInfoWindow();
/*							
							page = (chosenFolderLine-1) / NUMBER_OF_LINES;
                            if (printLine == NUMBER_OF_LINES)    // We've scrolled off the top of the page, so determine where to start next.
                            {
                                if (chosenFolderLine < NUMBER_OF_LINES) // But if we're right near the top with only a small number of entries, only
                                {                                 // show the small list.
                                     printLine = chosenFolderLine;
                                }
                                DrawArchiveList();    // Since this is a page change, redraw the entire list. 
                            }    
                            else                        // Just unhighlight the old line, and redraw the new selection.
							{
								if ( oldChosenLine > 0 ) DisplayArchiveLine(oldChosenLine, oldPrintLine);
					      		DisplayArchiveLine(chosenFolderLine, printLine);
							}

                            UpdateListClock();
							UpdateSelectionNumber();
*/
							break;

		case RKEY_VolUp:	if ( moveCommandOption < (MOVE_COMMAND_OPTIONS-1) ) moveCommandOption++;
							else moveCommandOption = 0;
							DisplayInfoLine();
		     				break;

		case RKEY_VolDown:	if ( moveCommandOption > 0 ) moveCommandOption--;
							else moveCommandOption = (MOVE_COMMAND_OPTIONS-1);
							DisplayInfoLine();
							break;


		case RKEY_Ok :		switch ( moveCommandOption )
							{
								case 0 :   // Move
							               CloseArchiveMoveWindow();	// Close the move window
                                           returnFromInfo = TRUE;		// will cause a redraw of file list
                                           break;	

								case 1 :   // Cancel
							               CloseArchiveMoveWindow();	// Close the move window
                                           returnFromInfo = TRUE;		// will cause a redraw of file list
                                           break;	

								default :  
                                           break;
							}

							break;


        case RKEY_Info :
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

}



