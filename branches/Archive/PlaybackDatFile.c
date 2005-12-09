/************************************************************
		Part of the ukEPG project
	This module handles loading, and saving of the playback info

Name	: PlaybackDatFile.c
Author	: kidhazy based on DarkMatter's ini file.
Version	: 0.1
For		: Topfield TF5x00 series PVRs
Licence	:
Descr.	:
Usage	:
History	: v0.0 kidhazy: 2-11-05 	Inception date

	Last change:  USE   2 Aug 105   11:59 pm
************************************************************/

#define PROJECT_DIRECTORY "UK TAP Project"
#define PLAYDATA_FILENAME "OZArchivePlayback.dat"

#define PLAYBACKDATA_BUFFER_SIZE_ini 10240   // How many KB to read from the dat file.  (was 1024)

#define PLAYBACK_INFO_WRITE_INTERVAL   12000  // Write playback info to disk ever 6000 100th seconds 1minute (25000=4 minutes)  

static char *playDataBuffer_ini;
static int playDataBufferPtr_ini;


//----------------
//
void WriteDatFile( TYPE_File *writeFile )
{
    char* currentDir;
     
    appendToLogfile("WriteDatFile: Started.");
    appendToLogfile("WriteDatFile: GetCurrentDir called.");
    currentDir = GetCurrentDir();  // Store the current  directory.

    appendStringToLogfile("WriteDatFile: GetCurrentDir returned=%s.",currentDir);
    appendStringToLogfile("WriteDatFile: Moving to=%s.",TAPIniDir);
    GotoPath(TAPIniDir);           // Go to the Project Directory.
    
	if ( TAP_Hdd_Exist( PLAYDATA_FILENAME ) ) TAP_Hdd_Delete( PLAYDATA_FILENAME );	// Just delete any old copies

    appendToLogfile("WriteDatFile: Creating DATA file.");
	TAP_Hdd_Create( PLAYDATA_FILENAME, ATTR_PROGRAM );						// Create the file

    appendToLogfile("WriteDatFile: Opening DATA file.");
	writeFile = TAP_Hdd_Fopen( PLAYDATA_FILENAME );
	if ( writeFile == NULL ) return; 										// Check we can open it

    appendToLogfile("WriteDatFile: Writing to DATA file.");
	TAP_Hdd_Fwrite( playDataBuffer_ini, PLAYBACKDATA_BUFFER_SIZE_ini, 1, writeFile );	// dump the whole buffer in one hit

    appendToLogfile("WriteDatFile: Closing DATA file.");
	TAP_Hdd_Fclose( writeFile );

    appendStringToLogfile("WriteDatFile: Returning to=%s.",currentDir);
	GotoPath(currentDir);            // Return to the original directory.

    appendToLogfile("WriteDatFile: Finished.");
}



void WriteStrToDatBuf( char *str )											// add str to current end of buffer
{																			// and move end out
	int count, i;

    count = strlen(str);
	
	for ( i=0; i<count; i++)
	{
		playDataBuffer_ini[playDataBufferPtr_ini+i] = str[i];							// copy accross 1 char at a time
	}
	playDataBufferPtr_ini += i;
}



void SaveDatToFile( void )
{
	TYPE_File	*writeFile;
	int i;
	char	str[256];

    appendToLogfile("SaveDatToFile: Started.");
	playDataBuffer_ini = TAP_MemAlloc( PLAYBACKDATA_BUFFER_SIZE_ini );				// Buffer the write data to memory before writing all in one hit
	memset( playDataBuffer_ini, '\0', PLAYBACKDATA_BUFFER_SIZE_ini );				// set the whole buffer to the string termination character (null)
	playDataBufferPtr_ini = 0;

	TAP_SPrint(str, "%d\r\n", numberOfPlayedFiles);   // First entry is the number of played files.
	WriteStrToDatBuf( str );

    for (i = 0; i <= numberOfPlayedFiles; i++)
    {
         // Write entry TAB with delimited fields.
         // 1. startcluster
         // 2. current block position.
         // 3. total block size.
         // 4. filename.
         TAP_SPrint(str, "%d\t%d\t%d\t%s\r\n", playedFiles[i]->startCluster, playedFiles[i]->currentBlock, playedFiles[i]->totalBlock, playedFiles[i]->name); 
	     WriteStrToDatBuf( str );
    }

    appendToLogfile("SaveDatToFile: WriteDatFile called to write DATA file.");
	WriteDatFile( writeFile );										// write all the data in one pass
	TAP_MemFree( playDataBuffer_ini );										// must return the memory back to the heap
    appendToLogfile("SaveDatToFile: Finished.");
}


//------------------------------------------------------------------
//
char *ReadDatField( void )
{
	int i;
	char readChar;
	char *oldPtr;

	oldPtr = (char *) playDataBuffer_ini + playDataBufferPtr_ini;

	i=0;
	while ( playDataBufferPtr_ini < PLAYBACKDATA_BUFFER_SIZE_ini )					// Bounds check
	{
		readChar = playDataBuffer_ini[playDataBufferPtr_ini];					// read the file one character at a time

		if ( readChar == '\t' ) break;									// looking for TAB delimiter,
		if ( readChar == '\n' ) break;									// or end of line.
		if ( readChar == '\r' )
		{																// Carriage return is followed by new line -> discard them both.
			playDataBuffer_ini[playDataBufferPtr_ini] = '\0';
			playDataBufferPtr_ini++;
			break;
		}
		if ( readChar == '\0' ) break;									// Null terminator signifies end of file

		playDataBufferPtr_ini++;
	}

	playDataBuffer_ini[playDataBufferPtr_ini] = '\0';							// add a terminator to the string
	playDataBufferPtr_ini++;
	return oldPtr;														// return a pointer to the start of the current field
}



dword ReadDatDecimal( void )
{
    char 	str[256];
	dword	tmp;
	int i;
	
	strcpy( str, ReadDatField() );										// Read the next field
	tmp = 0;
	i = 0;

	while ( str[i] != '\0' )
	{
	    tmp = (tmp * 10) + (str[i] - '0');
		i++;
	}

	return tmp;
}


void SetDatVariables( void )
{
    int index;
     
	numberOfPlayedFiles = ReadDatDecimal();  // First entry is the number of played files.
	
	if (numberOfPlayedFiles > MAX_FILES)  // Boundary check.
	   numberOfPlayedFiles = MAX_FILES;

    for (index = 0; index<=numberOfPlayedFiles; index ++)
    {
         playedFiles[index] = TAP_MemAlloc( sizeof (*playedFiles[index]));
         playedFiles[index]->startCluster = ReadDatDecimal();   // Read the disk start cluster.
         playedFiles[index]->currentBlock = ReadDatDecimal();   // Read the current block position.
         playedFiles[index]->totalBlock   = ReadDatDecimal();   // Read the total block size.
         strcpy(playedFiles[index]->name,ReadDatField());        // Read the file name.
    }

}


bool ReadDatFile( void )
{
	TYPE_File *readFile;
	int i;
	dword fileLength;

    appendToLogfile("ReadDatFile: Started.");

//	TAP_Hdd_ChangeDir( PROJECT_DIRECTORY );
	GotoPath( TAPIniDir );
	if ( ! TAP_Hdd_Exist( PLAYDATA_FILENAME ) ) 
    {
         appendToLogfile("ReadDatFile: no PLAYDATA file found.");
         return FALSE;			// check the ini file exits in the current directory
    }     
	
	readFile = TAP_Hdd_Fopen( PLAYDATA_FILENAME );
	if ( readFile == NULL ) return FALSE;								// and we can open it ok

	playDataBuffer_ini = TAP_MemAlloc( PLAYBACKDATA_BUFFER_SIZE_ini );	// Read the data in one hit (quicker), then process from RAM
	memset( playDataBuffer_ini, '\0', PLAYBACKDATA_BUFFER_SIZE_ini );	// set the whole buffer to the string termination character (null)
	playDataBufferPtr_ini = 0;

	fileLength = TAP_Hdd_Flen( readFile );								// how big is the file
	if ( fileLength > PLAYBACKDATA_BUFFER_SIZE_ini  ) fileLength = PLAYBACKDATA_BUFFER_SIZE_ini;	// ensure we don't overflow the buffer
	
	TAP_Hdd_Fread( playDataBuffer_ini, fileLength, 1, readFile );			// grab all the data from the file

	TAP_Hdd_Fclose( readFile );
//	TAP_Hdd_ChangeDir("..");											// return to original directory

	SetDatVariables();

	TAP_MemFree( playDataBuffer_ini );										// must return the memory back to the heap

    appendToLogfile("ReadDatFile: Finished.");

	return TRUE;
}


void LoadPlayData( void )
{
    int i;
    char str[200];
    
    appendToLogfile("LoadPlayData: Started.");
     
	if ( !ReadDatFile() )   // If we can't find/read the data file, create an empty one.
	{
        appendToLogfile("LoadPlayData: Call to ReadDatFile returned FALSE.");
		
        numberOfPlayedFiles = 1;
		
        playedFiles[0] = TAP_MemAlloc( sizeof (*playedFiles[0]));
        playedFiles[0]->startCluster = 0;   // Store the disk start cluster.
        playedFiles[0]->currentBlock = 0;   // Store the current block position.
        playedFiles[0]->totalBlock   = 0;   // Store the total block size.
        strcpy(playedFiles[0]->name,"Placeholder for last played file.rec");        // Store the file name.
        
        playedFiles[1] = TAP_MemAlloc( sizeof (*playedFiles[1]));
        playedFiles[1]->startCluster = 12345678;   // Store the disk start cluster.
        playedFiles[1]->currentBlock = 9876543;   // Store the current block position.
        playedFiles[1]->totalBlock   = 9999999;   // Store the total block size.
        strcpy(playedFiles[1]->name,"This is a dummy filename.rec");        // Store the file name.

        appendToLogfile("LoadPlayData: Calling SaveDataToFile for new file.");
		SaveDatToFile();
	}
	playbackInfoTick = TAP_GetTick();		// Set timer to indicate when we retrieved the playback information.
	playinfoChanged  = FALSE;               // We've just loaded the data, so it hasn't changed yet.

/*
    TAP_Print("\r\n from loading numberOfPlayedFiles=%d \r\n",numberOfPlayedFiles);
    for (i = 1; i<=numberOfPlayedFiles; i ++)
    {
         TAP_Print("%d<< %d<< %d<<\r\n", playedFiles[i]->startCluster, playedFiles[i]->currentBlock, playedFiles[i]->totalBlock); 
         TAP_Print("%s<<\r\n", playedFiles[i]->name); 
    }
*/
    appendToLogfile("LoadPlayData: Finished.");

}


void DeletePlayData( void )
{   // Will delete the playback progress file, and then recreate a new one.
    char* currentDir;
    int i;
     
    appendToLogfile("DeletePlayData: Started.");

    appendToLogfile("DeletePlayData: GetCurrentDir called.");
    currentDir = GetCurrentDir();  // Store the current  directory.
    appendStringToLogfile("DeletePlayData: GetCurrentDir returned=%s.",currentDir);
    appendStringToLogfile("DeletePlayData: Moving to=%s.",TAPIniDir);
    GotoPath(TAPIniDir);           // Go to the Project Directory.

    appendToLogfile("DeletePlayData: Deleting DATA file.");
	if ( TAP_Hdd_Exist( PLAYDATA_FILENAME ) ) TAP_Hdd_Delete( PLAYDATA_FILENAME );	// Just delete any old copies

    appendToLogfile("DeletePlayData: Calling LoadPlayData.");
    LoadPlayData();  // Recreate and reload the new information.

    appendStringToLogfile("DeletePlayData: Returning to=%s.",currentDir);
	GotoPath(currentDir);            // Return to the original directory.

    // Got through all of our current files and remove any progress information.
    for (i=1; i <= numberOfFiles; i += 1)
    {
        myfiles[CurrentDirNumber][i]->hasPlayed = FALSE;  // Reset indicator to show that file has not been played.
        myfiles[CurrentDirNumber][i]->currentBlock = 0;
        myfiles[CurrentDirNumber][i]->totalBlock = 0;
    }   

    ShowMessageWin( rgn, "All Progress Info Cleared.", "Removed playback information", "for all recorded files.", 400 );

    appendToLogfile("DeletePlayData: Finished.");
}
     


void CheckPlaybackStatus( void )
{
   bool         matchFound;
   int          i, l, l1, l2;
  
     
   // Check for Playbackmode
   iOverRunCheck = ApiOverRunCheck;   // Set flag to catch any buffer overrun from API call.
   TAP_Hdd_GetPlayInfo (&CurrentPlaybackInfo);

   if ( iOverRunCheck != ApiOverRunCheck )  // TAP_Hdd_GetPlayInfo V1.22 has a buffer overrun issue, so check if it has overrun by more than 128 bytes.
   {
      ShowMessageWin( rgn, "Archive:  Error retrieving Playback Info", "TAP_Hdd_GetPlayInfo buffer overrun", "in 'CheckPlaybackStatus'.", 500 );
      return;
   }

   // Check the various playmodes to see if we are playing a file back.
   // PLAYMODE_None               Nothing playing
   // PLAYMODE_Playing            Playing a file
   // PLAYMODE_RecPlaying         Playing a currently recording file
   //
   // If we're not in playback then just return.
   if (CurrentPlaybackInfo.playMode != PLAYMODE_Playing)   // We're not in playback mode  
   {
       if (inPlaybackMode) // We were in playback mode, but now we've stopped.
       {
           SaveDatToFile(); // Write the information (particularly the last played file details) to disk
       }
       inPlaybackMode = FALSE;
       return;
   }    
   
   inPlaybackMode = TRUE;

   // Assign variable to the file entry for easier use.
   CurrentPlaybackFile = CurrentPlaybackInfo.file;
   
   // Copy the current playing info to the 'last' variable so we can restart the very last playback.
   playedFiles[0]->startCluster = CurrentPlaybackFile->startCluster;   // Save the disk start cluster.
   playedFiles[0]->currentBlock = CurrentPlaybackInfo.currentBlock;   // Save the current block position.
   playedFiles[0]->totalBlock   = CurrentPlaybackInfo.totalBlock;     // Save the total block size.
   strcpy(playedFiles[0]->name,CurrentPlaybackFile->name);        // Save the file name.
   
   matchFound = FALSE;
   
   // Search through the existing played file list to see if we have a match.
   for (i=1; i<=numberOfPlayedFiles; i++)
   {
        l1 = strlen(playedFiles[i]->name);
        l2 = strlen(CurrentPlaybackFile->name);
        l = max(l1, l2);
        if ((playedFiles[i]->startCluster == CurrentPlaybackFile->startCluster) && (strncmp(playedFiles[i]->name,CurrentPlaybackFile->name,TS_FILE_NAME_SIZE)==0))
        {
              // If we match an existing entry in our playedFiles array, update the currentBlock position information.
              playedFiles[i]->currentBlock = CurrentPlaybackInfo.currentBlock;
              matchFound = TRUE;
              break; // We've found a match, so don't bother checking other playback entries.
        }
   }

   // If we didn't find a match in our existing list of played files, so add to our list if we have space.   
   if ((!matchFound) && (numberOfPlayedFiles < MAX_FILES) && (strcmp(CurrentPlaybackFile->name,"__temprec__.ts")!=0))
   {
         numberOfPlayedFiles++;  // Increase the number of Played Files.
         playedFiles[numberOfPlayedFiles] = TAP_MemAlloc( sizeof (*playedFiles[numberOfPlayedFiles]));
         playedFiles[numberOfPlayedFiles]->startCluster = CurrentPlaybackFile->startCluster;   // Save the disk start cluster.
         playedFiles[numberOfPlayedFiles]->currentBlock = CurrentPlaybackInfo.currentBlock;   // Save the current block position.
         playedFiles[numberOfPlayedFiles]->totalBlock   = CurrentPlaybackInfo.totalBlock;     // Save the total block size.
         strcpy(playedFiles[numberOfPlayedFiles]->name,CurrentPlaybackFile->name);        // Save the file name.
         playinfoChanged = TRUE;  // Set flag so that playback info will get written to disk.
   }     
        
                
     
}

