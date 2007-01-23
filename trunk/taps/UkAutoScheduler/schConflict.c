/************************************************************
Part of the ukEPG project
This module handles timer conflicts

  v0.0 sl8:	03-10-06	Inception date

**************************************************************/

byte schConflictCombineHandler(TYPE_TimerInfo* schTimerInfo)
{
	char fileNameStr[132], fileNameStrConflict[132];
	char startTimeStr[16], endTimeStr[16];
	int timerError = 0;
	TYPE_TimerInfo startConflictTimerInfo, endConflictTimerInfo;
	int startMultipleConflictCount = 0, endMultipleConflictCount = 0;
	char multipleConflictStr[3];
	int i = 0;
	int startTimerError = 0xFFFF, endTimerError = 0xFFFF;
	byte result = SCH_MAIN_TIMER_FAILED;
//	char buffer1[256];

	memset(fileNameStrConflict, 0, 132);
	memset(multipleConflictStr, 0 ,3);

	schConflictFindSameChannelConflicts(schTimerInfo, &startTimerError, &endTimerError);

	if
	(
		(startTimerError != 0xFFFF)
		&&
		(endTimerError == 0xFFFF)
	)
	{
		TAP_Timer_GetInfo( startTimerError, &startConflictTimerInfo );


//sprintf(buffer1,"Existing: %s %02d:%02d - %02d:%02d\r\n", startConflictTimerInfo.fileName, ((startConflictTimerInfo.startTime >> 8) & 0xFF), (startConflictTimerInfo.startTime & 0xFF), ((((schMainConvertTimeToMins(startConflictTimerInfo.startTime) + startConflictTimerInfo.duration) % (24 * 60)) / 60) & 0xFF), ((((schMainConvertTimeToMins(startConflictTimerInfo.startTime) + startConflictTimerInfo.duration) % (24 * 60)) % 60) & 0xFF));
//TAP_Print(buffer1);
//sprintf(buffer1,"New: %s %02d:%02d - %02d:%02d\r\n", schTimerInfo->fileName, ((schTimerInfo->startTime >> 8) & 0xFF), (schTimerInfo->startTime & 0xFF), ((((schMainConvertTimeToMins(schTimerInfo->startTime) + schTimerInfo->duration) % (24 * 60)) / 60) & 0xFF), ((((schMainConvertTimeToMins(schTimerInfo->startTime) + schTimerInfo->duration) % (24 * 60)) % 60) & 0xFF));
//TAP_Print(buffer1);


		strncpy(fileNameStrConflict, startConflictTimerInfo.fileName, strlen(startConflictTimerInfo.fileName)-4);

		startMultipleConflictCount = schMainFindMultipleConflictCount(fileNameStrConflict);
//sprintf(buffer1,"Count: %d\r\n", startMultipleConflictCount);
//TAP_Print(buffer1);

		memset(fileNameStr,0,sizeof(fileNameStr));
		sprintf(multipleConflictStr, "[M%d", (startMultipleConflictCount + 1));
		strcat(fileNameStr,multipleConflictStr);
		strcat(fileNameStr,"]");

		if(strlen(fileNameStrConflict) > SCH_MAX_FILENAME_LENGTH)
		{
			strncat(fileNameStr, fileNameStrConflict, SCH_MAX_FILENAME_LENGTH);
		}
		else
		{
			strcat(fileNameStr, fileNameStrConflict);
		}

		strcat(fileNameStr,".rec");

		startConflictTimerInfo.duration = schMainConvertTimeToMins(schTimerInfo->startTime) + schTimerInfo->duration - schMainConvertTimeToMins(startConflictTimerInfo.startTime);
		strcpy(startConflictTimerInfo.fileName, fileNameStr);

		
//sprintf(buffer1,"Modified: %s %02d:%02d - %02d:%02d\r\n", startConflictTimerInfo.fileName, ((startConflictTimerInfo.startTime >> 8) & 0xFF), (startConflictTimerInfo.startTime & 0xFF), ((((schMainConvertTimeToMins(startConflictTimerInfo.startTime) + startConflictTimerInfo.duration) % (24 * 60)) / 60) & 0xFF), ((((schMainConvertTimeToMins(startConflictTimerInfo.startTime) + startConflictTimerInfo.duration) % (24 * 60)) % 60) & 0xFF));
//TAP_Print(buffer1);
//TAP_Print("\r\n");

		timerError = TAP_Timer_Modify(startTimerError, &startConflictTimerInfo);
		if(timerError != 0)
		{
		}
		else
		{
			result = SCH_MAIN_TIMER_SUCCESS;
		}
	}
	else if
	(
		(startTimerError == 0xFFFF)
		&&
		(endTimerError != 0xFFFF)
	)
	{
		TAP_Timer_GetInfo( endTimerError, &endConflictTimerInfo );

//sprintf(buffer1,"Existing: %s %02d:%02d - %02d:%02d\r\n", endConflictTimerInfo.fileName, ((endConflictTimerInfo.startTime >> 8) & 0xFF), (endConflictTimerInfo.startTime & 0xFF), ((((schMainConvertTimeToMins(endConflictTimerInfo.startTime) + endConflictTimerInfo.duration) % (24 * 60)) / 60) & 0xFF), ((((schMainConvertTimeToMins(endConflictTimerInfo.startTime) + endConflictTimerInfo.duration) % (24 * 60)) % 60) & 0xFF));
//TAP_Print(buffer1);
//sprintf(buffer1,"New: %s %02d:%02d - %02d:%02d\r\n", schTimerInfo->fileName, ((schTimerInfo->startTime >> 8) & 0xFF), (schTimerInfo->startTime & 0xFF), ((((schMainConvertTimeToMins(schTimerInfo->startTime) + schTimerInfo->duration) % (24 * 60)) / 60) & 0xFF), ((((schMainConvertTimeToMins(schTimerInfo->startTime) + schTimerInfo->duration) % (24 * 60)) % 60) & 0xFF));
//TAP_Print(buffer1);

		strncpy(fileNameStrConflict, endConflictTimerInfo.fileName, strlen(endConflictTimerInfo.fileName)-4);

		endMultipleConflictCount = schMainFindMultipleConflictCount(fileNameStrConflict);
//sprintf(buffer1,"Count: %d\r\n", endMultipleConflictCount);
//TAP_Print(buffer1);

		memset(fileNameStr,0,sizeof(fileNameStr));
		sprintf(multipleConflictStr, "[M%d", (endMultipleConflictCount + 1));
		strcat(fileNameStr,multipleConflictStr);
		strcat(fileNameStr,"]");

		memset(fileNameStrConflict,0,sizeof(fileNameStrConflict));
		strncpy(fileNameStrConflict, schTimerInfo->fileName, strlen(schTimerInfo->fileName)-4);

		if(strlen(fileNameStrConflict) > SCH_MAX_FILENAME_LENGTH)
		{
			strncat(fileNameStr, fileNameStrConflict, SCH_MAX_FILENAME_LENGTH);
		}
		else
		{
			strcat(fileNameStr, fileNameStrConflict);
		}

		strcat(fileNameStr,".rec");
		
		endConflictTimerInfo.duration = schMainConvertTimeToMins(endConflictTimerInfo.startTime) + endConflictTimerInfo.duration - schMainConvertTimeToMins(schTimerInfo->startTime);
		endConflictTimerInfo.startTime = schTimerInfo->startTime;
		strcpy(endConflictTimerInfo.fileName, fileNameStr);
		
//sprintf(buffer1,"Modified: %s %02d:%02d - %02d:%02d\r\n", endConflictTimerInfo.fileName, ((endConflictTimerInfo.startTime >> 8) & 0xFF), (endConflictTimerInfo.startTime & 0xFF), ((((schMainConvertTimeToMins(endConflictTimerInfo.startTime) + endConflictTimerInfo.duration) % (24 * 60)) / 60) & 0xFF), ((((schMainConvertTimeToMins(endConflictTimerInfo.startTime) + endConflictTimerInfo.duration) % (24 * 60)) % 60) & 0xFF));
//TAP_Print(buffer1);
//TAP_Print("\r\n");

		timerError = TAP_Timer_Modify(endTimerError, &endConflictTimerInfo);
		if(timerError != 0)
		{
		}
		else
		{
			result = SCH_MAIN_TIMER_SUCCESS;
		}
	}
	else if
	(
		(startTimerError != 0xFFFF)
		&&
		(endTimerError != 0xFFFF)
	)
	{
		TAP_Timer_GetInfo( startTimerError, &startConflictTimerInfo );
		TAP_Timer_GetInfo( endTimerError, &endConflictTimerInfo );

		strncpy(fileNameStrConflict, endConflictTimerInfo.fileName, strlen(endConflictTimerInfo.fileName)-4);
		endMultipleConflictCount = schMainFindMultipleConflictCount(fileNameStrConflict);
		TAP_Timer_Delete( endTimerError );

		memset(fileNameStrConflict,0,sizeof(fileNameStrConflict));
		strncpy(fileNameStrConflict, startConflictTimerInfo.fileName, strlen(startConflictTimerInfo.fileName)-4);
		startMultipleConflictCount = schMainFindMultipleConflictCount(fileNameStrConflict);

		memset(fileNameStr,0,sizeof(fileNameStr));
		sprintf(multipleConflictStr, "[M%d", (startMultipleConflictCount + 1 + endMultipleConflictCount));
		strcat(fileNameStr,multipleConflictStr);
		strcat(fileNameStr,"]");

		if(strlen(fileNameStrConflict) > SCH_MAX_FILENAME_LENGTH)
		{
			strncat(fileNameStr, fileNameStrConflict, SCH_MAX_FILENAME_LENGTH);
		}
		else
		{
			strcat(fileNameStr, fileNameStrConflict);
		}
		
		strcat(fileNameStr,".rec");

		startConflictTimerInfo.duration = schMainConvertTimeToMins(endConflictTimerInfo.startTime) + endConflictTimerInfo.duration - schMainConvertTimeToMins(startConflictTimerInfo.startTime);
		strcpy(startConflictTimerInfo.fileName, fileNameStr);
		
//sprintf(buffer1,"Modified: %s %02d:%02d - %02d:%02d\r\n", startConflictTimerInfo.fileName, ((startConflictTimerInfo.startTime >> 8) & 0xFF), (startConflictTimerInfo.startTime & 0xFF), ((((schMainConvertTimeToMins(startConflictTimerInfo.startTime) + startConflictTimerInfo.duration) % (24 * 60)) / 60) & 0xFF), ((((schMainConvertTimeToMins(startConflictTimerInfo.startTime) + startConflictTimerInfo.duration) % (24 * 60)) % 60) & 0xFF));
//TAP_Print(buffer1);
//TAP_Print("\r\n");

		timerError = TAP_Timer_Modify(startTimerError, &startConflictTimerInfo);
		if(timerError != 0)
		{
			// Combine failed. Recover end conflict
			TAP_Timer_Add(&endConflictTimerInfo);
		}
		else
		{
			result = SCH_MAIN_TIMER_SUCCESS;
		}
	}
	else
	{
		// Must be cross channel conflict. Unable to combine.
	}

	return result;
}



byte schConflictSeparateHandler(TYPE_TimerInfo* schTimerInfo, dword eventStartTime, dword eventEndTime, bool* schMoveListModified)
{
	TYPE_TimerInfo startSameChannelConflictTimerInfo, endSameChannelConflictTimerInfo;
	int startSameChannelConflictTimerError = 0xFFFF, endSameChannelConflictTimerError = 0xFFFF;
	int timerError = 0;
	byte result = SCH_MAIN_TIMER_FAILED;

	schConflictFindSameChannelConflicts(schTimerInfo, &startSameChannelConflictTimerError, &endSameChannelConflictTimerError);

	if
	(
		(startSameChannelConflictTimerError != 0xFFFF)
		&&
		(endSameChannelConflictTimerError == 0xFFFF)
	)
	{
		TAP_Timer_GetInfo( startSameChannelConflictTimerError, &startSameChannelConflictTimerInfo );

		if(schConflictProcessSeparateTimers(schTimerInfo, eventStartTime, eventEndTime, startSameChannelConflictTimerError) == TRUE)
		{
			timerError = TAP_Timer_Add(schTimerInfo);
			if(timerError == 0)
			{
				// Success

				schConflictUpdateMoveList(&startSameChannelConflictTimerInfo, startSameChannelConflictTimerError);

				(*schMoveListModified) = TRUE;

				result = SCH_MAIN_TIMER_SUCCESS_WITH_MODIFICATIONS;
			}
			else if
			(
				(timerError == 1)
				||
				(timerError == 2)
			)
			{
//TAP_Print("Invalid\r\n");
				// Recover startSameChannelConflictTimerError

				TAP_Timer_Modify(startSameChannelConflictTimerError, &startSameChannelConflictTimerInfo);
			}
			else
			{
				// Attempt to resolve cross channel conflict

				schConflictResolveCrossChannelConflicts(schTimerInfo, eventStartTime, eventEndTime, schMoveListModified, &result);

				if(result == SCH_MAIN_TIMER_FAILED)
				{
					// Recover endSameChannelConflictTimerError

					TAP_Timer_Modify(startSameChannelConflictTimerError, &startSameChannelConflictTimerInfo);
				}
			}
		}
	}
	else if
	(
		(startSameChannelConflictTimerError == 0xFFFF)
		&&
		(endSameChannelConflictTimerError != 0xFFFF)
	)
	{
		TAP_Timer_GetInfo( endSameChannelConflictTimerError, &endSameChannelConflictTimerInfo );

		if(schConflictProcessSeparateTimers(schTimerInfo, eventStartTime, eventEndTime, endSameChannelConflictTimerError) == TRUE)
		{
			timerError = TAP_Timer_Add(schTimerInfo);
			if(timerError == 0)
			{
				// Success

				schConflictUpdateMoveList(&endSameChannelConflictTimerInfo, endSameChannelConflictTimerError);

				(*schMoveListModified) = TRUE;

				result = SCH_MAIN_TIMER_SUCCESS_WITH_MODIFICATIONS;
			}
			else if
			(
				(timerError == 1)
				||
				(timerError == 2)
			)
			{
//TAP_Print("Invalid\r\n");
				// Recover endSameChannelConflictTimerError

				TAP_Timer_Modify(endSameChannelConflictTimerError, &endSameChannelConflictTimerInfo);
			}
			else
			{
				// Attempt to resolve cross channel conflict

				schConflictResolveCrossChannelConflicts(schTimerInfo, eventStartTime, eventEndTime, schMoveListModified, &result);

				if(result == SCH_MAIN_TIMER_FAILED)
				{
					// Recover endSameChannelConflictTimerError

					TAP_Timer_Modify(endSameChannelConflictTimerError, &endSameChannelConflictTimerInfo);
				}
			}
		}
	}
	else if
	(
		(startSameChannelConflictTimerError != 0xFFFF)
		&&
		(endSameChannelConflictTimerError != 0xFFFF)
	)
	{
		TAP_Timer_GetInfo( startSameChannelConflictTimerError, &startSameChannelConflictTimerInfo );
		TAP_Timer_GetInfo( endSameChannelConflictTimerError, &endSameChannelConflictTimerInfo );

		if
		(
			(schConflictProcessSeparateTimers(schTimerInfo, eventStartTime, eventEndTime, startSameChannelConflictTimerError) == TRUE)
			&&
			(schConflictProcessSeparateTimers(schTimerInfo, eventStartTime, eventEndTime, endSameChannelConflictTimerError) == TRUE)
		)
		{
			timerError = TAP_Timer_Add(schTimerInfo);
			if(timerError == 0)
			{
				// Success

				schConflictUpdateMoveList(&startSameChannelConflictTimerInfo, startSameChannelConflictTimerError);
				schConflictUpdateMoveList(&endSameChannelConflictTimerInfo, endSameChannelConflictTimerError);

				(*schMoveListModified) = TRUE;

				result = SCH_MAIN_TIMER_SUCCESS_WITH_MODIFICATIONS;
			}
			else if
			(
				(timerError == 1)
				||
				(timerError == 2)
			)
			{
//TAP_Print("Invalid\r\n");
				// Recover startSameChannelConflictTimerError and endSameChannelConflictTimerError

				TAP_Timer_Modify(startSameChannelConflictTimerError, &startSameChannelConflictTimerInfo);
				TAP_Timer_Modify(endSameChannelConflictTimerError, &endSameChannelConflictTimerInfo);
			}
			else
			{
				// Attempt to resolve cross channel conflict

				schConflictResolveCrossChannelConflicts(schTimerInfo, eventStartTime, eventEndTime, schMoveListModified, &result);

				if(result == SCH_MAIN_TIMER_FAILED)
				{
					// Recover endSameChannelConflictTimerError

					TAP_Timer_Modify(startSameChannelConflictTimerError, &startSameChannelConflictTimerInfo);
					TAP_Timer_Modify(endSameChannelConflictTimerError, &endSameChannelConflictTimerInfo);
				}
			}
		}
	}
	else
	{
		// Attempt to resolve cross channel conflict

		schConflictResolveCrossChannelConflicts(schTimerInfo, eventStartTime, eventEndTime, schMoveListModified, &result);
	}

	return result;
}




void schConflictResolveCrossChannelConflicts(TYPE_TimerInfo* schTimerInfo, dword eventStartTime, dword eventEndTime, bool* schMoveListModified, byte* result)
{
	TYPE_TimerInfo startCrossChannelConflictTimerInfo[2];
	int startCrossChannelConflictTimerError[2] = {0xFFFF,0xFFFF};
	int startCrossChannelConflictTimerErrorIndex = 0;
	dword startCrossChannelConflictStartTimeInMins[2] = {0,0};
	dword startCrossChannelConflictEndTimeInMins[2] = {0,0};
	TYPE_TimerInfo endCrossChannelConflictTimerInfo[2];
	int endCrossChannelConflictTimerError[2] = {0xFFFF,0xFFFF};
	int endCrossChannelConflictTimerErrorIndex = 0;
	dword endCrossChannelConflictStartTimeInMins[2] = {0,0};
	dword endCrossChannelConflictEndTimeInMins[2] = {0,0};
	int timerError = 0;

	schConflictFindCrossChannelConflicts(schTimerInfo, &startCrossChannelConflictTimerError[0], &endCrossChannelConflictTimerError[0]);

	if
	(
		(
			(startCrossChannelConflictTimerError[0] != 0xFFFF)
			||
			(startCrossChannelConflictTimerError[1] != 0xFFFF)
		)
		&&
		(endCrossChannelConflictTimerError[0] == 0xFFFF)
		&&
		(endCrossChannelConflictTimerError[1] == 0xFFFF)
	)
	{
		if(startCrossChannelConflictTimerError[0] == 0xFFFF)
		{
			startCrossChannelConflictTimerErrorIndex = 1;
		}
		else if(startCrossChannelConflictTimerError[1] == 0xFFFF)
		{
			startCrossChannelConflictTimerErrorIndex = 0;
		}
		else
		{
			TAP_Timer_GetInfo(startCrossChannelConflictTimerError[0], &startCrossChannelConflictTimerInfo[0]);
			startCrossChannelConflictStartTimeInMins[0] = schMainConvertTimeToMins(startCrossChannelConflictTimerInfo[0].startTime);
			startCrossChannelConflictEndTimeInMins[0] = startCrossChannelConflictStartTimeInMins[0] + startCrossChannelConflictTimerInfo[0].duration;

			TAP_Timer_GetInfo(startCrossChannelConflictTimerError[1], &startCrossChannelConflictTimerInfo[1]);
			startCrossChannelConflictStartTimeInMins[1] = schMainConvertTimeToMins(startCrossChannelConflictTimerInfo[1].startTime);
			startCrossChannelConflictEndTimeInMins[1] = startCrossChannelConflictStartTimeInMins[1] + startCrossChannelConflictTimerInfo[1].duration;

			if(startCrossChannelConflictEndTimeInMins[0] <= startCrossChannelConflictEndTimeInMins[1])
			{
				startCrossChannelConflictTimerErrorIndex = 0;
			}
			else
			{
				startCrossChannelConflictTimerErrorIndex = 1;
			}
		}

		TAP_Timer_GetInfo( startCrossChannelConflictTimerError[startCrossChannelConflictTimerErrorIndex], &startCrossChannelConflictTimerInfo[startCrossChannelConflictTimerErrorIndex] );

		if(schConflictProcessSeparateTimers(schTimerInfo, eventStartTime, eventEndTime, startCrossChannelConflictTimerError[startCrossChannelConflictTimerErrorIndex]) == TRUE)
		{
			timerError = TAP_Timer_Add(schTimerInfo);
			if(timerError == 0)
			{
				// Success

				schConflictUpdateMoveList(&startCrossChannelConflictTimerInfo[startCrossChannelConflictTimerErrorIndex], startCrossChannelConflictTimerError[startCrossChannelConflictTimerErrorIndex]);

				(*schMoveListModified) = TRUE;

				(*result) = SCH_MAIN_TIMER_SUCCESS_WITH_MODIFICATIONS;
			}
			else if
			(
				(timerError == 1)
				||
				(timerError == 2)
			)
			{
				// Recover startCrossChannelConflictTimerError

				TAP_Timer_Modify(startCrossChannelConflictTimerError[startCrossChannelConflictTimerErrorIndex], &startCrossChannelConflictTimerInfo[startCrossChannelConflictTimerErrorIndex]);
			}
			else
			{
				// Try other start conflict

				if(startCrossChannelConflictTimerErrorIndex == 0)
				{
					startCrossChannelConflictTimerErrorIndex = 1;
				}
				else
				{
					startCrossChannelConflictTimerErrorIndex = 0;
				}

				if(startCrossChannelConflictTimerError[startCrossChannelConflictTimerErrorIndex] != 0xFFFF)
				{
					TAP_Timer_GetInfo( startCrossChannelConflictTimerError[startCrossChannelConflictTimerErrorIndex], &startCrossChannelConflictTimerInfo[startCrossChannelConflictTimerErrorIndex] );

					if(schConflictProcessSeparateTimers(schTimerInfo, eventStartTime, eventEndTime, startCrossChannelConflictTimerError[startCrossChannelConflictTimerErrorIndex]) == TRUE)
					{
						timerError = TAP_Timer_Add(schTimerInfo);
						if(timerError == 0)
						{
							// Success

							schConflictUpdateMoveList(&startCrossChannelConflictTimerInfo[startCrossChannelConflictTimerErrorIndex], startCrossChannelConflictTimerError[startCrossChannelConflictTimerErrorIndex]);

							(*schMoveListModified) = TRUE;

							(*result) = SCH_MAIN_TIMER_SUCCESS_WITH_MODIFICATIONS;
						}
						else
						{
							// Recover startCrossChannelConflictTimerError

							TAP_Timer_Modify(startCrossChannelConflictTimerError[startCrossChannelConflictTimerErrorIndex], &startCrossChannelConflictTimerInfo[startCrossChannelConflictTimerErrorIndex]);
						}
					}
				}
			}
		}
	}
	else if
	(
		(startCrossChannelConflictTimerError[0] == 0xFFFF)
		&&
		(startCrossChannelConflictTimerError[1] == 0xFFFF)
		&&
		(
			(endCrossChannelConflictTimerError[0] != 0xFFFF)
			||
			(endCrossChannelConflictTimerError[1] != 0xFFFF)
		)
	)
	{
		if(endCrossChannelConflictTimerError[0] == 0xFFFF)
		{
			endCrossChannelConflictTimerErrorIndex = 1;
		}
		else if(endCrossChannelConflictTimerError[1] == 0xFFFF)
		{
			endCrossChannelConflictTimerErrorIndex = 0;
		}
		else
		{
			TAP_Timer_GetInfo(endCrossChannelConflictTimerError[0], &endCrossChannelConflictTimerInfo[0]);
			endCrossChannelConflictStartTimeInMins[0] = schMainConvertTimeToMins(endCrossChannelConflictTimerInfo[0].startTime);

			TAP_Timer_GetInfo(endCrossChannelConflictTimerError[1], &endCrossChannelConflictTimerInfo[1]);
			endCrossChannelConflictStartTimeInMins[1] = schMainConvertTimeToMins(endCrossChannelConflictTimerInfo[1].startTime);

			if(endCrossChannelConflictStartTimeInMins[0] >= endCrossChannelConflictStartTimeInMins[1])
			{
				endCrossChannelConflictTimerErrorIndex = 0;
			}
			else
			{
				endCrossChannelConflictTimerErrorIndex = 1;
			}
		}

		TAP_Timer_GetInfo( endCrossChannelConflictTimerError[endCrossChannelConflictTimerErrorIndex], &endCrossChannelConflictTimerInfo[endCrossChannelConflictTimerErrorIndex] );

		if(schConflictProcessSeparateTimers(schTimerInfo, eventStartTime, eventEndTime, endCrossChannelConflictTimerError[endCrossChannelConflictTimerErrorIndex]) == TRUE)
		{
			timerError = TAP_Timer_Add(schTimerInfo);
			if(timerError == 0)
			{
				// Success

				schConflictUpdateMoveList(&endCrossChannelConflictTimerInfo[endCrossChannelConflictTimerErrorIndex], endCrossChannelConflictTimerError[endCrossChannelConflictTimerErrorIndex]);

				(*schMoveListModified) = TRUE;

				(*result) = SCH_MAIN_TIMER_SUCCESS_WITH_MODIFICATIONS;
			}
			else if
			(
				(timerError == 1)
				||
				(timerError == 2)
			)
			{
				// Recover endCrossChannelConflictTimerError

				TAP_Timer_Modify(endCrossChannelConflictTimerError[endCrossChannelConflictTimerErrorIndex], &endCrossChannelConflictTimerInfo[endCrossChannelConflictTimerErrorIndex]);
			}
			else
			{
				// Try other end conflict

				if(endCrossChannelConflictTimerErrorIndex == 0)
				{
					endCrossChannelConflictTimerErrorIndex = 1;
				}
				else
				{
					endCrossChannelConflictTimerErrorIndex = 0;
				}

				if(endCrossChannelConflictTimerError[endCrossChannelConflictTimerErrorIndex] != 0xFFFF)
				{
					TAP_Timer_GetInfo( endCrossChannelConflictTimerError[endCrossChannelConflictTimerErrorIndex], &endCrossChannelConflictTimerInfo[endCrossChannelConflictTimerErrorIndex] );

					if(schConflictProcessSeparateTimers(schTimerInfo, eventStartTime, eventEndTime, endCrossChannelConflictTimerError[endCrossChannelConflictTimerErrorIndex]) == TRUE)
					{
						timerError = TAP_Timer_Add(schTimerInfo);
						if(timerError == 0)
						{
							// Success

							schConflictUpdateMoveList(&endCrossChannelConflictTimerInfo[endCrossChannelConflictTimerErrorIndex], endCrossChannelConflictTimerError[endCrossChannelConflictTimerErrorIndex]);

							(*schMoveListModified) = TRUE;

							(*result) = SCH_MAIN_TIMER_SUCCESS_WITH_MODIFICATIONS;
						}
						else
						{
							// Recover endCrossChannelConflictTimerError

							TAP_Timer_Modify(endCrossChannelConflictTimerError[endCrossChannelConflictTimerErrorIndex], &endCrossChannelConflictTimerInfo[endCrossChannelConflictTimerErrorIndex]);
						}
					}
				}
			}
		}
	}
	else
	{
	}

}	


bool schConflictCheckEvent(TYPE_TapEvent *schEpgData, int schEpgDataIndex, word schSvcNum, byte schSvcType)
{
	TYPE_TimerInfo setTimerInfo;
	dword	eventStartTimeInMins = 0, eventEndTimeInMins = 0;
	dword	timerStartTimeInMins = 0, timerEndTimeInMins = 0;
	int	numberOfTimers = 0, i = 0;
	bool	result = FALSE;

	numberOfTimers = TAP_Timer_GetTotalNum();

	if(numberOfTimers > 0)
	{
		eventStartTimeInMins = schMainConvertTimeToMins(schEpgData[schEpgDataIndex].startTime);
		eventEndTimeInMins = schMainConvertTimeToMins(schEpgData[schEpgDataIndex].endTime);

		for(i = 0; i < numberOfTimers ; i++ )
		{
			if(TAP_Timer_GetInfo(i, &setTimerInfo ))
			{
				if
				(
					(schSvcNum == setTimerInfo.svcNum)
					&&
					(schSvcType == setTimerInfo.svcType)
				)
				{
					timerStartTimeInMins = schMainConvertTimeToMins(setTimerInfo.startTime);
					timerEndTimeInMins = timerStartTimeInMins + setTimerInfo.duration;

					if
					(
						(timerStartTimeInMins <= eventStartTimeInMins)
						&&
						(timerEndTimeInMins >= eventEndTimeInMins)
					)
					{
						i = numberOfTimers;

						result = TRUE;
					}
				}
			}
		}
	}

	return result;
}




void schConflictFindSameChannelConflicts(TYPE_TimerInfo *newTimerInfo, int *startTimerError, int *endTimerError)
{
	TYPE_TimerInfo setTimerInfo;
	dword	newTimerStartTimeInMins = 0, newTimerEndTimeInMins = 0;
	dword	setTimerStartTimeInMins = 0, setTimerEndTimeInMins = 0;
	int	numberOfTimers = 0, i = 0;

	numberOfTimers = TAP_Timer_GetTotalNum();

	if(numberOfTimers > 0)
	{
		newTimerStartTimeInMins = schMainConvertTimeToMins(newTimerInfo->startTime);
		newTimerEndTimeInMins = newTimerStartTimeInMins + newTimerInfo->duration;

		for(i = 0; i < numberOfTimers ; i++ )
		{
			if(TAP_Timer_GetInfo(i, &setTimerInfo ))
			{
				if
				(
					(setTimerInfo.svcNum == newTimerInfo->svcNum)
					&&
					(setTimerInfo.svcType == newTimerInfo->svcType)
					&&
					(setTimerInfo.reservationType == RESERVE_TYPE_Onetime)
				)
				{
					setTimerStartTimeInMins = schMainConvertTimeToMins(setTimerInfo.startTime);
					setTimerEndTimeInMins = setTimerStartTimeInMins + setTimerInfo.duration;

					if
					(
						(newTimerStartTimeInMins > setTimerStartTimeInMins)
						&&
						(newTimerStartTimeInMins < setTimerEndTimeInMins)
						&&
						(newTimerEndTimeInMins > setTimerEndTimeInMins)
					)
					{
						(*startTimerError) = i;
					}
					else if
					(
						(newTimerStartTimeInMins < setTimerStartTimeInMins)
						&&
						(newTimerEndTimeInMins > setTimerStartTimeInMins)
						&&
						(newTimerEndTimeInMins < setTimerEndTimeInMins)
					)
					{
						(*endTimerError) = i;
					}
				}
			}
		}
	}
}


void schConflictFindCrossChannelConflicts(TYPE_TimerInfo *newTimerInfo, int *startTimerError, int *endTimerError)
{
	TYPE_TimerInfo setTimerInfo;
	dword	newTimerStartTimeInMins = 0, newTimerEndTimeInMins = 0;
	dword	setTimerStartTimeInMins = 0, setTimerEndTimeInMins = 0;
	int	numberOfTimers = 0, i = 0;
	int	startConflictCount = 0;
	int	endConflictCount = 0;

	numberOfTimers = TAP_Timer_GetTotalNum();

	if(numberOfTimers > 0)
	{
		newTimerStartTimeInMins = schMainConvertTimeToMins(newTimerInfo->startTime);
		newTimerEndTimeInMins = newTimerStartTimeInMins + newTimerInfo->duration;

		for(i = 0; i < numberOfTimers ; i++ )
		{
			if(TAP_Timer_GetInfo(i, &setTimerInfo ))
			{
				if
				(
					(setTimerInfo.svcNum != newTimerInfo->svcNum)
					&&
					(setTimerInfo.svcType == newTimerInfo->svcType)
					&&
					(setTimerInfo.reservationType == RESERVE_TYPE_Onetime)
				)
				{
					setTimerStartTimeInMins = schMainConvertTimeToMins(setTimerInfo.startTime);
					setTimerEndTimeInMins = setTimerStartTimeInMins + setTimerInfo.duration;

					if
					(
						(
							(startConflictCount < 2)
							&&
							(
								(newTimerStartTimeInMins > setTimerStartTimeInMins)
								&&
								(newTimerStartTimeInMins < setTimerEndTimeInMins)
								&&
								(newTimerEndTimeInMins > setTimerEndTimeInMins)
							)
						)
					)
					{
						startTimerError[startConflictCount] = i;

						startConflictCount++;
					}
					else if
					(
						(
							(endConflictCount < 2)
							&&
							(
								(newTimerStartTimeInMins < setTimerStartTimeInMins)
								&&
								(newTimerEndTimeInMins > setTimerStartTimeInMins)
								&&
								(newTimerEndTimeInMins < setTimerEndTimeInMins)
							)
						)
					)
					{
						endTimerError[endConflictCount] = i;

						endConflictCount++;
					}
				}
			}
		}
	}
}


bool schConflictProcessSeparateTimers(TYPE_TimerInfo* schTimerInfo, dword eventStartTime, dword eventEndTime, int timerError)
{
	TYPE_TimerInfo	conflictTimerInfo;
//	dword	endConflictInMins = 0, endTimerInMins = 0, endEventInMins = 0, currentTimeInMins = 0;
	bool	result = FALSE;

	dword	currentTimeInMins = 0;
	dword	startConflictTimerInMins = 0, endConflictTimerInMins = 0;
	dword	startConflictEventInMins = 0, endConflictEventInMins = 0;
	dword	startNewTimerInMins = 0, endNewTimerInMins = 0;
	dword	startNewEventInMins = 0, endNewEventInMins = 0;
	dword	startConflictPaddingInMins = 0, endConflictPaddingInMins = 0;
	dword	startNewPaddingInMins = 0, endNewPaddingInMins = 0;
	dword	diffEventInMins = 0;
	int	schConflictEpgTotalEvents = 0;
	int	schConflictEpgIndex = 0, schConflictLastEpgIndex = 0;
	dword	firstEvent = 0, lastEvent = 0;
//	char buffer1[256];

//sprintf(buffer1,"Event %02d:%02d - %02d:%02d\r\n",((eventStartTime >> 8) & 0xFF), (eventStartTime & 0xFF), ((eventEndTime >> 8) & 0xFF), (eventEndTime & 0xFF));
//TAP_Print(buffer1);

//sprintf(buffer1,"Original Timer %02d:%02d - %02d:%02d\r\n",((schTimerInfo->startTime >> 8) & 0xFF), (schTimerInfo->startTime & 0xFF), ((((schMainConvertTimeToMins(schTimerInfo->startTime) + schTimerInfo->duration) % (24 * 60)) / 60) & 0xFF), ((((schMainConvertTimeToMins(schTimerInfo->startTime) + schTimerInfo->duration) % (24 * 60)) % 60) & 0xFF));
//TAP_Print(buffer1);

//sprintf(buffer1,"Original Conflict %02d:%02d - %02d:%02d\r\n",((conflictTimerInfo.startTime >> 8) & 0xFF), (conflictTimerInfo.startTime & 0xFF), ((((schMainConvertTimeToMins(conflictTimerInfo.startTime) + conflictTimerInfo.duration) % (24 * 60)) / 60) & 0xFF), ((((schMainConvertTimeToMins(conflictTimerInfo.startTime) + conflictTimerInfo.duration) % (24 * 60)) % 60) & 0xFF));
//TAP_Print(buffer1);

	currentTimeInMins = (schTimeMjd * 24 * 60) + (schTimeHour * 60) + schTimeMin;

	TAP_Timer_GetInfo( (timerError & 0x0000ffff), &conflictTimerInfo );

	if(schMainConvertTimeToMins(conflictTimerInfo.startTime) > (currentTimeInMins + 2))
	{
		startConflictTimerInMins = schMainConvertTimeToMins(conflictTimerInfo.startTime);
		endConflictTimerInMins = schMainConvertTimeToMins(conflictTimerInfo.startTime) + conflictTimerInfo.duration;

		startNewTimerInMins = schMainConvertTimeToMins(schTimerInfo->startTime);
		endNewTimerInMins = schMainConvertTimeToMins(schTimerInfo->startTime) + schTimerInfo->duration;

		startNewEventInMins = schMainConvertTimeToMins(eventStartTime);
		endNewEventInMins = schMainConvertTimeToMins(eventEndTime);

		if(schTimerInfo->svcNum == conflictTimerInfo.svcNum)
		{
			if
			(
				(conflictTimerInfo.startTime <= eventStartTime)
				&&
				(endConflictTimerInMins >= endNewEventInMins)
			)
			{
				/* Do Nothing - Event already covered */
			}
			else if
			(
				(conflictTimerInfo.startTime <= schTimerInfo->startTime)
				&&
				(endConflictTimerInMins > schMainConvertTimeToMins(schTimerInfo->startTime))
				&&
				(endConflictTimerInMins < endNewTimerInMins)
			)
			{
				/* Conflict before new event */

				if(schMainConflictOption == SCH_MAIN_CONFLICT_SEPARATE) //Discard paddings between timers
				{
					if(conflictTimerInfo.reservationType == RESERVE_TYPE_Onetime)
					{
						schTimerInfo->startTime = eventStartTime;
						schTimerInfo->duration = endNewTimerInMins - schMainConvertTimeToMins(eventStartTime);
	
						if(endConflictTimerInMins > schMainConvertTimeToMins(eventStartTime))
						{
							// Remove end padding from conflict
	
							conflictTimerInfo.duration -= (endConflictTimerInMins - schMainConvertTimeToMins(eventStartTime));
	
							if(TAP_Timer_Modify((timerError & 0x0000ffff), &conflictTimerInfo) == 0)
							{
								result = TRUE;
							}
						}
						else
						{
							result = TRUE;
						}
					}
				}
				else if(schMainConflictOption == SCH_MAIN_CONFLICT_SEPARATE_KEEP_END_PADDING) //Keep end padding between timers
				{
//janilxx: This works for one channel timers. 
//This should work also for multi-channel timers but for some reason this code is not called at all
					schTimerInfo->startTime = schMainConvertMinsToTime(schMainConvertTimeToMins(conflictTimerInfo.startTime) + conflictTimerInfo.duration);
					schTimerInfo->duration = endNewTimerInMins - schMainConvertTimeToMins(schTimerInfo->startTime);

					result = TRUE;
				}
				else
				{
				}
			}
			else if
			(
				(endConflictTimerInMins >= endNewTimerInMins)
				&&
				(schMainConvertTimeToMins(conflictTimerInfo.startTime) < endNewTimerInMins)
				&&
				(conflictTimerInfo.startTime > schTimerInfo->startTime)
			)
			{
				/* Conflict after new event */
				
				if(schMainConflictOption == SCH_MAIN_CONFLICT_SEPARATE) //Discard paddings between timers
				{
					if(conflictTimerInfo.reservationType == RESERVE_TYPE_Onetime)
					{
						schTimerInfo->duration = endNewEventInMins - schMainConvertTimeToMins(schTimerInfo->startTime);
	
						if(schMainConvertTimeToMins(conflictTimerInfo.startTime) < endNewEventInMins)
						{
							// Remove start padding from conflict
	
							conflictTimerInfo.duration -= (endNewEventInMins - schMainConvertTimeToMins(conflictTimerInfo.startTime));
							conflictTimerInfo.startTime = eventEndTime;
	
							if(TAP_Timer_Modify((timerError & 0x0000ffff), &conflictTimerInfo) == 0)
							{
								result = TRUE;
							}
						}
						else
						{
							result = TRUE;
						}
					}
				}
				else if(schMainConflictOption == SCH_MAIN_CONFLICT_SEPARATE_KEEP_END_PADDING) //Keep end padding between timers
				{
					conflictTimerInfo.startTime = schMainConvertMinsToTime(schMainConvertTimeToMins(schTimerInfo->startTime) + schTimerInfo->duration);
					conflictTimerInfo.duration = endConflictTimerInMins - schMainConvertTimeToMins(conflictTimerInfo.startTime);
	
					if(TAP_Timer_Modify((timerError & 0x0000ffff), &conflictTimerInfo) == 0)
					{
						result = TRUE;
					}
				}	
			}
			else
			{
			}
		}
		else
		{
			/* Different channel conflict */
#ifndef WIN32
			if( schConflictEpgData )
			{
				TAP_MemFree( schConflictEpgData );
				schConflictEpgData = 0;
			}

			schConflictEpgData = TAP_GetEvent( conflictTimerInfo.svcType, conflictTimerInfo.svcNum, &schConflictEpgTotalEvents );
#else
			schConflictEpgData = TAP_GetEvent_SDK( conflictTimerInfo.svcType, conflictTimerInfo.svcNum, &schConflictEpgTotalEvents );
#endif

			schConflictLastEpgIndex = 0;
			firstEvent = 0xFFFFFFFF;
			for(schConflictEpgIndex = 0; schConflictEpgIndex <  schConflictEpgTotalEvents; schConflictEpgIndex++)
			{

//TAP_Print(schConflictEpgData[schConflictEpgIndex].eventName);
//TAP_Print("\r\n");

//sprintf(buffer1,"Conflict start time: %x   Event start time: %x\r\n", conflictTimerInfo.startTime, schConflictEpgData[schConflictEpgIndex].startTime);
//TAP_Print(buffer1);

//sprintf(buffer1,"Conflict end time (in mins): %x   Event end time (in mins): %x\r\n", endConflictTimerInMins, (schMainConvertTimeToMins(schConflictEpgData[schConflictEpgIndex].startTime) + schConflictEpgData[schConflictEpgIndex].duration));
//TAP_Print(buffer1);
				if
				(
					(
						(schConflictEpgData[schConflictEpgIndex].startTime != schConflictEpgData[schConflictLastEpgIndex].startTime)
						||
						(schConflictEpgIndex == 0)
					)
					&&
					(conflictTimerInfo.startTime <= schConflictEpgData[schConflictEpgIndex].startTime)
					&&
					(endConflictTimerInMins >= (schMainConvertTimeToMins(schConflictEpgData[schConflictEpgIndex].endTime)))
				)
				{
					if(firstEvent == 0xFFFFFFFF)
					{
						firstEvent = schConflictEpgIndex;
					}
					lastEvent = schConflictEpgIndex;
				}

				schConflictLastEpgIndex = schConflictEpgIndex;
			}

			if(firstEvent != 0xFFFFFFFF)
			{
				startConflictEventInMins = schMainConvertTimeToMins(schConflictEpgData[firstEvent].startTime);
				endConflictEventInMins = schMainConvertTimeToMins(schConflictEpgData[lastEvent].endTime);

				startConflictPaddingInMins = startConflictEventInMins - startConflictTimerInMins;
				endConflictPaddingInMins = endConflictTimerInMins - endConflictEventInMins;
				startNewPaddingInMins = startNewEventInMins - startNewTimerInMins;
				endNewPaddingInMins = endNewTimerInMins - endNewEventInMins;

				if
				(
					(startConflictTimerInMins <= startNewTimerInMins)
					&&
					(endConflictTimerInMins > startNewTimerInMins)
					&&
					(endConflictTimerInMins < endNewTimerInMins)
					&&
					(endConflictEventInMins <= startNewEventInMins)
				)
				{
					/* Conflict before event */

					if(schMainConflictOption == SCH_MAIN_CONFLICT_SEPARATE_KEEP_END_PADDING)
					{
						schTimerInfo->startTime = schMainConvertMinsToTime(endConflictTimerInMins);
						schTimerInfo->duration = endNewTimerInMins - endConflictTimerInMins;

						result = TRUE;					
					}	
					else if(endConflictEventInMins == startNewEventInMins)
					{
						// Remove all padding

						schTimerInfo->startTime = eventStartTime;
						schTimerInfo->duration = endNewTimerInMins - startNewEventInMins;

						if(endConflictTimerInMins > startNewEventInMins)
						{
							// Remove end padding from conflict

							conflictTimerInfo.duration -= (endConflictTimerInMins - startNewEventInMins);

							if(TAP_Timer_Modify((timerError & 0x0000ffff), &conflictTimerInfo) == 0)
							{
								result = TRUE;
							}
						}
						else
						{
							result = TRUE;
						}
					}
					else 
					{
						// Attempt to share available padding between conflict and new timer

						diffEventInMins = startNewEventInMins - endConflictEventInMins;

						if
						(
							(endConflictPaddingInMins > (diffEventInMins / 2))
							&&
							(startNewPaddingInMins > (diffEventInMins / 2))
						)
						{
							startNewPaddingInMins = (diffEventInMins / 2);

							endConflictPaddingInMins = diffEventInMins - startNewPaddingInMins;
						}
						else if (endConflictPaddingInMins > (diffEventInMins - startNewPaddingInMins))
						{
							endConflictPaddingInMins = diffEventInMins - startNewPaddingInMins;
						}
						else
						{
							startNewPaddingInMins = diffEventInMins - endConflictPaddingInMins;
						}

						schTimerInfo->startTime = schMainConvertMinsToTime(startNewEventInMins - startNewPaddingInMins);
						schTimerInfo->duration = (endNewTimerInMins - startNewEventInMins) + startNewPaddingInMins;

						conflictTimerInfo.duration = (endConflictEventInMins + endConflictPaddingInMins) - startConflictTimerInMins;

						if(TAP_Timer_Modify((timerError & 0x0000ffff), &conflictTimerInfo) == 0)
						{
							result = TRUE;
						}
					}				
				}
				else if
				(
					(endConflictTimerInMins >= endNewTimerInMins)
					&&
					(schMainConvertTimeToMins(conflictTimerInfo.startTime) < endNewTimerInMins)
					&&
					(conflictTimerInfo.startTime > schTimerInfo->startTime)
					&&
					(schMainConvertTimeToMins(schConflictEpgData[firstEvent].startTime) >= endNewEventInMins)
				)
				{
					/* Conflict after event */

					if(schMainConflictOption == SCH_MAIN_CONFLICT_SEPARATE_KEEP_END_PADDING)
					{
						conflictTimerInfo.startTime = schMainConvertMinsToTime(endNewTimerInMins);
						conflictTimerInfo.duration = endConflictTimerInMins - endNewTimerInMins;
		
						if(TAP_Timer_Modify((timerError & 0x0000ffff), &conflictTimerInfo) == 0)
						{
							result = TRUE;
						}
					}	
					else if(endNewEventInMins == startConflictEventInMins)
					{
						// Remove all padding

						schTimerInfo->duration = endNewEventInMins - startNewTimerInMins;

						if(startConflictTimerInMins < endNewEventInMins)
						{
							// Remove start padding from conflict

							conflictTimerInfo.startTime = schMainConvertMinsToTime(startConflictEventInMins);
							conflictTimerInfo.duration = (endConflictTimerInMins - startConflictEventInMins);

							if(TAP_Timer_Modify((timerError & 0x0000ffff), &conflictTimerInfo) == 0)
							{
								result = TRUE;
							}
						}
						else
						{
							result = TRUE;
						}
					}
					else 
					{
						// Attempt to share available padding between conflict and new timer

						diffEventInMins = startConflictEventInMins - endNewEventInMins;

						if
						(
							(endNewPaddingInMins > (diffEventInMins / 2))
							&&
							(startConflictPaddingInMins > (diffEventInMins / 2))
						)
						{
							endNewPaddingInMins = (diffEventInMins / 2);

							startConflictPaddingInMins = diffEventInMins - endNewPaddingInMins;
						}
						else if (startConflictPaddingInMins > (diffEventInMins - endNewPaddingInMins))
						{
							startConflictPaddingInMins = diffEventInMins - endNewPaddingInMins;
						}
						else
						{
							endNewPaddingInMins = diffEventInMins - startConflictPaddingInMins;
						}

						schTimerInfo->duration = (endNewEventInMins + endNewPaddingInMins) - startNewTimerInMins;
							
						conflictTimerInfo.startTime = schMainConvertMinsToTime(startConflictEventInMins - startConflictPaddingInMins);
						conflictTimerInfo.duration = (endConflictEventInMins - startConflictEventInMins) + startConflictPaddingInMins + endConflictPaddingInMins;

						if(TAP_Timer_Modify((timerError & 0x0000ffff), &conflictTimerInfo) == 0)
						{
							result = TRUE;
						}
					}
				}
				else
				{
				}
			}
		}
	}

//sprintf(buffer1,"Modified Timer %02d:%02d - %02d:%02d\r\n",((schTimerInfo->startTime >> 8) & 0xFF), (schTimerInfo->startTime & 0xFF), ((((schMainConvertTimeToMins(schTimerInfo->startTime) + schTimerInfo->duration) % (24 * 60)) / 60) & 0xFF), ((((schMainConvertTimeToMins(schTimerInfo->startTime) + schTimerInfo->duration) % (24 * 60)) % 60) & 0xFF));
//TAP_Print(buffer1);

//sprintf(buffer1,"Modified Conflict %02d:%02d - %02d:%02d\r\n",((conflictTimerInfo.startTime >> 8) & 0xFF), (conflictTimerInfo.startTime & 0xFF), ((((schMainConvertTimeToMins(conflictTimerInfo.startTime) + conflictTimerInfo.duration) % (24 * 60)) / 60) & 0xFF), ((((schMainConvertTimeToMins(conflictTimerInfo.startTime) + conflictTimerInfo.duration) % (24 * 60)) % 60) & 0xFF));
//TAP_Print(buffer1);

	return result;
}


void schConflictUpdateMoveList(TYPE_TimerInfo* origConflictTimerInfo, dword modTimerError)
{
	TYPE_TimerInfo	modConflictTimerInfo;
	dword origConflictTimerEndTime = 0, temp = 0;
	int i = 0;
	
	temp = schMainConvertTimeToMins(origConflictTimerInfo->startTime) + origConflictTimerInfo->duration;
	origConflictTimerEndTime = schMainConvertMinsToTime(temp);

	for ( i = 0; i < schMainTotalValidMoves; i++)
	{
		if
		(
			(schMoveData[i].moveEnabled == TRUE)
			&&
			(strcmp(schMoveData[i].moveFileName, origConflictTimerInfo->fileName) == 0)
			&&
			(schMoveData[i].moveStartTime == origConflictTimerInfo->startTime)
			&&
			(schMoveData[i].moveEndTime == origConflictTimerEndTime)
		)
		{
			TAP_Timer_GetInfo( (modTimerError & 0x0000ffff), &modConflictTimerInfo );

			schMoveData[i].moveStartTime = modConflictTimerInfo.startTime;

			temp = schMainConvertTimeToMins(modConflictTimerInfo.startTime) + modConflictTimerInfo.duration;
			schMoveData[i].moveEndTime = schMainConvertMinsToTime(temp);
		}
	}
}

