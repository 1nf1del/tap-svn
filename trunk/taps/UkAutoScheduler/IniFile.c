/************************************************************
		Part of the ukEPG project
	This module handles to loading, and saving of the configuration

Name	: IniFile.c
Author	: Darkmatter
Version	: 0.1
For	: Topfield TF5x00 series PVRs
Licence	:
Descr.	:
Usage	:
History	: v0.0 Darkmatter:	02-08-05 	Inception date
	  v0.1 sl8:		05-02-06	Modified for UKAS

************************************************************/

#define OPTIONS_FILENAME "UkAuto.ini"

#define DATA_BUFFER_SIZE_ini 1024

static char *dataBuffer_ini = NULL;
static int dataBufferPtr_ini = 0;


//----------------
//
void WriteIniFile( TYPE_File *writeFile )
{
	GotoTapDir();
	TAP_Hdd_ChangeDir( PROJECT_DIRECTORY );
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



dword ReadIniDecimal( void )
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

	return tmp;
}


void SetConfigurationVariables( void )
{
	char *str = NULL;

	str = ReadIniField();

	if ( strcmp( str, "TF5000" ) == 0 ) unitModelType = TF5000;
	else unitModelType = TF5800;

	mainActivationKey = ReadIniDecimal();
}


bool ReadConfigurationFile( void )
{
	TYPE_File *readFile = NULL;
	int	i = 0;
	dword	fileLength = 0;

	GotoTapDir();
	TAP_Hdd_ChangeDir( PROJECT_DIRECTORY );
	if ( ! TAP_Hdd_Exist( OPTIONS_FILENAME ) ) return FALSE;			// check the timer file exits in the current directory
	
	readFile = TAP_Hdd_Fopen( OPTIONS_FILENAME );
	if ( readFile == NULL ) return FALSE;						// and we can open it ok

	dataBuffer_ini = TAP_MemAlloc( DATA_BUFFER_SIZE_ini );				// Read the data in one hit (quicker), then process from RAM
	memset( dataBuffer_ini, '\0', DATA_BUFFER_SIZE_ini );				// set the whole buffer to the string termination character (null)
	dataBufferPtr_ini = 0;

	fileLength = TAP_Hdd_Flen( readFile );						// how big is the file
	if ( fileLength > DATA_BUFFER_SIZE_ini  ) fileLength = DATA_BUFFER_SIZE_ini;	// ensure we don't overflow the buffer
	
	TAP_Hdd_Fread( dataBuffer_ini, fileLength, 1, readFile );			// grab all the data from the file

	TAP_Hdd_Fclose( readFile );

	SetConfigurationVariables();

	TAP_MemFree( dataBuffer_ini );							// must return the memory back to the heap

	return TRUE;
}


void LoadConfiguration( void )
{
	if ( !ReadConfigurationFile() )
	{
		unitModelType = TF5800;
#ifndef WIN32	
		mainActivationKey = RKEY_Recall;
#else
		mainActivationKey = RKEY_F1;
#endif
		SaveConfigurationToFile();
	}
}

