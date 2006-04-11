/************************************************************
Part of the ukEPG project
This module handles all file activity

v0.0 sl8:	20-11-05	Inception date
v0.1 sl8:	20-01-06	Uses Kidhazy's method of changing directories. Modified for TAP_SDK
					All variables initialised.
v0.2 sl8:	06-02-06	Use Project directory define
v0.3 sl8:	15-02-06	Modified for new UkAutoSearch.txt file
v0.4 sl8:	09-03-06	Rewirte to accommodate Move file and remote file.
v0.5 sl8:	15-03-06	Bug Fix - Not saving the move file correctly
v0.6 sl8:	11-04-06	Show window added and tidy up.

**************************************************************/

void	schPrintSearchLine( int, TYPE_File* );
void	schPrintMoveLine( int, int, TYPE_File* );
void	schWriteFile( dword, TYPE_File*, char* );
void	WriteStrToBuf( char*, TYPE_File* );
bool	schConvertLcnToSvcNum(word, word*, bool);

void	schFileParseString(char*, bool*, int*, char*, int*, int, bool);
dword	schFileParseHex(bool*, int*, char*, int*, int, dword);
dword	schFileParseTime(bool*, int*, char*, int*);


static char *dataBuffer_sr = NULL;
static int dataBufferPtr_sr = 0;

#define DELIMIT_TAB		0x09
#define DELIMIT_NL		0x0A
#define DELIMIT_CR		0x0D

byte schFileRetreiveSearchData(void)
{
	TYPE_File	*searchFile = NULL;
	struct	schDataTag schTempUserData;
	bool	schValidSearch = FALSE;
	bool	schValidHeader = FALSE;
	int	bufferIndex = 0;
	dword	fileLength = 0;
	int	iTemp = 0;
	char	*buffer = NULL;
	dword	schLcn = 0;
	byte	schUserDataSearches = 0;
	int	maxBufferSize = 0;
	byte	version = 0;
	char	versionStr[128];
	dword	dwTemp = 0;
	char	buffer1[256];
	int	schNewLineCount = 0;
	
	maxBufferSize = (((SCH_MAX_SEARCHES * ((sizeof( struct schDataTag )) + 20)) / 512) + 1) * 512;

	GotoTapDir();
	TAP_Hdd_ChangeDir( PROJECT_DIRECTORY );

	if ( TAP_Hdd_Exist( SEARCH_FILENAME ) )
	{
		version = 2;

		searchFile = TAP_Hdd_Fopen( SEARCH_FILENAME );
	}
	else if ( TAP_Hdd_Exist( "SearchList.txt" ) )
	{
		version = 1;

		searchFile = TAP_Hdd_Fopen( "SearchList.txt" );
	}
	else
	{
		return 0;
	}

	if ( searchFile == NULL ) return 0;

	buffer = (char *)TAP_MemAlloc( maxBufferSize );

	fileLength = TAP_Hdd_Flen( searchFile );
	if ( fileLength > maxBufferSize ) fileLength = maxBufferSize;

	memset( buffer, 0, fileLength );

	TAP_Hdd_Fread( buffer, fileLength, 1, searchFile );

	TAP_Hdd_Fclose( searchFile );

	schMainTotalValidSearches = 0;

	bufferIndex = 0;

	schValidHeader = TRUE;

	if(version == 2)
	{
		schFileParseString(&versionStr[0], &schValidHeader, &schNewLineCount, buffer, &bufferIndex, 128, FALSE);
	}
	
	schUserDataSearches = schFileParseHex(&schValidHeader, &schNewLineCount, &buffer[0], &bufferIndex, 2, SCH_MAX_SEARCHES);
	if(schValidHeader == FALSE)
	{
		schUserDataSearches = 0;
	}

	if(schUserDataSearches > 0)
	{
		schValidSearch = TRUE;

		for(searchIndex = 0; ((searchIndex < schUserDataSearches) && (schValidSearch == TRUE)); searchIndex++)
		{
			schNewLineCount = 0;

			// ----------------- Search Number -----------------
			dwTemp = schFileParseHex(&schValidSearch, &schNewLineCount, &buffer[0], &bufferIndex, 2, SCH_MAX_SEARCHES);

			// ----------------- Search Status -----------------
			schTempUserData.searchStatus = schFileParseHex(&schValidSearch, &schNewLineCount, buffer, &bufferIndex, 2, 0xFF);

			// ----------------- Search Term -----------------
			schFileParseString(&schTempUserData.searchTerm[0], &schValidSearch, &schNewLineCount, buffer, &bufferIndex, 128, FALSE);

			// ----------------- Search Start Time -----------------
			schTempUserData.searchStartTime = schFileParseTime(&schValidSearch, &schNewLineCount, buffer, &bufferIndex);

			// ----------------- Search End Time -----------------
			schTempUserData.searchEndTime = schFileParseTime(&schValidSearch, &schNewLineCount, buffer, &bufferIndex);

			// ----------------- Search Start Channel -----------------
			schLcn = schFileParseHex(&schValidSearch, &schNewLineCount, buffer, &bufferIndex, 4, 0xFFFF);

			schTempUserData.searchTvRadio = SCH_TV;
			if((schLcn & 0x8000) != 0)
			{
				schTempUserData.searchTvRadio = SCH_RADIO;

				schLcn &= 0x7FFF;
			}

			if
			(
				(schLcn > 999)
				||
				(schConvertLcnToSvcNum(schLcn, &schTempUserData.searchStartSvcNum, schTempUserData.searchTvRadio) == FALSE)
			)
			{
				schValidSearch = FALSE;
			}

			// ----------------- Search End Channel -----------------
			schLcn = schFileParseHex(&schValidSearch, &schNewLineCount, buffer, &bufferIndex, 4, 0xFFFF);

			if
			(
				(schLcn > 999)
				||
				(schConvertLcnToSvcNum(schLcn, &schTempUserData.searchEndSvcNum, schTempUserData.searchTvRadio) == FALSE)
			)
			{
				schValidSearch = FALSE;
			}

			if(schTempUserData.searchEndSvcNum < schTempUserData.searchStartSvcNum)
			{
				schValidSearch = FALSE;
			}

			// ----------------- Search Day -----------------
			schTempUserData.searchDay = schFileParseHex(&schValidSearch, &schNewLineCount, &buffer[0], &bufferIndex, 2, 0x7F);

			// ----------------- Search Start Padding -----------------
			schTempUserData.searchStartPadding = schFileParseTime(&schValidSearch, &schNewLineCount, buffer, &bufferIndex);


			// ----------------- Search End Padding -----------------
			schTempUserData.searchEndPadding = schFileParseTime(&schValidSearch, &schNewLineCount, buffer, &bufferIndex);

			// ----------------- Search Attach -----------------
			schTempUserData.searchAttach = schFileParseHex(&schValidSearch, &schNewLineCount, buffer, &bufferIndex, 4, 0xFFFF);

			// ----------------- Search Options -----------------
			schTempUserData.searchOptions = schFileParseHex(&schValidSearch, &schNewLineCount, buffer, &bufferIndex, 2, 0xFF);

			// ----------------- Search Folder -----------------
			memset(schTempUserData.searchFolder,0,132);
			if(version == 2)
			{
				schFileParseString(&schTempUserData.searchFolder[0], &schValidSearch, &schNewLineCount, buffer, &bufferIndex, 128, TRUE);
			}

			// ----------------------------------------------

			if(schNewLineCount != 1)
			{
				schValidSearch = FALSE;
			}

			if(schValidSearch == TRUE)
			{
				schUserData[schMainTotalValidSearches] = schTempUserData;

				schMainTotalValidSearches++;
			}
			else
			{

			}
		}
	}

	TAP_MemFree( buffer );

	if(version == 1)
	{
		schWriteSearchList();
	}
//sprintf(buffer1,"Valid searches: %d\r\n", schMainTotalValidSearches );
//TAP_Print(buffer1);		

	return schMainTotalValidSearches;
}


int axtodw(char *hexString)
{
	dword result = 0;
	int i = 0;
	int j = 0;
	int length = 0;
	char buffer1[256];

	length = strlen( hexString );

	if( length == 0)
	{
		result = 0x0;
	}
	else if (length > 8)
	{
		result = 0xFFFFFFFF;
	}
	else
	{
		for (i = (length - 1); i >= 0; i--)
		{
			if ((hexString[i] >= '0') && (hexString[i] <= '9' ))		// 0 to 9
			{
				result += (hexString[i] & 0x0f) << (j * 4);
			}
			else if ((hexString[i] >='a') && (hexString[i] <= 'f'))		// a to f
			{
				result += ((hexString[i] & 0x0f) + 9) << (j * 4);
			}
			else if ((hexString[i] >='A') && (hexString[i] <= 'F'))		// A to F
			{
				result += ((hexString[i] & 0x0f) + 9) << (j * 4);
			}
			else
			{
			}

			j++;
		}
	}

	return (result);
}



bool schConvertLcnToSvcNum(word schLcn, word *schSvcNum, bool schRadio)
{
	word i = 0;
	bool found = FALSE;

	if(schLcn == 0)
	{
		schLcn = schLcnToServiceTv[0];		/* Pick first valid LCN */
	}

	if(schRadio == FALSE)
	{
		for (i = 0; ((i < schTotalTvSvc) && (found == FALSE)); i++)
		{
			if(schLcnToServiceTv[i] == schLcn)
			{
				*schSvcNum = i;

				found = TRUE;
			}
		}
	}
	else
	{
		for (i = 0; ((i < schTotalRadioSvc) && (found == FALSE)); i++)
		{
			if(schLcnToServiceRadio[i] == schLcn)
			{
				*schSvcNum = i;

				found = TRUE;
			}
		}
	}

	return found;
}



void schWriteSearchList( void )
{
	TYPE_File *searchFile = NULL;
	int	i = 0;
	dword	bufferSize = 0;
	char	str[256];

	bufferSize = (((schMainTotalValidSearches * ((sizeof( struct schDataTag )) + 20)) / 512) + 1) * 512;

	dataBuffer_sr = TAP_MemAlloc( bufferSize );	// Buffer the write data to memory before writing all in one hit
	memset( dataBuffer_sr, '\0', bufferSize );	// set the whole buffer to the string termination character (null)
	dataBufferPtr_sr = 0;

	WriteStrToBuf( SEARCH_INFO, searchFile );

	sprintf(str, "%02x\r\n", schMainTotalValidSearches);	// Total number of searches
	WriteStrToBuf( str, searchFile );

	for ( i = 0; i < schMainTotalValidSearches; i++)			// build an ordered list of searches, eariest=0
	{
		schPrintSearchLine( i, searchFile );
	}

	schWriteFile( bufferSize, searchFile, SEARCH_FILENAME );			// write all the data in one pass

	TAP_MemFree( dataBuffer_sr );				// must return the memory back to the heap
}


void schPrintSearchLine( int searchIndex, TYPE_File *searchFile )
{
	char	str[1024];
	int 	min = 0, hour = 0;

	sprintf(str,"%02x\t", (searchIndex+1) );				// Search Number
	WriteStrToBuf( str, searchFile );

	sprintf(str, "%02x\t", schUserData[searchIndex].searchStatus);	// Status
	WriteStrToBuf( str, searchFile );

	sprintf(str, "%s\t", schUserData[searchIndex].searchTerm);		// Search Term
	WriteStrToBuf( str, searchFile );

	hour = (schUserData[searchIndex].searchStartTime & 0xff00) >> 8;
	min = (schUserData[searchIndex].searchStartTime & 0xff);
	sprintf(str, "%02d:%02d\t", hour, min);				// Start Time
	WriteStrToBuf( str, searchFile );

	hour = (schUserData[searchIndex].searchEndTime & 0xff00) >> 8;
	min = (schUserData[searchIndex].searchEndTime & 0xff);
	sprintf(str, "%02d:%02d\t", hour, min);				// End Time
	WriteStrToBuf( str, searchFile );
	
	if(schUserData[searchIndex].searchTvRadio == SCH_TV)
	{
		sprintf(str, "%04x\t", (schLcnToServiceTv[schUserData[searchIndex].searchStartSvcNum]));
		WriteStrToBuf( str, searchFile );					// Start Channel with tv/radio indicator
	}
	else
	{
		sprintf(str, "%04x\t", (0x8000 + schLcnToServiceRadio[schUserData[searchIndex].searchStartSvcNum]));
		WriteStrToBuf( str, searchFile );					// Start Channel with tv/radio indicator
	}

	if(schUserData[searchIndex].searchTvRadio == SCH_TV)
	{
		sprintf(str, "%04x\t", schLcnToServiceTv[schUserData[searchIndex].searchEndSvcNum]);	// End Channel
		WriteStrToBuf( str, searchFile );
	}
	else
	{
		sprintf(str, "%04x\t", schLcnToServiceRadio[schUserData[searchIndex].searchEndSvcNum]);	// End Channel
		WriteStrToBuf( str, searchFile );
	}

	sprintf(str, "%02x\t", schUserData[searchIndex].searchDay);		// Days
	WriteStrToBuf( str, searchFile );

	hour = (schUserData[searchIndex].searchStartPadding & 0xff00) >> 8;
	min = (schUserData[searchIndex].searchStartPadding & 0xff);
	sprintf(str, "%02d:%02d\t", hour, min);				// Start Padding
	WriteStrToBuf( str, searchFile );

	hour = (schUserData[searchIndex].searchEndPadding & 0xff00) >> 8;
	min = (schUserData[searchIndex].searchEndPadding & 0xff);
	sprintf(str, "%02d:%02d\t", hour, min);				// Start Padding
	WriteStrToBuf( str, searchFile );

	sprintf(str, "%04x\t", schUserData[searchIndex].searchAttach);	// Attachments
	WriteStrToBuf( str, searchFile );

	sprintf(str, "%02x\t", schUserData[searchIndex].searchOptions);	// Options
	WriteStrToBuf( str, searchFile );

	sprintf(str, "%s", schUserData[searchIndex].searchFolder);	// Search Folder
	WriteStrToBuf( str, searchFile );

	WriteStrToBuf( "\r\n", searchFile );
}

void schWriteFile( dword bufferSize, TYPE_File *file, char* fileName )
{
	GotoTapDir();
	TAP_Hdd_ChangeDir( PROJECT_DIRECTORY );
	if ( TAP_Hdd_Exist( fileName ) ) TAP_Hdd_Delete( fileName );	// Just delete any old copies

	TAP_Hdd_Create( fileName, ATTR_PROGRAM );				// Create the file

	file = TAP_Hdd_Fopen( fileName );
	if ( file == NULL ) return; 						// Check we can open it

	TAP_Hdd_Fwrite( dataBuffer_sr, bufferSize, 1, file );			// dump the whole buffer in one hit

	TAP_Hdd_Fclose( file );
	//TAP_Hdd_ChangeDir("..");							// return to original directory
}

void WriteStrToBuf( char *str, TYPE_File *searchFile )					// add str to current end of buffer
{																		// and move end out
	int	count = 0, i = 0;

	count = strlen(str);
	
	for ( i=0; i<count; i++)
	{
		dataBuffer_sr[dataBufferPtr_sr+i] = str[i];				// copy accross 1 char at a time
	}
	dataBufferPtr_sr += i;
}


byte schFileRetreiveMoveData(void)
{

	TYPE_File	*moveFile = NULL;
	struct	schMoveTag	schTempMoveData;
	bool	schValidMove = FALSE;
	bool	schValidHeader = FALSE;
	int	bufferIndex = 0;
	int	elementIndex = 0, moveIndex = 0;
	dword	fileLength = 0;
	int	min = 0, hour = 0;
//	int	iTemp = 0;
	dword	dwTemp = 0;
	char	*buffer = NULL;
	word	schLcn = 0;
	byte	schFileMoves = 0;
	char	versionStr[128];
	char	buffer1[256];
	char	tempBuffer[10];
	int	maxBufferSize = 0;
	int	schNewLineCount = 0;

	maxBufferSize = (((SCH_MAX_MOVES * ((sizeof( struct schMoveTag )) + 20)) / 512) + 1) * 512;

	GotoTapDir();
	TAP_Hdd_ChangeDir( PROJECT_DIRECTORY );

	if ( ! TAP_Hdd_Exist( MOVE_FILENAME ) )
	{
//TAP_Print("Failed - Doesn't exist\r\n");		
	
		return 0;
	}
	moveFile = TAP_Hdd_Fopen( MOVE_FILENAME );

	if ( moveFile == NULL )
	{
//TAP_Print("Failed - Null\r\n");		
	
		return 0;
	}

	buffer = (char *)TAP_MemAlloc( maxBufferSize );

	fileLength = TAP_Hdd_Flen( moveFile );
	if ( fileLength > maxBufferSize ) fileLength = maxBufferSize;

	memset( buffer, 0, fileLength );

	TAP_Hdd_Fread( buffer, fileLength, 1, moveFile );

	TAP_Hdd_Fclose( moveFile );

	schMainTotalValidMoves = 0;

	bufferIndex = 0;

	schValidHeader = TRUE;

	schFileParseString(&versionStr[0], &schValidHeader, &schNewLineCount, buffer, &bufferIndex, 128, FALSE);
	schFileMoves = schFileParseHex(&schValidHeader, &schNewLineCount, &buffer[0], &bufferIndex, 2, SCH_MAX_MOVES);
	if(schValidHeader == FALSE)
	{
		schFileMoves = 0;
	}

	if(schFileMoves > 0)
	{
		schValidMove = TRUE;

		for(moveIndex = 0; ((moveIndex < schFileMoves) && (schValidMove == TRUE)); moveIndex++)
	     	{
			schNewLineCount = 0;

			schTempMoveData.moveEnabled = TRUE;
			schTempMoveData.moveFailedCount = 0;

			// ----------------- Move Number -----------------

			dwTemp = schFileParseHex(&schValidMove, &schNewLineCount, &buffer[0], &bufferIndex, 2, SCH_MAX_MOVES);

			// ----------------- Move File Name -----------------

			schFileParseString(schTempMoveData.moveFileName, &schValidMove, &schNewLineCount, buffer, &bufferIndex, 128, FALSE);

			// ----------------- Move Folder -----------------

			schFileParseString(schTempMoveData.moveFolder, &schValidMove, &schNewLineCount, buffer, &bufferIndex, 128, FALSE);

			// ----------------- Move Start Time -----------------

			schTempMoveData.moveStartTime = schFileParseHex(&schValidMove, &schNewLineCount, buffer, &bufferIndex, 8, 0xFFFFFFFF);

			// ----------------- Move End Time -----------------

			schTempMoveData.moveEndTime = schFileParseHex(&schValidMove, &schNewLineCount, buffer, &bufferIndex, 8, 0xFFFFFFFF);

			// ----------------------------------------------

			if(schNewLineCount != 1)
			{
				schValidMove = FALSE;
			}

			if(schValidMove == TRUE)
			{
				schMoveData[schMainTotalValidMoves] = schTempMoveData;

				schMainTotalValidMoves++;
			}
		}
	}

	TAP_MemFree( buffer );

//sprintf(buffer1,"Total Valid Moves: %d\r\n", schMainTotalValidMoves );
//TAP_Print(buffer1);		

	return schMainTotalValidMoves;
}


void schWriteMoveList( void )
{
	TYPE_File *moveFile = NULL;
	int	i = 0;
	dword	bufferSize = 0;
	char	str[256];
	char	buffer1[256];
	byte	schNewTotalMoves = 0;
	struct	schMoveTag	schTempMoveData;

	bufferSize = (((schMainTotalValidMoves * ((sizeof( struct schMoveTag )) + 20)) / 512) + 1) * 512;

	dataBuffer_sr = TAP_MemAlloc( bufferSize );	// Buffer the write data to memory before writing all in one hit
	memset( dataBuffer_sr, '\0', bufferSize );	// set the whole buffer to the string termination character (null)
	dataBufferPtr_sr = 0;

	WriteStrToBuf( MOVE_INFO, moveFile );

	schNewTotalMoves = 0;
	for ( i = 0; i < schMainTotalValidMoves; i++)
	{
		if (schMoveData[i].moveEnabled == TRUE)
		{
			schNewTotalMoves++;
		}
	}

//sprintf(buffer1,"schNewTotalMoves: %x\r\n", schNewTotalMoves );
//TAP_Print(buffer1);		

	sprintf(str, "%02x\r\n", schNewTotalMoves);		// Total number of moves
	WriteStrToBuf( str, moveFile );

	schNewTotalMoves = 0;
	for ( i = 0; i < schMainTotalValidMoves; i++)		// build an ordered list of moves, eariest=0
	{
		if (schMoveData[i].moveEnabled == TRUE)
		{
			schPrintMoveLine( i, schNewTotalMoves, moveFile );

			schTempMoveData = schMoveData[i];
			schMoveData[schNewTotalMoves] = schTempMoveData;

			schNewTotalMoves++;
		}
	}

	schWriteFile( bufferSize, moveFile, MOVE_FILENAME );	// write all the data in one pass

	TAP_MemFree( dataBuffer_sr );				// must return the memory back to the heap

	schMainTotalValidMoves = schNewTotalMoves;
}


void schPrintMoveLine( int moveIndex, int totalMoves, TYPE_File *moveFile )
{
	char str[1024];
	int 	min = 0, hour = 0;

	sprintf(str,"%02x\t", (totalMoves+1) );				// Move Number
	WriteStrToBuf( str, moveFile );
//TAP_Print("Move Number\r\n");

	sprintf(str, "%s\t", schMoveData[moveIndex].moveFileName);		// File Name
	WriteStrToBuf( str, moveFile );

	sprintf(str, "%s\t", schMoveData[moveIndex].moveFolder);		// Folder
	WriteStrToBuf( str, moveFile );

	sprintf(str, "%08x\t", schMoveData[moveIndex].moveStartTime);		// Start Time
	WriteStrToBuf( str, moveFile );

	sprintf(str, "%08x", schMoveData[moveIndex].moveEndTime);		// End Time
	WriteStrToBuf( str, moveFile );

	WriteStrToBuf( "\r\n", moveFile );
}


byte schFileRetreiveRemoteData(void)
{
	TYPE_File	*remoteFile = NULL;
	struct	schDataTag schTempUserData;
	bool	schValidSearch = FALSE;
	bool	schValidHeader = FALSE;
	int	bufferIndex = 0;
	dword	fileLength = 0;
	int	iTemp = 0;
	char	*buffer = NULL;
	dword	schLcn = 0;
	byte	schUserRemoteSearches = 0;
	int	maxBufferSize = 0;
	char	versionStr[128];
	dword	dwTemp = 0;
	char	buffer1[256];
	int	schNewLineCount = 0;
	
	maxBufferSize = (((SCH_MAX_SEARCHES * ((sizeof( struct schDataTag )) + 20)) / 512) + 1) * 512;

	GotoProgramFiles();

	if ( TAP_Hdd_Exist( REMOTE_FILENAME ) == FALSE )
	{
		return 0;
	}

	remoteFile = TAP_Hdd_Fopen( REMOTE_FILENAME );

	if ( remoteFile == NULL ) return 0;

	buffer = (char *)TAP_MemAlloc( maxBufferSize );

	fileLength = TAP_Hdd_Flen( remoteFile );
	if ( fileLength > maxBufferSize ) fileLength = maxBufferSize;

	memset( buffer, 0, fileLength );

	TAP_Hdd_Fread( buffer, fileLength, 1, remoteFile );

	TAP_Hdd_Fclose( remoteFile );

	schMainTotalValidRemoteSearches = 0;

	bufferIndex = 0;

	schValidHeader = TRUE;

	schFileParseString(&versionStr[0], &schValidHeader, &schNewLineCount, buffer, &bufferIndex, 128, FALSE);
	
	schUserRemoteSearches = schFileParseHex(&schValidHeader, &schNewLineCount, &buffer[0], &bufferIndex, 2, SCH_MAX_SEARCHES);
	if(schValidHeader == FALSE)
	{
		schUserRemoteSearches = 0;
	}


//sprintf(buffer1,"Unchecked Remotes Searches: %x\r\n", schUserRemoteSearches );
//TAP_Print(buffer1);		

	if(schUserRemoteSearches > 0)
	{
		schValidSearch = TRUE;

		for(searchIndex = 0; ((searchIndex < schUserRemoteSearches) && (schValidSearch == TRUE)); searchIndex++)
		{
			schNewLineCount = 0;

			// ----------------- Search Number -----------------
			dwTemp = schFileParseHex(&schValidSearch, &schNewLineCount, &buffer[0], &bufferIndex, 2, SCH_MAX_SEARCHES);

			// ----------------- Search Status -----------------
			schTempUserData.searchStatus = schFileParseHex(&schValidSearch, &schNewLineCount, buffer, &bufferIndex, 2, 0xFF);

			// ----------------- Search Term -----------------
			schFileParseString(&schTempUserData.searchTerm[0], &schValidSearch, &schNewLineCount, buffer, &bufferIndex, 128, FALSE);

			// ----------------- Search Start Time -----------------
			schTempUserData.searchStartTime = schFileParseTime(&schValidSearch, &schNewLineCount, buffer, &bufferIndex);

			// ----------------- Search End Time -----------------
			schTempUserData.searchEndTime = schFileParseTime(&schValidSearch, &schNewLineCount, buffer, &bufferIndex);

			// ----------------- Search Start Channel -----------------
			schLcn = schFileParseHex(&schValidSearch, &schNewLineCount, buffer, &bufferIndex, 4, 0xFFFF);

			schTempUserData.searchTvRadio = SCH_TV;
			if((schLcn & 0x8000) != 0)
			{
				schTempUserData.searchTvRadio = SCH_RADIO;

				schLcn &= 0x7FFF;
			}

			if
			(
				(schLcn > 999)
				||
				(schConvertLcnToSvcNum(schLcn, &schTempUserData.searchStartSvcNum, schTempUserData.searchTvRadio) == FALSE)
			)
			{
				schValidSearch = FALSE;
			}

			// ----------------- Search End Channel -----------------
			schLcn = schFileParseHex(&schValidSearch, &schNewLineCount, buffer, &bufferIndex, 4, 0xFFFF);

			if
			(
				(schLcn > 999)
				||
				(schConvertLcnToSvcNum(schLcn, &schTempUserData.searchEndSvcNum, schTempUserData.searchTvRadio) == FALSE)
			)
			{
				schValidSearch = FALSE;
			}

			if(schTempUserData.searchEndSvcNum < schTempUserData.searchStartSvcNum)
			{
				schValidSearch = FALSE;
			}

			// ----------------- Search Day -----------------
			schTempUserData.searchDay = schFileParseHex(&schValidSearch, &schNewLineCount, &buffer[0], &bufferIndex, 2, 0x7F);

			// ----------------- Search Start Padding -----------------
			schTempUserData.searchStartPadding = schFileParseTime(&schValidSearch, &schNewLineCount, buffer, &bufferIndex);


			// ----------------- Search End Padding -----------------
			schTempUserData.searchEndPadding = schFileParseTime(&schValidSearch, &schNewLineCount, buffer, &bufferIndex);

			// ----------------- Search Attach -----------------
			schTempUserData.searchAttach = schFileParseHex(&schValidSearch, &schNewLineCount, buffer, &bufferIndex, 4, 0xFFFF);

			// ----------------- Search Options -----------------
			schTempUserData.searchOptions = schFileParseHex(&schValidSearch, &schNewLineCount, buffer, &bufferIndex, 2, 0xFF);

			// ----------------- Search Folder -----------------
			schFileParseString(&schTempUserData.searchFolder[0], &schValidSearch, &schNewLineCount, buffer, &bufferIndex, 128, TRUE);

			// ----------------------------------------------

			if(schNewLineCount != 1)
			{
				schValidSearch = FALSE;
			}

			if(schValidSearch == TRUE)
			{
				schRemoteData[schMainTotalValidRemoteSearches] = schTempUserData;

				schMainTotalValidRemoteSearches++;
			}
			else
			{

			}
		}
	}

	TAP_MemFree( buffer );

	return schMainTotalValidRemoteSearches;
}







void schFileParseString(char* strResult, bool* valid, int* newLineCount, char* buffer, int* bufferIndex, int maxLength, bool zeroAllowed)
{
	dword	bufferLength = 0;
	int	elementIndex = 0;
	char	strTemp[256];
	
	bufferLength = strlen(buffer);

	elementIndex = 0;

	memset(strTemp, 0, 256);

	while
	(
		(buffer[*bufferIndex] != DELIMIT_TAB)
		&&
		(buffer[*bufferIndex] != DELIMIT_CR)
		&&
		(buffer[*bufferIndex] != DELIMIT_NL)
		&&
		(buffer[*bufferIndex] != 0)
		&&
		(elementIndex < maxLength)
		&&
		(*bufferIndex <= bufferLength)
	)
	{
		strTemp[elementIndex++] = buffer[(*bufferIndex)++];
	}

	if
	(
		(
			(zeroAllowed == FALSE)
			&&
			(strlen(strTemp) == 0)
		)
		||
		(strlen(strTemp) > maxLength)
	)
	{
		*valid = FALSE;

		memset(strResult, 0, maxLength);
	}
	else
	{
		strcpy(strResult,strTemp);
	}

	if(buffer[*bufferIndex] == DELIMIT_TAB)
	{
		(*bufferIndex)++;
	}
	else
	{
		while
		(
			(
				(buffer[*bufferIndex] == DELIMIT_CR)
				||
				(buffer[*bufferIndex] == DELIMIT_NL)
			)
			&&
			(buffer[*bufferIndex] != 0)
			&&
			(*bufferIndex <= bufferLength)
		)
		{
			(*bufferIndex)++;
		}

		(*newLineCount)++;
	}
}

dword schFileParseHex(bool* valid, int* newLineCount, char* buffer, int* bufferIndex, int size, dword maxValue)
{
	dword	bufferLength = 0;
	int	elementIndex = 0;
	char	tempBuffer[10];
	dword	dwResult = 0;
	dword	dwTemp = 0;

	bufferLength = strlen(buffer);

	elementIndex = 0;
	memset(tempBuffer, 0, 10);

	while
	(
		(buffer[*bufferIndex] != DELIMIT_TAB)
		&&
		(buffer[*bufferIndex] != DELIMIT_CR)
		&&
		(buffer[*bufferIndex] != DELIMIT_NL)
		&&
		(buffer[*bufferIndex] != 0)
		&&
		(elementIndex < size)
		&&
		(*bufferIndex <= bufferLength)
	)
	{
		tempBuffer[elementIndex++] = buffer[(*bufferIndex)++];
	}

	dwTemp = axtodw(tempBuffer);

	if
	(
		(dwTemp > maxValue)
		||
		(strlen(tempBuffer) != size)
	)
	{
		*valid = FALSE;	
	}
	else
	{
		dwResult = dwTemp;
	}

	if(buffer[*bufferIndex] == DELIMIT_TAB)
	{
		(*bufferIndex)++;
	}
	else
	{
		while
		(
			(
				(buffer[*bufferIndex] == DELIMIT_CR)
				||
				(buffer[*bufferIndex] == DELIMIT_NL)
			)
			&&
			(buffer[*bufferIndex] != 0)
			&&
			(*bufferIndex <= bufferLength)
		)
		{
			(*bufferIndex)++;
		}

		(*newLineCount)++;
	}

	return dwResult;
}


dword schFileParseTime(bool* valid, int* newLineCount, char* buffer, int* bufferIndex)
{
	dword	bufferLength = 0;
	int	elementIndex = 0;
	int	hour = 0, min = 0;
	char	tempBuffer[10];
	dword	dwResult = 0;
	
	bufferLength = strlen(buffer);

	elementIndex = 0;
	memset(tempBuffer, 0, 10);

	while
	(
		(buffer[*bufferIndex] != DELIMIT_TAB)
		&&
		(buffer[*bufferIndex] != DELIMIT_CR)
		&&
		(buffer[*bufferIndex] != DELIMIT_NL)
		&&
		(buffer[*bufferIndex] != 0)
		&&
		(elementIndex < 8)
		&&
		(*bufferIndex <= bufferLength)
	)
	{
		tempBuffer[elementIndex++] = buffer[(*bufferIndex)++];
	}

	hour = ( ((tempBuffer[0] - '0') * 10) + (tempBuffer[1] - '0') );
	min = ( ((tempBuffer[3] - '0') * 10) + (tempBuffer[4] - '0') );

	if
	(
		(hour > 23)
		||
		(min > 59)
		||
		(strlen(tempBuffer) != 5)
	)
	{
		*valid = FALSE;
	}
	else
	{
	 	dwResult = (hour << 8) + (min);
	}	 	

	if(buffer[*bufferIndex] == DELIMIT_TAB)
	{
		(*bufferIndex)++;
	}
	else
	{
		while
		(
			(
				(buffer[*bufferIndex] == DELIMIT_CR)
				||
				(buffer[*bufferIndex] == DELIMIT_NL)
			)
			&&
			(buffer[*bufferIndex] != 0)
			&&
			(*bufferIndex <= bufferLength)
		)
		{
			(*bufferIndex)++;
		}

		(*newLineCount)++;
	}
	
	return dwResult;
}


