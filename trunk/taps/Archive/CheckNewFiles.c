/************************************************************
		Part of the Archive project
	This module handles loading, and saving of the last viewed data information

Name	: CheckNewFiles.c
Author	: kidhazy
Version	: 0.1
For		: Topfield TF5x00 series PVRs
Licence	:
Descr.	:
Usage	:
History	: v0.0 kidhazy: 2/11/06 	Inception date

	Last change:  USE   2 Aug 105   11:59 pm
************************************************************/
#define LASTVIEWDATA_FILENAME "OZArchiveLastView.dat"

#define LASTVIEWDATA_BUFFER_SIZE_ini 10240   // How many KB to read from the dat file.  (was 1024)

#define LASTVIEW_INFO_WRITE_INTERVAL   12000  // Write last viewed info to disk ever 6000 100th seconds 1minute (25000=4 minutes)  

static char *lastviewDataBuffer_ini;
static int lastviewDataBufferPtr_ini;

//------------
//
void ResetNewFileIndicators(int directory)
{
     int i;
     
     for (i=1; i<=numberOfFiles; i++)
     {
          myfiles[directory][i]->isNew = FALSE;
     }
}     


//------------
//
void SetDirectoryLastViewed(int directory)
{
     int i;
	 byte 	currentHour, currentMin, currentSec;
	 word 	currentMJD;

	 TAP_GetTime( &currentMJD, &currentHour, &currentMin, &currentSec);
	 myfolders[directory]->lastViewMjd  = currentMJD;
	 myfolders[directory]->lastViewHour = currentHour;
	 myfolders[directory]->lastViewMin  = currentMin;
	 myfolders[directory]->lastViewSec  = currentSec;
}     



//----------------
//
void WriteLastViewDatFile( TYPE_File *writeFile )
{
    char* currentDir;
     
    appendToLogfile("WriteLastViewDatFile: Started.", INFO);
    #ifdef WIN32
    #else
    appendToLogfile("WriteLastViewDatFile: GetCurrentDir called.", WARNING);
    currentDir = GetCurrentDir();  // Store the current  directory.
    appendStringToLogfile("WriteLastViewDatFile: GetCurrentDir returned=%s.",currentDir, WARNING);
    #endif

    appendStringToLogfile("WriteLastViewDatFile: Moving to=%s.",TAPIniDir, WARNING);
    GotoPath(TAPIniDir);           // Go to the Project Directory.
 
 	if ( TAP_Hdd_Exist( LASTVIEWDATA_FILENAME ) ) TAP_Hdd_Delete( LASTVIEWDATA_FILENAME );	// Just delete any old copies

    appendToLogfile("WriteLastViewDatFile: Creating DATA file.", ERR);
	TAP_Hdd_Create( LASTVIEWDATA_FILENAME, ATTR_PROGRAM );						// Create the file

    appendToLogfile("WriteLastViewDatFile: Opening DATA file.", ERR);
	writeFile = TAP_Hdd_Fopen( LASTVIEWDATA_FILENAME );
	if ( writeFile == NULL ) return; 										// Check we can open it

    appendToLogfile("WriteLastViewDatFile: Writing to DATA file.", ERR);
	TAP_Hdd_Fwrite( lastviewDataBuffer_ini, LASTVIEWDATA_BUFFER_SIZE_ini, 1, writeFile );	// dump the whole buffer in one hit

    appendToLogfile("WriteLastViewDatFile: Closing DATA file.", ERR);
	TAP_Hdd_Fclose( writeFile );

    #ifdef WIN32
    #else
    appendStringToLogfile("WriteLastViewDatFile: Returning to=%s.",currentDir, WARNING);
	GotoPath(currentDir);        // Return to the original directory.
    TAP_MemFree( currentDir );   // Free allocated memory.
	#endif
    
    appendToLogfile("WriteLastViewDatFile: Finished.", INFO);
}



void WriteStrToLastViewDatBuf( char *str )											// add str to current end of buffer
{																			// and move end out
	int count, i;

    count = strlen(str);
	
	for ( i=0; i<count; i++)
	{
		lastviewDataBuffer_ini[lastviewDataBufferPtr_ini+i] = str[i];							// copy accross 1 char at a time
	}
	lastviewDataBufferPtr_ini += i;
}



void SaveLastViewDatToFile( void )
{
	TYPE_File	*writeFile;
	int i;
	char	str[256];

    appendToLogfile("SaveLastViewDatToFile: Started.", INFO);
	lastviewDataBuffer_ini = TAP_MemAlloc( LASTVIEWDATA_BUFFER_SIZE_ini );				// Buffer the write data to memory before writing all in one hit
	memset( lastviewDataBuffer_ini, '\0', LASTVIEWDATA_BUFFER_SIZE_ini );				// set the whole buffer to the string termination character (null)
	lastviewDataBufferPtr_ini = 0;

	TAP_SPrint(str, "%d\r\n", numberOfFolders);   // First entry is the number of folders
	WriteStrToLastViewDatBuf( str );

    for (i = 0; i <= numberOfFolders; i++)
    {
         // Write entry TAB with delimited fields.
         // 1. directory name.
         // 2. last date viewed (MJD format).
         // 3. last hour viewed.
         // 4. last minute viewed.
         // 5. last hour viewed.
         TAP_SPrint(str, "%s\t%d\t%d\t%d\t%d\r\n", myfiles[myfolders[i]->directoryNumber][1]->directory, myfolders[i]->lastViewMjd, myfolders[i]->lastViewHour, myfolders[i]->lastViewMin, myfolders[i]->lastViewSec ); 
//         TAP_SPrint(str, "/DataFiles\t20051103\t19\t05\t38\r\n"); 
	     WriteStrToLastViewDatBuf( str );
    }

    appendToLogfile("SaveLastViewDatToFile: WriteDatFile called to write DATA file.", WARNING);
	WriteLastViewDatFile( writeFile );										// write all the data in one pass
	TAP_MemFree( lastviewDataBuffer_ini );										// must return the memory back to the heap
    appendToLogfile("SaveLastViewDatToFile: Finished.", INFO);
}


//------------------------------------------------------------------
//
char *ReadLastViewDatField( void )
{
	int i;
	char readChar;
	char *oldPtr;

	oldPtr = (char *) lastviewDataBuffer_ini + lastviewDataBufferPtr_ini;

	i=0;
	while ( lastviewDataBufferPtr_ini < LASTVIEWDATA_BUFFER_SIZE_ini )					// Bounds check
	{
		readChar = lastviewDataBuffer_ini[lastviewDataBufferPtr_ini];					// read the file one character at a time

		if ( readChar == '\t' ) break;									// looking for TAB delimiter,
		if ( readChar == '\n' ) break;									// or end of line.
		if ( readChar == '\r' )
		{																// Carriage return is followed by new line -> discard them both.
			lastviewDataBuffer_ini[lastviewDataBufferPtr_ini] = '\0';
			lastviewDataBufferPtr_ini++;
			break;
		}
		if ( readChar == '\0' ) break;									// Null terminator signifies end of file

		lastviewDataBufferPtr_ini++;
	}

	lastviewDataBuffer_ini[lastviewDataBufferPtr_ini] = '\0';							// add a terminator to the string
	lastviewDataBufferPtr_ini++;
	return oldPtr;														// return a pointer to the start of the current field
}



dword ReadLastViewDatDecimal( void )
{
    char 	str[256];
	dword	tmp;
	int i;
	
	strcpy( str, ReadLastViewDatField() );										// Read the next field
	tmp = 0;
	i = 0;

	while ( str[i] != '\0' )
	{
	    tmp = (tmp * 10) + (str[i] - '0');
		i++;
	}

	return tmp;
}


void SetLastViewDatVariables( void )
{
    int index, folders;
     
	folders = ReadLastViewDatDecimal();  // First entry is the number of played files.
	
	if (folders > MAX_DIRS)  // Boundary check.
	   folders = MAX_DIRS;

    for (index = 0; index<=folders; index ++)
    {
         strcpy(folderLastViewInfo[index].name,ReadLastViewDatField());       // Read the folder name.
         folderLastViewInfo[index].lastViewMjd  = ReadLastViewDatDecimal();   // Read the disk start cluster.
         folderLastViewInfo[index].lastViewHour = ReadLastViewDatDecimal();   // Read the current block position.
         folderLastViewInfo[index].lastViewMin  = ReadLastViewDatDecimal();   // Read the total block size.
         folderLastViewInfo[index].lastViewSec  = ReadLastViewDatDecimal();   // Read the total block size.
//TAP_Print("Fld:%s<< Mjd=%d Hour=%d Min=%d Sec=%d \r\n", folderLastViewInfo[index].name, folderLastViewInfo[index].lastViewMjd, folderLastViewInfo[index].lastViewHour, folderLastViewInfo[index].lastViewMin, folderLastViewInfo[index].lastViewSec);
    }

}

void ReloadLastViewDatVariables( void )
{
    int index, folders;
     
	folders = numberOfFolders;  // First entry is the number of played files.
	
	if (folders > MAX_DIRS)  // Boundary check.
	   folders = MAX_DIRS;

    for (index = 0; index<=folders; index ++)
    {
         strcpy(folderLastViewInfo[index].name,myfiles[myfolders[index]->directoryNumber][1]->directory);              // Set the folder name.
         folderLastViewInfo[index].lastViewMjd  = myfolders[index]->lastViewMjd;    // Set the last view date.
         folderLastViewInfo[index].lastViewHour = myfolders[index]->lastViewHour;   // Set the last view hour.
         folderLastViewInfo[index].lastViewMin  = myfolders[index]->lastViewMin;    // Set the last view min.
         folderLastViewInfo[index].lastViewSec  = myfolders[index]->lastViewSec;    // Set the last view sec.         
    }
}


bool ReadLastViewDatFile( void )
{
	TYPE_File *readFile;
	int i;
	dword fileLength;

    appendToLogfile("ReadLastViewDatFile: Started.", INFO);

	GotoPath( TAPIniDir );

	if ( ! TAP_Hdd_Exist( LASTVIEWDATA_FILENAME ) ) 
    {
         appendToLogfile("ReadLastViewDatFile: no PLAYDATA file found.", ERR);
         return FALSE;			// check the ini file exits in the current directory
    }     

	readFile = TAP_Hdd_Fopen( LASTVIEWDATA_FILENAME );
	if ( readFile == NULL ) return FALSE;								// and we can open it ok

	lastviewDataBuffer_ini = TAP_MemAlloc( LASTVIEWDATA_BUFFER_SIZE_ini );	// Read the data in one hit (quicker), then process from RAM
	memset( lastviewDataBuffer_ini, '\0', LASTVIEWDATA_BUFFER_SIZE_ini );	// set the whole buffer to the string termination character (null)
	lastviewDataBufferPtr_ini = 0;

	fileLength = TAP_Hdd_Flen( readFile );								// how big is the file
	if ( fileLength > LASTVIEWDATA_BUFFER_SIZE_ini  ) fileLength = LASTVIEWDATA_BUFFER_SIZE_ini;	// ensure we don't overflow the buffer
	
	TAP_Hdd_Fread( lastviewDataBuffer_ini, fileLength, 1, readFile );			// grab all the data from the file

	TAP_Hdd_Fclose( readFile );

	SetLastViewDatVariables();

	TAP_MemFree( lastviewDataBuffer_ini );										// must return the memory back to the heap

    appendToLogfile("ReadLastViewDatFile: Finished.", INFO);

	return TRUE;
}


void LoadLastViewData( void )
{
    int i;
    char str[200];
    
    appendToLogfile("LoadLastViewData: Started.", INFO);
     
	if ( !ReadLastViewDatFile() )   // If we can't find/read the data file, create an empty one.
	{
        appendToLogfile("LoadLastViewData: Call to ReadLastViewDatFile returned FALSE.", ERR);
/*		
        numberOfPlayedFiles = 1;
		
        playedFiles[0] = TAP_MemAlloc( sizeof (*playedFiles[0]));
        playedFiles[0]->startCluster = 0;   // Store the disk start cluster.
        playedFiles[0]->currentBlock = 0;   // Store the current block position.
        playedFiles[0]->totalBlock   = 1;   // Store the total block size. Set to 1 to ensure no division by 0 errors.
        strcpy(playedFiles[0]->name,"Placeholder for last played file.rec");        // Store the file name.
        
        playedFiles[1] = TAP_MemAlloc( sizeof (*playedFiles[1]));
        playedFiles[1]->startCluster = 12345678;   // Store the disk start cluster.
        playedFiles[1]->currentBlock = 9876543;   // Store the current block position.
        playedFiles[1]->totalBlock   = 9999999;   // Store the total block size.
        strcpy(playedFiles[1]->name,"This is a dummy filename.rec");        // Store the file name.
*/
        appendToLogfile("LoadLastViewData: Calling SaveDataToFile for new file.", ERR);
		SaveLastViewDatToFile();
	}
//	playbackInfoTick = TAP_GetTick();		// Set timer to indicate when we retrieved the playback information.
//	playinfoChanged  = FALSE;               // We've just loaded the data, so it hasn't changed yet.

/*
    TAP_Print("\r\n from loading numberOfPlayedFiles=%d \r\n",numberOfPlayedFiles);
    for (i = 1; i<=numberOfPlayedFiles; i ++)
    {
         TAP_Print("%d<< %d<< %d<<\r\n", playedFiles[i]->startCluster, playedFiles[i]->currentBlock, playedFiles[i]->totalBlock); 
         TAP_Print("%s<<\r\n", playedFiles[i]->name); 
    }
*/
    appendToLogfile("LoadLastViewData: Finished.", INFO);

}


void ClearLastViewFlags( void )
{   // Will clear the 'new' flags for all files and folders.
    char* currentDir;
    int i, i2;
     
    appendToLogfile("ClearLastViewFlags: Started.", INFO);

    appendToLogfile("ClearLastViewFlags: GetCurrentDir called.", WARNING);
    currentDir = GetCurrentDir();  // Store the current  directory.
    appendStringToLogfile("ClearLastViewFlags: GetCurrentDir returned=%s.",currentDir, WARNING);
    appendStringToLogfile("ClearLastViewFlags: Moving to=%s.",TAPIniDir, WARNING);
    GotoPath(TAPIniDir);           // Go to the Project Directory.

    appendToLogfile("ClearLastViewFlags: Deleting DATA file.", ERR);
	if ( TAP_Hdd_Exist( LASTVIEWDATA_FILENAME ) ) TAP_Hdd_Delete( LASTVIEWDATA_FILENAME );	// Just delete any old copies

    appendToLogfile("ClearLastViewFlags: Calling LoadLastViewData.", ERR);
    LoadLastViewData();  // Recreate and reload the new information.

    appendStringToLogfile("ClearLastViewFlags: Returning to=%s.",currentDir, WARNING);
	GotoPath(currentDir);            // Return to the original directory.

    // Got through all of our current files and remove Last View flags
    for (i=0; i <= numberOfFolders; i += 1)
    {
        myfolders[i]->newRecs      = 0;
        myfolders[i]->newRecDirs   = 0;
        SetDirectoryLastViewed( i );
                
        for (i2=1; i2 <= myfolders[i]->numberOfFiles; i2 += 1)
        {
            myfiles[i][i2]->isNew = FALSE;  // Reset indicator to show that file has not been played.
        }   
    }

    ShowMessageWin( rgn, "All 'new' Flags Cleared.", "All 'new' file indicators", "removed for all files.", 400 );

    TAP_MemFree( currentDir );   // Free allocated memory.

    appendToLogfile("ClearLastViewFlags: Finished.", INFO);
}

void SetLastViewFlags( void )
{   // Will SET the 'new' flags for all files and folders.
    char* currentDir;
    int i, i2;
     
    appendToLogfile("SetLastViewFlags: Started.", INFO);

    appendToLogfile("SetLastViewFlags: GetCurrentDir called.", WARNING);
    currentDir = GetCurrentDir();  // Store the current  directory.
    appendStringToLogfile("SetLastViewFlags: GetCurrentDir returned=%s.",currentDir, WARNING);
    appendStringToLogfile("SetLastViewFlags: Moving to=%s.",TAPIniDir, WARNING);
    GotoPath(TAPIniDir);           // Go to the Project Directory.

    appendToLogfile("SetLastViewFlags: Deleting DATA file.", ERR);
	if ( TAP_Hdd_Exist( LASTVIEWDATA_FILENAME ) ) TAP_Hdd_Delete( LASTVIEWDATA_FILENAME );	// Just delete any old copies

    appendToLogfile("SetLastViewFlags: Calling LoadLastViewData.", ERR);
    LoadLastViewData();  // Recreate and reload the new information.

    appendStringToLogfile("SetLastViewFlags: Returning to=%s.",currentDir, WARNING);
	GotoPath(currentDir);            // Return to the original directory.

    // Got through all of our current files and remove Last View flags
    for (i=1; i <= numberOfFolders; i += 1)
    {
        myfolders[i]->newRecs      = 0;
        myfolders[i]->newRecDirs   = 0;
        myfolders[i]->lastViewMjd  = 0;
        myfolders[i]->lastViewHour = 0;
        myfolders[i]->lastViewMin  = 0;
        myfolders[i]->lastViewSec  = 0;
                
        for (i2=1; i2 <= myfolders[i]->numberOfFiles; i2 += 1)
        {
            myfiles[i][i2]->isNew = TRUE;  // Reset indicator to show that file has not been seen.
        }   
    }

    ShowMessageWin( rgn, "Set 'new' Flags.",LASTVIEWDATA_FILENAME , "deleted. Restart TAP", 400 );

    TAP_MemFree( currentDir );   // Free allocated memory.

    appendToLogfile("SetLastViewFlags: Finished.", INFO);
}
