/************************************************************
				Part of the UK TAP Project
	This module handles to loading, and saving of the configuration

Name	: IniFile.c
Author	: Darkmatter
Version	: 0.0
For		: Topfield TF5x00 series PVRs
Licence	:
Descr.	:
Usage	:
History	: v0.0 Darkmatter: 02-08-05 	Inception date
		  v0.1 Kidhazy:    17-08-05     Added Now/Next/Later tag option
		                                Added ProgressInfo option

	Last change:  USE   7 Aug 105   11:01 pm
************************************************************/

#define PROJECT_DIRECTORY "UK TAP Project"
#define OPTIONS_FILENAME "Surfer.ini"
#define OLD_OPTIONS_FILENAME "UkChannels.ini"

#define DATA_BUFFER_SIZE_ini 1024

static char *dataBuffer_ini;
static int dataBufferPtr_ini;

//  Variables for configuration options.
static dword mainActivationKey;
static TYPE_ModelType unitModelType;
static byte highlightChoice;
static byte displayOption;
static byte NowNextTagOption;
static byte ProgressInfoOption;
static byte RecallKeyOption;
static int  screenFadeOption;
static byte StartChannelOption;  
static byte optionStartOffset;
static byte optionEndOffset;  
static byte GuideStartOption=2;
//static byte TvRatioOption;  // Moved to Main routine
static byte  GuideWindowOption=0;
static byte progressBarOption;

//  Variables for temporary configuration options.
static dword CurrentActivationKey;
static TYPE_ModelType currentModelType;
static byte currentHighlight;
static byte currentNowNextTag;
static byte currentProgressInfo;
static byte currentRecallKey;
static int  currentScreenFade;
static byte currentStartChannel;
static byte currentStartOffset;
static byte currentEndOffset;
static byte currentGuideStart;
static byte currentTvRatio;
static byte currentGuideWindowOption;
static byte currentProgressBarOption;



//----------------
//
void WriteIniFile( TYPE_File *writeFile )
{
//	TAP_Hdd_ChangeDir( PROJECT_DIRECTORY );
    GotoPath( TAPIniDir );
	if ( TAP_Hdd_Exist( OPTIONS_FILENAME ) ) TAP_Hdd_Delete( OPTIONS_FILENAME );	// Just delete any old copies

	TAP_Hdd_Create( OPTIONS_FILENAME, ATTR_PROGRAM );						// Create the file

	writeFile = TAP_Hdd_Fopen( OPTIONS_FILENAME );
	if ( writeFile == NULL ) return; 										// Check we can open it

	TAP_Hdd_Fwrite( dataBuffer_ini, DATA_BUFFER_SIZE_ini, 1, writeFile );	// dump the whole buffer in one hit

	TAP_Hdd_Fclose( writeFile );
//	TAP_Hdd_ChangeDir("..");												// return to original directory
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
	    case TF5800 : TAP_SPrint(str, "TF5800\r\n" ); break;
		case TF5000 : TAP_SPrint(str, "TF5000\r\n" ); break;
		default 	: TAP_SPrint(str, "BAD\r\n" ); break;
	}
	WriteStrToIniBuf( str );

	TAP_SPrint(str, "%d\r\n", mainActivationKey );
	WriteStrToIniBuf( str );

	TAP_SPrint(str, "%d\r\n", highlightChoice );
	WriteStrToIniBuf( str );

	TAP_SPrint(str, "%d\r\n", displayOption );
	WriteStrToIniBuf( str );
	
	TAP_SPrint(str, "%d\r\n", NowNextTagOption );
	WriteStrToIniBuf( str );
	
	TAP_SPrint(str, "%d\r\n", ProgressInfoOption );
	WriteStrToIniBuf( str );
	
	TAP_SPrint(str, "%d\r\n", RecallKeyOption );
	WriteStrToIniBuf( str );
	
	TAP_SPrint(str, "%d\r\n", screenFadeOption );
	WriteStrToIniBuf( str );
	
	TAP_SPrint(str, "%d\r\n", StartChannelOption );
	WriteStrToIniBuf( str );
	
	TAP_SPrint(str, "%d\r\n", optionStartOffset );
	WriteStrToIniBuf( str );
	
	TAP_SPrint(str, "%d\r\n", optionEndOffset );
	WriteStrToIniBuf( str );
	
	TAP_SPrint(str, "%d\r\n", GuideStartOption );
	WriteStrToIniBuf( str );
	
	TAP_SPrint(str, "%d\r\n", TvRatioOption );
	WriteStrToIniBuf( str );

	TAP_SPrint(str, "%d\r\n", GuideWindowOption );
	WriteStrToIniBuf( str );

	TAP_SPrint(str, "%d\r\n", SCREEN_X );
	WriteStrToIniBuf( str );
	
	TAP_SPrint(str, "%d\r\n", SCREEN_Y );
	WriteStrToIniBuf( str );
	
	TAP_SPrint(str, "%d\r\n", SCREEN_W );
	WriteStrToIniBuf( str );
	
	TAP_SPrint(str, "%d\r\n", SCREEN_H );
	WriteStrToIniBuf( str );
	
	TAP_SPrint(str, "%d\r\n", GDE_SCREEN_X );
	WriteStrToIniBuf( str );
	
	TAP_SPrint(str, "%d\r\n", GDE_SCREEN_Y );
	WriteStrToIniBuf( str );
	
	TAP_SPrint(str, "%d\r\n", GDE_SCREEN_W );
	WriteStrToIniBuf( str );
	
	TAP_SPrint(str, "%d\r\n", GDE_SCREEN_H );
	WriteStrToIniBuf( str );
	
	TAP_SPrint(str, "%d\r\n", PIP_SCRX );
	WriteStrToIniBuf( str );
	
	TAP_SPrint(str, "%d\r\n", PIP_SCRY );
	WriteStrToIniBuf( str );
	
	TAP_SPrint(str, "%d\r\n", PIP_SCRW );
	WriteStrToIniBuf( str );
	
	TAP_SPrint(str, "%d\r\n", PIP_SCRH );
	WriteStrToIniBuf( str );
	
	TAP_SPrint(str, "%d\r\n", SCREEN_RATIO );
	WriteStrToIniBuf( str );
	
	TAP_SPrint(str, "%d\r\n", BASE_X );
	WriteStrToIniBuf( str );
	
	TAP_SPrint(str, "%d\r\n", BASE_Y );
	WriteStrToIniBuf( str );
	
	TAP_SPrint(str, "%d\r\n", GDE_W );
	WriteStrToIniBuf( str );
	
	TAP_SPrint(str, "%d\r\n", EVENT_X );
	WriteStrToIniBuf( str );
	
	TAP_SPrint(str, "%d\r\n", EVENT_Y );
	WriteStrToIniBuf( str );
	
	TAP_SPrint(str, "%d\r\n", EVENT_H );
	WriteStrToIniBuf( str );
	
	TAP_SPrint(str, "%d\r\n", CLOCK_X );
	WriteStrToIniBuf( str );
	
	TAP_SPrint(str, "%d\r\n", CLOCK_Y );
	WriteStrToIniBuf( str );
	
	TAP_SPrint(str, "%d\r\n", OVERSCAN_X );
	WriteStrToIniBuf( str );
	
	TAP_SPrint(str, "%d\r\n", OVERSCAN_Y );
	WriteStrToIniBuf( str );
	
	TAP_SPrint(str, "%d\r\n", OVERSCAN_W );
	WriteStrToIniBuf( str );
	
	TAP_SPrint(str, "%d\r\n", OVERSCAN_H );
	WriteStrToIniBuf( str );
	
	TAP_SPrint(str, "%d\r\n", progressBarOption );
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
	
	strcpy( str, ReadIniField() );										// Read the Duration
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

	if ( strcmp( str, "TF5000" ) == 0 ) unitModelType = TF5000;
	else unitModelType = TF5800;

	mainActivationKey  = ReadIniDecimal();
	highlightChoice    = ReadIniDecimal();
	displayOption      = ReadIniDecimal();
	NowNextTagOption   = ReadIniDecimal();
	ProgressInfoOption = ReadIniDecimal();
	RecallKeyOption    = ReadIniDecimal();
	screenFadeOption   = ReadIniDecimal();
	StartChannelOption = ReadIniDecimal();
	optionStartOffset  = ReadIniDecimal();
	optionEndOffset    = ReadIniDecimal();
	GuideStartOption   = ReadIniDecimal();
	TvRatioOption      = ReadIniDecimal();     
	GuideWindowOption  = ReadIniDecimal();     
    SCREEN_X           = ReadIniDecimal();
    SCREEN_Y           = ReadIniDecimal();   
    SCREEN_W           = ReadIniDecimal();
    SCREEN_H           = ReadIniDecimal();
    GDE_SCREEN_X       = ReadIniDecimal();
    GDE_SCREEN_Y       = ReadIniDecimal();
    GDE_SCREEN_W       = ReadIniDecimal();
    GDE_SCREEN_H       = ReadIniDecimal();
    PIP_SCRX           = ReadIniDecimal();
    PIP_SCRY           = ReadIniDecimal();
    PIP_SCRW           = ReadIniDecimal();
    PIP_SCRH           = ReadIniDecimal();
    SCREEN_RATIO       = ReadIniDecimal();
    BASE_X             = ReadIniDecimal();
    BASE_Y             = ReadIniDecimal();
    GDE_W              = ReadIniDecimal();
    EVENT_X            = ReadIniDecimal();
    EVENT_Y            = ReadIniDecimal();
    EVENT_H            = ReadIniDecimal();
    CLOCK_X            = ReadIniDecimal();
    CLOCK_Y            = ReadIniDecimal();
    OVERSCAN_X         = ReadIniDecimal();
    OVERSCAN_Y         = ReadIniDecimal();
    OVERSCAN_W         = ReadIniDecimal();
    OVERSCAN_H         = ReadIniDecimal();
    progressBarOption  = ReadIniDecimal();
	
}


bool ReadConfigurationFile( void )
{
	TYPE_File *readFile;
	int i;
	dword fileLength;

//	TAP_Hdd_ChangeDir( PROJECT_DIRECTORY );
    GotoPath( TAPIniDir );
	if ( ! TAP_Hdd_Exist( OPTIONS_FILENAME ) ) return FALSE;			// check the timer file exits in the current directory
	
	readFile = TAP_Hdd_Fopen( OPTIONS_FILENAME );
	if ( readFile == NULL ) return FALSE;								// and we can open it ok

	dataBuffer_ini = TAP_MemAlloc( DATA_BUFFER_SIZE_ini );				// Read the data in one hit (quicker), then process from RAM
	memset( dataBuffer_ini, '\0', DATA_BUFFER_SIZE_ini );					// set the whole buffer to the string termination character (null)
	dataBufferPtr_ini = 0;

	fileLength = TAP_Hdd_Flen( readFile );								// how big is the file
	if ( fileLength > DATA_BUFFER_SIZE_ini  ) fileLength = DATA_BUFFER_SIZE_ini;	// ensure we don't overflow the buffer
	
	TAP_Hdd_Fread( dataBuffer_ini, fileLength, 1, readFile );			// grab all the data from the file

	TAP_Hdd_Fclose( readFile );
//	TAP_Hdd_ChangeDir("..");											// return to original directory

	SetConfigurationVariables();

	TAP_MemFree( dataBuffer_ini );										// must return the memory back to the heap

	return TRUE;
}


void LoadConfiguration( void )
{
	if ( !ReadConfigurationFile() )
	{
//		unitModelType      = TF5800;
		unitModelType      = TF5000;
//		mainActivationKey  = RKEY_TvRadio;
		mainActivationKey  = RKEY_Ok;
		highlightChoice    = 2;  // Yellow arrows on black.
//		displayOption      = 0;
		displayOption      = 1;  // Linear
		NowNextTagOption   = 1;  // On
		ProgressInfoOption = 1;  // On
		RecallKeyOption    = 1;  // Swap channels
		screenFadeOption   = 1;  // Manual
		StartChannelOption = 0;  // Last channel
		optionStartOffset  = 0;  // 0 minutes
		optionEndOffset    = 0;  // 0 minutes
		GuideStartOption   = 0;  // Hidden 
		TvRatioOption      = 0;  // 4:3 Standard
		GuideWindowOption  = 1;  // Shrink
		Set43ScreenSizes();      // Set default screen size and layout for 4:3
        progressBarOption  = 0;  // Multicoloured
        		
		SaveConfigurationToFile();
	}
}

