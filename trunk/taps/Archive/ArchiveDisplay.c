/************************************************************
	    Part of the ukEPG project
	This module displays the timer list

Name	: ArchiveDisplay.c
Author	: kidhazy
Version	: 0.01
For		: Topfield TF5x00 series PVRs
Descr.	:
Licence	:
Usage	:
History	: v0.0 kidhazy:
	
	Last change:
**************************************************************/
//
//

#define NUMBER_OF_LINES numberLinesOption
#define Y1_OFFSET yOffsetOption
#define Y1_STEP 42					// was 44

#define LIST_PROGRESS_BAR_WIDTH (column2Width-96)  // Width of the Progress Bar in the display list.  (default is 286-96 = 190)
#define INFO_PROGRESS_BAR_WIDTH 200  // Width of the Progress Bar in the info window.
#define RECORDING_PROGRESS_BAR_WIDTH 200  // Width of the Progress Bar in the recording status info.

#define ARCHIVE_HELP_LINES1  7                          // Number of help lines in 1st column
#define ARCHIVE_HELP_LINES2  7                          // Number of help lines in 2nd column
#define ARCHIVE_HELP_HEIGHT 490 //269
#define ARCHIVE_HELP_WIDTH 476 //520
#define ARCHIVE_HELP_BASE_X  ((MAX_SCREEN_X-ARCHIVE_HELP_WIDTH)/2)
#define ARCHIVE_HELP_BASE_Y	((MAX_SCREEN_Y-ARCHIVE_HELP_HEIGHT)/2)
#define ARCHIVE_HELP_TEXT_X1  (ARCHIVE_HELP_BASE_X + 10)
#define ARCHIVE_HELP_TEXT_X2  (ARCHIVE_HELP_BASE_X + (ARCHIVE_HELP_WIDTH/2)+10)
#define ARCHIVE_HELP_TEXT_Y   (ARCHIVE_HELP_BASE_Y + 65)
#define ARCHIVE_HELP_TITLE_Y  (ARCHIVE_HELP_BASE_Y + 15)
#define ARCHIVE_HELP_KEY_W  80
#define ARCHIVE_HELP_LINE_SPACING 19



static TYPE_Window Win;


static bool windowShowing;
static int page;
static byte* archiveHelpWindowCopy;



//------------------------------------------------------------------
//
void ResetScreenColumns( void )
{
     // Time & Duration Column
     switch (column4Option)  
     {
            case 0:  // HH:MM (dur)
                     column2Width=COLUMN2_OPTION1_W;  // Reset FileName column width
                     column4Width=COLUMN4_OPTION1_W;  // Reset Time&Dur column width
                     break;                     
            
            case 1:  // HH:MM ~ HH:MM
                     column2Width=COLUMN2_OPTION2_W;  // Reset FileName column width
                     column4Width=COLUMN4_OPTION2_W;  // Reset Time&Dur column width
                     break;                     
            
            case 2:  // dur
                     column2Width=COLUMN2_OPTION3_W;  // Reset FileName column width
                     column4Width=COLUMN4_OPTION3_W;  // Reset Time&Dur column width
                     break;                     
            
            default:
                     column2Width=COLUMN2_OPTION1_W;  // Reset FileName column width
                     column4Width=COLUMN4_OPTION1_W;  // Reset Time&Dur column width
                     break;                     
     }
     
     // Logo & Size Column
     column5Width=COLUMN5_DEFAULT_W;  // Reset Logo/Size column width
     switch (column5Option)  
     {
            case 0:  // Logo
                     break;                     

            case 1:  //  size in MB
                     column2Width -= 20;  // Reduce FileName column width
                     column5Width += 20;  // Increase Logo/Size column width
                     break;                     
            
            case 3:  // size in MB without unit
            case 4:  // size in GB without unit
                     break;                     

            case 2:  // size in GB
            case 5:  // size in MB/GB
                     column2Width -= 20;  // Reduce FileName column width
                     column5Width += 20;  // Increase Logo/Size column width
                     break;                     
     }
     
}




//------------------------------------------------------------------
//
void DisplayArchiveHelp( void )
{
    char str[200];
    char key[20][15], help[20][30];
    
    int i, l;

	archiveHelpWindowShowing = TRUE;
    
    TAP_SPrint( key[1] ,"Up/Down :");
    TAP_SPrint(help[1] ,"Move Up/Down.");
    
    TAP_SPrint( key[2] ,"Left/Right :");
    TAP_SPrint(help[2] ,"Page Up/Down.");
    
    TAP_SPrint( key[3] ,"OK (File):");
    TAP_SPrint(help[3] ,"Restart playback.");
    
    TAP_SPrint( key[4] ,"OK (Folder):");
    TAP_SPrint(help[4] ,"Open selected folder.");
    
    TAP_SPrint( key[5] ,"Play :");
    TAP_SPrint(help[5] ,"Playback from start.");
    
    TAP_SPrint( key[6] ,"Blue :");
    TAP_SPrint(help[6] ,"Restart last file.");
    
    TAP_SPrint( key[7] ,"White :");
    TAP_SPrint(help[7] ,"Delete file/folder.");
    
    TAP_SPrint( key[8] ,"Green :");
    TAP_SPrint(help[8] ,"Create new folder.");
    
    TAP_SPrint( key[9] ,"Pause :");
    TAP_SPrint(help[9] ,"Change sort order.");
    
    TAP_SPrint( key[10],"Exit :");
    TAP_SPrint(help[10],"Exit file list.");
    
    TAP_SPrint( key[11],"Stop :");
    TAP_SPrint(help[11],"Stop rec/playback.");
        
    TAP_SPrint( key[12],"FileList :");
    TAP_SPrint(help[12],"Standard filelist.");
    
    TAP_SPrint( key[13],"Info :");
    TAP_SPrint(help[13],"Detailed info.");
    
    TAP_SPrint( key[14],"Menu :");
    TAP_SPrint(help[14],"Open config menu.");
    
    TAP_SPrint( key[15],"1-9: ");
    TAP_SPrint(help[15],"Jump to line.");
        
    TAP_SPrint( key[16]," :");
    TAP_SPrint(help[16],"");
        
    TAP_SPrint( key[17]," :");
    TAP_SPrint(help[17],"");
        
    TAP_SPrint( key[18]," :");
    TAP_SPrint(help[18],"");
        
    TAP_SPrint( key[19]," :");
    TAP_SPrint(help[19],"");
        
    TAP_SPrint( key[20]," :");
    TAP_SPrint(help[20],"");
        
        
	// Store the currently displayed screen area where we're about to put our pop-up window on.
    archiveHelpWindowCopy = TAP_Osd_SaveBox(rgn, ARCHIVE_HELP_BASE_X, ARCHIVE_HELP_BASE_Y, ARCHIVE_HELP_WIDTH, ARCHIVE_HELP_HEIGHT);

    // Display the pop-up window.
    if ( unitModelType==TF5800t) // Display the UK style remote
       TAP_Osd_PutGd( rgn, ARCHIVE_HELP_BASE_X, ARCHIVE_HELP_BASE_Y, &_archive_help_ukGd, TRUE );
    else  
       TAP_Osd_PutGd( rgn, ARCHIVE_HELP_BASE_X, ARCHIVE_HELP_BASE_Y, &_archive_help_ozGd, TRUE );
/*    
    TAP_Osd_PutGd( rgn, ARCHIVE_HELP_BASE_X, ARCHIVE_HELP_BASE_Y, &_popup520x269Gd, TRUE );

    TAP_SPrint(str, "Filelist Help");
	PrintCenter( rgn, ARCHIVE_HELP_TEXT_X1, ARCHIVE_HELP_TITLE_Y, ARCHIVE_HELP_BASE_X+ARCHIVE_HELP_WIDTH, str, MAIN_TEXT_COLOUR, 0, FNT_Size_1926 );
	 
    i = 1;
    
    // Display the 1st column of help.
    for (l = 0; l< ARCHIVE_HELP_LINES1; l++)
    {     
    	PrintRight( rgn, ARCHIVE_HELP_TEXT_X1,                  ARCHIVE_HELP_TEXT_Y + (l * ARCHIVE_HELP_LINE_SPACING), ARCHIVE_HELP_TEXT_X1+ARCHIVE_HELP_KEY_W, key[i] , HEADING_TEXT_COLOUR, 0, FNT_Size_1419 );
        PrintLeft ( rgn, ARCHIVE_HELP_TEXT_X1+ARCHIVE_HELP_KEY_W+10, ARCHIVE_HELP_TEXT_Y + (l * ARCHIVE_HELP_LINE_SPACING), ARCHIVE_HELP_BASE_X+ARCHIVE_HELP_WIDTH,  help[i], MAIN_TEXT_COLOUR,    0, FNT_Size_1419 );
        i++;
    }
                 
    // Display the 2nd column of help.
    for (l = 0; l< ARCHIVE_HELP_LINES2; l++)
    {     
    	PrintRight( rgn, ARCHIVE_HELP_TEXT_X2,                  ARCHIVE_HELP_TEXT_Y + (l * ARCHIVE_HELP_LINE_SPACING), ARCHIVE_HELP_TEXT_X2+ARCHIVE_HELP_KEY_W, key[i] , HEADING_TEXT_COLOUR, 0, FNT_Size_1419 );
        PrintLeft ( rgn, ARCHIVE_HELP_TEXT_X2+ARCHIVE_HELP_KEY_W+10, ARCHIVE_HELP_TEXT_Y + (l * ARCHIVE_HELP_LINE_SPACING), ARCHIVE_HELP_BASE_X+ARCHIVE_HELP_WIDTH,  help[i], MAIN_TEXT_COLOUR,    0, FNT_Size_1419 );
        i++;
    }
    
    // Print some instructions at the bottom.
    TAP_SPrint(str, "(Press EXIT or RED      to close this help window)");
	PrintCenter( rgn, ARCHIVE_HELP_TEXT_X1, ARCHIVE_HELP_BASE_Y + ARCHIVE_HELP_HEIGHT - 35, ARCHIVE_HELP_BASE_X+ARCHIVE_HELP_WIDTH, str, MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );
    TAP_Osd_PutGd( rgn, ARCHIVE_HELP_TEXT_X1+207, ARCHIVE_HELP_BASE_Y + ARCHIVE_HELP_HEIGHT - 35+2, &_redoval38x19Gd, TRUE );
*/
}


//-----------------------------------------------
//
void CloseArchiveHelp( void )
{
	archiveHelpWindowShowing = FALSE;
	TAP_Osd_RestoreBox(rgn, ARCHIVE_HELP_BASE_X, ARCHIVE_HELP_BASE_Y, ARCHIVE_HELP_WIDTH, ARCHIVE_HELP_HEIGHT, archiveHelpWindowCopy);
	TAP_MemFree(archiveHelpWindowCopy);
	
}


//-----------------------------------------------
//
void ArchiveHelpKeyHandler( dword key )
{
	switch ( key )
	{
		case RKEY_Exit:
        case RKEY_Red :	CloseArchiveHelp();									// quick access key : cancel and exit
						break;

		default :		break;
	}
}




//------------
//
void DeterminePrintingLine(int line)
{
    // Determine where to start printing the selected line.
    appendIntToLogfile("DeterminePrintingLine: Called with line=%d", line, WARNING);

    if ((line <= maxShown) && (maxShown < NUMBER_OF_LINES)) // Only one page to display, so print on current line.
    {
        printLine = line;
        return;  
    }    
    
    if (line == maxShown) 
        printLine = NUMBER_OF_LINES;
    else
    if (line >= (maxShown - (NUMBER_OF_LINES/2)))
    {
        printLine = NUMBER_OF_LINES - (maxShown - line);              
    }
    else
    if (line <= (NUMBER_OF_LINES/2))
    {
        printLine = line;         // Display from the start of the list.
    }
    else
    {
        printLine = (NUMBER_OF_LINES/2);
    } 
    
    page = (line-1) / NUMBER_OF_LINES;               // Calculate the starting page.
    appendIntToLogfile("DeterminePrintingLine: Finished with printLine=%d", printLine, WARNING);
}


//------------
//
void DetermineStartingLine(int *line)
{
    int i, numberOfFiles;
    appendIntToLogfile("DetermineStartingLine: Called with line=%d", *line, WARNING);
    
    numberOfFiles = myfolders[CurrentDirNumber]->numberOfFiles;

    *line = numberOfFiles;                            // Select the last file when we start in the "DataFiles" directory.	

    // If the user has chosen to sort by NAME, then we start by pointing to the first file entry.
    if (sortOrder == SORT_NAME_OPTION)
    {
        *line = 1;                            // Select the first folder/file.
        // Scan through the files in the current directory to find the first file.
        for (i=1; i<=numberOfFiles; i++)
        {
            if (myfiles[CurrentDirNumber][i]->attr == ATTR_TS) 
            {   // We've found a match, so allocate line number.
                *line = i;
                break;
            }
        }
    }                    

    printLine = NUMBER_OF_LINES;                      // And move to the last line.
    
    GetPlaybackInfo();                                // Get the current playback info (if any)
    if (inPlaybackMode)
    {
        // Scan through the files in the current directory to see if we have a match.
        for (i=1; i<=numberOfFiles; i++)
        {
            if ((myfiles[CurrentDirNumber][i]->startCluster == CurrentPlaybackFile->startCluster) && (strncmp(myfiles[CurrentDirNumber][i]->name,CurrentPlaybackFile->name, TS_FILE_NAME_SIZE)==0))
            {   // We've found a match, so allocate line number.
                *line = i;
                break;
            }
        }
    }                    

    DeterminePrintingLine(*line);
    appendIntToLogfile("DetermineStartingLine: Finished with line=%d", *line, WARNING);
}

//------------
//
void DrawFreeSpaceBar()
{
    int freePercent;
    dword hoursRemaining, minutesRemaining;
    dword freeSpace, totalSpace;
    char str[40];
    
    freeSpace  = TAP_Hdd_FreeSize();
    totalSpace = TAP_Hdd_TotalSize();
    
    freePercent = (freeSpace*100) / totalSpace;   // Calculate percent disk free.
    
    // Calculate remaining time, taking into consideration 1 hour of timeshift buffer (ie 1 x recordingRateOption)
    hoursRemaining   = ((freeSpace-recordingRateOption)/recordingRateOption);
    minutesRemaining = (((freeSpace-recordingRateOption)*100)/((recordingRateOption*100)/60)) - (hoursRemaining*60);  // Use '100' multiplier to help with integer maths.
    DisplayProgressBar(memRgn, totalSpace-freeSpace, totalSpace, DISK_INFO_X, DISK_INFO_Y, DISK_PROGRESS_BAR_WIDTH, 20, COLOR_Black, 1, COLOR_Gray, 0);

    TAP_SPrint(str,"%02d%% %dMB Remaining: %01dhr %01dmin  ", freePercent, freeSpace, hoursRemaining, minutesRemaining);
	TAP_Osd_PutStringAf1419( memRgn, DISK_INFO_X+DISK_PROGRESS_BAR_WIDTH+5, DISK_INFO_Y, DISK_INFO_X+INFO_TEXT_W, str, INFO_COLOUR, INFO_FILL_COLOUR );
    
}     
    
    

//------------
//
void DisplayInstructions(void)
{
    TAP_Osd_FillBox( memRgn, INSTR_AREA_X, INSTR_AREA_Y, INSTR_AREA_W, INSTR_AREA_H, INFO_FILL_COLOUR );	// clear the area

    switch (recycleWindowMode)
    {
           case FALSE:  // We are in the standard file viewing mode.
                        TAP_Osd_PutGd( memRgn, INSTR_AREA_X, INSTR_AREA_Y+1, &_whiteoval38x19Gd, TRUE );	
                    	TAP_Osd_PutStringAf1622( memRgn, INSTR_AREA_X+40, INSTR_AREA_Y, INSTR_AREA_X + INSTR_AREA_W, "Delete File", INFO_COLOUR, INFO_FILL_COLOUR );
                    
                    	TAP_Osd_PutGd( memRgn, INSTR_AREA_X, INSTR_AREA_Y+26, &_infooval38x19Gd, TRUE );	
                    	TAP_Osd_PutStringAf1622( memRgn, INSTR_AREA_X+40, INSTR_AREA_Y+25, INSTR_AREA_X + INSTR_AREA_W, "Information", INFO_COLOUR, INFO_FILL_COLOUR );
                    
                        TAP_Osd_PutGd( memRgn, INSTR_AREA_X, INSTR_AREA_Y+51, &_redoval38x19Gd, TRUE );
                    	TAP_Osd_PutStringAf1622( memRgn, INSTR_AREA_X+40, INSTR_AREA_Y+50, INSTR_AREA_X + INSTR_AREA_W, "Help", INFO_COLOUR, INFO_FILL_COLOUR );
                    	break;
                    	
           case TRUE:   // We are in the recycle bin viewing mode.
                        TAP_Osd_PutGd( memRgn, INSTR_AREA_X, INSTR_AREA_Y+1, &_whiteoval38x19Gd, TRUE );	
                    	TAP_Osd_PutStringAf1622( memRgn, INSTR_AREA_X+40, INSTR_AREA_Y, INSTR_AREA_X + INSTR_AREA_W, "Delete File", INFO_COLOUR, INFO_FILL_COLOUR );
                    
                    	TAP_Osd_PutGd( memRgn, INSTR_AREA_X, INSTR_AREA_Y+26, &_blueoval38x19Gd, TRUE );	
                    	TAP_Osd_PutStringAf1622( memRgn, INSTR_AREA_X+40, INSTR_AREA_Y+25, INSTR_AREA_X + INSTR_AREA_W, "Restore File", INFO_COLOUR, INFO_FILL_COLOUR );
                    
                        TAP_Osd_PutGd( memRgn, INSTR_AREA_X, INSTR_AREA_Y+51, &_redoval38x19Gd, TRUE );
                    	TAP_Osd_PutStringAf1622( memRgn, INSTR_AREA_X+40, INSTR_AREA_Y+50, INSTR_AREA_X + INSTR_AREA_W, "Help", INFO_COLOUR, INFO_FILL_COLOUR );
                    	break;
    }
                    	

}	
     

//------------
//
void SortList(int sortOrder)
{
    //  Sorts the current list of files.
    //
    //  sortOrder = 0    sort by date (earliest first)
    //  sortOrder = 1    sort by name
    //  sortOrder = 2    sort by svcNum
    //  sortOrder = 3    sort by size
    // 
	int 	i, i2, swaps, numberOfFiles;
    TYPE_My_Files *tempfile;
    
    void SwapEntries(void)
    {
        // Do swap by swapping pointers around - faster than moving contents. 
		tempfile = myfiles[CurrentDirNumber][i+1];
		myfiles[CurrentDirNumber][i+1] = myfiles[CurrentDirNumber][i];
		myfiles[CurrentDirNumber][i]   = tempfile ;
		swaps++;         
    }	

    numberOfFiles = myfolders[CurrentDirNumber]->numberOfFiles;

    strcpy(sortTitle,"[by name]");   // Default the sort title to by name.
    //	Always sort files in the array by name 
	do { 
		for ( i = swaps = 1 ; ( i < numberOfFiles ) ; i += 1)
		{
			if ( strcmp(myfiles[CurrentDirNumber][i]->sortName,myfiles[CurrentDirNumber][i+1]->sortName)>0 ) SwapEntries();
			if ( strcmp(myfiles[CurrentDirNumber][i]->sortName,myfiles[CurrentDirNumber][i+1]->sortName)==0 ) // If the names are the same, sort on date/time.
            {
                 if ( myfiles[CurrentDirNumber][i]->mjd > myfiles[CurrentDirNumber][i+1]->mjd ) SwapEntries();
                 if ( myfiles[CurrentDirNumber][i]->mjd == myfiles[CurrentDirNumber][i+1]->mjd )
			     {
				      if ( myfiles[CurrentDirNumber][i]->hour > myfiles[CurrentDirNumber][i+1]->hour ) SwapEntries();
				      if ( myfiles[CurrentDirNumber][i]->hour == myfiles[CurrentDirNumber][i+1]->hour )
				      {
					       if ( myfiles[CurrentDirNumber][i]->min > myfiles[CurrentDirNumber][i+1]->min ) SwapEntries();
				      }
			     }
            }     
		}
	} 
	while ( swaps > 1 );

    if (sortOrder == SORT_DATE_OPTION)
    {	
       strcpy(sortTitle,"[by date]");   // Set the Sort Title.
       //	Sort files in the array by date/time 
	   do { 
		for ( i = swaps = 1 ; ( i < numberOfFiles ) ; i += 1)
		{
            if ( myfiles[CurrentDirNumber][i]->mjd > myfiles[CurrentDirNumber][i+1]->mjd ) SwapEntries();
            if ( myfiles[CurrentDirNumber][i]->mjd == myfiles[CurrentDirNumber][i+1]->mjd )
			{
				if ( myfiles[CurrentDirNumber][i]->hour > myfiles[CurrentDirNumber][i+1]->hour ) SwapEntries();
				if ( myfiles[CurrentDirNumber][i]->hour == myfiles[CurrentDirNumber][i+1]->hour )
				{
					if ( myfiles[CurrentDirNumber][i]->min > myfiles[CurrentDirNumber][i+1]->min ) SwapEntries();
				}
			}
		}
	   } 
	   while ( swaps > 1 );
    }   


    if (sortOrder == SORT_SVCNUM_OPTION)
    {	
       strcpy(sortTitle,"[by channel]");   // Set the Sort Title.
       //	Sort files in the array by svcNum 
	   do { 
	   	for ( i = swaps = 1 ; ( i < numberOfFiles ) ; i += 1)
		{
            if ( myfiles[CurrentDirNumber][i]->svcNum > myfiles[CurrentDirNumber][i+1]->svcNum ) SwapEntries();
		}
	   } 
	   while ( swaps > 1 );
    }   


    if (sortOrder == SORT_SIZE_OPTION)
    {	
       strcpy(sortTitle,"[by size]");   // Set the Sort Title.
       //	Sort files in the array by size 
	   do { 
	   	for ( i = swaps = 1 ; ( i < numberOfFiles ) ; i += 1)
		{
            if ( myfiles[CurrentDirNumber][i]->size > myfiles[CurrentDirNumber][i+1]->size ) SwapEntries();
		}
	   } 
	   while ( swaps > 1 );
    }   


    //	Sort files in the array by attribute so Parent Folder (250/240) is first, Folders (242) are
    //  second, and Files (209) are last or first as specified by configuration option. 
	do { 
		for ( i = swaps = 1 ; ( i < numberOfFiles ) ; i += 1)
		{
			switch (folderSortOrder)
			{
                   case 0: // Put Folders at top. 
                           if ( myfiles[CurrentDirNumber][i]->attr < myfiles[CurrentDirNumber][i+1]->attr ) SwapEntries(); 
                           break;
                           
                   case 1: // Put Folders at bottom. 
                           if ( myfiles[CurrentDirNumber][i]->attr > myfiles[CurrentDirNumber][i+1]->attr ) SwapEntries(); 
                           break;
            }
		}
	} 
	while ( swaps > 1 );

}

//------------
//
void FormatFileSize (ulong64 size, char* str, int option)
{
     dword mbsize, gbsize, digit1, digit2;
        
     mbsize = (size + 524288) >> 20; // round to nearest megabyte (2**20 bytes)
     gbsize = (mbsize * 100)/1024;  // Calculate GB multiplied by 100 so we can make a x.xx number.
     digit1 = gbsize / 100;  // Get the whole number.
     digit2 = gbsize % 100;  // Get the two decimal points.
     
     switch (option)
     {
            case 1: sprintf (str, "%dMB", mbsize);
                    break;
                    
            case 2: sprintf (str, "%01d.%02dGB", digit1, digit2);
                    break;
                    
            case 3: sprintf (str, "%d", mbsize);
                    break;
                    
            case 4: sprintf (str, "%01d.%02d", digit1, digit2);
                    break;
                    
            case 5: if (mbsize < 1000) // Print as MB
                       sprintf (str, "%dMB", mbsize);
                    else   // Print as GB
                       sprintf (str, "%01d.%02dGB", digit1, digit2);
                    break;
                    
            default: sprintf (str, "%dMB", mbsize);
                    break;
     }
}


//------------
//
void WeekdayToAlpha (byte weekday, char *str)
{
     switch (weekday)
     {
            case 0: strcpy (str, "Monday"); break;
            case 1: strcpy (str, "Tuesday"); break;
            case 2: strcpy (str, "Wednesday"); break;
            case 3: strcpy (str, "Thursday"); break;
            case 4: strcpy (str, "Friday"); break;
            case 5: strcpy (str, "Saturday"); break;
            case 6: strcpy (str, "Sunday"); break;
            default: strcpy (str, "BAD"); break;
     }
}


//------------
//
void MonthToAlpha (byte month, char *str)
{
	switch ( month )
	{
		case 1:	strcpy (str,"Jan"); break;
		case 2:	strcpy (str,"Feb"); break;
		case 3:	strcpy (str,"Mar"); break;
		case 4:	strcpy (str,"Apr"); break;
		case 5:	strcpy (str,"May"); break;
		case 6:	strcpy (str,"Jun"); break;
		case 7:	strcpy (str,"Jul"); break;
		case 8:	strcpy (str,"Aug"); break;
		case 9:	strcpy (str,"Sep"); break;
		case 10:strcpy (str,"Oct"); break;
		case 11:strcpy (str,"Nov"); break;
	   	case 12:strcpy (str,"Dec"); break;
		default : strcpy (str,"BAD"); break;
	}
}	


//------------
//
void DrawGraphicBorders(void)
{
    switch (borderOption)
    {
           case 0:       // Traditional Blue screen border.  
                         #ifdef WIN32
                            TAP_Osd_FillBox( rgn, 0, 0, 720, 576, COLOR_DarkGray );				// clear the screen
                         #else    
                            TAP_Osd_FillBox( rgn, 0, 0, 720, 576, FILL_COLOUR );				// clear the screen
                         #endif
                         //TAP_Osd_FillBox( rgn, 0, 0, 720, 576, FILL_COLOUR );				    // Clear the entire screen with black.
                         TAP_Osd_PutGd( rgn, 0, 0, &_topGd, TRUE );							    // draw top graphics
                         TAP_Osd_PutGd( clockRgn, 0, 0, &_topGd, TRUE );							    // draw top graphics
                         TAP_Osd_PutGd( rgn, 0, 0, &_sideGd, TRUE );					        // draw left side graphics
	                     TAP_Osd_PutGd( rgn, 672, 0, &_sideGd, TRUE );						    // draw right side graphics
	                     if (numberLinesOption == 9)           
                            TAP_Osd_PutGd( rgn, 0, (MAX_SCREEN_Y-39), &_bottomGd, TRUE );	    // draw bottom graphics
                         break;
           case 1:       // Transparent border    
                         #ifdef WIN32
                            TAP_Osd_FillBox( rgn, 0, 0, 720, 576, COLOR_DarkGray );				// clear the screen
                         #else    
                            TAP_Osd_FillBox( rgn, 0, 0, 720, 576, 0 );		      		        // clear the screen with transparency colour
                         #endif
                            TAP_Osd_FillBox( rgn, INFO_AREA_X-COLUMN_GAP_W, Y1_STEP+Y1_OFFSET-8, INFO_AREA_W+(2*COLUMN_GAP_W), (INFO_AREA_Y+INFO_AREA_H)-(Y1_STEP+Y1_OFFSET-11), FILL_COLOUR );	// Fill the center with black.
//                         #endif
                         TAP_Osd_PutGd( rgn, 0, 0, &_top_blackGd, TRUE );					    // draw top graphics
                         TAP_Osd_PutGd( clockRgn, 0, 0, &_top_blackGd, TRUE );					    // draw top graphics
                         TAP_Osd_PutGd( rgn, 0, 0, &_side_blackGd, TRUE );						// draw left side graphics
	                     TAP_Osd_PutGd( rgn, 672, 0, &_side_blackGd, TRUE );					// draw right side graphics
	                     if (numberLinesOption == 9)           
                            TAP_Osd_PutGd( rgn, 0, (MAX_SCREEN_Y-39), &_bottom_blackGd, TRUE );	// draw bottom graphics
                         break;
    }   
}


	
//------------
//
void CreateArchiveWindow(void)
{
	windowShowing = TRUE;
	archiveHelpWindowShowing = FALSE;
	TAP_Osd_FillBox( memRgn, 0, 0, 720, 576, 0 );					// clear the memory region with transparency colour
	TAP_Osd_FillBox( clockRgn, 0, 0, CLOCK_W, CLOCK_H, 0 );			// clear the memory region with transparency colour
	TAP_Osd_FillBox( listRgn, 0, 0, 720, 576, FILL_COLOUR );		// clear the memory region with FILL COLOUR
	DrawGraphicBorders();
}


void CloseArchiveWindow( void )
{
	windowShowing = FALSE;
	archiveHelpWindowShowing = FALSE;
	TAP_Osd_Delete( memRgn );
	TAP_Osd_Delete( listRgn );
	TAP_Osd_Delete( clockRgn );
	TAP_Osd_Delete( rgn );
}



//------------
//
void DrawBackground(void)
{
	char	str[200];
	
	if (InDataFilesFolder(CurrentDir)) strcpy(str,"DataFiles");  // Don't print the "/" for the base directory.
	else strcpy(str,myfolders[CurrentDirNumber]->name);
    TAP_SPrint( str, "%s %s",str, sortTitle ); 
	TAP_Osd_PutStringAf1926( rgn, 58, 40, 390, str, TITLE_COLOUR, COLOR_Black );
	
}


//------------
//
void DrawColumnGap(int x, int y)
{
    if (recycleWindowMode) TAP_Osd_FillBox( listRgn, x, y, COLUMN_GAP_W, Y1_STEP, COLOR_Yellow );	// draw the column seperators in a different colour for the recycle window
    else                   TAP_Osd_FillBox( listRgn, x, y, COLUMN_GAP_W, Y1_STEP, FILL_COLOUR );	// draw the column seperators for the normal viewing mode.
}



//------------
//
void DisplayFolderText(int line, int i)
{
    char	str[80], str2[80], str3[80];
    appendIntToLogfile("DisplayFolderText: Called with line=%d", line, WARNING);
    appendIntToLogfile("DisplayFolderText: Called with i=%d", i, WARNING);
    appendStringToLogfile("DisplayFolderText: Name=%s<", myfiles[CurrentDirNumber][line]->name, WARNING);
    appendIntToLogfile("DisplayFolderText: directoryNumber=%d", myfiles[CurrentDirNumber][line]->directoryNumber, WARNING);
    appendIntToLogfile("DisplayFolderText: numberOfFolders=%d", myfolders[myfiles[CurrentDirNumber][line]->directoryNumber]->numberOfFolders, WARNING);
    appendIntToLogfile("DisplayFolderText: numberOfRecordings=%d", myfolders[myfiles[CurrentDirNumber][line]->directoryNumber]->numberOfRecordings, WARNING);

    switch (myfiles[CurrentDirNumber][line]->attr)
    {
           case PARENT_DIR_ATTR:
           case 240:        // Parent Directory  ".."
                            // Folder icon
	                        TAP_Osd_PutGd( listRgn, COLUMN1_START, i*Y1_STEP+Y1_OFFSET-8, &_folder_yellow_parentGd, TRUE );
	                        break;   
	                        
           case ATTR_FOLDER:
           default:   
                            // Folder icon
                            TAP_Osd_PutGd( listRgn, COLUMN1_START, i*Y1_STEP+Y1_OFFSET-8, &_folder_yellowGd, TRUE );
                       
                            // Number of subfolders in folder.
                            if (myfolders[myfiles[CurrentDirNumber][line]->directoryNumber]->numberOfFolders > 0)
                            {
	                           TAP_SPrint(str, "%d folder%c",myfolders[myfiles[CurrentDirNumber][line]->directoryNumber]->numberOfFolders, myfolders[myfiles[CurrentDirNumber][line]->directoryNumber]->numberOfFolders >= 2 ? 's':' ');
	                           PrintCenter( listRgn, COLUMN3_START+COLUMN_GAP_W, i*Y1_STEP+Y1_OFFSET, COLUMN3_END, str, MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );	
                            }   
                            
                            // Number of recordings in folder.
                            if (myfolders[myfiles[CurrentDirNumber][line]->directoryNumber]->numberOfRecordings > 0)
                            {
	                           TAP_SPrint(str, "%d %s%c",myfolders[myfiles[CurrentDirNumber][line]->directoryNumber]->numberOfRecordings, column4Option == 2 ? "rec":"recording", myfolders[myfiles[CurrentDirNumber][line]->directoryNumber]->numberOfRecordings == 1 ? ' ':'s');
	                           PrintCenter( listRgn, COLUMN4_START+COLUMN_GAP_W, i*Y1_STEP+Y1_OFFSET, COLUMN4_END, str, MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );
                            }
                            break;
    }                                                        

    // Print the Folder name
	TAP_SPrint(str,"%s", myfiles[CurrentDirNumber][line]->name);
    PrintLeft( listRgn, COLUMN2_TEXT_START, i*Y1_STEP+Y1_OFFSET, COLUMN2_END, str, MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );	

}



//------------
//
void FormatListDuration(int dur, char* str)
{
    char	buf[20];
	int 	hour, min;

    hour = dur / 60;
    min  = dur % 60;
    
    str[0]='\0';  // Blank out any existing string.
    buf[0]='\0';  // Blank out the buffer.
    
	if ( hour )
	{	
		sprintf( buf, "%dh", hour);
		strcat(str, buf);
		if ( !min )   // If there are no minutes, then expand hours to be "hour" or "hours".
		{
             strcat(str,"our");
		     if ( hour > 1 )
			    strcat(str, "s");
        }	    
	}

	if ( min )
	{	
		if ( hour )   // If there was hours, then seperate with a space.
			strcat(str, " ");
		sprintf( buf, "%dm", min);
		strcat(str, buf);
		if ( !hour )   // If there are no hours, then expand minutes to be "min" or "mins".
		{
			strcat(str, "in");
		    if ( min > 1 )
			  strcat(str, "s");
        }	  
	}
	
}	



//------------
//
void FormatRecDuration(int dur, char* str)
{
    char	buf[20];
	int 	hour, min;

    hour = dur / 60;
    min  = dur % 60;
    
    str[0]='\0';  // Blank out any existing string.
    
	if ( hour )
	{	
		sprintf( str, "%dH", hour);
		strcat(str, buf);
		if ( min ) // If there are minutes, then make the hour tag "Hrs" else "Hours"
		    strcat(str, "r");
		else
            strcat(str, "our");   
        if ( hour > 1 )
			    strcat(str, "s");
	}

	if ( min )
	{	
		if ( hour )   // If there was hours, then seperate with a space.
			strcat(str, " ");
		sprintf( buf, "%dMin", min);
		strcat(str, buf);
	    if ( min > 1 )
			  strcat(str, "s");
	}
	
}	



//------------
//
void FormatTimeAndDuration(int line, char* str, int option)
{
    char	str2[80], str3[80];
	byte	hour, min, sec, month, day, weekDay;
	word	startTime;
	int		endHour, endMin;

    hour = myfiles[CurrentDirNumber][line]->hour;							// extract the time
	min  = myfiles[CurrentDirNumber][line]->min;

	endMin  = min + myfiles[CurrentDirNumber][line]->recDuration;				// add the duration in miutes
	endHour = hour + endMin/60;							// should we have increamented the hour
	if ( endHour >= 24 ) endHour -= 24;					// did the day roll over ?
	endMin = endMin%60;									// Finally remove the hours from the minutes field.
				
    FormatListDuration(	myfiles[CurrentDirNumber][line]->recDuration, str2 );		

    switch (option)
    {
           case 0:  // Format as "18:30 (30min)"
                    TAP_SPrint(str, "%02d:%02d (%s)", hour, min,str2);
                    break;
                    
           case 1:  // FFormat as "18:30 ~ 19:00"
                    TAP_SPrint(str, "%02d:%02d ~ %02d:%02d", hour, min, endHour, endMin);
                    break;
                    
           case 2:  // Format as "30min"
                    TAP_SPrint(str, "%s", str2);
                    break;

           default:  // Format as "18:30 (30min)"
                    TAP_SPrint(str, "%02d:%02d (%s)", hour, min,str2);
                    break;
                    
    }                    
                    
}



//------------
//
void FormatDate(int line, char* str, int option, int *fontSize)
{
    char	str2[20], str3[20], str4[40];
	byte	month, day, weekDay;
    word    year, mjd, mjdDiff;

	byte 	currentWeekDay, currentMonth, currentDay, currentHour, currentMin, currentSec;
	word 	currentYear, currentMJD, currentTime;

	TAP_GetTime( &currentMJD, &currentHour, &currentMin, &currentSec);
	TAP_ExtractMjd( currentMJD, &currentYear, &currentMonth, &currentDay, &currentWeekDay) ;

 	mjd = myfiles[CurrentDirNumber][line]->mjd;
	TAP_ExtractMjd( mjd, &year, &month, &day, &weekDay) ;

	WeekdayToAlpha( weekDay, str2 );  // Get the name of the day.
    MonthToAlpha( month, str3 );      // Get the name of the month.
	
    mjdDiff = (currentMJD - mjd)/7;   // Calculate how many weeks between today and the recording date.
    str4[0]='\0';

    switch (mjdDiff)
    {
           case 0:  // Current weekday.
                    TAP_SPrint(str4,"%s", str2);
                    break;

           case 1:  // Last week's weekday.
                    TAP_SPrint(str4,"%0.3s last week", str2);
                    break;
           
           case 2:
           case 3:
           case 4:
           case 5:  // Previous week's weekday.
                    TAP_SPrint(str4,"%0.3s %dwks ago", str2, mjdDiff);
                    break;

           default:  // Previous month's weekday.
                    TAP_SPrint(str4,"%0.3s %dmth%s ago", str2, mjdDiff/4, (mjdDiff/4) > 1 ? "s":"");
                    break;
    }
           
    switch (option)
    {
           case 0: // Format as "Mon 3 Oct"
                   TAP_SPrint(str,"%0.3s %d %s", str2, day, str3);  
                   *fontSize = FNT_Size_1622;
                   break;

           case 1: // Format as "Monday" or "Mon 2wks ago"
                   TAP_SPrint(str,"%s", str4);
                   if (mjdDiff ==0) // it is this week's day and therefore, just the day name, print in a medium font.
                      *fontSize = FNT_Size_1622;
                   else             // print in a small font.   
                      *fontSize = FNT_Size_1419;
                   break;

           default: // Format as "Mon 3 Oct"
                   TAP_SPrint(str,"%0.3s %d %s", str2, day, str3);  
                   *fontSize = FNT_Size_1622;
                   break;
    }                   
}


//------------
//
void PrintListFileSize(int line, int i, int option)
{
     char str[30];
     
     FormatFileSize(myfiles[CurrentDirNumber][line]->size, str, option);

     switch (option)
     {
            case 1: //  xxMB  (small font)
//                    PrintCenter( listRgn, COLUMN5_TEXT_START, i*Y1_STEP+Y1_OFFSET+2, COLUMN5_END, str, MAIN_TEXT_COLOUR, 0, FNT_Size_1419 );
                    PrintCenter( listRgn, COLUMN5_TEXT_START, i*Y1_STEP+Y1_OFFSET+2, COLUMN5_END, str, MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );
                    break;
                    
            case 2: //  xxGB  (medium font)
            case 3: //  xx    (medium font)
            case 4: //  xx    (medium font)
            case 5: //  xxMB/GB    (medium font)
                    PrintCenter( listRgn, COLUMN5_TEXT_START, i*Y1_STEP+Y1_OFFSET, COLUMN5_END, str, MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );
                    break;
                    
            default: break;
     }
}


//------------
//
void FormatFilename(int x, int y, int max, int line, char* strSource, int option)
{
    char str[TS_FILE_NAME_SIZE];
    
    strcpy(str, strSource);
    
    switch (option)
    {
           case 0: // Filename with ".rec" or ".rec.del" removed.
                   str[strlen(str)-tagLength]='\0';         // Truncate the filename to remove the ".rec" or ".rec.del" from the end of the filename.
                   break;
                   
           case 1: // Filename with -# ##-##-## patterns removed.
                   strcpy(str, myfiles[CurrentDirNumber][line]->sortName);
                   break;

           case 2: // Full filename - including .rec
                   break;
                   
    }
    
    PrintLeft( listRgn, x, y, max, str, MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );	
}    



//------------
//
void DisplayFileText(int line, int i)
{
    char	str[80], str2[80], str3[80];
    dword   curDuration, curPercent;
    int     dateFontSize;
    appendIntToLogfile("DisplayFileText: Called with line=%d", line, WARNING);
    appendIntToLogfile("DisplayFileText: Called with i=%d", i, WARNING);
    appendStringToLogfile("DisplayFileText: Filename=%s", myfiles[CurrentDirNumber][line]->name, WARNING);
    
    /////////////////////////////////////////////////   
    // COLUMN 1 - Print the file number, play or recording indicator.
    /////////////////////////////////////////////////   
    if (recycleWindowMode) // If we are in the Recycle Bin view mode, display a recycle bin icon next to each file.
    {
//         TAP_Osd_PutGd( listRgn, COLUMN1_START+3, i*Y1_STEP+Y1_OFFSET-4, &_recycle_bin_fullGd, TRUE );
         TAP_Osd_PutGd( listRgn, COLUMN1_START, i*Y1_STEP+Y1_OFFSET-10, &_trash_binGd, TRUE );
    }
    else
    if (myfiles[CurrentDirNumber][line]->isRecording)  // If the file is recording print recording icon.
    {
         appendToLogfile("DisplayFileText: isRecording TRUE", WARNING);
         TAP_Osd_PutGd( listRgn, COLUMN1_START+6, i*Y1_STEP+Y1_OFFSET, &_redglasscircle25x25Gd, TRUE );
		 TAP_Osd_PutStringAf1419( listRgn, COLUMN1_START+13, i*Y1_STEP+Y1_OFFSET+3, COLUMN1_END, "R", MAIN_TEXT_COLOUR, 0 );
         
         // Flag that there is a recording on the screen, and indicate which recording slot it is.
         if (strncmp(myfiles[CurrentDirNumber][line]->name, recInfo[0].fileName, TS_FILE_NAME_SIZE)==0)   // It's the 1st recording slot.
         {
             recordingOnScreenEntry1 = line;         
             recordingOnScreenLine1 = i;
         }
         if (strncmp(myfiles[CurrentDirNumber][line]->name, recInfo[1].fileName, TS_FILE_NAME_SIZE)==0)   // It's the 2nd recording slot.
         {
             recordingOnScreenEntry2 = line;         
             recordingOnScreenLine2 = i;
         }
    }
    else
    if (myfiles[CurrentDirNumber][line]->isPlaying)  // If the file is playing print play icon.
    {
         appendToLogfile("DisplayFileText: isPlaying TRUE", WARNING);
         TAP_Osd_PutGd( listRgn, COLUMN1_START+6, i*Y1_STEP+Y1_OFFSET, &_greenglasscircle25x25Gd, TRUE );
         TAP_Osd_PutStringAf1622( listRgn, COLUMN1_START+15, i*Y1_STEP+Y1_OFFSET+2, COLUMN1_END, ">", COLOR_Black, 0 );

         // Flag that there is an active playback on the screen.
         playbackOnScreenEntry = line;
         playbackOnScreenLine = i;
    }
//    else  // Print the file number.
//    {
//        TAP_SPrint(str,"%d", line);
//        TAP_Osd_PutStringAf1622( rgn, FILE_NUMBER_X, i*Y1_STEP+Y1_OFFSET, 90, str, textColour, 0 );
//    }    

    
    /////////////////////////////////////////////////   
    // COLUMN 2 - Print the File name
    /////////////////////////////////////////////////   
	TAP_SPrint(str,"%s", myfiles[CurrentDirNumber][line]->name);

    // For testing purposes on Windows - always assume some set data.
#ifdef WIN32    
       myfiles[CurrentDirNumber][line]->hasPlayed=TRUE;
       myfiles[CurrentDirNumber][line]->currentBlock = 200;
       myfiles[CurrentDirNumber][line]->totalBlock = 300;
       myfiles[CurrentDirNumber][line]->recDuration = 240;
#endif       
    
     if (myfiles[CurrentDirNumber][line]->hasPlayed)
     {
         // Calculate how many minutes have been watched. (Round up to nearest minute)
         curDuration = (( max(0,myfiles[CurrentDirNumber][line]->currentBlock) * myfiles[CurrentDirNumber][line]->recDuration)  / max(1,myfiles[CurrentDirNumber][line]->totalBlock) );
         curPercent  = (( max(0,myfiles[CurrentDirNumber][line]->currentBlock) * 100)                        / max(1,myfiles[CurrentDirNumber][line]->totalBlock) );
         appendIntToLogfile("DisplayFileText: hasPlayed TRUE curPercent=%d", curPercent, WARNING);
         if ((curPercent < 95) || (myfiles[CurrentDirNumber][line]->isPlaying)) // If we haven't watched the entire show, or the show is playing, display the progress bar.
         {
            // Print the Filename at the top of the row.
            FormatFilename( COLUMN2_TEXT_START, i*Y1_STEP+Y1_OFFSET-7, COLUMN2_END, line, myfiles[CurrentDirNumber][line]->name, column2Option);
            // Display the progress bar at the bottom of the row.
            switch (progressBarOption)
            {
                   case PB_MULTI:
                   case PB_SINGLE:
                   case PB_SOLID:
                               DisplayProgressBar(listRgn, max(0,myfiles[CurrentDirNumber][line]->currentBlock), max(1,myfiles[CurrentDirNumber][line]->totalBlock), COLUMN2_TEXT_START, i*Y1_STEP+Y1_OFFSET+19, LIST_PROGRESS_BAR_WIDTH, 8, COLOR_Black, 1, COLOR_Black, progressBarOption);
                               break;
                               
                   case PB_REDGREEN:
                               DisplayProgressBar(listRgn, max(0,myfiles[CurrentDirNumber][line]->currentBlock), max(1,myfiles[CurrentDirNumber][line]->totalBlock), COLUMN2_TEXT_START, i*Y1_STEP+Y1_OFFSET+19, LIST_PROGRESS_BAR_WIDTH, 8, COLOR_Black, 1, COLOR_Black, COLOR_Green);
                               break;
                               
                   case PB_WHITE:
                               DisplayProgressBar(listRgn, max(0,myfiles[CurrentDirNumber][line]->currentBlock), max(1,myfiles[CurrentDirNumber][line]->totalBlock), COLUMN2_TEXT_START, i*Y1_STEP+Y1_OFFSET+19, LIST_PROGRESS_BAR_WIDTH, 8, COLOR_Black, 1, COLOR_Black, MAIN_TEXT_COLOUR);
                               break;
            } 

            // Display the progress in minutes watched and minutes not watch at the end of the progress bar in small font.
	        TAP_SPrint( str, "+%dm/-%dm", curDuration, myfiles[CurrentDirNumber][line]->recDuration-curDuration);
            PrintLeft( listRgn, COLUMN2_TEXT_START + LIST_PROGRESS_BAR_WIDTH + 5, i*Y1_STEP+Y1_OFFSET+14, COLUMN2_END, str, MAIN_TEXT_COLOUR, 0, FNT_Size_1419 );
         }
         else   // Don't display the progress bar, and just indicate a "Watched" tick.
         {
            // Display a green tick in front of the filename.
//            TAP_Osd_PutGd( listRgn, COLUMN1_START+6, i*Y1_STEP+Y1_OFFSET-2, &_greentick25x26Gd, TRUE );
            TAP_Osd_PutGd( listRgn, COLUMN1_START+7, i*Y1_STEP+Y1_OFFSET-2, &_blueglasscircletick25x25Gd, TRUE );
            // Print the Filename in the middle of the row.
            FormatFilename( COLUMN2_TEXT_START, i*Y1_STEP+Y1_OFFSET, COLUMN2_END, line, myfiles[CurrentDirNumber][line]->name, column2Option);
         }    
    }
    
    if (myfiles[CurrentDirNumber][line]->isRecording)
    {
         // Calculate how many minutes have been recorded. (Round up to nearest minute)
         curDuration = max(0, myfiles[CurrentDirNumber][line]->recordedSec/60);
         appendIntToLogfile("DisplayFileText: isRecording TRUE curDuration=%d", curDuration, WARNING);
         // Print the Filename at the top of the row.
         FormatFilename( COLUMN2_TEXT_START, i*Y1_STEP+Y1_OFFSET-7, COLUMN2_END, line, myfiles[CurrentDirNumber][line]->name, column2Option);
         // Display the progress bar at the bottom of the row.
         switch (progressBarOption)
         {
                case PB_MULTI:
                case PB_SINGLE:
                case PB_SOLID:
                            DisplayProgressBar(listRgn, myfiles[CurrentDirNumber][line]->recordedSec, myfiles[CurrentDirNumber][line]->recDuration*60 , COLUMN2_TEXT_START, i*Y1_STEP+Y1_OFFSET+19, LIST_PROGRESS_BAR_WIDTH, 8, COLOR_Black, 1, COLOR_Black, progressBarOption);
                            break;
                             
                case PB_REDGREEN:
                            DisplayProgressBar(listRgn, myfiles[CurrentDirNumber][line]->recordedSec, myfiles[CurrentDirNumber][line]->recDuration*60 , COLUMN2_TEXT_START, i*Y1_STEP+Y1_OFFSET+19, LIST_PROGRESS_BAR_WIDTH, 8, COLOR_Black, 1, COLOR_Black, COLOR_Red);
                            break;
                               
                case PB_WHITE:
                            DisplayProgressBar(listRgn, myfiles[CurrentDirNumber][line]->recordedSec, myfiles[CurrentDirNumber][line]->recDuration*60 , COLUMN2_TEXT_START, i*Y1_STEP+Y1_OFFSET+19, LIST_PROGRESS_BAR_WIDTH, 8, COLOR_Black, 1, COLOR_Black, COLOR_White);
                            break;
         } 

         // Display the progress in minutes watched and minutes not watch at the end of the progress bar in small font.
         TAP_SPrint( str, "+%dm/-%dm", curDuration, myfiles[CurrentDirNumber][line]->recDuration-curDuration);
         PrintLeft( listRgn, COLUMN2_TEXT_START + LIST_PROGRESS_BAR_WIDTH + 5, i*Y1_STEP+Y1_OFFSET+14, COLUMN2_END, str, MAIN_TEXT_COLOUR, 0, FNT_Size_1419 );
    }
    
    if ((!myfiles[CurrentDirNumber][line]->hasPlayed) && (!myfiles[CurrentDirNumber][line]->isRecording))
    {
         appendToLogfile("DisplayFileText: isRecording FALSE and  hasPlayed FALSE", WARNING);
         FormatFilename( COLUMN2_TEXT_START, i*Y1_STEP+Y1_OFFSET, COLUMN2_END, line, myfiles[CurrentDirNumber][line]->name, column2Option);
    }
    
    
    /////////////////////////////////////////////////   
    // COLUMN 3 - Date
    /////////////////////////////////////////////////   
	FormatDate(line, str, column3Option, &dateFontSize);
	if (dateFontSize == FNT_Size_1622)
       PrintCenter( listRgn, COLUMN3_TEXT_START, i*Y1_STEP+Y1_OFFSET, COLUMN3_END, str, MAIN_TEXT_COLOUR, 0, dateFontSize );
    else // Smaller font, so print slightly lower.
       PrintCenter( listRgn, COLUMN3_TEXT_START, i*Y1_STEP+Y1_OFFSET+2, COLUMN3_END, str, MAIN_TEXT_COLOUR, 0, dateFontSize );
	
	

    /////////////////////////////////////////////////   
    // COLUMN 4 - Time & Duration
    /////////////////////////////////////////////////   
    FormatTimeAndDuration(line, str, column4Option);
	PrintCenter( listRgn, COLUMN4_TEXT_START, i*Y1_STEP+Y1_OFFSET, COLUMN4_END, str, MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );



    /////////////////////////////////////////////////   
    // COLUMN 5 - Channel logo or File Size.
    /////////////////////////////////////////////////   
    //myfiles[CurrentDirNumber][line]->size=290000000;
    switch (column5Option)
    {
           case 0: // Channel Logo
                   if (sortOrder == SORT_SIZE_OPTION)  // If we've sorted by Size then override logo option and display the size.
	                   PrintListFileSize(line, i, 1);     // Display size as xxxMB
                   else
	                   DisplayLogo( listRgn, COLUMN5_TEXT_START, i*Y1_STEP+Y1_OFFSET-8, myfiles[CurrentDirNumber][line]->svcNum, myfiles[CurrentDirNumber][line]->svcType );
                   break;
                   
           case 1: // Recording size.
           default:
                   PrintListFileSize(line, i, column5Option);  // Display size according to size option.
	               break;
	               
    }   
    
    appendToLogfile("DisplayFileText: Finished.", WARNING);


}



//------------
//
void DisplayArchiveText(int line, int i)
{
	if ( line == 0 ) return;											// bounds check
    
    switch (myfiles[CurrentDirNumber][line]->attr)
    {
           case PARENT_DIR_ATTR:
           case 240:          // Parent Directory  ".."
           case ATTR_FOLDER:  // Normal Folder
	                          DisplayFolderText(line, i);
	                          break;
           case ATTR_TS:
           default:
                              DisplayFileText(line, i);
		                      break;
    }

}



//------------
//
void DisplayArchiveLine(int line, int i)
{

    appendIntToLogfile("DisplayArchiveLine: Called with line=%d", line, WARNING);
    appendIntToLogfile("DisplayArchiveLine: Called with i=%d", i, WARNING);

	if ( line == 0 ) return;											// bounds check

	if ( chosenLine == line )											// highlight the current cursor line
	   TAP_Osd_PutGd( listRgn, INFO_AREA_X, i*Y1_STEP+Y1_OFFSET-8, &_highlightGd, TRUE );
	else
       TAP_Osd_PutGd( listRgn, INFO_AREA_X, i*Y1_STEP+Y1_OFFSET-8, &_rowaGd, TRUE );

 	if ( line <= maxShown )	DisplayArchiveText(line, i);								// anything to display ?

    DrawColumnGap(COLUMN2_START, i*Y1_STEP+Y1_OFFSET-8);
    DrawColumnGap(COLUMN3_START, i*Y1_STEP+Y1_OFFSET-8);
    DrawColumnGap(COLUMN4_START, i*Y1_STEP+Y1_OFFSET-8);
    DrawColumnGap(COLUMN5_START, i*Y1_STEP+Y1_OFFSET-8);

    TAP_Osd_Copy( listRgn, rgn, INFO_AREA_X, i*Y1_STEP+Y1_OFFSET-8, INFO_AREA_W, Y1_STEP, INFO_AREA_X, i*Y1_STEP+Y1_OFFSET-8, FALSE );
}


//------------
//
void DrawArchiveList(void)
{
	int		listLine, pLine, start;

    if (printLine > NUMBER_OF_LINES) printLine = NUMBER_OF_LINES; // Catch any changes to number of lines.

	DrawBackground();

	start = (chosenLine + 1) - printLine;

    // Blank out the flags that indicate whether there are active playbacks or recordings on the screen.
    playbackOnScreenEntry = 0;
    playbackOnScreenLine = 0;
    recordingOnScreenEntry1 = 0;
    recordingOnScreenLine1 = 0;
    recordingOnScreenEntry2 = 0;
    recordingOnScreenLine2 = 0;

    listLine = start;
	for ( pLine=1; pLine<=NUMBER_OF_LINES ; pLine++)
	{
		DisplayArchiveLine(listLine,pLine);
		listLine++;
	}
	
}


//------------
//
void UpdateFileSelectionText(int chosenLine)
{
    char	str[600], fileSize[30], dateStr[30];
    int     dateFontSize;
    
    appendIntToLogfile("UpdateFileSelectionText: Called with chosenLine=%d", chosenLine, WARNING);
    
    // Print Event Name & Description over 2 lines.  
    appendStringToLogfile("UpdateFileSelectionText: eventName=%s<", myfiles[CurrentDirNumber][chosenLine]->eventName, WARNING);
    appendStringToLogfile("UpdateFileSelectionText: eventDescName=%s<", myfiles[CurrentDirNumber][chosenLine]->eventDescName, WARNING);
    TAP_SPrint(str,"%s. %s", myfiles[CurrentDirNumber][chosenLine]->eventName, myfiles[CurrentDirNumber][chosenLine]->eventDescName);
    appendStringToLogfile("UpdateFileSelectionText: str=%s<", str, WARNING);
    appendIntToLogfile("UpdateFileSelectionText: strlen=%d<", strlen(str), WARNING);
    LastWrapPutStr_Start = 0;  // Reset "first character" pointer to start for Event Name & Description
    LastWrapPutStr_P = 0;      // Reset "last character" pointer to start for Event Name & Description
    LastWrapPutStr_Y = INFO_TEXT_Y + (0*INFO_TEXT_H); // Set the starting y-coordinate for the Event Name & Description
    WrapPutStr_StartPos = LastWrapPutStr_P; // Set the start position of the string to where we got up to last time.

    switch (infoLineOption)
    {
           case 0:   // 2 lines of event information.
                     WrapPutStr( memRgn, str, INFO_TEXT_X, LastWrapPutStr_Y, INFO_TEXT_W-8, INFO_COLOUR, INFO_FILL_COLOUR, 2, FNT_Size_1622, 0);
                     break;
                     
           case 1:   // 1 line of event info, 1 line of extra info.
                     WrapPutStr( memRgn, str, INFO_TEXT_X, LastWrapPutStr_Y, INFO_TEXT_W-8, INFO_COLOUR, INFO_FILL_COLOUR, 1, FNT_Size_1622, 0);
	                 FormatDate(chosenLine, dateStr, 0, &dateFontSize);  // Format date as "DDD X MMM"
	                 FormatFileSize(myfiles[CurrentDirNumber][chosenLine]->size, fileSize, 1);  // Get size as xxxMB
                     TAP_SPrint(str, "%s.   %s.    %s", myfiles[CurrentDirNumber][chosenLine]->serviceName, dateStr, fileSize);
                     PrintLeft( memRgn, INFO_TEXT_X, INFO_TEXT_Y + (1*INFO_TEXT_H), INFO_TEXT_W, str, INFO_COLOUR, 0, FNT_Size_1622 );	
                     break;
    }
    appendToLogfile("UpdateFileSelectionText: Finished.", WARNING);

}


//------------
//
void UpdateRecordingSelectionText(int chosenLine)
{
    char	str[100], str2[80];
	int 	l;

	dword	recHour, recMin, totalRecSec, curRecHour, curRecMin, curRecSec;

    appendIntToLogfile("UpdateRecordingSelectionText: Called with chosenLine=%d", chosenLine, WARNING);

    // Print Recording Tag.
    TAP_SPrint(str,"Recording...");
	TAP_Osd_PutStringAf1622( memRgn, INFO_TEXT_X, INFO_TEXT_Y, INFO_TEXT_W, str, INFO_COLOUR, INFO_FILL_COLOUR );

    recHour = myfiles[CurrentDirNumber][chosenLine]->recDuration / 60;
    recMin  = myfiles[CurrentDirNumber][chosenLine]->recDuration % 60;
    totalRecSec = myfiles[CurrentDirNumber][chosenLine]->recordedSec;
    curRecHour  = totalRecSec / 60 / 60;
    curRecMin   = (totalRecSec - (curRecHour * 60 * 60)) / 60;
    curRecSec   = (totalRecSec - (curRecHour * 60 * 60)) % 60;

    DisplayProgressBar(memRgn, myfiles[CurrentDirNumber][chosenLine]->recordedSec, myfiles[CurrentDirNumber][chosenLine]->recDuration*60 , INFO_TEXT_X, INFO_TEXT_Y+27, RECORDING_PROGRESS_BAR_WIDTH, 10, COLOR_Black, 1, COLOR_Gray, 0);

    TAP_SPrint(str,"%02d:%02d:%02d Recorded ", curRecHour, curRecMin, curRecSec);
	TAP_Osd_PutStringAf1622( memRgn, INFO_TEXT_X+RECORDING_PROGRESS_BAR_WIDTH+5, INFO_TEXT_Y+21, INFO_TEXT_X+INFO_TEXT_W, str, INFO_COLOUR, INFO_FILL_COLOUR );
    
    appendToLogfile("UpdateRecordingSelectionText: Finished.", WARNING);
    
}

//------------
//
void UpdateFolderSelectionText(int chosenLine)
{
    char	str[500];

    appendIntToLogfile("UpdateFolderSelectionText: Called with chosenLine=%d", chosenLine, WARNING);

    // folder name
	if ((myfiles[CurrentDirNumber][chosenLine]->attr == 240) || (myfiles[CurrentDirNumber][chosenLine]->attr == PARENT_DIR_ATTR))
	    TAP_SPrint(str,"Moves back to the previous directory.");
    else
	    TAP_SPrint(str,"Folder: %s",  myfiles[CurrentDirNumber][chosenLine]->name );
	TAP_Osd_PutStringAf1622( memRgn, INFO_TEXT_X, INFO_TEXT_Y, INFO_TEXT_W, str, INFO_COLOUR, INFO_FILL_COLOUR );

    appendToLogfile("UpdateFolderSelectionText: Finished.", WARNING);

}


//------------
//
void UpdateSelectionNumber(void)
{
    char	str[80], str2[80], str3[80], str4[80], str5[80];
	int 	l;

	byte	hour, min, sec, month, day, weekDay;
	word	year, mjd;
	int		endHour, endMin;
	
	TYPE_TapChInfo	currentChInfo;

    appendToLogfile("UpdateSelectionNumber: Called.", WARNING);

    TAP_Osd_FillBox( memRgn, INFO_AREA_X, INFO_AREA_Y, INFO_AREA_W, INFO_AREA_H, INFO_FILL_COLOUR );		// clear the bottom portion

	if ( chosenLine > 0 )												// update, or blank the last line
	{
         switch (myfiles[CurrentDirNumber][chosenLine]->attr)
         {
                case PARENT_DIR_ATTR:
                case 240:
                case ATTR_FOLDER:
                             UpdateFolderSelectionText( chosenLine );
                             break;
                     
                case ATTR_TS:
                default:
                             if (myfiles[CurrentDirNumber][chosenLine]->isRecording)
                             {
                                UpdateRecordingSelectionText( chosenLine );
                             }
                             else
                             {
                                UpdateFileSelectionText( chosenLine );
                             }
                             break;
         }
    }
    appendToLogfile("UpdateSelectionNumber: Calling DisplayInstructions.", WARNING);
    DisplayInstructions();
    appendToLogfile("UpdateSelectionNumber: Calling DrawFreeSpaceBar.", WARNING);
    DrawFreeSpaceBar();               // Update the diskspace info.
     
    //
    // Copy the memory region on to the displayed screen region.
    //
    TAP_Osd_Copy( memRgn, rgn, INFO_AREA_X, INFO_AREA_Y, INFO_AREA_W, INFO_AREA_H, INFO_AREA_X, INFO_AREA_Y, FALSE );

    appendToLogfile("UpdateSelectionNumber: Finished.", WARNING);
}


//------------
//
dword ArchiveWindowKeyHandler(dword key)
{
	int		oldPage, oldChosenLine, oldPrintLine;
	oldPage = page;
	oldChosenLine = chosenLine;
	oldPrintLine  = printLine;
	
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
                                           appendToLogfile("ArchiveWindowKeyHandler: Playlist pressed.", WARNING);
                                           generatedPlayList = TRUE;
	                                       TAP_GenerateEvent( EVT_KEY, RKEY_PlayList, 0 );
							               break;													// and enter the normal state

                                   case 1: // Ignore key
                                           break;

                                   case 2: // Exit Archive.
                                           exitFlag = TRUE;
                                           break;
                            }       

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
		case RKEY_9 :		break;   // Problem with page jumping, so removed for v0.04
                            chosenLine = (key - RKEY_0) + (page * NUMBER_OF_LINES);		// direct keyboard selection of any line
							if ( chosenLine > maxShown ) chosenLine = maxShown;

							DisplayArchiveLine( oldChosenLine, oldPrintLine );
							printLine = (key - RKEY_0);
							DisplayArchiveLine( chosenLine, printLine );
							UpdateSelectionNumber();							
							break;
							
		case RKEY_0 :       break; // Problem with page jumping, so removed for v0.04
                            if (NUMBER_OF_LINES < 10) break;                            // If we're showing less than 10 lines, ignore 0.
							chosenLine = (10) + (page * NUMBER_OF_LINES);				// make "0" select the 10th (last) line
							if ( chosenLine > maxShown ) chosenLine = maxShown;

							DisplayArchiveLine( oldChosenLine, oldPrintLine );
							printLine = 10;
							DisplayArchiveLine( chosenLine, printLine );
							UpdateSelectionNumber();							
							break;


		case RKEY_Blue :	// Resume last playback file.
                            if ( playedFiles[0]->totalBlock <=0 ) // If no last playback has been set, ignore Blue.
                            {
                                 ShowMessageWin( rgn, "No Last Playback File", "The last playback file was not found.", "(It may have been deleted.)", 350 );
                                 break; 
                            }
                            if (RestartLastPlayback() == 0)             // Attempt to restart last playback file.
                                exitFlag = TRUE;						// signal exit to top level - will clean up, close window,                      
							break;
							
		case RKEY_Ok :		switch (okPlayOption)
                            {
                                   case 0: ArchiveAction(chosenLine);   // Normal "OK" action - open folder or resume playback.
                                           break;
                                           
                                   case 1: if (myfiles[CurrentDirNumber][chosenLine]->attr == ATTR_TS)
                                                RestartPlayback( chosenLine, 0);   // Start playback from the start, without jumping.
                                           else
                                                ArchiveAction(chosenLine);         // Handle opening of folders.
                                           break;
                            }
							break;
							
		case RKEY_Play :	if (myfiles[CurrentDirNumber][chosenLine]->attr != ATTR_TS) break;   // 'Play' key is only valid for recording files.
                            switch (okPlayOption)
                            {
                                   case 0: RestartPlayback( chosenLine, 0);   // Start playback from the start, without jumping.
                                           break;
                                           
                                   case 1: RestartPlayback( chosenLine, 1);   // Resume playback from last playback position.
                                           break; 
                            }               
							break;

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

        case RKEY_Green:    // Create New Folder
                            CreateNewFolder();
                            RefreshArchiveList(TRUE);
                            break;

        case RKEY_Yellow:   // Move file or folder
                            if (( chosenLine > 0 ) && (myfiles[CurrentDirNumber][chosenLine]->attr != PARENT_DIR_ATTR) && (!myfiles[CurrentDirNumber][chosenLine]->isRecording) )
                            { 
                                 ActivateMoveWindow();            // Display initial move window with an empty list and "Loading..." message
                                 populateMoveFileList();          // Populate the list of move folders.
                                 DisplayArchiveMoveFolderList();  // Display the list of move folders.
                            }     
                            break;

        case RKEY_Red:      DisplayArchiveHelp();
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
                            TAP_Osd_PutStringAf1926( rgn, 58, 40, 390, "             LOADING...            ", TITLE_COLOUR, COLOR_Black );
                            loadInitialArchiveInfo(FALSE, 99); // Load all the files for the new view, but don't delete any progress info.
   		                    RefreshArchiveList(TRUE);      // Redraw the contents of the screen.
                            break;                            

                            							
		case RKEY_Menu :	ActivateMenu();
						    break;

		case RKEY_Mute :	// Pass thru Mute key
                            return key;


        case RKEY_Stop:     // Stop playback / recording.
                            if ((myfiles[CurrentDirNumber][chosenLine]->isRecording) || (myfiles[CurrentDirNumber][chosenLine]->isPlaying)) ActivateStopWindow(myfiles[CurrentDirNumber][chosenLine]->name);
                            break;
    
        #ifdef WIN32
        case RKEY_Fav   :   // If testing on WIN32 SDK also allow the Favourite key 
        #endif
        case RKEY_White :   // Delete
                            if (chosenLine == 0) break;   // Don't allow delete if there is nothing displayed.
                            if (myfiles[CurrentDirNumber][chosenLine]->attr != PARENT_DIR_ATTR) 
                            {
                                 currentFile   = *myfiles[CurrentDirNumber][chosenLine];
                                 currentFolder = *myfolders[myfiles[CurrentDirNumber][chosenLine]->directoryNumber];
                                 ActivateDeleteWindow(myfiles[CurrentDirNumber][chosenLine]->name,myfiles[CurrentDirNumber][chosenLine]->attr);
                            }    
                            break;
							
		default :			break;
	}
	return 0;
}


//---------------------------
//
void ActivateArchiveWindow( void )
{
    appendToLogfile("ActivateArchiveWindow: Started.", INFO);
    CreateArchiveWindow();
    appendToLogfile("ActivateArchiveWindow: Calling DrawArchiveList.", WARNING);
	DrawArchiveList();
    appendToLogfile("ActivateArchiveWindow: Calling UpdateSelectionNumber.", WARNING);
	UpdateSelectionNumber();
	UpdateListClock();
    appendToLogfile("ActivateArchiveWindow: Calling DrawFreeSpaceBar.", WARNING);
    DrawFreeSpaceBar();              
    appendToLogfile("ActivateArchiveWindow: Finished.", INFO);

}

void initialiseArchiveWindow( void )
{
    windowShowing            = FALSE;
	archiveHelpWindowShowing = FALSE;
	msgWindowShowing         = FALSE;
	chosenLine               = 0;               // Which line has been chosen.
	printLine                = 0;               // Which line are we starting to print from.
	page                     = 0;               // Which page of files are we on.
	sortOrder                = sortOrderOption; // Default to sort order specified in the config file.
	folderSortOrder          = 0;               // Default folders to sort to top.

}

void RefreshArchiveWindow( void )
{
    numberOfFiles = myfolders[CurrentDirNumber]->numberOfFiles;          // Set the number of files for this directory.
    maxShown      = numberOfFiles;                                      // Set the number of files shown for this directory.

	SortList(sortOrder);						   // sort the files in selected order
	while ( chosenLine > maxShown )                // cater for delete of the last file - move up one, or clear hightlight if = 0
	{
          chosenLine--;
          printLine--;                             // move the printline back as well.
          if (printLine == 0) printLine = NUMBER_OF_LINES;  // If we delete the last file on the page, move make to the previous page.
    }
    
    if (printLine > NUMBER_OF_LINES) printLine = NUMBER_OF_LINES; // Catch any changes to number of lines.

	page = (chosenLine-1) / NUMBER_OF_LINES;
							
	DrawGraphicBorders();
	
	DeterminePrintingLine( chosenLine );
	UpdateListClock();
	DrawArchiveList();
	UpdateSelectionNumber();
}

void RefreshArchiveList( bool reposition )
{
    appendToLogfile("RefreshArchiveList: Started.", INFO);

    numberOfFiles = myfolders[CurrentDirNumber]->numberOfFiles;          // Set the number of files for this directory.
    maxShown      = numberOfFiles;                                      // Set the number of files shown for this directory.

	SortList(sortOrder);						   // sort the files in selected order
	while ( chosenLine > maxShown )                // cater for delete of the last file - move up one, or clear hightlight if = 0
	{
          chosenLine--;
          printLine--;                             // move the printline back as well.
          if (printLine == 0) printLine = NUMBER_OF_LINES;  // If we delete the last file on the page, move make to the previous page.
    }

    if (printLine > NUMBER_OF_LINES) printLine = NUMBER_OF_LINES; // Catch any changes to number of lines.

	page = (chosenLine-1) / NUMBER_OF_LINES;
							
	if (reposition) DeterminePrintingLine( chosenLine );  // If we have selected to reposition the list, determine where to print from.
	UpdateListClock();
	DrawArchiveList();
    appendToLogfile("RefreshArchiveList: Calling UpdateSelectionNumber.", WARNING);
	UpdateSelectionNumber();
    appendToLogfile("RefreshArchiveList: Finished.", INFO);
}


