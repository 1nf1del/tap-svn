void schMoveService(void);


static byte schMoveServiceSV = SCH_MOVE_SERVICE_INITIALISE;


void schService(void)
{
word mjd;
byte hour,min,sec;
char buffer1[128];

	switch(schMoveServiceSV)
	{
	/*--------------------------------------------------*/
	case SCH_MOVE_SERVICE_INITIALISE:

		schServiceSV = SCH_MOVE_SERVICE_WAIT_TO_CHECK;

		break;
	/*--------------------------------------------------*/
	case SCH_MOVE_SERVICE_WAIT_TO_CHECK:

		TAP_GetTime( &mjd, &hour, &min, &sec);

		if(sec == 40)
		{
			schMoveIndex = 0;

			schMoveCurrentTime = (mjd * 16) + (hour * 8) + min;

			schMoveServiceSV = SCH_MOVE_SERVICE_CHECK_MOVE;
		}

		break;
	/*--------------------------------------------------*/
	case SCH_SERVICE_CHECK_MOVE:

		if(schMoveCurrentTime >= schMoveData[schMoveIndex].endTime)
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

int   totalCnt, i;
TYPE_File tempFile;
     
     *files = 0;
     *subfolders = 0;
     
//     TAP_Hdd_ChangeDir(NewDir);  // change to the directory that is to be counted.

		totalCnt = TAP_Hdd_FindFirst(&tempFile); 
    
		for ( i=1; i <= totalCnt ; i += 1 )
		{
			if
			(
				(IsFileRec(tempFile.name, tempFile.attr))
				&&



			TAP_Hdd_FindNext(&tempFile); 
		}

		break;
	/*--------------------------------------------------*/
	case SCH_MOVE_SERVICE_NEXT_MOVE:

		schMoveIndex++;

		if(schMoveIndex >= schMoveTotal)
		{
			schMoveServiceSV = SCH_MOVE_SERVICE_COMPLETE;
		}
		else
		{
			schMoveServiceSV = SCH_MOVE_SERVICE_CHECK_MOVE;
		}

		break;
	/*--------------------------------------------------*/
	case SCH_MOVE_SERVICE_COMPLETE:
	
		TAP_GetTime( &mjd, &hour, &min, &sec);

		if(sec != 40)
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


bool schPerformSearch(TYPE_TapEvent *epgData, int epgDataIndex, int schSearch)
{
	int eventNameIndex = 0;
	bool foundSearchTerm = FALSE;
	word eventMjd,eventYear = 0;
	byte eventHour,eventMin;
	byte eventMonth,eventDay,eventWeekDay, eventWeekDayBit = 0;
	word currentMjd;
	byte currentHour,currentMin,currentSec;
	dword eventStartInMins,currentTimeInMins;

	eventMjd = ((epgData[epgDataIndex].startTime >> 16) & 0xFFFF);
	eventHour = ((epgData[epgDataIndex].startTime >> 8) & 0xFF);
	eventMin = (epgData[epgDataIndex].startTime & 0xFF);
	eventStartInMins = (eventHour * 24) + eventMin;

	TAP_ExtractMjd( eventMjd, &eventYear, &eventMonth, &eventDay, &eventWeekDay);
	eventWeekDayBit = 0x01 << eventWeekDay;

	TAP_GetTime( &currentMjd, &currentHour, &currentMin, &currentSec);
	currentTimeInMins = (currentHour * 24) + currentMin;

if
(
	(epgDataIndex < 10)
	&&
	(
		(schUserData[schSearch].searchStartTime == schUserData[schSearch].searchEndTime)
		||
		(
			(schUserData[schSearch].searchStartTime < schUserData[schSearch].searchEndTime)
			&&
			((epgData[epgDataIndex].startTime & 0xFFFF) >= schUserData[schSearch].searchStartTime)
			&&
			((epgData[epgDataIndex].startTime & 0xFFFF) <= schUserData[schSearch].searchEndTime)
		)
		||
		(
			(schUserData[schSearch].searchStartTime > schUserData[schSearch].searchEndTime)
			&&
			(
				((epgData[epgDataIndex].startTime & 0xFFFF) >= schUserData[schSearch].searchStartTime)
				||
				((epgData[epgDataIndex].startTime & 0xFFFF) <= schUserData[schSearch].searchEndTime)
			)
		)
	)
)

{
//TAP_Print("Searching %s\r\n", epgData[epgDataIndex].eventName);
}

	if
	(
		(
			(eventMjd > currentMjd)
			||
			(
				(eventStartInMins > currentTimeInMins)
				&&
				((eventStartInMins - currentTimeInMins) > 30)
				&&
				(eventMjd == currentMjd)
			)
		)
		&&
		(
			(schUserData[schSearch].searchStartTime == schUserData[schSearch].searchEndTime)
			||
			(
				(schUserData[schSearch].searchStartTime < schUserData[schSearch].searchEndTime)
				&&
				((epgData[epgDataIndex].startTime & 0xFFFF) >= schUserData[schSearch].searchStartTime)
				&&
				((epgData[epgDataIndex].startTime & 0xFFFF) <= schUserData[schSearch].searchEndTime)
			)
			||
			(
				(schUserData[schSearch].searchStartTime > schUserData[schSearch].searchEndTime)
				&&
				(
					((epgData[epgDataIndex].startTime & 0xFFFF) >= schUserData[schSearch].searchStartTime)
					||
					((epgData[epgDataIndex].startTime & 0xFFFF) <= schUserData[schSearch].searchEndTime)
				)
			)
		)
		&&
		((schUserData[schSearch].searchDay & eventWeekDayBit) == eventWeekDayBit)
	)
	{
if
(
	(epgDataIndex < 10)
)

{
//TAP_Print("Searching %s\r\n", epgData[epgDataIndex].eventName);
}

		
		
		if
		(
			((schUserData[schSearch].searchOptions & SCH_USER_DATA_OPTIONS_EVENTNAME) == SCH_USER_DATA_OPTIONS_EVENTNAME)
			&&
			(
				((schUserData[schSearch].searchOptions & SCH_USER_DATA_OPTIONS_EXACT_MATCH) != SCH_USER_DATA_OPTIONS_EXACT_MATCH)
				||
				(strlen(epgData[epgDataIndex].eventName) == strlen(schUserData[schSearch].searchTerm))
			)
		)
		{
			foundSearchTerm = schCompareStrings(epgData[epgDataIndex].eventName, schUserData[schSearch].searchTerm);
		}

if(foundSearchTerm == TRUE)
{
//TAP_Print("Found %s\r\n", epgData[epgDataIndex].eventName);
}
		if
		(
			(foundSearchTerm == FALSE)
			&&
			((schUserData[schSearch].searchOptions & SCH_USER_DATA_OPTIONS_DESCRIPTION) == SCH_USER_DATA_OPTIONS_DESCRIPTION)
		)
		{
			foundSearchTerm = schCompareStrings(epgData[epgDataIndex].description, schUserData[schSearch].searchTerm);
		}

		if
		(
			(foundSearchTerm == FALSE)
			&&
			((schUserData[schSearch].searchOptions & SCH_USER_DATA_OPTIONS_EXT_INFO) == SCH_USER_DATA_OPTIONS_EXT_INFO)
		)
		{
			if( schEpgDataExtendedInfo )
			{
				TAP_MemFree( schEpgDataExtendedInfo );
				schEpgDataExtendedInfo = 0;
			}
			schEpgDataExtendedInfo = TAP_EPG_GetExtInfo(&epgData[epgDataIndex]);

			if(schEpgDataExtendedInfo)
			{
				foundSearchTerm = schCompareStrings((char*)schEpgDataExtendedInfo, schUserData[schSearch].searchTerm);
			}
		}
	}

	return foundSearchTerm;
}



void schSetTimer(TYPE_TapEvent *epgData, int epgDataIndex, int searchIndex, word svcNum)
{
TYPE_TimerInfo schTimerInfo;
dword schStartTimeWithPadding = 0;
word schStartTimeInMins = 0;
word schEndTimeInMins = 0;
word mjd,year = 0;
byte month,day,weekDay, weekDayBit = 0;
byte hour,min;
byte attachPosition[2];
byte attachType[2];
char dateStr[64];
char numbStr[64];
char fileNameStr[132];
int fileNameLen;
int timerError;
int i;

	// ------------- Attachments ----------------

	memset(fileNameStr,0,132);

	attachPosition[0] = (schUserData[searchIndex].searchAttach >> 6) & 0x03;
	attachType[0] = schUserData[searchIndex].searchAttach & 0x3F;
	attachPosition[1] = (schUserData[searchIndex].searchAttach >> 14) & 0x03;
	attachType[1] = (schUserData[searchIndex].searchAttach >> 8) & 0x3F;

	fileNameLen = strlen(epgData[epgDataIndex].eventName);

	mjd = ((epgData[epgDataIndex].startTime >> 16) & 0xFFFF);

	TAP_ExtractMjd( mjd, &year, &month, &day, &weekDay);

	memset(dateStr,0,64);
	sprintf(dateStr,"%02d.%02d.%02d", day, month, (year % 10));

	memset(numbStr,0,64);
	sprintf(numbStr,"S#%d", (searchIndex + 1));

	for(i = 0; i < 2; i++)
	{
		if(attachPosition[i] == SCH_ATTACH_POS_PREFIX)
		{
			switch(attachType[i])
			{
			/* ---------------------------------------------------------------------------- */
			case SCH_ATTACH_TYPE_NUMBER:

				if((strlen(epgData[epgDataIndex].eventName) + (strlen(numbStr)+1) + strlen(fileNameStr)) < (128 - 6))
				{
					strcat(fileNameStr,numbStr);
					strcat(fileNameStr,"_");
				}

				break;
			/* ---------------------------------------------------------------------------- */
			case SCH_ATTACH_TYPE_DATE:

				if((strlen(epgData[epgDataIndex].eventName) + (strlen(dateStr)+1) + strlen(fileNameStr)) < (128 - 6))
				{
					strcat(fileNameStr,dateStr);
					strcat(fileNameStr,"_");
				}

				break;
			/* ---------------------------------------------------------------------------- */
			default:
				break;
			/* ---------------------------------------------------------------------------- */
			}
		}	
	}

	strcat(fileNameStr,epgData[epgDataIndex].eventName);

	for(i = 0; i < 2; i++)
	{
		if(attachPosition[i] == SCH_ATTACH_POS_APPEND)
		{
			switch(attachType[i])
			{
			/* ---------------------------------------------------------------------------- */
			case SCH_ATTACH_TYPE_NUMBER:

				if((strlen(epgData[epgDataIndex].eventName) + (strlen(numbStr)+1) + strlen(fileNameStr)) < (128 - 6))
				{
					strcat(fileNameStr,"_");
					strcat(fileNameStr,numbStr);
				}

				break;
			/* ---------------------------------------------------------------------------- */
			case SCH_ATTACH_TYPE_DATE:

				if((strlen(epgData[epgDataIndex].eventName) + (strlen(dateStr)+1) + strlen(fileNameStr)) < (128 - 6))
				{
					strcat(fileNameStr,"_");
					strcat(fileNameStr,dateStr);
				}

				break;
			/* ---------------------------------------------------------------------------- */
			default:
				break;
			/* ---------------------------------------------------------------------------- */
			}
		}	
	}

	strcat(fileNameStr,".rec");

	// ------------- Add padding to start time ----------------

	hour = ((epgData[epgDataIndex].startTime >> 8) & 0xFF);
	min = (epgData[epgDataIndex].startTime & 0xFF);

	if(min >= schUserData[searchIndex].searchStartPadding)
	{
		min -= schUserData[searchIndex].searchStartPadding;
	}
	else
	{
		min = (min + 60) - schUserData[searchIndex].searchStartPadding;

		if(hour > 0)
		{
			hour -= 1;
		}
		else
		{
			hour = 23;
			mjd--;
		}
	}

	schStartTimeWithPadding = (mjd << 16) + (hour << 8) + min;
	schStartTimeInMins = (hour * 60) + min;

	// ------------- Add padding to end time ----------------

	hour = ((epgData[epgDataIndex].endTime >> 8) & 0xFF);
	min = (epgData[epgDataIndex].endTime & 0xFF);

	if(min < (60 - schUserData[searchIndex].searchEndPadding))
	{
		min += schUserData[searchIndex].searchEndPadding;
	}
	else
	{
		min = (min + schUserData[searchIndex].searchEndPadding) - 60;

		if(hour < 23)
		{
			hour++;
		}
		else
		{
			hour = 0;
		}
	}

	schEndTimeInMins = (hour * 60) + min;

	// -------------------------------------------------------

	if(schEndTimeInMins < schStartTimeInMins)
	{
		schEndTimeInMins += (24 * 60);
	}

	if(schUserData[searchIndex].searchStatus == SCH_USER_DATA_STATUS_RECORD)
	{
		schTimerInfo.isRec = 1;						// Needs to be R or P
	}
	else
	{
		schTimerInfo.isRec = 0;
	}
	schTimerInfo.tuner = 3;							// Next available tuner
	schTimerInfo.svcType = schUserData[searchIndex].searchTvRadio;		// 0 TV, 1 Radio
	schTimerInfo.reserved = 0;	
	schTimerInfo.svcNum = svcNum;						// Channel number
	schTimerInfo.reservationType = 0;					// ?
	schTimerInfo.nameFix = 1;						// ?
	schTimerInfo.duration = (schEndTimeInMins - schStartTimeInMins);
	schTimerInfo.startTime = schStartTimeWithPadding;
	strcpy(schTimerInfo.fileName, fileNameStr);

	timerError = TAP_Timer_Add(&schTimerInfo);
	if( timerError == 0)
	{
//		TAP_Print(fileNameStr);
//		TAP_Print("\r\n");

//		ShowMessageWin("Timer Set:",schTimerInfo.fileName);
	}
	else
	{
//		sprintf(buffer1,"Timer Failed - Error: %x\r\n\r\n",timerError);
//		TAP_Print(buffer1);
	}
}





bool schCompareStrings(char *eventName, char *searchTerm)
{
int eventNameIndex = 0;
bool foundSearchTerm = FALSE;
int eventNameLength = 0;
int searchTermLength = 0;

	eventNameLength = strlen(eventName);
	searchTermLength = strlen(searchTerm);

	if
	(
		(eventNameLength > 0)
		&&
		(searchTermLength > 0)
	)
	{
		if(strlen(eventName) >= strlen(searchTerm))
		{
			for( eventNameIndex=0; ((eventNameIndex <= (eventNameLength - searchTermLength)) && (foundSearchTerm == FALSE)); eventNameIndex++ )
			{
//strncasecmp
//strncmp
				if(strncasecmp(&eventName[eventNameIndex],searchTerm,searchTermLength) == 0)
				{
					foundSearchTerm = TRUE;
				}
			}
		}
	}

	return foundSearchTerm;
}


void schInitLcnToSvcNumMap(void)
{
int i = 0;
TYPE_TapChInfo chInfo;

	TAP_Channel_GetTotalNum( &schTotalTvSvc, &schTotalRadioSvc );

	schLcnToServiceTv = (word*)TAP_MemAlloc(schTotalTvSvc * 2);
	for (i = 0; i < schTotalTvSvc; i++)
	{
		TAP_Channel_GetInfo(SCH_TV,i,&chInfo);

		schLcnToServiceTv[i] = chInfo.logicalChNum; 
	}

	schLcnToServiceRadio = (word*)TAP_MemAlloc(schTotalRadioSvc * 2);
	for (i = 0; i < schTotalRadioSvc; i++)
	{
		TAP_Channel_GetInfo(SCH_RADIO,i,&chInfo);

		schLcnToServiceRadio[i] = chInfo.logicalChNum; 
	}
}
