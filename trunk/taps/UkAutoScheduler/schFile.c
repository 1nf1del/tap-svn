/************************************************************
Part of the ukEPG project
This module handles all file activity

v0.0 sl8:	20-11-05	Inception date
v0.1 sl8:	20-01-06	Uses Kidhazy's method of changing directories. Modified for TAP_SDK
					All variables initialised.
v0.2 sl8:	06-02-06	Use Project directory define
v0.3 sl8:	15-02-06	Modified for new UkAutoSearch.txt file

**************************************************************/

void schPrintSearchLine( int, TYPE_File* );
void schWriteFile( dword, TYPE_File*, char* );
void WriteStrToBuf( char*, TYPE_File* );

bool schConvertLcnToSvcNum(word, word*, bool);
static char *dataBuffer_sr = NULL;
static int dataBufferPtr_sr = 0;

#define SEARCH_FILENAME		"UkAutoSearch.txt"
#define SEARCH_INFO		"UK Auto Scheduler Search List v0.2\r\n"


byte schInitRetreiveData(void)
{
	TYPE_File	*searchFile = NULL;
	struct	schDataTag schTempUserData;
	bool	schValidSearch = 0;
	int	bufferIndex = 0;
	int	elementIndex = 0, searchIndex = 0;
	dword	fileLength = 0;
	int	min = 0, hour = 0;
	int	iTemp = 0;
	char	*buffer = NULL;
	word	schLcn = 0;
	byte	schUserDataSearches = 0;
	char	buffer1[256];
	char	tempBuffer[10];
	int	maxBufferSize = 0;
	byte	version = 0;
	char	versionStr[128];

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
//	TAP_Hdd_ChangeDir("..");

	schMainTotalValidSearches = 0;

	bufferIndex = 0;

	if(version == 2)
	{
		elementIndex = 0;
		memset(versionStr,0,128);
		while
		(
			(buffer[bufferIndex] != '\r')
			&&
			(elementIndex < 128)
			&&
			(bufferIndex <= fileLength)
		)
		{
			versionStr[elementIndex++] = buffer[bufferIndex++];
		}

		bufferIndex += 2;
	}
	elementIndex = 0;
	memset(tempBuffer,0,10);
	while
	(	
		(buffer[bufferIndex] != '\r')
		&&
		(bufferIndex <= fileLength)
		&&
		(elementIndex < 8)
	)
	{
		tempBuffer[elementIndex++] = buffer[bufferIndex++];
	}
	iTemp = axtoi(tempBuffer);
	if
	(
		(iTemp > SCH_MAX_SEARCHES)
		||
		(strlen(tempBuffer) != 2)
	)
	{
		schUserDataSearches = 0;
	}
	else
	{
		schUserDataSearches = iTemp;
	}

	bufferIndex += 2;

	if(schUserDataSearches > 0)
	{
		for(searchIndex = 0; searchIndex < schUserDataSearches; searchIndex++)
		{
			schValidSearch = TRUE;

			// ----------------- Search Number -----------------

			elementIndex = 0;
			memset(tempBuffer,0,10);
			while
			(	
				(buffer[bufferIndex] != '\t')
				&&
				(bufferIndex <= fileLength)
				&&
				(elementIndex < 8)
			)
			{
				tempBuffer[elementIndex++] = buffer[bufferIndex++];
			}
			iTemp = axtoi(tempBuffer);
			if
			(
				(iTemp > SCH_MAX_SEARCHES)
				||
				(strlen(tempBuffer) != 2)
			)
			{
				schValidSearch = FALSE;
			}
			else
			{
				// Not used at the moment
			}

			bufferIndex++;

			// ----------------- Search Status -----------------

			elementIndex = 0;
			memset(tempBuffer,0,10);
			while
			(	
				(buffer[bufferIndex] != '\t')
				&&
				(bufferIndex <= fileLength)
				&&
				(elementIndex < 8)
			)
			{
				tempBuffer[elementIndex++] = buffer[bufferIndex++];
			}
			iTemp = axtoi(tempBuffer);
			if
			(
				(iTemp > 255)
				||
				(strlen(tempBuffer) != 2)
			)
			{
				schValidSearch = FALSE;
			}
			else
			{
				schTempUserData.searchStatus = iTemp;
			}

			bufferIndex++;

			// ----------------- Search Term -----------------

			elementIndex = 0;
			memset(schTempUserData.searchTerm,0,132);
			while
			(
				(buffer[bufferIndex] != '\t')
				&&
				(elementIndex < 128)
				&&
				(bufferIndex <= fileLength)
			)
			{
				schTempUserData.searchTerm[elementIndex++] = buffer[bufferIndex++];
			}
			if
			(
				(strlen(schTempUserData.searchTerm) == 0)
				||
				(strlen(schTempUserData.searchTerm) > 128)
			)
			{
				schValidSearch = FALSE;
			}

			bufferIndex++;

			// ----------------- Search Start Time -----------------

			elementIndex = 0;
			memset(tempBuffer,0,10);
			while
			(	
				(buffer[bufferIndex] != '\t')
				&&
				(bufferIndex <= fileLength)
				&&
				(elementIndex < 8)
			)
			{
				tempBuffer[elementIndex++] = buffer[bufferIndex++];
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
				schValidSearch = FALSE;
			}
			else
			{
				schTempUserData.searchStartTime = (hour << 8) + (min);
			}

			bufferIndex++;

			// ----------------- Search End Time -----------------

			elementIndex = 0;
			memset(tempBuffer,0,10);
			while
			(
				(buffer[bufferIndex] != '\t')
				&&
				(bufferIndex <= fileLength)
				&&
				(elementIndex < 8)
			)
			{
				tempBuffer[elementIndex++] = buffer[bufferIndex++];
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
				schValidSearch = FALSE;
			}
			else
			{
				schTempUserData.searchEndTime = (hour << 8) + (min);
			}

			bufferIndex++;

			// ----------------- Search Start Channel -----------------

			elementIndex = 0;
			memset(tempBuffer,0,10);
			while
			(
				(buffer[bufferIndex] != '\t')
				&&
				(bufferIndex <= fileLength)
				&&
				(elementIndex < 8)
			)
			{
				tempBuffer[elementIndex++] = buffer[bufferIndex++];
			}
			schLcn = axtoi(tempBuffer);

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
				||
				(strlen(tempBuffer) != 4)
			)
			{
				schValidSearch = FALSE;
			}

			bufferIndex++;

			// ----------------- Search End Channel -----------------

			elementIndex = 0;
			memset(tempBuffer,0,10);
			while
			(
				(buffer[bufferIndex] != '\t')
				&&
				(bufferIndex <= fileLength)
				&&
				(elementIndex < 8)
			)
			{
				tempBuffer[elementIndex++] = buffer[bufferIndex++];
			}
			schLcn = axtoi(tempBuffer);
			if
			(
				(schLcn > 999)
				||
				(schConvertLcnToSvcNum(schLcn, &schTempUserData.searchEndSvcNum, schTempUserData.searchTvRadio) == FALSE)
				||
				(strlen(tempBuffer) != 4)
			)
			{
				schValidSearch = FALSE;
			}

			if(schTempUserData.searchEndSvcNum < schTempUserData.searchStartSvcNum)
			{
				schValidSearch = FALSE;
			}
			
			bufferIndex++;

			// ----------------- Search Day -----------------

			elementIndex = 0;
			memset(tempBuffer,0,10);
			while
			(
				(buffer[bufferIndex] != '\t')
				&&
				(bufferIndex <= fileLength)
				&&
				(elementIndex < 8)
			)
			{
				tempBuffer[elementIndex++] = buffer[bufferIndex++];
			}
			iTemp = axtoi(tempBuffer);
			if
			(
				(iTemp > 127)
				||
				(strlen(tempBuffer) != 2)
			)
			{
				schValidSearch = FALSE;
			}
			else
			{
				schTempUserData.searchDay = iTemp;
			}

			bufferIndex++;

			// ----------------- Search Start Padding -----------------

			elementIndex = 0;
			memset(tempBuffer,0,10);
			while
			(	
				(buffer[bufferIndex] != '\t')
				&&
				(bufferIndex <= fileLength)
				&&
				(elementIndex < 8)
			)
			{
				tempBuffer[elementIndex++] = buffer[bufferIndex++];
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
				schValidSearch = FALSE;
			}
			else
			{
				schTempUserData.searchStartPadding = (hour << 8) + (min);
			}

			bufferIndex++;

			// ----------------- Search End Padding -----------------

			elementIndex = 0;
			memset(tempBuffer,0,10);
			while
			(	
				(buffer[bufferIndex] != '\t')
				&&
				(bufferIndex <= fileLength)
				&&
				(elementIndex < 8)
			)
			{
				tempBuffer[elementIndex++] = buffer[bufferIndex++];
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
				schValidSearch = FALSE;
			}
			else
			{
				schTempUserData.searchEndPadding = (hour << 8) + (min);
			}

			bufferIndex++;

			// ----------------- Search Attach -----------------

			elementIndex = 0;
			memset(tempBuffer,0,10);
			while
			(
				(buffer[bufferIndex] != '\t')
				&&
				(bufferIndex <= fileLength)
				&&
				(elementIndex < 8)
			)
			{
				tempBuffer[elementIndex++] = buffer[bufferIndex++];
			}
			iTemp = axtoi(tempBuffer);
			if
			(
				(strlen(tempBuffer) != 4)
			)
			{
				schValidSearch = FALSE;
			}
			else
			{
				schTempUserData.searchAttach = iTemp;
			}

			bufferIndex++;

			// ----------------- Search Options -----------------

			elementIndex = 0;
			memset(tempBuffer,0,10);
			while
			(
				(
					(
						(buffer[bufferIndex] != '\r')	// old 'SearchList.txt' file format
						&&
						(version == 1)
					)
					||
					(
						(buffer[bufferIndex] != '\t')	// new 'UkAutoSearch.txt' format with move
						&&
						(version == 2)
					)
				)
				&&
				(bufferIndex <= fileLength)
				&&
				(elementIndex < 8)
			)
			{
				tempBuffer[elementIndex++] = buffer[bufferIndex++];
			}
			iTemp = axtoi(tempBuffer);
			if
			(
				(iTemp > 255)
				||
				(strlen(tempBuffer) != 2)
			)
			{
				schValidSearch = FALSE;
			}
			else
			{
				schTempUserData.searchOptions = iTemp;
			}

			if(version == 1)		// old 'SearchList.txt' file format
			{
				memset(schTempUserData.searchFolder,0,132);
				bufferIndex += 2;
			}
			else
			{				// new 'UkAutoSearch.txt' format with move
				bufferIndex++;

				// ------------- Search Folder --------------

				elementIndex = 0;
				memset(schTempUserData.searchFolder,0,132);
				while
				(
					(buffer[bufferIndex] != '\r')
					&&
					(elementIndex < 128)
					&&
					(bufferIndex <= fileLength)
				)
				{
					schTempUserData.searchFolder[elementIndex++] = buffer[bufferIndex++];
				}
				if
				(
					(strlen(schTempUserData.searchFolder) > 128)
				)
				{
					schValidSearch = FALSE;
				}

				bufferIndex += 2;
			}

			// ----------------------------------------------

			if(schValidSearch == TRUE)
			{
				schUserData[schMainTotalValidSearches] = schTempUserData;

				schMainTotalValidSearches++;
			}
		}
	}

	TAP_MemFree( buffer );

	if(version == 1)
	{
		schWriteSearchList();
	}

	return schMainTotalValidSearches;
}


int axtoi(char *hexString)
{
	int result = 0;
	int i = 0;
	int j = 0;
	int length = 0;
	char buffer1[256];

	length = strlen( hexString );

	if( length == 0)
	{
		result = 0x0;
	}
	else if (length > 4)
	{
		result = 0xFFFF;
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
