/************************************************************
Part of the ukEPG project
This module moves a file to a specified directory

v0.0 sl8:	09-03-06	Inception date

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
	int	totalFileCount = 0, i = 0;
	TYPE_PlayInfo	CurrentPlaybackInfo;
	TYPE_RecInfo	CurrentRecordInfo[2];
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

	switch(schMoveServiceSV)
	{
	/*--------------------------------------------------*/
	case SCH_MOVE_SERVICE_INITIALISE:

		if(schInitRetreiveMoveData() > 0)
		{
		}
		else
		{
		}

		schMoveServiceSV = SCH_MOVE_SERVICE_WAIT_TO_CHECK;

//TAP_Print("MOVE - Initialised\r\n");		
//sprintf(buffer1,"Numb of moves: %x\r\n", schMainTotalValidMoves );
//TAP_Print(buffer1);		

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
			schMoveServiceSV = SCH_MOVE_SERVICE_CHECK_ARCHIVE;
		}
		else
		{
			schMoveServiceSV = SCH_MOVE_SERVICE_NEXT_MOVE;
		}

		break;
	/*--------------------------------------------------*/
	case SCH_MOVE_SERVICE_CHECK_ARCHIVE:

		GotoDataFiles();
		totalFileCount = TAP_Hdd_FindFirst(&tempFile); 

		schMoveSuccessful = FALSE;
		schFileFound = FALSE;
		for ( i=1; ((i <= totalFileCount) && (schFileFound == FALSE)) ; i++ )
		{
			if
			(
				(schMoveData[schMoveIndex].moveStartTime == ((tempFile.mjd << 16) + (tempFile.hour << 8) + tempFile.min))
				&&
				(tempFile.attr == ATTR_TS)
				&&
				(strcmp(tempFile.name, schMoveData[schMoveIndex].moveFileName) == 0)
			)
			{
				TAP_Hdd_GetPlayInfo (&CurrentPlaybackInfo);
				TAP_Hdd_GetRecInfo (0, &CurrentRecordInfo[0]);
				TAP_Hdd_GetRecInfo (1, &CurrentRecordInfo[1]);

				memset(CurrentPlaybackFileName,0,TS_FILE_NAME_SIZE);
				if(CurrentPlaybackInfo.playMode == PLAYMODE_Playing)
				{
					strcpy(CurrentPlaybackFileName,CurrentPlaybackInfo.file->name);
				}

				memset(CurrentRecord0FileName,0,SCH_REC_FILE_NAME_SIZE);
				if
				(
					(CurrentRecordInfo[0].recType == RECTYPE_Normal)
					&&
					(schMoveData[schMoveIndex].moveStartTime == CurrentRecordInfo[0].startTime)
				)
				{
					strcpy(CurrentRecord0FileName,CurrentRecordInfo[0].fileName);
				}

				memset(CurrentRecord1FileName,0,SCH_REC_FILE_NAME_SIZE);
				if
				(
					(CurrentRecordInfo[1].recType == RECTYPE_Normal)
					&&
					(schMoveData[schMoveIndex].moveStartTime == CurrentRecordInfo[1].startTime)
				)
				{
					strcpy(CurrentRecord1FileName,CurrentRecordInfo[1].fileName);
				}

				if
				(
					(strcmp(CurrentPlaybackFileName, schMoveData[schMoveIndex].moveFileName) != 0)
					&&
					(strcmp(CurrentRecord0FileName, schMoveData[schMoveIndex].moveFileName) != 0)
					&&
					(strcmp(CurrentRecord1FileName, schMoveData[schMoveIndex].moveFileName) != 0)
				)
				{
					sprintf( fileStr, "/DataFiles/%s", schMoveData[schMoveIndex].moveFileName );
					sprintf( folderStr, "/DataFiles/%s", schMoveData[schMoveIndex].moveFolder );
#ifndef WIN32
					if(TAP_Hdd_Move( fileStr, folderStr ) == TRUE)
					{
						schMoveSuccessful = TRUE;

						schMoveData[schMoveIndex].moveEnabled = FALSE;

						schMoveExpired = TRUE;				// Delete this move when all moves checked

//						memset(logBuffer,0,LOG_BUFFER_SIZE);
//						sprintf( logBuffer, "Found (Move Successful): %s    %d", schMoveData[schMoveIndex].moveFileName, result );
//						logStoreEvent(logBuffer);			
					}
					else
					{
//						memset(logBuffer,0,LOG_BUFFER_SIZE);
//						sprintf( logBuffer, "Found (Move Failed): %s    %d", schMoveData[schMoveIndex].moveFileName, result );
//						logStoreEvent(logBuffer);
					}
#endif

				}
				else
				{
//					memset(logBuffer,0,LOG_BUFFER_SIZE);
//					sprintf( logBuffer, "Found (Failed second test): %s   %d,%d,%d,%d", schMoveData[schMoveIndex].moveFileName, (strcmp(CurrentPlaybackFileName, schMoveData[schMoveIndex].moveFileName) != 0), (strcmp(CurrentRecord0FileName, schMoveData[schMoveIndex].moveFileName) != 0), (strcmp(CurrentRecord1FileName, schMoveData[schMoveIndex].moveFileName) != 0), (strcmp(tempFile.name, schMoveData[schMoveIndex].moveFileName) == 0));
//					logStoreEvent(logBuffer);
				}

				schFileFound = TRUE;			
			}
			else
			{
				TAP_Hdd_FindNext(&tempFile);
			}

#ifdef WIN32
	WIN32_ReplaceTempFileData(&tempFile);
#endif
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
	if(strcmp(tempFile->name,"Little Britain_S2E1.rec") == 0)
	{
		tempFile->attr = 0xd1;
		tempFile->mjd = 53775;		// 09/02/06
		tempFile->hour = 12;
		tempFile->min = 00;
	}

	if(strcmp(tempFile->name,"Little Britain_S2E2.rec") == 0)
	{
		tempFile->attr = 0xd1;
		tempFile->mjd = 53775;		// 09/02/06
		tempFile->hour = 13;
		tempFile->min = 00;
	}

	if(strcmp(tempFile->name,"Little Britain_S2E3.rec") == 0)
	{
		tempFile->attr = 0xd1;
		tempFile->mjd = 53775;		// 09/02/06
		tempFile->hour = 14;
		tempFile->min = 00;
	}

	if(strcmp(tempFile->name,"Little Britain_S2E4.rec") == 0)
	{
		tempFile->attr = 0xd1;
		tempFile->mjd = 53775;		// 09/02/06
		tempFile->hour = 15;
		tempFile->min = 00;
	}


}
