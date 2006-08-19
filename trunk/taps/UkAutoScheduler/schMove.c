/************************************************************
Part of the ukEPG project
This module moves a file to a specified directory

v0.0 sl8:	09-03-06	Inception date
v0.1 sl8:	11-04-06	Tidy up.
v0.2 sl8:	08-05-06	API move added.
v0.3 sl8:	05-08-06	Renames file if file name already exists in target folder.

**************************************************************/

#define SCH_MAIN_DELAY_MOVE_ALARM	120			// Move not allowed within 2 Minutes of TAP starting
#define SCH_REC_FILE_NAME_SIZE		95

void schMoveService(void);
void WIN32_ReplaceTempFileData(TYPE_File* tempFile);

void schMoveService(void)
{
	char	logBuffer[LOG_BUFFER_SIZE];
	char	fileStr[200];
	char	folderStr[200];
	static	char	schMoveActualFileName[128];
	static	char	schMoveNewFileName[128];
	int	fileNumber = 0;
	char	fileNumberStr[10];
	int	totalFileCount = 0, i = 0;
	TYPE_PlayInfo	CurrentPlaybackInfo;
	char	playInfoOverRun[512];
	TYPE_RecInfo	CurrentRecordInfo0;
	char	rec0InfoOverRun[512];
	TYPE_RecInfo	CurrentRecordInfo1;
	char	rec1InfoOverRun[512];
	char	CurrentPlaybackFileName[TS_FILE_NAME_SIZE];
	char	CurrentRecord0FileName[SCH_REC_FILE_NAME_SIZE];
	char	CurrentRecord1FileName[SCH_REC_FILE_NAME_SIZE];
	bool	schFileFound = FALSE;
	static	TYPE_File	tempFile;
	static	dword	schMoveCurrentTime = 0;
	static	byte	schMoveIndex = 0;
	static	bool	schMoveExpired = FALSE;
	static	bool	schMoveSuccessful = FALSE;
	static	bool	moveFailed = FALSE;
	int	result = 0;
	bool	schMoveResult = FALSE;

	char	buffer1[256];

	switch(schMoveServiceSV)
	{
	/*--------------------------------------------------*/
	case SCH_MOVE_SERVICE_INITIALISE:

		if(schFileRetreiveMoveData() > 0)
		{
		}
		else
		{
		}

		schMoveServiceSV = SCH_MOVE_SERVICE_WAIT_TO_CHECK;

		break;
	/*--------------------------------------------------*/
	case SCH_MOVE_SERVICE_WAIT_TO_CHECK:

		if
		(
			(schTimeSec == 40)
			&&
			(schMainTotalValidMoves > 0)
			&&
			(schStartUpCounter > SCH_MAIN_DELAY_MOVE_ALARM)
			&&
			(schKeepServiceSV == SCH_KEEP_SERVICE_WAIT_TO_CHECK)
		)
		{
			schMoveIndex = 0;

			schMoveCurrentTime = (schTimeMjd << 16) + (schTimeHour << 8) + schTimeMin;

			schMoveExpired = FALSE;

			moveFailed = FALSE;

			schMoveServiceSV = SCH_MOVE_SERVICE_CHECK_MOVE;
		}

		break;
	/*--------------------------------------------------*/
	case SCH_MOVE_SERVICE_CHECK_MOVE:

		if (schMoveCurrentTime >= schMoveData[schMoveIndex].moveEndTime)
		{
			schMoveSuccessful = FALSE;

			schMoveServiceSV = SCH_MOVE_SERVICE_CHECK_ARCHIVE;
		}
		else
		{
			schMoveServiceSV = SCH_MOVE_SERVICE_NEXT_MOVE;
		}

		break;
	/*--------------------------------------------------*/
	case SCH_MOVE_SERVICE_CHECK_ARCHIVE:

		memset(schMoveActualFileName, 0 , sizeof(schMoveActualFileName));

		if(GotoDataFiles() == TRUE)
		{
			totalFileCount = TAP_Hdd_FindFirst(&tempFile); 

			schFileFound = FALSE;
			for ( i=1; ((i <= totalFileCount) && (schFileFound == FALSE)) ; i++ )
			{
				if
				(
					(schMoveData[schMoveIndex].moveStartTime == ((tempFile.mjd << 16) + (tempFile.hour << 8) + tempFile.min))
					&&
					(tempFile.attr == ATTR_TS)
					&&
					(strlen(schMoveData[schMoveIndex].moveFileName) > 4)
					&&
					(strncmp(tempFile.name, schMoveData[schMoveIndex].moveFileName, (strlen(schMoveData[schMoveIndex].moveFileName) - 4)) == 0)
				)
				{
					strcpy(schMoveActualFileName, tempFile.name);

					schFileFound = TRUE;			
				}
				else
				{
					TAP_Hdd_FindNext(&tempFile);
				}
			}

			if(schFileFound == TRUE)
			{
				schMoveServiceSV = SCH_MOVE_SERVICE_CHECK_TARGET_FOLDER;
			}
			else
			{
				schMoveServiceSV = SCH_MOVE_SERVICE_MOVE_NOT_SUCCESSFUL;
			}
		}
		else
		{
			schMoveServiceSV = SCH_MOVE_SERVICE_MOVE_NOT_SUCCESSFUL;
		}

		break;
	/*--------------------------------------------------*/
	case SCH_MOVE_SERVICE_CHECK_TARGET_FOLDER:

		memset(schMoveNewFileName, 0 , sizeof(schMoveNewFileName));

		sprintf( folderStr, "DataFiles/%s", schMoveData[schMoveIndex].moveFolder );
		if(GotoPath(folderStr) == TRUE)
		{
			if (TAP_Hdd_Exist( schMoveActualFileName ) == FALSE)
			{
				schMoveServiceSV = SCH_MOVE_SERVICE_PERFORM_MOVE;
			}
			else
			{
				fileNumber = 2;

				sprintf(fileNumberStr, "-%d", fileNumber);

				if(strlen(schMoveActualFileName) > 4)
				{
					strncpy(schMoveNewFileName, schMoveActualFileName,(strlen(schMoveActualFileName) - 4));
					strcat(schMoveNewFileName, fileNumberStr);
					strcat(schMoveNewFileName, ".rec");
			
					while
					(
						(TAP_Hdd_Exist( schMoveNewFileName ) == TRUE)
						&&
						(fileNumber < 100)
					)
					{
						fileNumber++;

						memset(fileNumberStr, 0 , sizeof(fileNumberStr));
						sprintf(fileNumberStr, "-%d", fileNumber);
						
						memset(schMoveNewFileName, 0 , sizeof(schMoveNewFileName));
						strncpy(schMoveNewFileName, schMoveActualFileName,(strlen(schMoveActualFileName) - 4));
						strcat(schMoveNewFileName, fileNumberStr);
						strcat(schMoveNewFileName, ".rec");
					}
				}
				else
				{
					fileNumber = 100;
				}

				if(fileNumber < 100)
				{
					schMoveServiceSV = SCH_MOVE_SERVICE_PERFORM_MOVE;
				}
				else
				{
					schMoveServiceSV = SCH_MOVE_SERVICE_MOVE_NOT_SUCCESSFUL;
				}
			}
		}
		else
		{
			schMoveServiceSV = SCH_MOVE_SERVICE_MOVE_NOT_SUCCESSFUL;
		}

		break;
	/*--------------------------------------------------*/
	case SCH_MOVE_SERVICE_PERFORM_MOVE:

		schMoveSuccessful = FALSE;

		if(GotoDataFiles() == TRUE)
		{
			TAP_Hdd_GetPlayInfo (&CurrentPlaybackInfo);
			TAP_Hdd_GetRecInfo (0, &CurrentRecordInfo0);
			TAP_Hdd_GetRecInfo (1, &CurrentRecordInfo1);

			memset(CurrentPlaybackFileName,0,TS_FILE_NAME_SIZE);
			if(CurrentPlaybackInfo.playMode == PLAYMODE_Playing)
			{
				strcpy(CurrentPlaybackFileName,CurrentPlaybackInfo.file->name);
			}

			memset(CurrentRecord0FileName,0,SCH_REC_FILE_NAME_SIZE);
			if
			(
				(CurrentRecordInfo0.recType == RECTYPE_Normal)
				&&
				(schMoveData[schMoveIndex].moveStartTime == CurrentRecordInfo0.startTime)
			)
			{
				strcpy(CurrentRecord0FileName,CurrentRecordInfo0.fileName);
			}

			memset(CurrentRecord1FileName,0,SCH_REC_FILE_NAME_SIZE);
			if
			(
				(CurrentRecordInfo1.recType == RECTYPE_Normal)
				&&
				(schMoveData[schMoveIndex].moveStartTime == CurrentRecordInfo1.startTime)
			)
			{
				strcpy(CurrentRecord1FileName,CurrentRecordInfo1.fileName);
			}

			if
			(
				(strcmp(CurrentPlaybackFileName, schMoveActualFileName) != 0)
				&&
				(strcmp(CurrentRecord0FileName, schMoveActualFileName) != 0)
				&&
				(strcmp(CurrentRecord1FileName, schMoveActualFileName) != 0)
			)
			{
				if(strlen(schMoveNewFileName) > 0)
				{
					if(TAP_Hdd_Rename(schMoveActualFileName, schMoveNewFileName) == TRUE)
					{
						memset(schMoveActualFileName, 0 , sizeof(schMoveActualFileName));
						strcpy(schMoveActualFileName, schMoveNewFileName);
					}
				}

				sprintf( folderStr, "/DataFiles/%s", schMoveData[schMoveIndex].moveFolder );
#ifndef WIN32
				if(schMainApiMoveAvailable == TRUE)
				{
					schMoveResult = TAP_Hdd_ApiMove( "/DataFiles", folderStr, schMoveActualFileName );
				}
				else if(schMainDebugMoveAvailable == TRUE)
				{
					sprintf( fileStr, "/DataFiles/%s", schMoveActualFileName );

					schMoveResult = TAP_Hdd_DebugMove( fileStr, folderStr );
				}
				else
				{
				}

#else
				schMoveResult = TRUE;	// For the SDK
#endif
				if(schMoveResult == TRUE)
				{
					schMoveSuccessful = TRUE;

					schMoveData[schMoveIndex].moveEnabled = FALSE;

					schMoveExpired = TRUE;				// Delete this move when all moves checked
				}
			}
		}

		if(schMoveSuccessful == TRUE)
		{
			schMoveServiceSV = SCH_MOVE_SERVICE_NEXT_MOVE;
		}
		else
		{
			schMoveServiceSV = SCH_MOVE_SERVICE_MOVE_NOT_SUCCESSFUL;
		}

		break;
	/*--------------------------------------------------*/
	case SCH_MOVE_SERVICE_MOVE_NOT_SUCCESSFUL:

		schMoveData[schMoveIndex].moveFailedCount++;

		if(schMoveData[schMoveIndex].moveFailedCount >= 3)
		{
			schMoveData[schMoveIndex].moveEnabled = FALSE;

			schMoveExpired = TRUE;				// Delete this move when all moves checked

			moveFailed = TRUE;

//			memset(logBuffer,0,LOG_BUFFER_SIZE);
//			sprintf( logBuffer, "Failed (Move to be deleted): %s    Count: %d", schMoveData[schMoveIndex].moveFileName, schMoveData[schMoveIndex].moveFailedCount  );
//			logStoreEvent(logBuffer);		
		}
		else
		{
//			memset(logBuffer,0,LOG_BUFFER_SIZE);
//			sprintf( logBuffer, "Failed: %s    Count: %d", schMoveData[schMoveIndex].moveFileName, schMoveData[schMoveIndex].moveFailedCount  );
//			logStoreEvent(logBuffer);		
		}

		schMoveServiceSV = SCH_MOVE_SERVICE_NEXT_MOVE;

		break;
	/*--------------------------------------------------*/
	case SCH_MOVE_SERVICE_NEXT_MOVE:

		schMoveIndex++;

		if(schMoveIndex >= schMainTotalValidMoves)
		{
			if(schMoveExpired == TRUE)
			{
				schMoveServiceSV = SCH_MOVE_SERVICE_DELETE_EXPIRED_MOVES;
			}
			else
			{
				schMoveServiceSV = SCH_MOVE_SERVICE_COMPLETE;
			}
		}
		else
		{
			schMoveServiceSV = SCH_MOVE_SERVICE_CHECK_MOVE;
		}

		break;
	/*--------------------------------------------------*/
	case SCH_MOVE_SERVICE_DELETE_EXPIRED_MOVES:

		schWriteMoveList();

//		memset(logBuffer,0,LOG_BUFFER_SIZE);
//		sprintf( logBuffer, "Move list updated" );
//		logStoreEvent(logBuffer);		

		if (moveFailed == FALSE)
		{
			schMoveServiceSV = SCH_MOVE_SERVICE_COMPLETE;
		}
		else
		{
			schMoveServiceSV = SCH_MOVE_LOG_ARCHIVE;
		}

		break;
	/*--------------------------------------------------*/
	case SCH_MOVE_LOG_ARCHIVE:

		logArchive();

		schMoveServiceSV = SCH_MOVE_SERVICE_COMPLETE;

		break;
	/*--------------------------------------------------*/
	case SCH_MOVE_SERVICE_COMPLETE:

		if(schTimeSec != 40)
		{
			schMoveServiceSV = SCH_MOVE_SERVICE_WAIT_TO_CHECK;
		}

		break;
	/*--------------------------------------------------*/
	default:

		break;
	/*--------------------------------------------------*/
	}
}



void WIN32_ReplaceTempFileData(TYPE_File* tempFile)
{
	if(strcmp(tempFile->name,"test1.rec") == 0)
	{
		tempFile->attr = 0xd1;
		tempFile->mjd = 53775;		// 09/02/06
		tempFile->hour = 01;
		tempFile->min = 01;
		tempFile->size = 6000;
	}

	if(strcmp(tempFile->name,"test2.rec") == 0)
	{
		tempFile->attr = 0xd1;
		tempFile->mjd = 53775;		// 09/02/06
		tempFile->hour = 01;
		tempFile->min = 02;
		tempFile->size = 8000;
	}

	if(strcmp(tempFile->name,"test3.rec") == 0)
	{
		tempFile->attr = 0xd1;
		tempFile->mjd = 53775;		// 09/02/06
		tempFile->hour = 01;
		tempFile->min = 03;
		tempFile->size = 10000;
	}

	if(strcmp(tempFile->name,"test4.rec") == 0)
	{
		tempFile->attr = 0xd1;
		tempFile->mjd = 53775;		// 09/02/06
		tempFile->hour = 01;
		tempFile->min = 04;
		tempFile->size = 12000;
	}

	if(strcmp(tempFile->name,"test5.rec") == 0)
	{
		tempFile->attr = 0xd1;
		tempFile->mjd = 53775;		// 09/02/06
		tempFile->hour = 01;
		tempFile->min = 05;
		tempFile->size = 14000;
	}


}
