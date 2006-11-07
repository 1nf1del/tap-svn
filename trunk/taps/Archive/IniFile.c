/************************************************************
		Part of the ukEPG project
	This module handles to loading, and saving of the configuration

Name	: IniFile.c
Author	: Darkmatter
Version	: 0.15
For		: Topfield TF5x00 series PVRs
Licence	:
Descr.	:
Usage	:
History	: v0.0 Darkmatter: 02-08-05 	Inception date

	Last change:  USE   2 Aug 105   11:59 pm
************************************************************/

#define PROJECT_DIRECTORY  "UK TAP Project"
#define OPTIONS_FILENAME "OZArchive.ini"

#define DATA_BUFFER_SIZE_ini 1024

static char *dataBuffer_ini;
static int dataBufferPtr_ini;



//----------------
//
void WriteIniFile( TYPE_File *writeFile )
{
    GotoPath( TAPIniDir );
	if ( TAP_Hdd_Exist( OPTIONS_FILENAME ) ) TAP_Hdd_Delete( OPTIONS_FILENAME );	// Just delete any old copies

	TAP_Hdd_Create( OPTIONS_FILENAME, ATTR_PROGRAM );						// Create the file

	writeFile = TAP_Hdd_Fopen( OPTIONS_FILENAME );
	if ( writeFile == NULL ) return; 										// Check we can open it

	TAP_Hdd_Fwrite( dataBuffer_ini, DATA_BUFFER_SIZE_ini, 1, writeFile );	// dump the whole buffer in one hit

	TAP_Hdd_Fclose( writeFile );
    GotoPath( CurrentDir );												// return to original directory
}



void WriteStrToIniBuf( char *str )											// add str to current end of buffer
{																			// and move end out
	int count, i;

    count = strlen(str);
	
	for ( i=0; i<count; i++)
	{
		dataBuffer_ini[dataBufferPtr_ini+i] = str[i];							// copy accross 1 char at a time
	}
	dataBufferPtr_ini += i;
}



void SaveConfigurationToFile( void )
{
	TYPE_File	*writeFile;
	int i;
	char	str[256];

	dataBuffer_ini = TAP_MemAlloc( DATA_BUFFER_SIZE_ini );				// Buffer the write data to memory before writing all in one hit
	memset( dataBuffer_ini, '\0', DATA_BUFFER_SIZE_ini );				// set the whole buffer to the string termination character (null)
	dataBufferPtr_ini = 0;


	switch ( unitModelType )
	{
	    case TF5800t : TAP_SPrint(str, "TF5800\r\n" ); break;
		case TF5000t : TAP_SPrint(str, "TF5000\r\n" ); break;
		default 	: TAP_SPrint(str, "BAD\r\n" ); break;
	}
	WriteStrToIniBuf( str );

	TAP_SPrint(str, "%d\r\n", mainActivationKey );
	WriteStrToIniBuf( str );

	TAP_SPrint(str, "%d\r\n", (GMToffsetOption+(12*60)) );
	WriteStrToIniBuf( str );

	TAP_SPrint(str, "%d\r\n", (recordingRateOption) );
	WriteStrToIniBuf( str );

	TAP_SPrint(str, "%d\r\n", (column1Option) );
	WriteStrToIniBuf( str );

	TAP_SPrint(str, "%d\r\n", (column2Option) );
	WriteStrToIniBuf( str );

	TAP_SPrint(str, "%d\r\n", (column3Option) );
	WriteStrToIniBuf( str );

	TAP_SPrint(str, "%d\r\n", (column4Option) );
	WriteStrToIniBuf( str );

	TAP_SPrint(str, "%d\r\n", (column5Option) );
	WriteStrToIniBuf( str );

	TAP_SPrint(str, "%d\r\n", (infoLineOption) );
	WriteStrToIniBuf( str );

	TAP_SPrint(str, "%d\r\n", (sortOrderOption) );
	WriteStrToIniBuf( str );

	TAP_SPrint(str, "%d\r\n", (progressBarOption) );
	WriteStrToIniBuf( str );

	TAP_SPrint(str, "%d\r\n", (numberLinesOption) );
	WriteStrToIniBuf( str );

	TAP_SPrint(str, "%d\r\n", (borderOption) );
	WriteStrToIniBuf( str );

	TAP_SPrint(str, "%d\r\n", (recCheckOption) );
	WriteStrToIniBuf( str );

	TAP_SPrint(str, "%d\r\n", (okPlayOption) );
	WriteStrToIniBuf( str );

	TAP_SPrint(str, "%d\r\n", (folderDeleteOption) );
	WriteStrToIniBuf( str );

	TAP_SPrint(str, "%d\r\n", (extInfoFontOption) );
	WriteStrToIniBuf( str );

	TAP_SPrint(str, "%d\r\n", (recycleBinOption) );
	WriteStrToIniBuf( str );

	TAP_SPrint(str, "%d\r\n", (recycleBinCleanoutOption) );
	WriteStrToIniBuf( str );

	TAP_SPrint(str, "%d\r\n", (recycleBinThresholdOption) );
	WriteStrToIniBuf( str );

	TAP_SPrint(str, "%d\r\n", (fileListKeyOption) );
	WriteStrToIniBuf( str );

	TAP_SPrint(str, "%d\r\n", (splashScreenOption) );
	WriteStrToIniBuf( str );
	
	TAP_SPrint(str, "%d\r\n", (PBKgmtOffsetOption) );
	WriteStrToIniBuf( str );

	TAP_SPrint(str, "%d\r\n", (NewIndicatorOption) );
	WriteStrToIniBuf( str );

	WriteIniFile( writeFile );										// write all the data in one pass
	TAP_MemFree( dataBuffer_ini );										// must return the memory back to the heap
}


//------------------------------------------------------------------
//
char *ReadIniField( void )
{
	int i;
	char readChar;
	char *oldPtr;

	oldPtr = (char *) dataBuffer_ini + dataBufferPtr_ini;

	i=0;
	while ( dataBufferPtr_ini < DATA_BUFFER_SIZE_ini )					// Bounds check
	{
		readChar = dataBuffer_ini[dataBufferPtr_ini];					// read the file one character at a time

		if ( readChar == '\t' ) break;									// looking for TAB delimiter,
		if ( readChar == '\n' ) break;									// or end of line.
		if ( readChar == '\r' )
		{																// Carriage return is followed by new line -> discard them both.
			dataBuffer_ini[dataBufferPtr_ini] = '\0';
			dataBufferPtr_ini++;
			break;
		}
		if ( readChar == '\0' ) break;									// Null terminator signifies end of file

		dataBufferPtr_ini++;
	}

	dataBuffer_ini[dataBufferPtr_ini] = '\0';							// add a terminator to the string
	dataBufferPtr_ini++;
	return oldPtr;														// return a pointer to the start of the current field
}



dword ReadIniDecimal( void )
{
    char 	str[256];
	dword	tmp;
	int i;
	
	strcpy( str, ReadIniField() );										// Read the field
	tmp = 0;
	i = 0;

	while ( str[i] != '\0' )
	{
	    tmp = (tmp * 10) + (str[i] - '0');
		i++;
	}

	return tmp;
}


void SetConfigurationVariables( void )
{
	char *str;

	str = ReadIniField();

	if ( strcmp( str, "TF5000" ) == 0 ) unitModelType = TF5000t;
	else unitModelType = TF5800t;

	mainActivationKey         = ReadIniDecimal();
	GMToffsetOption           = ReadIniDecimal()-(12*60);
	recordingRateOption       = ReadIniDecimal();
	column1Option             = ReadIniDecimal();
	column2Option             = ReadIniDecimal();
	column3Option             = ReadIniDecimal();
	column4Option             = ReadIniDecimal();
	column5Option             = ReadIniDecimal();
	infoLineOption            = ReadIniDecimal();
	sortOrderOption           = ReadIniDecimal();
	progressBarOption         = ReadIniDecimal();
	numberLinesOption         = ReadIniDecimal();
	borderOption              = ReadIniDecimal();
	recCheckOption            = ReadIniDecimal();
	okPlayOption              = ReadIniDecimal();
	folderDeleteOption        = ReadIniDecimal();
	extInfoFontOption         = ReadIniDecimal();
	recycleBinOption          = ReadIniDecimal();
	recycleBinCleanoutOption  = ReadIniDecimal();
	recycleBinThresholdOption = ReadIniDecimal();
	fileListKeyOption         = ReadIniDecimal();  
	splashScreenOption        = ReadIniDecimal();  
	PBKgmtOffsetOption        = ReadIniDecimal();
	NewIndicatorOption        = ReadIniDecimal();
}


bool ReadConfigurationFile( void )
{
	TYPE_File *readFile;
	int i;
	dword fileLength;

	GotoPath( TAPIniDir );
	if ( ! TAP_Hdd_Exist( OPTIONS_FILENAME ) ) return FALSE;			// check the timer file exits in the current directory
	
	readFile = TAP_Hdd_Fopen( OPTIONS_FILENAME );
	if ( readFile == NULL ) return FALSE;								// and we can open it ok

	dataBuffer_ini = TAP_MemAlloc( DATA_BUFFER_SIZE_ini );				// Read the data in one hit (quicker), then process from RAM
	memset( dataBuffer_ini, '\0', DATA_BUFFER_SIZE_ini );				// set the whole buffer to the string termination character (null)
	dataBufferPtr_ini = 0;

	fileLength = TAP_Hdd_Flen( readFile );								// how big is the file
	if ( fileLength > DATA_BUFFER_SIZE_ini  ) fileLength = DATA_BUFFER_SIZE_ini;	// ensure we don't overflow the buffer
	
	TAP_Hdd_Fread( dataBuffer_ini, fileLength, 1, readFile );			// grab all the data from the file

	TAP_Hdd_Fclose( readFile );

	SetConfigurationVariables();

	TAP_MemFree( dataBuffer_ini );										// must return the memory back to the heap

	return TRUE;
}


void LoadConfiguration( void )
{
	if ( (!ReadConfigurationFile()) || (recordingRateOption < 1000))  // If file is invalid, recreate a new one.
	{
        if (unitModelType==TF5800t) 
        {		
            GMToffsetOption = TF5800_GMT_OFFSET_DEFAULT;
            recordingRateOption = 2900;
        } 
        else     
        {		
            GMToffsetOption = TF5000_GMT_OFFSET_DEFAULT;
            unitModelType = TF5000t;
            recordingRateOption = 2900;
        } 
		mainActivationKey         = RKEY_Slow;
		sortOrderOption           = SORT_DATE_OPTION;         // Default to sort by date.
		progressBarOption         = PB_MULTI;
		column1Option             = 0;
		column2Option             = 0;
		column3Option             = 0;
		column4Option             = 0;
		column5Option             = 0;
		numberLinesOption         = 9;
		borderOption              = 1;
		recCheckOption            = 0;
		okPlayOption              = 0;
		folderDeleteOption        = 0;
		extInfoFontOption         = 0;
		recycleBinOption          = 0;
		recycleBinCleanoutOption  = 0;
		recycleBinThresholdOption = 0;
        fileListKeyOption         = 0;    
        splashScreenOption        = 0;
        PBKgmtOffsetOption        = 0;
        NewIndicatorOption        = 0;

		SaveConfigurationToFile();
	}
	
	if ((numberLinesOption <9) || (numberLinesOption >10)) 
    {
        numberLinesOption=9;
        SaveConfigurationToFile();
    }
}

void locateIniDirectory()
{
     bool UKProjectExists, SettingsExists;
     
     char* StartingDir;
     
     // Looks for the ini and logo.dat directory.
     //  Sets TAPIniDir and TAPLogoDir to the directories holding the INI and Logo files.
     //
     // 1. Will look for "/ProgramFiles/Settings/Archive" and "/ProgramFiles/Settings/Logos".  If present will use them.
     // 2. Will look for "UK TAP Project" in current directory.  If present, will set that as the TAPIniDir and TAPLogoDir.
     // 3. If it doesn't find either (1) or (2) it will create directories for (1) and set TAPIniDir and TAPLogoDir as per (1)
     
     UKProjectExists = FALSE;
     SettingsExists  = FALSE;
     
     StartingDir = GetCurrentDir();  // Save the current directory.
     
     // See if the "UK TAPS PROJECT" Directory exists.
	 if ( TAP_Hdd_Exist( PROJECT_DIRECTORY) ) 
	 {
         UKProjectExists = TRUE;
     }
     
     // Look for "/ProgramFiles/Settings/Archive"
     GotoPath("/ProgramFiles");              // Go back to the ProgramFiles Directory
     if ( TAP_Hdd_Exist( "Settings" ) )      // Check for the "Settings" subdirectory
     {
         TAP_Hdd_ChangeDir( "Settings" );       
         if ( TAP_Hdd_Exist( "Archive" ) ) 
         {
              if ( TAP_Hdd_Exist( "Logos" ) )     // Check for the "Archive" and "Logos" subdirectory
              {
                   SettingsExists  = TRUE;         // Flag that the Settings Directories are present. 
              } 
         }      
     }       

     if ((!SettingsExists) && (!UKProjectExists))  // If neither lot of directories exist, create and use the "settings" directories.
     {
         GotoPath("/ProgramFiles");              // Go back to the ProgramFiles Directory
         // Look for, and create if missing, "Settings"
	     if ( !TAP_Hdd_Exist( "Settings" ) )     // Check for the "Settings" subdirectory
	     {
              TAP_Hdd_Create( "Settings", ATTR_FOLDER );     // If it's missing, create it.
         }
         TAP_Hdd_ChangeDir( "Settings" );        // Switch into the "Settings" subdirectory
         
         // Look for, and create if missing, "Archive"
	     if ( !TAP_Hdd_Exist( "Archive" ) )      // Check for the "Archive" subdirectory
	     {
               TAP_Hdd_Create( "Archive", ATTR_FOLDER );     // If it's missing, create a new "Archive" subdirectory
         }
         
         // Look for, and create if missing, "Logos"
	     if ( !TAP_Hdd_Exist( "Logos" ) )        // Check for the "Archive" subdirectory
	     {
               TAP_Hdd_Create( "Logos", ATTR_FOLDER );       // If it's missing, create a new "Archive" subdirectory
         }
         SettingsExists  = TRUE;         // Flag that the Settings Directories are present. 
      }
     
     if (SettingsExists)   // Use the Settings dirs.
     {
         TAPIniDir  = TAP_MemAlloc (strlen("/ProgramFiles/Settings/Archive")+2);
         strcpy(TAPIniDir,  "/ProgramFiles/Settings/Archive");
         TAPLogoDir = TAP_MemAlloc (strlen("/ProgramFiles/Settings/Logos")+2);
         strcpy(TAPLogoDir, "/ProgramFiles/Settings/Logos");
     }
     else
     if (UKProjectExists)   // Use the UK TAP Project dirs.
     {
         GotoPath(StartingDir);                  // Return to the directory we started from.
         TAP_Hdd_ChangeDir(PROJECT_DIRECTORY);   // Change to the UK TAP Project SubDirectory.
         TAPIniDir  = GetCurrentDir();           // Store the directory location of the INI file.	
         TAPLogoDir = GetCurrentDir();           // Store the directory location of the LOGO file.	
     }
         

}
     
     
