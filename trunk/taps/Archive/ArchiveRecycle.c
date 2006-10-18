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
    strncpy( baseFileName, currentFile.name, 256 );      // Copy the current filename
	StripFileName( baseFileName );                       // Strip off the ".rec"
    strncpy( fileName, baseFileName, 256 );  // Copy the stripped off filename in case we need to rename on a move.
	strcat( fileName, RECYCLED_STRING );	                 // Append ".rec.del" to the base filename

    // Check if the file already exists.
    fileAlreadyExists = TAP_Hdd_Exist(fileName);
    
    // If the 'renamed' recycle-bin file already exists, we'll add a counter to the end
    // of the file name and try again.
    while (fileAlreadyExists)
    {
         fileIncrement++;         // Increase the counter for the number to append to the filename.
         if (fileIncrement > 2)   // If there are more than 99999 files in the recycle bin with the same name, abort the delete.
         {
              sprintf(str1,"More than 99999 files in the bin called:");
              ShowMessageWin( rgn, "Recyle Bin Failure.", str1, str2, 500 ); 
              fileDeleted = FALSE;
              return;   // Jump out of recycle option.
         }   
         sprintf(counter, "-%d", fileIncrement);      // Create the counter text
         strncpy( fileName, baseFileName, 256);                    // Copy back the original filename
         strcat(  fileName, counter);                      // Append the counter text to the filename
	     strcat(  fileName, RECYCLED_STRING );	                  // Append the ".del" to the end.
         fileAlreadyExists = TAP_Hdd_Exist(fileName);      // Check if the file exists.
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
       

//------------------------------------------------------------------
//
void DisplayRecycleBinWindowLine(void)
{
    // Display all the action buttons on the bottom of the Recycle window.
    
    // "OK" button.
	TAP_Osd_PutGd( rgn, RECYCLE_OPTION_X+(0*RECYCLE_OPTION_X_SPACE), RECYCLE_OPTION_Y,   &_keyblue82x43Gd, FALSE );
    PrintCenter(rgn,    RECYCLE_OPTION_X+(0*RECYCLE_OPTION_X_SPACE), RECYCLE_OPTION_Y+9, RECYCLE_OPTION_X+(0*RECYCLE_OPTION_X_SPACE)+RECYCLE_OPTION_W, "OK", MAIN_TEXT_COLOUR, 0, FNT_Size_1926 );

    // "Delete" button.
	TAP_Osd_PutGd( rgn, RECYCLE_OPTION_X+(1*RECYCLE_OPTION_X_SPACE), RECYCLE_OPTION_Y,   &_keyblue82x43Gd, FALSE );
	PrintCenter(rgn,    RECYCLE_OPTION_X+(1*RECYCLE_OPTION_X_SPACE), RECYCLE_OPTION_Y+9, RECYCLE_OPTION_X+(1*RECYCLE_OPTION_X_SPACE)+RECYCLE_OPTION_W, "Delete", MAIN_TEXT_COLOUR, 0, FNT_Size_1926 );

    // "Rename" button.
	TAP_Osd_PutGd( rgn, RECYCLE_OPTION_X+(2*RECYCLE_OPTION_X_SPACE), RECYCLE_OPTION_Y,   &_keyblue82x43Gd, FALSE );
	PrintCenter(rgn,    RECYCLE_OPTION_X+(2*RECYCLE_OPTION_X_SPACE), RECYCLE_OPTION_Y+9, RECYCLE_OPTION_X+(2*RECYCLE_OPTION_X_SPACE)+RECYCLE_OPTION_W, "Rename", MAIN_TEXT_COLOUR, 0, FNT_Size_1926 );

    // "Move" button.
	TAP_Osd_PutGd( rgn, RECYCLE_OPTION_X+(3*RECYCLE_OPTION_X_SPACE), RECYCLE_OPTION_Y,   &_keyblue82x43Gd, FALSE );
	PrintCenter(rgn,    RECYCLE_OPTION_X+(3*RECYCLE_OPTION_X_SPACE), RECYCLE_OPTION_Y+9, RECYCLE_OPTION_X+(3*RECYCLE_OPTION_X_SPACE)+RECYCLE_OPTION_W, "Move", MAIN_TEXT_COLOUR, 0, FNT_Size_1926 );

    // "Reset Progress" button.
	TAP_Osd_PutGd( rgn, RECYCLE_OPTION_X+(4*RECYCLE_OPTION_X_SPACE), RECYCLE_OPTION_Y,    &_keyblue82x43Gd, FALSE );
	PrintCenter(rgn,    RECYCLE_OPTION_X+(4*RECYCLE_OPTION_X_SPACE), RECYCLE_OPTION_Y+0,  RECYCLE_OPTION_X+(4*RECYCLE_OPTION_X_SPACE)+RECYCLE_OPTION_W, "Reset", MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );
	PrintCenter(rgn,    RECYCLE_OPTION_X+(4*RECYCLE_OPTION_X_SPACE), RECYCLE_OPTION_Y+19, RECYCLE_OPTION_X+(4*RECYCLE_OPTION_X_SPACE)+RECYCLE_OPTION_W, "Progress", MAIN_TEXT_COLOUR, 0, FNT_Size_1622);


    // Highlight which ever action button has been selected.
	switch ( recycleCommandOption )
	{

		case 0 :    // "OK"               
					TAP_Osd_PutGd( rgn, RECYCLE_OPTION_X+(0*RECYCLE_OPTION_X_SPACE), RECYCLE_OPTION_Y, &_keygreen82x43Gd, FALSE );
				    PrintCenter(rgn,    RECYCLE_OPTION_X+(0*RECYCLE_OPTION_X_SPACE), RECYCLE_OPTION_Y+9, RECYCLE_OPTION_X+(0*RECYCLE_OPTION_X_SPACE)+RECYCLE_OPTION_W, "OK", COLOR_Yellow, 0, FNT_Size_1926 );
                    break;
					
		case 1 :    // "Delete"
					TAP_Osd_PutGd( rgn, RECYCLE_OPTION_X+(1*RECYCLE_OPTION_X_SPACE), RECYCLE_OPTION_Y, &_keygreen82x43Gd, FALSE );
					PrintCenter(rgn,    RECYCLE_OPTION_X+(1*RECYCLE_OPTION_X_SPACE), RECYCLE_OPTION_Y+9, RECYCLE_OPTION_X+(1*RECYCLE_OPTION_X_SPACE)+RECYCLE_OPTION_W, "Delete", COLOR_Yellow, 0, FNT_Size_1926 );
                    break;
					
		case 2 :    // "Rename"
					TAP_Osd_PutGd( rgn, RECYCLE_OPTION_X+(2*RECYCLE_OPTION_X_SPACE), RECYCLE_OPTION_Y, &_keygreen82x43Gd, FALSE );
					PrintCenter(rgn,    RECYCLE_OPTION_X+(2*RECYCLE_OPTION_X_SPACE), RECYCLE_OPTION_Y+9, RECYCLE_OPTION_X+(2*RECYCLE_OPTION_X_SPACE)+RECYCLE_OPTION_W, "Rename", COLOR_Yellow, 0, FNT_Size_1926 );
                    break;
				
		case 3 :    // "Move"
					TAP_Osd_PutGd( rgn, RECYCLE_OPTION_X+(3*RECYCLE_OPTION_X_SPACE), RECYCLE_OPTION_Y, &_keygreen82x43Gd, FALSE );
					PrintCenter(rgn,    RECYCLE_OPTION_X+(3*RECYCLE_OPTION_X_SPACE), RECYCLE_OPTION_Y+9, RECYCLE_OPTION_X+(3*RECYCLE_OPTION_X_SPACE)+RECYCLE_OPTION_W, "Move", COLOR_Yellow, 0, FNT_Size_1926 );
                    break;
				
		case 4 :
					TAP_Osd_PutGd( rgn, RECYCLE_OPTION_X+(4*RECYCLE_OPTION_X_SPACE), RECYCLE_OPTION_Y, &_keygreen82x43Gd, FALSE );
					PrintCenter(rgn,    RECYCLE_OPTION_X+(4*RECYCLE_OPTION_X_SPACE), RECYCLE_OPTION_Y+0, RECYCLE_OPTION_X+(4*RECYCLE_OPTION_X_SPACE)+RECYCLE_OPTION_W, "Reset", COLOR_Yellow, 0, FNT_Size_1622 );
					PrintCenter(rgn,    RECYCLE_OPTION_X+(4*RECYCLE_OPTION_X_SPACE), RECYCLE_OPTION_Y+19, RECYCLE_OPTION_X+(4*RECYCLE_OPTION_X_SPACE)+RECYCLE_OPTION_W, "Progress", COLOR_Yellow, 0, FNT_Size_1622 );
                    break;
				
	}
}



//-----------------------------------------------------------------------
//
void DisplayArchiveRecycleBinWindow()
{
    char title[50], str1[200], str2[200], str3[200];
    int  fontHeight, fontSize;
//    byte* keyboardWindowCopy;
    TAP_Osd_PutGd( rgn, RECYCLE_WINDOW_X, RECYCLE_WINDOW_Y, &_popup476x416Gd, TRUE );


}



//-----------------------------------------------------------------------
//
void ActivateRecycleBinWindow()
{
	recycleBinWindowShowing = TRUE;
	recycleCommandOption = 0; // Default to the "OK" option.
	fileRestored = FALSE;
    listMoved   = FALSE;

	// Store the currently displayed screen area where we're about to put our pop-up window on.
    recycleBinWindowCopy = TAP_Osd_SaveBox(rgn, RECYCLE_WINDOW_X, RECYCLE_WINDOW_Y, RECYCLE_WINDOW_W, RECYCLE_WINDOW_H);

    DisplayArchiveRecycleBinWindow();
}


//-----------------------------------------------------------------------
//
void CloseRecycleBinWindow(void)
{
	recycleBinWindowShowing = FALSE;
	TAP_Osd_RestoreBox(rgn, RECYCLE_WINDOW_X, RECYCLE_WINDOW_Y, RECYCLE_WINDOW_W, RECYCLE_WINDOW_H, recycleBinWindowCopy);
	TAP_MemFree(recycleBinWindowCopy);
}


//------------
//
dword ArchiveRecycleBinWindowKeyHandler(dword key)
{

	switch ( key )
	{
		case RKEY_ChDown :	if ( chosenLine < maxShown ) 
                            {
                                 chosenLine++;         // 0=hidden - can't hide once cursor moved
                                 printLine++;
                                 if (printLine > NUMBER_OF_LINES) printLine=1;
                                 listMoved   = TRUE;
                            }     
                            else 
                            {    // Roll past bottom to start again.
                                 chosenLine=1;	
                                 printLine=1;
                                 listMoved   = TRUE;
                            }
							if ( maxShown == 0 ) chosenLine = 0;					// don't move the cursor if no files shown
                            // Make sure we're not pointing to the parent directory, or a recording file entry.
                            while ((myfiles[CurrentDirNumber][chosenLine]->attr == PARENT_DIR_ATTR) || (myfiles[CurrentDirNumber][chosenLine]->isRecording))
                            {
                                 // If we are, then keep moving forward until we're not.                             
                                 if ( chosenLine < maxShown ) 
                                 {
                                      chosenLine++;         // 0=hidden - can't hide once cursor moved
                                      printLine++;
                                      if (printLine == 0) printLine = NUMBER_OF_LINES;
                                 }     
                                 else 
                                 {    // Roll past bottom to start again.
                                      chosenLine=1;	
                                      printLine=1;
                                 }
                            }     
                            currentFile   = *myfiles[CurrentDirNumber][chosenLine];
                            currentFolder = *myfolders[myfiles[CurrentDirNumber][chosenLine]->directoryNumber];
							DisplayArchiveInfoWindow();

/*	
						page = (chosenLine-1) / NUMBER_OF_LINES;
                            
		
            				if (printLine!=1)  // We're still on the same page.
							{
								if ( oldChosenLine > 0 ) DisplayArchiveLine(oldChosenLine,oldPrintLine);
								DisplayArchiveLine(chosenLine,printLine);
							}
							else DrawArchiveList();
                            UpdateListClock();
							UpdateSelectionNumber();
*/
							break;

		case RKEY_ChUp :	if ( chosenLine > 1 ) 
                            {
                                 chosenLine--; 
                                 printLine--;
                                 if (printLine == 0) printLine = NUMBER_OF_LINES;
                                 listMoved   = TRUE;
                            }     
                            else 
                            {    // Roll past top to start again.
                                 chosenLine = maxShown;
                                 printLine  = NUMBER_OF_LINES;
                                 listMoved   = TRUE;
                            }
							if ( maxShown == 0 ) chosenLine = 0;					// not strictly needed - included in above logic

                            // Make sure we're not pointing to the parent directory, or a recording file entry.
                            while ((myfiles[CurrentDirNumber][chosenLine]->attr == PARENT_DIR_ATTR) || (myfiles[CurrentDirNumber][chosenLine]->isRecording))
                            {
                                 // If we are, then keep moving back until we're not.                             
                                 if ( chosenLine > 1 ) 
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
                            }     
                            currentFile   = *myfiles[CurrentDirNumber][chosenLine];
                            currentFolder = *myfolders[myfiles[CurrentDirNumber][chosenLine]->directoryNumber];

							DisplayArchiveInfoWindow();
/*							
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
*/
							break;

		case RKEY_VolUp:	if ( recycleCommandOption < (RECYCLE_COMMAND_OPTIONS-1) ) recycleCommandOption++;
							else recycleCommandOption = 0;
							DisplayRecycleBinWindowLine();
		     				break;

		case RKEY_VolDown:	if ( recycleCommandOption > 0 ) recycleCommandOption--;
							else recycleCommandOption = (RECYCLE_COMMAND_OPTIONS-1);
							DisplayRecycleBinWindowLine();
							break;


		case RKEY_Ok :		switch ( recycleCommandOption )
							{
								case 0 :   // OK
							               CloseArchiveInfoWindow();	// Close the info window
                                           returnFromInfo = TRUE;		// will cause a redraw of file list
                                           break;	

								case 1 :   // DELETE
                                           if (myfiles[CurrentDirNumber][chosenLine]->attr != PARENT_DIR_ATTR) ActivateDeleteWindow(myfiles[CurrentDirNumber][chosenLine]->name,myfiles[CurrentDirNumber][chosenLine]->attr);
                                           break;

								case 2 :   // RENAME
                                           EditFileName();
                                           break;

								case 3 :   // MOVE
//                                           MoveAFile();
                                           break;

								case 4 :   // RESET PROGRESS
                                           if ((myfiles[CurrentDirNumber][chosenLine]->attr == ATTR_FOLDER) || (myfiles[CurrentDirNumber][chosenLine]->attr == PARENT_DIR_ATTR)) break;  // Ignore when looking at folders.
                                           DeleteProgressInfo(CurrentDirNumber, chosenLine, TRUE);
                                           fileReset = TRUE;
                                           recycleCommandOption = RECYCLE_OK_OPTION;  // Move to the OK option.
                                           DisplayRecycleBinWindowLine();                   // Redraw command buttons with new selection.
                                           break;              

								default :  
                                           break;
							}

							break;


        case RKEY_Info :
		case RKEY_Exit : 	
                            CloseArchiveInfoWindow();			    // Close the info window
							returnFromInfo = TRUE;					// will cause a redraw of file list
							break;
							
		case RKEY_Mute :	return key;


		default :
			break;
	}
	return 0;
}    
