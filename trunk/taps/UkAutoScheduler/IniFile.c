/************************************************************
		Part of the ukEPG project
	This module handles to loading, and saving of the configuration

Name	: IniFile.c
Author	: Darkmatter
Version	: 0.7
For	: Topfield TF5x00 series PVRs
Licence	:
Descr.	:
Usage	:
History	: v0.0 Darkmatter:	02-08-05 	Inception date
	  v0.1 sl8:		05-02-06	Modified for UKAS
	  v0.2 sl8:		15-02-06	'Perform Search' option added.
	  v0.3 sl8:		09-03-06	Firmware enable/disable option added
	  v0.4 sl8		11-04-06	SDK.
	  v0.5 sl8		12-04-06	Make TRC optional (default - enabled)
	  v0.6 sl8		05-08-06	Search ahead, date and time format added.
	  v0.7 sl8		28-08-06	Keyboard types.
	  v0.8 sl8		28-09-06	Conflict handler option added.
	  v0.9 sl8		15-12-06	Settings/UkAuto folder

************************************************************/

#define OPTIONS_FILENAME "UkAuto.ini"

#define DATA_BUFFER_SIZE_ini 1024

static char *dataBuffer_ini = NULL;
static int dataBufferPtr_ini = 0;


//----------------
//
void WriteIniFile( TYPE_File *writeFile )
{
	GotoPath(SETTINGS_FOLDER);
	if ( TAP_Hdd_Exist( OPTIONS_FILENAME ) ) TAP_Hdd_Delete( OPTIONS_FILENAME );	// Just delete any old copies

	TAP_Hdd_Create( OPTIONS_FILENAME, ATTR_PROGRAM );				// Create the file

	writeFile = TAP_Hdd_Fopen( OPTIONS_FILENAME );
	if ( writeFile == NULL ) return; 										// Check we can open it

	TAP_Hdd_Fwrite( dataBuffer_ini, DATA_BUFFER_SIZE_ini, 1, writeFile );		// dump the whole buffer in one hit

	TAP_Hdd_Fclose( writeFile );
}



void WriteStrToIniBuf( char *str )											// add str to current end of buffer
{																			// and move end out
	int count = 0, i = 0;

	count = strlen(str);
	
	for ( i=0; i<count; i++)
	{
		dataBuffer_ini[dataBufferPtr_ini+i] = str[i];		// copy accross 1 char at a time
	}
	dataBufferPtr_ini += i;
}



void SaveConfigurationToFile( void )
{
	TYPE_File	*writeFile = NULL;
	int	i = 0;
	char	str[256];

	dataBuffer_ini = TAP_MemAlloc( DATA_BUFFER_SIZE_ini );		// Buffer the write data to memory before writing all in one hit
	memset( dataBuffer_ini, '\0', DATA_BUFFER_SIZE_ini );		// set the whole buffer to the string termination character (null)
	dataBufferPtr_ini = 0;

	switch ( unitModelType )
	{
		case TF5800 : TAP_SPrint(str, "TF5800\r\n" ); break;
		case TF5000 : TAP_SPrint(str, "TF5000\r\n" ); break;
		default	: TAP_SPrint(str, "BAD\r\n" ); break;
	}
	WriteStrToIniBuf( str );

	TAP_SPrint(str, "%d\r\n", mainActivationKey );
	WriteStrToIniBuf( str );

	TAP_SPrint(str, "%d\r\n", schMainPerformSearchMode );
	WriteStrToIniBuf( str );

	TAP_SPrint(str, "%d\r\n", schMainPerformSearchTime );
	WriteStrToIniBuf( str );

	TAP_SPrint(str, "%d\r\n", FirmwareCallsEnabled );
	WriteStrToIniBuf( str );

	TAP_SPrint(str, "%d\r\n", schMainTRCEnabled );
	WriteStrToIniBuf( str );

	TAP_SPrint(str, "%d\r\n", schMainPerformSearchDays );
	WriteStrToIniBuf( str );

	TAP_SPrint(str, "%d\r\n", schMainDateFormat );
	WriteStrToIniBuf( str );

	TAP_SPrint(str, "%d\r\n", schMainTimeFormat );
	WriteStrToIniBuf( str );

	TAP_SPrint(str, "%d\r\n", keyboardLanguage );
	WriteStrToIniBuf( str );

	TAP_SPrint(str, "%d\r\n", schMainConflictOption );
	WriteStrToIniBuf( str );

	WriteIniFile( writeFile );					// write all the data in one pass

	TAP_MemFree( dataBuffer_ini );					// must return the memory back to the heap
}


//------------------------------------------------------------------
//
char *ReadIniField( void )
{
	int	i = 0;
	char	readChar = 0;
	char	*oldPtr = NULL;

	oldPtr = (char *) dataBuffer_ini + dataBufferPtr_ini;

	i=0;
	while ( dataBufferPtr_ini < DATA_BUFFER_SIZE_ini )		// Bounds check
	{
		readChar = dataBuffer_ini[dataBufferPtr_ini];		// read the file one character at a time

		if ( readChar == '\t' ) break;				// looking for TAB delimiter,
		if ( readChar == '\n' ) break;				// or end of line.
		if ( readChar == '\r' )
		{																// Carriage return is followed by new line -> discard them both.
			dataBuffer_ini[dataBufferPtr_ini] = '\0';
			dataBufferPtr_ini++;
			break;
		}
		if ( readChar == '\0' ) break;				// Null terminator signifies end of file

		dataBufferPtr_ini++;
	}

	dataBuffer_ini[dataBufferPtr_ini] = '\0';			// add a terminator to the string
	dataBufferPtr_ini++;
	return oldPtr;														// return a pointer to the start of the current field
}



dword ReadIniDecimal( dword defaultValue, bool *configPassed )
{
	char 	str[256];
	dword	tmp = 0;
	int	i = 0;
	
	strcpy( str, ReadIniField() );					// Read the Duration
	tmp = 0;
	i = 0;

	while ( str[i] != '\0' )
	{
	    tmp = (tmp * 10) + (str[i] - '0');
		i++;
	}

	if(i > 0)
	{
		return tmp;
	}
	else
	{
		*configPassed = FALSE;

		return defaultValue;
	}
}


bool SetConfigurationVariables( void )
{
	char	*str = NULL;
	bool	configPassed = TRUE;

	str = ReadIniField();

	if( strcmp( str, "TF5000" ) == 0 )
	{
		unitModelType = TF5000;
	}
	else
	{
		unitModelType = TF5800;
	}

	configPassed = TRUE;
	mainActivationKey = ReadIniDecimal(RKEY_Recall, &configPassed);
	schMainPerformSearchMode = ReadIniDecimal(SCH_CONFIG_SEARCH_PERIOD_TEN_MINS, &configPassed);
	schMainPerformSearchTime = ReadIniDecimal(0, &configPassed);
	FirmwareCallsEnabled = ReadIniDecimal(FALSE, &configPassed);
	schMainTRCEnabled = ReadIniDecimal(TRUE, &configPassed);
	schMainPerformSearchDays = ReadIniDecimal(14, &configPassed);
	schMainDateFormat = ReadIniDecimal(0, &configPassed);
	schMainTimeFormat = ReadIniDecimal(0, &configPassed);
	keyboardLanguage = ReadIniDecimal(0, &configPassed);
	schMainConflictOption = ReadIniDecimal(0, &configPassed);

	return configPassed;
}


bool ReadConfigurationFile( void )
{
	TYPE_File *readFile = NULL;
	int	i = 0;
	dword	fileLength = 0;
	bool	schSaveIniFile = FALSE;

	GotoPath( SETTINGS_FOLDER );
	if ( ! TAP_Hdd_Exist( OPTIONS_FILENAME ) )
	{
		GotoTapDir();
		TAP_Hdd_ChangeDir( PROJECT_DIRECTORY );
		if ( ! TAP_Hdd_Exist( OPTIONS_FILENAME ) ) return FALSE;

		schSaveIniFile = TRUE;
	}
	
	readFile = TAP_Hdd_Fopen( OPTIONS_FILENAME );
	if ( readFile == NULL ) return FALSE;						// and we can open it ok

	dataBuffer_ini = TAP_MemAlloc( DATA_BUFFER_SIZE_ini );				// Read the data in one hit (quicker), then process from RAM
	memset( dataBuffer_ini, '\0', DATA_BUFFER_SIZE_ini );				// set the whole buffer to the string termination character (null)
	dataBufferPtr_ini = 0;

	fileLength = TAP_Hdd_Flen( readFile );						// how big is the file
	if ( fileLength > DATA_BUFFER_SIZE_ini  ) fileLength = DATA_BUFFER_SIZE_ini;	// ensure we don't overflow the buffer
	
	TAP_Hdd_Fread( dataBuffer_ini, fileLength, 1, readFile );			// grab all the data from the file

	TAP_Hdd_Fclose( readFile );

	if(SetConfigurationVariables() == FALSE)
	{
		TAP_MemFree( dataBuffer_ini );						// must return the memory back to the heap

		SaveConfigurationToFile();
	}
	else
	{
		TAP_MemFree( dataBuffer_ini );						// must return the memory back to the heap
	}

	if(schSaveIniFile == TRUE)
	{
		SaveConfigurationToFile();

		GotoTapDir();
		TAP_Hdd_ChangeDir( PROJECT_DIRECTORY );

		if ( TAP_Hdd_Exist( OPTIONS_FILENAME ) ) TAP_Hdd_Delete( OPTIONS_FILENAME );
	}

	return TRUE;
}


void LoadConfiguration( void )
{
	if ( !ReadConfigurationFile() )
	{
		unitModelType = TF5800;

		mainActivationKey = RKEY_Recall;
		schMainPerformSearchMode = SCH_CONFIG_SEARCH_PERIOD_TEN_MINS;
		schMainPerformSearchTime = 0;
		FirmwareCallsEnabled = FALSE;
		schMainTRCEnabled = TRUE;
		schMainPerformSearchDays = 14;
		schMainDateFormat = 0;
		schMainTimeFormat = 0;
		keyboardLanguage = 0;
		schMainConflictOption = 0;

		SaveConfigurationToFile();
	}
}

