/************************************************************
Part of the ukEPG project
This module handles the 'Keep' feature. 

v0.0 sl8:	05-08-06	Inception date

**************************************************************/

#if DEBUG_KEEP == 0
	#define SCH_MAIN_DELAY_KEEP_ALARM	120			// Keep not allowed within 2 Minutes of TAP starting
#else
	#define SCH_MAIN_DELAY_KEEP_ALARM	1			// Keep not allowed within 1 second of TAP starting
#endif

void schKeepService(void);
bool schKeepCheckFolder(byte);
void schKeepCheckProgrammes(byte);
void schKeepDeleteFile(char*);

void schKeepService(void)
{
	int	totalFileCount = 0, i = 0;
	static	TYPE_File	tempFile;
	static	dword	schKeepCurrentTime = 0;
	static	byte	schKeepIndex = 0;
	char	buffer1[256];
	char	folderStr[256];

	switch(schKeepServiceSV)
	{
	/*--------------------------------------------------*/
	case SCH_KEEP_SERVICE_INITIALISE:

		if(schMainChangeDirAvailable == TRUE)
		{
			schKeepServiceSV = SCH_KEEP_SERVICE_WAIT_TO_CHECK;
		}
		else
		{
			schKeepServiceSV = SCH_KEEP_SERVICE_NOT_AVAILABLE;
		}

		break;
	/*--------------------------------------------------*/
	case SCH_KEEP_SERVICE_WAIT_TO_CHECK:

		if
		(
#if DEBUG_KEEP == 0
			((schTimeMin % 10) == 5)
			&&
#endif
			(schTimeSec == 20)
			&&
			(schMainTotalValidSearches > 0)
			&&
			(schStartUpCounter > SCH_MAIN_DELAY_KEEP_ALARM)
			&&
			(schMoveServiceSV == SCH_MOVE_SERVICE_WAIT_TO_CHECK)
		)
		{
			schKeepIndex = 0;

			schKeepCurrentTime = (schTimeMjd << 16) + (schTimeHour << 8) + schTimeMin;

			schKeepServiceSV = SCH_KEEP_SERVICE_BEGIN;
		}

		break;
	/*--------------------------------------------------*/
	case SCH_KEEP_SERVICE_BEGIN:

		if
		(
			(schUserData[schKeepIndex].searchStatus == SCH_USER_DATA_STATUS_RECORD)
			&&
			(strlen(schUserData[schKeepIndex].searchFolder) > 0)
			&&
			(schUserData[schKeepIndex].searchKeepMode != SCH_DISPLAY_KEEP_ALL)
			&&
			(schUserData[schKeepIndex].searchKeepValue > 0)
		)
		{
			schKeepServiceSV = SCH_KEEP_SERVICE_CHECK_FOLDER;
		}
		else
		{
			schKeepServiceSV = SCH_KEEP_SERVICE_NEXT_KEEP;
		}

		break;	
	/*--------------------------------------------------*/
	case SCH_KEEP_SERVICE_CHECK_FOLDER:

		memset(folderStr,0,sizeof(folderStr));
		sprintf( folderStr, "DataFiles/%s", schUserData[schKeepIndex].searchFolder );

		if(GotoPath(folderStr) == TRUE)
		{
			schKeepCheckProgrammes(schKeepIndex);
		}

		schKeepServiceSV = SCH_KEEP_SERVICE_NEXT_KEEP;

		break;
	/*--------------------------------------------------*/
	case SCH_KEEP_SERVICE_NEXT_KEEP:

		schKeepIndex++;

		if(schKeepIndex >= schMainTotalValidSearches)
		{
			schKeepServiceSV = SCH_KEEP_SERVICE_COMPLETE;
		}
		else
		{
			schKeepServiceSV = SCH_KEEP_SERVICE_BEGIN;
		}

		break;
	/*--------------------------------------------------*/
	case SCH_KEEP_SERVICE_COMPLETE:

		if(schTimeSec != 20)
		{
			schKeepServiceSV = SCH_KEEP_SERVICE_WAIT_TO_CHECK;
		}

		break;
	/*--------------------------------------------------*/
	case SCH_KEEP_SERVICE_NOT_AVAILABLE:
	default:

		break;
	/*--------------------------------------------------*/
	}
}


void schKeepCheckProgrammes(byte schKeepIndex)
{
	int	numberOfProgrammes = 0;
	int	numberToBeDeleted = 0;
	TYPE_File	tempFile;
	int	totalFileCount = 0, i = 0;
	dword	currentTimeInMins = 0, fileStartTimeInMins = 0;
	dword	fileStart = 0;
	ulong64	fileSize = 0;
	char	fileName[256];

	switch(schUserData[schKeepIndex].searchKeepMode)
	{
	/* ---------------------------------------------------------------------------- */
	case SCH_DISPLAY_KEEP_LAST:
	case SCH_DISPLAY_KEEP_SMALLEST:
	case SCH_DISPLAY_KEEP_LARGEST:

		totalFileCount = TAP_Hdd_FindFirst(&tempFile); 

		numberOfProgrammes = 0;
		for ( i=1; (i <= totalFileCount) ; i++ )
		{
#ifdef WIN32
	WIN32_ReplaceTempFileData(&tempFile);
#endif
			if (tempFile.attr == ATTR_TS)
			{
				numberOfProgrammes++;
			}

			TAP_Hdd_FindNext(&tempFile);
		}

		if(schUserData[schKeepIndex].searchKeepValue < numberOfProgrammes)
		{
			numberToBeDeleted = numberOfProgrammes - schUserData[schKeepIndex].searchKeepValue;

			switch(schUserData[schKeepIndex].searchKeepMode)
			{
			/* ---------------------------------------------------------------------------- */
			case SCH_DISPLAY_KEEP_LAST:

				while(numberToBeDeleted > 0)
				{
					totalFileCount = TAP_Hdd_FindFirst(&tempFile); 

					fileStart = 0xFFFFFFFF;

					for ( i=1; (i <= totalFileCount) ; i++ )
					{
#ifdef WIN32
	WIN32_ReplaceTempFileData(&tempFile);
#endif
						if (tempFile.attr == ATTR_TS)
						{
							if(((tempFile.mjd << 16) + (tempFile.hour << 8) + tempFile.min) < fileStart)
							{
								fileStart = (tempFile.mjd << 16) + (tempFile.hour << 8) + tempFile.min;

								memset(fileName,0,sizeof(fileName));
								strcpy(fileName, tempFile.name);
							}
						}

						TAP_Hdd_FindNext(&tempFile);
					}

					schKeepDeleteFile( fileName );

					numberToBeDeleted--;
				}

				break;
			/* ---------------------------------------------------------------------------- */
			case SCH_DISPLAY_KEEP_SMALLEST:

				while(numberToBeDeleted > 0)
				{
					totalFileCount = TAP_Hdd_FindFirst(&tempFile); 

					fileSize = 0;

					for ( i=1; (i <= totalFileCount) ; i++ )
					{
#ifdef WIN32
	WIN32_ReplaceTempFileData(&tempFile);
#endif
						if (tempFile.attr == ATTR_TS)
						{
							if(tempFile.size > fileSize)
							{
								fileSize = tempFile.size;

								strcpy(fileName, tempFile.name);
							}
						}

						TAP_Hdd_FindNext(&tempFile);
					}

					schKeepDeleteFile( fileName );

					numberToBeDeleted--;
				}

				break;
			/* ---------------------------------------------------------------------------- */
			case SCH_DISPLAY_KEEP_LARGEST:

				while(numberToBeDeleted > 0)
				{
					totalFileCount = TAP_Hdd_FindFirst(&tempFile); 

					fileSize = 0;
					fileSize--;

					for ( i=1; (i <= totalFileCount) ; i++ )
					{
						if (tempFile.attr == ATTR_TS)
						{
#ifdef WIN32
	WIN32_ReplaceTempFileData(&tempFile);
#endif
							if(tempFile.size < fileSize)
							{
								fileSize = tempFile.size;

								strcpy(fileName, tempFile.name);
							}
						}

						TAP_Hdd_FindNext(&tempFile);
					}

					schKeepDeleteFile( fileName );

					numberToBeDeleted--;
				}

				break;
			/* ---------------------------------------------------------------------------- */
			default:

				break;
			/* ---------------------------------------------------------------------------- */
			}
		}

		break;
	/* ---------------------------------------------------------------------------- */
	case SCH_DISPLAY_KEEP_DAYS:

		totalFileCount = TAP_Hdd_FindFirst(&tempFile); 

		currentTimeInMins = (schTimeMjd * 24 * 60) + (schTimeHour * 60) + schTimeMin;

		for ( i=1; (i <= totalFileCount) ; i++ )
		{
#ifdef WIN32
	WIN32_ReplaceTempFileData(&tempFile);
#endif
			if (tempFile.attr == ATTR_TS)
			{
				fileStartTimeInMins = (tempFile.mjd * 24 * 60) + (tempFile.hour * 60) + tempFile.min;
				
				if((currentTimeInMins - (schUserData[schKeepIndex].searchKeepValue * 24 * 60)) > fileStartTimeInMins)
				{
					schKeepDeleteFile( tempFile.name );
				}
			}

			TAP_Hdd_FindNext(&tempFile);
		}

		break;
	/* ---------------------------------------------------------------------------- */
	default:

		break;
	}
}


void schKeepDeleteFile(char *fileName)
{
	TYPE_PlayInfo	CurrentPlaybackInfo;
	char	playInfoOverRun[512];
	char	CurrentPlaybackFileName[TS_FILE_NAME_SIZE];

	TAP_Hdd_GetPlayInfo (&CurrentPlaybackInfo);

	memset(CurrentPlaybackFileName,0,TS_FILE_NAME_SIZE);
	if(CurrentPlaybackInfo.playMode == PLAYMODE_Playing)
	{
		strcpy(CurrentPlaybackFileName,CurrentPlaybackInfo.file->name);
	}

	if(strcmp(CurrentPlaybackFileName, fileName) != 0)
	{
		if (TAP_Hdd_Exist( fileName ) == TRUE)
		{
			TAP_Hdd_Delete( fileName );
		}
	}
}

