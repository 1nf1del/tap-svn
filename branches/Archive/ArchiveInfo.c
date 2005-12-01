/************************************************************
			Part of the ukEPG project
		This module displays info for the archive entries

Name	: ArchiveInfo.c
Author	: kidhazy
Version	: 0.3
For		: Topfield TF5x00 series PVRs
Licence	:
Descr.	:
Usage	:
History	: v0.0 kidhazy 25-10-05  Inception date

	Last change:  USE   1 Aug 105    8:34 pm
**************************************************************/

static bool  returnFromInfo;
static byte* infoWindowCopy;


// Position of the Information Screen
#define INFO_WINDOW_W 476
#define INFO_WINDOW_H 416
#define INFO_WINDOW_X ((MAX_SCREEN_X-INFO_WINDOW_W)/2)   // Centre the info window
#define INFO_WINDOW_Y ((MAX_SCREEN_Y-INFO_WINDOW_H)/2-10)   // Centre the info window

#define INFO_OPTION_W 102                                  // Width  of Info Options Buttons
#define INFO_OPTION_H 43                                   // Height of Info Options Buttons
#define INFO_OPTION_X  (INFO_WINDOW_X + 30)              // Starting x-position for first Option button  82
#define INFO_OPTION_Y  (INFO_WINDOW_Y + INFO_WINDOW_H - INFO_OPTION_H - 15)             // Starting y-position for Option buttons.
#define INFO_OPTION_X_SPACE   (INFO_OPTION_W+3)         // Space between options on Info window.

#define INFO_COMMAND_OPTIONS 4  // Number of command buttons in the info window.

//------------
//
void FormatInfoDuration(int dur, char* str)
{
    char	buf[20];
	int 	hour, min;

    hour = dur / 60;
    min  = dur % 60;
    
    str[0]='\0';  // Blank out any existing string.
    
	if ( hour )
	{	
		sprintf( buf, "%d Hour", hour);
		strcat(str, buf);
        if ( hour > 1 )
			    strcat(str, "s");
	}

	if ( min )
	{	
		if ( hour )   // If there was hours, then seperate with a space.
			strcat(str, " ");
		sprintf( buf, "%d Min", min);
		strcat(str, buf);
	    if ( min > 1 )
			  strcat(str, "s");
	}
	
}	


//------------------------------------------------------------------
//
void DisplayInfoLine(void)
{
	TAP_Osd_PutGd( rgn, INFO_OPTION_X+(0*INFO_OPTION_X_SPACE), INFO_OPTION_Y, &_bigkeyblueGd, FALSE );
    PrintCenter(rgn, INFO_OPTION_X+(0*INFO_OPTION_X_SPACE), INFO_OPTION_Y+9, INFO_OPTION_X+(0*INFO_OPTION_X_SPACE)+INFO_OPTION_W, "OK", MAIN_TEXT_COLOUR, 0, FNT_Size_1926 );

	TAP_Osd_PutGd( rgn, INFO_OPTION_X+(1*INFO_OPTION_X_SPACE), INFO_OPTION_Y, &_bigkeyblueGd, FALSE );
	PrintCenter(rgn, INFO_OPTION_X+(1*INFO_OPTION_X_SPACE), INFO_OPTION_Y+9, INFO_OPTION_X+(1*INFO_OPTION_X_SPACE)+INFO_OPTION_W, "Delete", MAIN_TEXT_COLOUR, 0, FNT_Size_1926 );

	TAP_Osd_PutGd( rgn, INFO_OPTION_X+(2*INFO_OPTION_X_SPACE), INFO_OPTION_Y, &_bigkeyblueGd, FALSE );
	PrintCenter(rgn, INFO_OPTION_X+(2*INFO_OPTION_X_SPACE), INFO_OPTION_Y+9, INFO_OPTION_X+(2*INFO_OPTION_X_SPACE)+INFO_OPTION_W, "Rename", MAIN_TEXT_COLOUR, 0, FNT_Size_1926 );

	TAP_Osd_PutGd( rgn, INFO_OPTION_X+(3*INFO_OPTION_X_SPACE), INFO_OPTION_Y, &_bigkeyblueGd, FALSE );
	PrintCenter(rgn, INFO_OPTION_X+(3*INFO_OPTION_X_SPACE), INFO_OPTION_Y+0, INFO_OPTION_X+(3*INFO_OPTION_X_SPACE)+INFO_OPTION_W, "Reset", MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );
	PrintCenter(rgn, INFO_OPTION_X+(3*INFO_OPTION_X_SPACE), INFO_OPTION_Y+19, INFO_OPTION_X+(3*INFO_OPTION_X_SPACE)+INFO_OPTION_W, "Progress", MAIN_TEXT_COLOUR, 0, FNT_Size_1622);

	switch ( infoCommandOption )
	{

		case 0 :
					TAP_Osd_PutGd( rgn, INFO_OPTION_X+(0*INFO_OPTION_X_SPACE), INFO_OPTION_Y, &_bigkeygreenGd, FALSE );
				    PrintCenter(rgn, INFO_OPTION_X+(0*INFO_OPTION_X_SPACE), INFO_OPTION_Y+9, INFO_OPTION_X+(0*INFO_OPTION_X_SPACE)+INFO_OPTION_W, "OK", COLOR_Yellow, 0, FNT_Size_1926 );
                    break;
					
		case 1 :
					TAP_Osd_PutGd( rgn, INFO_OPTION_X+(1*INFO_OPTION_X_SPACE), INFO_OPTION_Y, &_bigkeygreenGd, FALSE );
					PrintCenter(rgn, INFO_OPTION_X+(1*INFO_OPTION_X_SPACE), INFO_OPTION_Y+9, INFO_OPTION_X+(1*INFO_OPTION_X_SPACE)+INFO_OPTION_W, "Delete", COLOR_Yellow, 0, FNT_Size_1926 );
                    break;
					
		case 2 :
					TAP_Osd_PutGd( rgn, INFO_OPTION_X+(2*INFO_OPTION_X_SPACE), INFO_OPTION_Y, &_bigkeygreenGd, FALSE );
					PrintCenter(rgn, INFO_OPTION_X+(2*INFO_OPTION_X_SPACE), INFO_OPTION_Y+9, INFO_OPTION_X+(2*INFO_OPTION_X_SPACE)+INFO_OPTION_W, "Rename", COLOR_Yellow, 0, FNT_Size_1926 );
                    break;
				
		case 3 :
					TAP_Osd_PutGd( rgn, INFO_OPTION_X+(3*INFO_OPTION_X_SPACE), INFO_OPTION_Y, &_bigkeygreenGd, FALSE );
					PrintCenter(rgn, INFO_OPTION_X+(3*INFO_OPTION_X_SPACE), INFO_OPTION_Y+0, INFO_OPTION_X+(3*INFO_OPTION_X_SPACE)+INFO_OPTION_W, "Reset", COLOR_Yellow, 0, FNT_Size_1622 );
					PrintCenter(rgn, INFO_OPTION_X+(3*INFO_OPTION_X_SPACE), INFO_OPTION_Y+19, INFO_OPTION_X+(3*INFO_OPTION_X_SPACE)+INFO_OPTION_W, "Progress", COLOR_Yellow, 0, FNT_Size_1622 );
                    break;
				
	}
}



//-----------------------------------------------------------------------
//
void DisplayArchiveInfoWindow()
{
    char title[50], str1[200], str2[200], str3[200];
    
    
    void DisplayFileInfo()
    {
         int	hour, min; 
         byte   month, day, weekDay;
	     word	year;
	     int	endHour, endMin;
	     char   timeString[20], dayString[10], monthString[3], dateString[20], datetimeString[50], sizeString[20], durString[20];

	     byte 	currentWeekDay, currentMonth, currentDay, currentHour, currentMin, currentSec;
	     int    offsetHour, offsetMin;
	     word 	currentYear, currentMJD, currentTime;
	
	     word      	chosenMJD, mjd;

         // Get the GMT offset 
         offsetHour = GMToffsetOption / 60;
         offsetMin  = GMToffsetOption % 60;
         
         // Extract Event start and end times
	     hour = (currentFile.eventStartHour);		
	     min = (currentFile.eventStartMin);

	     endHour = (currentFile.eventEndHour);	
	     endMin = (currentFile.eventEndMin);
	     
	     // Apply the offsets.
	     hour = hour + offsetHour;
	     min  = min  + offsetMin;
	     if (min < 0) 
	     {
             min = -min;  // Make the minutes positive.
             hour--;      // descrease the hours by 1.
         }
	     if (min > 60)
	     {
             min = min%60;  // Get the minutes.
             hour++;        // increase the hours by 1.
         }
	     
	     endHour = endHour + offsetHour;
	     endMin  = endMin  + offsetMin;
	     if (endMin < 0) 
	     {
             endMin = -endMin;  // Make the minutes positive.
             endHour--;         // descrease the hours by 1.
         }
	     if (endMin >= 60)
	     {
             endMin = endMin%60;  // Get the minutes.
             endHour++;           // increase the hours by 1.
         }
         
         	

	     TAP_SPrint(timeString, "%d:%02d%cm ~ %d:%02d%cm", (hour+23)%12 + 1, min, hour >= 12 ? 'p':'a',(endHour+23)%12 + 1, endMin, endHour >= 12 ? 'p':'a');
	
         // Extract the Event date
	     mjd = currentFile.eventStartMJD;
	     TAP_ExtractMjd( mjd, &year, &month, &day, &weekDay) ;

	     WeekdayToAlpha( weekDay, dayString );    // Get the name of the day.
         MonthToAlpha( month, monthString );      // Get the name of the month.

	     TAP_SPrint(dateString, "%0.3s, %d %s %d", dayString, day, monthString, year);  // Format as:  "Mon, 10 Oct 2005"
	     
         TAP_SPrint(datetimeString,"%s %s", dateString, timeString);	     
	     
	     // Extract size
	     FormatFileSize(currentFile.size, sizeString, 1);  // Get size as xxxMB
	     
         FormatInfoDuration(currentFile.recDuration, durString );		

         TAP_SPrint(title, "Program Details");
	     PrintCenter( rgn, INFO_WINDOW_X, INFO_WINDOW_Y +  14, INFO_WINDOW_X+INFO_WINDOW_W, title, MAIN_TEXT_COLOUR, 0, FNT_Size_1926 );
	     
         PrintRight( rgn, INFO_WINDOW_X+10,  INFO_WINDOW_Y+ 55, INFO_WINDOW_X+120, "Program:", HEADING_TEXT_COLOUR, 0, FNT_Size_1622 );
         PrintLeft ( rgn, INFO_WINDOW_X+130, INFO_WINDOW_Y+ 55, INFO_WINDOW_X+INFO_WINDOW_W-8, currentFile.eventName, MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );
         
	     PrintRight( rgn, INFO_WINDOW_X+10, INFO_WINDOW_Y+ 80, INFO_WINDOW_X+120, "Channel:", HEADING_TEXT_COLOUR, 0, FNT_Size_1622 );
         PrintLeft ( rgn, INFO_WINDOW_X+130, INFO_WINDOW_Y+ 80, INFO_WINDOW_X+INFO_WINDOW_W-8, currentFile.serviceName, MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );

	     PrintRight( rgn, INFO_WINDOW_X+10, INFO_WINDOW_Y+105, INFO_WINDOW_X+120, "Date & Time:", HEADING_TEXT_COLOUR, 0, FNT_Size_1622 );
         PrintLeft ( rgn, INFO_WINDOW_X+130, INFO_WINDOW_Y+105, INFO_WINDOW_X+INFO_WINDOW_W-8, datetimeString, MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );
    
	     PrintRight( rgn, INFO_WINDOW_X+10, INFO_WINDOW_Y+142, INFO_WINDOW_X+120, "File Name:", HEADING_TEXT_COLOUR, 0, FNT_Size_1622 );
         PrintLeft ( rgn, INFO_WINDOW_X+130, INFO_WINDOW_Y+142, INFO_WINDOW_X+INFO_WINDOW_W-8, currentFile.name, MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );

	     PrintRight( rgn, INFO_WINDOW_X+10, INFO_WINDOW_Y+167, INFO_WINDOW_X+120, "File Size:", HEADING_TEXT_COLOUR, 0, FNT_Size_1622 );
         PrintLeft ( rgn, INFO_WINDOW_X+130, INFO_WINDOW_Y+167, INFO_WINDOW_X+INFO_WINDOW_W-8, sizeString, MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );

	     PrintRight( rgn, INFO_WINDOW_X+130, INFO_WINDOW_Y+167, INFO_WINDOW_X+310, "Duration:", HEADING_TEXT_COLOUR, 0, FNT_Size_1622 );
         PrintLeft ( rgn, INFO_WINDOW_X+320, INFO_WINDOW_Y+167, INFO_WINDOW_X+INFO_WINDOW_W-8, durString, MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );

	     PrintLeft(  rgn, INFO_WINDOW_X+10, INFO_WINDOW_Y+200, INFO_WINDOW_X+120, "Description:", HEADING_TEXT_COLOUR, 0, FNT_Size_1622 );
    
         LastWrapPutStr_Start = 0;  // Reset "first character" pointer to start for Event Name & Description
         LastWrapPutStr_P = 0;      // Reset "last character" pointer to start for Event Name & Description
         LastWrapPutStr_Y = INFO_WINDOW_Y+225; // Set the starting y-coordinate for the Event Name & Description

         WrapPutStr_StartPos = LastWrapPutStr_P; // Set the start position of the string to where we got up to last time.
         WrapPutStr( rgn, currentFile.eventDescName, INFO_WINDOW_X+10, LastWrapPutStr_Y, INFO_WINDOW_W-15, MAIN_TEXT_COLOUR, 0, 4, FNT_Size_1622, 0);


         //
         // Print Extended Event Information.
         //
         LastWrapPutStr_Start = 0;  // Reset "first character" pointer to start for Extended Event information.
         LastWrapPutStr_P = 0;      // Reset "last character" pointer to start for Extended Event information.

         ExtInfoRows = ((INFO_WINDOW_Y + INFO_WINDOW_H) - LastWrapPutStr_Y - INFO_OPTION_H -20) / 19 ;   // Calculate how many lines are left available for Extended Info with a font height of 19.
         if( currentFile.extInfo )
         {
            WrapPutStr_StartPos = LastWrapPutStr_P; // Set the start position of the string to where we got up to last time.
            WrapPutStr( rgn, currentFile.extInfo, INFO_WINDOW_X+10, LastWrapPutStr_Y, INFO_WINDOW_W-15, MAIN_TEXT_COLOUR, 0, ExtInfoRows, FNT_Size_1419, 0);
         }
	     

    }


    void DisplayFolderInfo()
    {
	     char   filesString[10], foldersString[10];
	     
	     // Format the number of files and subfolders in this folder.
	     TAP_SPrint(filesString,   "%d", currentFile.numberOfFiles);
	     TAP_SPrint(foldersString, "%d", currentFile.numberOfFolders);
	     
         TAP_SPrint(title, "Folder Details");
	     PrintCenter( rgn, INFO_WINDOW_X, INFO_WINDOW_Y +  14, INFO_WINDOW_X+INFO_WINDOW_W, title, MAIN_TEXT_COLOUR, 0, FNT_Size_1926 );
	     
         PrintRight( rgn, INFO_WINDOW_X+10,  INFO_WINDOW_Y+ 100, INFO_WINDOW_X+180, "Folder Name:", HEADING_TEXT_COLOUR, 0, FNT_Size_1926 );
         PrintLeft ( rgn, INFO_WINDOW_X+190, INFO_WINDOW_Y+ 100, INFO_WINDOW_X+INFO_WINDOW_W-8, currentFile.name, MAIN_TEXT_COLOUR, 0, FNT_Size_1926 );
         
	     PrintRight( rgn, INFO_WINDOW_X+10,  INFO_WINDOW_Y+ 150, INFO_WINDOW_X+180, "Number of Files:", HEADING_TEXT_COLOUR, 0, FNT_Size_1926 );
         PrintLeft ( rgn, INFO_WINDOW_X+190, INFO_WINDOW_Y+ 150, INFO_WINDOW_X+INFO_WINDOW_W-8, filesString, MAIN_TEXT_COLOUR, 0, FNT_Size_1926 );

	     PrintRight( rgn, INFO_WINDOW_X+10,  INFO_WINDOW_Y+ 200, INFO_WINDOW_X+180, "Subfolders:", HEADING_TEXT_COLOUR, 0, FNT_Size_1926 );
         PrintLeft ( rgn, INFO_WINDOW_X+190, INFO_WINDOW_Y+ 200, INFO_WINDOW_X+INFO_WINDOW_W-8, foldersString, MAIN_TEXT_COLOUR, 0, FNT_Size_1926 );

    }

    // Display the pop-up window - will also clear any old text if we are doing a refresh.
    TAP_Osd_PutGd( rgn, INFO_WINDOW_X, INFO_WINDOW_Y, &_popup476x416Gd, TRUE );
	
    
     	     
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
void ActivateInfoWindow()
{
	infoWindowShowing = TRUE;
	infoCommandOption = 0; // Default to the "OK" option.
	fileRenamed = FALSE;

	// Store the currently displayed screen area where we're about to put our pop-up window on.
    infoWindowCopy = TAP_Osd_SaveBox(rgn, INFO_WINDOW_X, INFO_WINDOW_Y, INFO_WINDOW_W, INFO_WINDOW_H);
#ifdef WIN32  // If testing on WIN32 platform 
TAP_Osd_FillBox( rgn,INFO_WINDOW_X, INFO_WINDOW_Y, INFO_WINDOW_W, INFO_WINDOW_H, FILL_COLOUR );				// clear the screen
#endif          

    DisplayArchiveInfoWindow();
}


//-----------------------------------------------------------------------
//
void CloseArchiveInfoWindow(void)
{
	infoWindowShowing = FALSE;
	TAP_Osd_RestoreBox(rgn, INFO_WINDOW_X, INFO_WINDOW_Y, INFO_WINDOW_W, INFO_WINDOW_H, infoWindowCopy);
	TAP_MemFree(infoWindowCopy);
}


//------------
//
dword ArchiveInfoKeyHandler(dword key)
{
    if ( keyboardWindowShowing ) { KeyboardKeyHandler( key ); return; }				// handle rename keyboard

	switch ( key )
	{
		case RKEY_VolUp:	if ( infoCommandOption < (INFO_COMMAND_OPTIONS-1) ) infoCommandOption++;
							else infoCommandOption = 0;
							DisplayInfoLine();
		     				break;

		case RKEY_VolDown:	if ( infoCommandOption > 0 ) infoCommandOption--;
							else infoCommandOption = (INFO_COMMAND_OPTIONS-1);
							DisplayInfoLine();
							break;


		case RKEY_Ok :		switch ( infoCommandOption )
							{
								case 0 :   // OK
							               CloseArchiveInfoWindow();	// Close the info window
                                           returnFromInfo = TRUE;		// will cause a redraw of file list
                                           break;	

								case 1 :   // DELETE
                                           if (myfiles[CurrentDirNumber][chosenLine].attr != PARENT_DIR_ATTR) ActivateDeleteWindow(myfiles[CurrentDirNumber][chosenLine].name,myfiles[CurrentDirNumber][chosenLine].attr);
                                           break;

								case 2 :   // RENAME
                                           EditFileName();
                                           break;

								case 3 :   // RESET PROGRESS
                                           if ((myfiles[CurrentDirNumber][chosenLine].attr == ATTR_FOLDER) || (myfiles[CurrentDirNumber][chosenLine].attr == PARENT_DIR_ATTR)) break;  // Ignore when looking at folders.
                                           DeleteProgressInfo(TRUE);
                                           fileReset = TRUE;
                                           infoCommandOption = INFO_OK_OPTION;  // Move to the OK option.
                                           DisplayInfoLine();                   // Redraw command buttons with new selection.
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



//------------
//
void initialiseArchiveInfo(void)
{
    infoWindowShowing = FALSE;
	infoCommandOption = 0;  // Default to the "OK" option.
	returnFromInfo = FALSE;
	fileReset = FALSE;

}



