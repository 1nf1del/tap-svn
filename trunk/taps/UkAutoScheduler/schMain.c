/************************************************************
Part of the ukEPG project
This module performs the search

v0.0 sl8:	20-11-05	Inception date
v0.1 sl8:	20-01-06	All variables initialised
v0.2 sl8:	06-02-06	Moved schInitLcnToSvcNumMap to ukauto.c
v0.3 sl8:	16-02-06	Bug fix - Searches were not performed correctly on current day. 30 minute window incorrectly calculated.
				Modified for 'Perform Search' config option
				Delay searches for 2 minutes when TAP starts.
				Removed - 30 minute window where timers would not set.
				Added - Timers can be set up to within 2 minutes of current time (start padding permitting)
v0.4 sl8:	09-03-06	Destination folder and remote search file mods
v0.5 sl8:	22-03-06	Bug fix - Move failed if programme spanned midnight

**************************************************************/

#define SCH_MAIN_DELAY_SEARCH_ALARM		120	// Search not allowed within 2 Minutes of TAP starting
#define SCH_MAX_FILE_LENGTH			30	// Max length of file name in archive - 4 (.rec)

bool schCompareStrings(char *, char *);
bool schPerformSearch(TYPE_TapEvent *, int, int);
void schSetTimer(TYPE_TapEvent *, int, int, word);
void schService(void);
void schInitLcnToSvcNumMap(void);
void schMainUpdateSearchList(void);

static struct schDataTag schUserData[SCH_MAX_SEARCHES];
static struct schDataTag schRemoteData[SCH_MAX_SEARCHES];

static struct schMoveTag schMoveData[SCH_MAX_MOVES];

static int schTotalTvSvc = 0;
static int schTotalRadioSvc = 0;
static byte schMainTotalValidSearches = 0;
static byte schMainTotalValidRemoteSearches = 0;
static byte schMainPerformSearchMode = 0;
static int schMainPerformSearchTime = 0;
static byte schMainTotalValidMoves = 0;

word* schLcnToServiceTv = NULL;
word* schLcnToServiceRadio = NULL;

byte* schEpgDataExtendedInfo = NULL;
TYPE_TapEvent* schEpgData = NULL;

void schService(void)
{
	static word schChannel = 0;
	static byte schUserSearchIndex = 0;
	static dword schEpgIndex = 0;
	static int schEpgTotalEvents = 0;

	char buffer1[128];

	switch(schServiceSV)
	{
	/*--------------------------------------------------*/
	case SCH_SERVICE_INITIALISE:

		if(schInitRetreiveData() > 0)
		{
//			sprintf(buffer1, "%d Valid Searches",schMainTotalValidSearches);
//			ShowMessageWin(buffer1,"Loaded");
		}
		else
		{
//			ShowMessageWin("Error loading","SearchList.txt");
//			TAP_Exit();
		}

		schServiceSV = SCH_SERVICE_RESET_SEARCH;

		break;
	/*--------------------------------------------------*/
	case SCH_SERVICE_RESET_SEARCH:

		schServiceSV = SCH_SERVICE_WAIT_TO_SEARCH;

		break;
	/*--------------------------------------------------*/
	case SCH_SERVICE_WAIT_TO_SEARCH:

		if(schMainTotalValidSearches > 0)
		{
			switch(schMainPerformSearchMode)
			{
			/*--------------------------------------------------*/
			case SCH_CONFIG_SEARCH_PERIOD_ONE_HOUR:

				if(schTimeMin == 0)
				{
					schServiceSV = SCH_SERVICE_CHECK_FOR_REMOTE_SEARCHES;
				}

				break;
			/*--------------------------------------------------*/
			case SCH_CONFIG_SEARCH_PERIOD_SPECIFIED:

				if
				(
					(schTimeHour == ((schMainPerformSearchTime >> 8) & 0xFF))
					&&
					(schTimeMin == (schMainPerformSearchTime & 0xFF))
				)
				{
					schServiceSV = SCH_SERVICE_CHECK_FOR_REMOTE_SEARCHES;
				}

				break;
			/*--------------------------------------------------*/
			case SCH_CONFIG_SEARCH_PERIOD_TEN_MINS:
			default:

				if((schTimeMin % 10) == 0)
				{
					schServiceSV = SCH_SERVICE_CHECK_FOR_REMOTE_SEARCHES;
				}

				break;
			/*--------------------------------------------------*/
			}
		}

		break;
	/*--------------------------------------------------*/
	case SCH_SERVICE_CHECK_FOR_REMOTE_SEARCHES:

		if(schInitRetreiveRemoteData() > 0)
		{
			schServiceSV = SCH_SERVICE_UPDATE_SEARCH_LIST;
		}
		else
		{
			schServiceSV = SCH_SERVICE_BEGIN_SEARCH;
		}

		break;
	/*--------------------------------------------------*/
	case SCH_SERVICE_UPDATE_SEARCH_LIST:

		schMainUpdateSearchList();

		schServiceSV = SCH_SERVICE_BEGIN_SEARCH;


		break;	
	/*-------------------------------------------------*/
	case SCH_SERVICE_BEGIN_SEARCH:

		if(schStartUpCounter > SCH_MAIN_DELAY_SEARCH_ALARM)
		{
//			logStoreEvent("Performing Search");

			schUserSearchIndex = 0;		
			schEpgIndex = 0;

			if((schUserData[schUserSearchIndex].searchOptions & SCH_USER_DATA_OPTIONS_ANY_CHANNEL) == SCH_USER_DATA_OPTIONS_ANY_CHANNEL)
			{
				schChannel = 0;
			}
			else
			{
				schChannel = schUserData[schUserSearchIndex].searchStartSvcNum;
			}

			if(schUserData[schUserSearchIndex].searchStatus != SCH_USER_DATA_STATUS_DISABLED)
			{
				schServiceSV = SCH_SERVICE_INITIALISE_EPG_DATA;
			}
			else
			{
				schServiceSV = SCH_SERVICE_NEXT_USER_SEARCH;
			}
		}

		break;
	/*--------------------------------------------------*/
	case SCH_SERVICE_INITIALISE_EPG_DATA:

		if( schEpgData )
		{
			TAP_MemFree( schEpgData );
			schEpgData = 0;
		}
		
		schEpgData = TAP_GetEvent( schUserData[schUserSearchIndex].searchTvRadio, schChannel, &schEpgTotalEvents );

		schServiceSV = SCH_SERVICE_PERFORM_SEARCH;
		
		break;	
	/*--------------------------------------------------*/
	case SCH_SERVICE_PERFORM_SEARCH:

		if
		(
			(schEpgData)
			&&
			(schEpgTotalEvents > 0)
			&&
			(schEpgIndex < schEpgTotalEvents)
		)
		{
			if((schPerformSearch(schEpgData, schEpgIndex, schUserSearchIndex)) == TRUE)
			{
				schSetTimer(schEpgData, schEpgIndex, schUserSearchIndex, schChannel);
			}

			schEpgIndex++;
		}
		else
		{
			schServiceSV = SCH_SERVICE_NEXT_CHANNEL;
		}

		break;
	/*--------------------------------------------------*/
	case SCH_SERVICE_NEXT_CHANNEL:

		schChannel++;
		schEpgIndex = 0;

		if((schUserData[schUserSearchIndex].searchOptions & SCH_USER_DATA_OPTIONS_ANY_CHANNEL) == SCH_USER_DATA_OPTIONS_ANY_CHANNEL)
		{
			if
			(
				(
					(schChannel >= schTotalTvSvc)
					&&
					(schUserData[schUserSearchIndex].searchTvRadio == SCH_TV)
				)
				||
				(
					(schChannel >= schTotalRadioSvc)
					&&
					(schUserData[schUserSearchIndex].searchTvRadio == SCH_RADIO)
				)
			)
			{
				schServiceSV = SCH_SERVICE_NEXT_USER_SEARCH;
			}
			else
			{
				schServiceSV = SCH_SERVICE_INITIALISE_EPG_DATA;
			}
		}
		else
		{
			if(schChannel > schUserData[schUserSearchIndex].searchEndSvcNum)
			{
				schServiceSV = SCH_SERVICE_NEXT_USER_SEARCH;
			}
			else
			{
				schServiceSV = SCH_SERVICE_INITIALISE_EPG_DATA;
			}
		}

		break;	
	/*--------------------------------------------------*/
	case SCH_SERVICE_NEXT_USER_SEARCH:

		schUserSearchIndex++;
		schEpgIndex = 0;

		if(schUserSearchIndex >= schMainTotalValidSearches)
		{
			schServiceSV = SCH_SERVICE_COMPLETE;
		}
		else if(schUserData[schUserSearchIndex].searchStatus != SCH_USER_DATA_STATUS_DISABLED)
		{
			if((schUserData[schUserSearchIndex].searchOptions & SCH_USER_DATA_OPTIONS_ANY_CHANNEL) == SCH_USER_DATA_OPTIONS_ANY_CHANNEL)
			{
				schChannel = 0;
			}
			else
			{
				schChannel = schUserData[schUserSearchIndex].searchStartSvcNum;
			}

			schServiceSV = SCH_SERVICE_INITIALISE_EPG_DATA;
		}
		else
		{
			schServiceSV = SCH_SERVICE_NEXT_USER_SEARCH;
		}

		break;
	/*--------------------------------------------------*/
	case SCH_SERVICE_COMPLETE:

		switch(schMainPerformSearchMode)
		{
		/*--------------------------------------------------*/
		case SCH_CONFIG_SEARCH_PERIOD_ONE_HOUR:

			if(schTimeMin != 0)
			{
//				logStoreEvent("Search Finished");

				schServiceSV = SCH_SERVICE_RESET_SEARCH;
			}

			break;
		/*--------------------------------------------------*/
		case SCH_CONFIG_SEARCH_PERIOD_SPECIFIED:

			if(schTimeMin != (schMainPerformSearchTime & 0xFF))
			{
//				logStoreEvent("Search Finished");

				schServiceSV = SCH_SERVICE_RESET_SEARCH;
			}

			break;
		/*--------------------------------------------------*/
		case SCH_CONFIG_SEARCH_PERIOD_TEN_MINS:
		default:

			if((schTimeMin % 10) != 0)
			{
//				logStoreEvent("Search Finished");

				schServiceSV = SCH_SERVICE_RESET_SEARCH;
			}

			break;
		/*--------------------------------------------------*/
		}

		break;
	/*--------------------------------------------------*/
	}
}


bool schPerformSearch(TYPE_TapEvent *epgData, int epgDataIndex, int schSearch)
{
	int eventNameIndex = 0;
	bool foundSearchTerm = FALSE;
	word eventMjd = 0, eventYear = 0;
	byte eventHour = 0, eventMin = 0;
	byte eventMonth = 0, eventDay = 0, eventWeekDay = 0, eventWeekDayBit = 0;
	dword eventStartInMins = 0, currentTimeInMins = 0, startPaddingInMins = 0;

	eventMjd = ((epgData[epgDataIndex].startTime >> 16) & 0xFFFF);
	eventHour = ((epgData[epgDataIndex].startTime >> 8) & 0xFF);
	eventMin = (epgData[epgDataIndex].startTime & 0xFF);

	TAP_ExtractMjd( eventMjd, &eventYear, &eventMonth, &eventDay, &eventWeekDay);
	eventWeekDayBit = 0x01 << eventWeekDay;

	eventStartInMins = (eventMjd * 24 * 60) + (eventHour * 60) + eventMin;
	currentTimeInMins = (schTimeMjd * 24 * 60) + (schTimeHour * 60) + schTimeMin;
	startPaddingInMins = (((schUserData[schSearch].searchStartPadding & 0xff00) >> 8) * 60) + (schUserData[schSearch].searchStartPadding & 0xff);

	if
	(
		(
			(eventStartInMins > (startPaddingInMins + 2))
			&&
			((eventStartInMins - (startPaddingInMins + 2)) > currentTimeInMins)
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
	dword schEndTimeWithPadding = 0;
	word schStartTimeInMins = 0;
	word schEndTimeInMins = 0;
	word mjd = 0,year = 0;
	byte month = 0, day = 0, weekDay = 0, weekDayBit = 0;
	byte hour = 0, min = 0;
	byte attachPosition[2];
	byte attachType[2];
	char prefixStr[64];
	char appendStr[64];
	char dateStr[64];
	char numbStr[64];
	char fileNameStr[132];
	char logBuffer[LOG_BUFFER_SIZE];
	int fileNameLen = 0;
	int timerError = 0;
	int i = 0;
	bool longName = FALSE;

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

	memset(prefixStr,0,64);
	memset(appendStr,0,64);

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
					strcat(prefixStr,numbStr);
					strcat(prefixStr,"_");
				}

				break;
			/* ---------------------------------------------------------------------------- */
			case SCH_ATTACH_TYPE_DATE:

				if((strlen(epgData[epgDataIndex].eventName) + (strlen(dateStr)+1) + strlen(fileNameStr)) < (128 - 6))
				{
					strcat(prefixStr,dateStr);
					strcat(prefixStr,"_");
				}

				break;
			/* ---------------------------------------------------------------------------- */
			default:
				break;
			/* ---------------------------------------------------------------------------- */
			}
		}	
	}

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
					strcat(appendStr,"_");
					strcat(appendStr,numbStr);
				}

				break;
			/* ---------------------------------------------------------------------------- */
			case SCH_ATTACH_TYPE_DATE:

				if((strlen(epgData[epgDataIndex].eventName) + (strlen(dateStr)+1) + strlen(fileNameStr)) < (128 - 6))
				{
					strcat(appendStr,"_");
					strcat(appendStr,dateStr);
				}

				break;
			/* ---------------------------------------------------------------------------- */
			default:
				break;
			/* ---------------------------------------------------------------------------- */
			}
		}	
	}

	strcat(fileNameStr,prefixStr);

	longName = FALSE;
	if((strlen(prefixStr) + strlen(epgData[epgDataIndex].eventName) + strlen(appendStr)) > SCH_MAX_FILE_LENGTH)
	{
		strncat(fileNameStr,epgData[epgDataIndex].eventName, (SCH_MAX_FILE_LENGTH - strlen(prefixStr) - strlen(appendStr)));

		longName = TRUE;
	}
	else
	{
		strcat(fileNameStr,epgData[epgDataIndex].eventName);
	}

	strcat(fileNameStr,appendStr);

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

	mjd = ((epgData[epgDataIndex].endTime >> 16) & 0xFFFF);
	hour = ((epgData[epgDataIndex].endTime >> 8) & 0xFF);
	min = (epgData[epgDataIndex].endTime & 0xFF);

	if((min + schUserData[searchIndex].searchEndPadding) < 60)
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
			mjd++;
		}
	}

	schEndTimeInMins = (hour * 60) + min;

	if(schEndTimeInMins < schStartTimeInMins)
	{
		schEndTimeInMins += (24 * 60);
	}

	schEndTimeWithPadding = (mjd << 16) + (hour << 8) + min;

	// -------------------------------------------------------

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
		if
		(
			(strlen(schUserData[searchIndex].searchFolder) > 0)
			&&
			(schMainTotalValidMoves < SCH_MAX_MOVES)
		)
		{
			schMoveData[schMainTotalValidMoves].moveEnabled = TRUE;
			strcpy(schMoveData[schMainTotalValidMoves].moveFileName, fileNameStr);
			strcpy(schMoveData[schMainTotalValidMoves].moveFolder, schUserData[searchIndex].searchFolder);
			schMoveData[schMainTotalValidMoves].moveStartTime = schStartTimeWithPadding;
			schMoveData[schMainTotalValidMoves].moveEndTime = schEndTimeWithPadding;
			schMoveData[schMainTotalValidMoves].moveFailedCount = 0;

			schMainTotalValidMoves++;

			schWriteMoveList();
/*
			if(longName == TRUE)
			{
				memset(logBuffer,0,LOG_BUFFER_SIZE);
				sprintf( logBuffer, "Timer Set with Move (L): %s   %s", fileNameStr, epgData[epgDataIndex].eventName );
				logStoreEvent(logBuffer);
			}
			else
			{
				memset(logBuffer,0,LOG_BUFFER_SIZE);
				sprintf( logBuffer, "Timer Set with Move: %s", fileNameStr );
				logStoreEvent(logBuffer);
			}
*/
		}
		else
		{
/*			if(longName == TRUE)
			{
				memset(logBuffer,0,LOG_BUFFER_SIZE);
				sprintf( logBuffer, "Timer Set without Move (L): %s   %s", fileNameStr,  epgData[epgDataIndex].eventName );
				logStoreEvent(logBuffer);		
			}
			else
			{
				memset(logBuffer,0,LOG_BUFFER_SIZE);
				sprintf( logBuffer, "Timer Set without Move: %s", fileNameStr );
				logStoreEvent(logBuffer);
			}
*/
		}
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


void schMainUpdateSearchList(void)
{
	byte i = 0;

	for(i = 0; i < schMainTotalValidRemoteSearches; i++)
	{
		if(schMainTotalValidSearches < SCH_MAX_SEARCHES)
		{
			schUserData[schMainTotalValidSearches] = schRemoteData[i];

			schMainTotalValidSearches++;
		}
	}

	schWriteSearchList();

	GotoProgramFiles();

	TAP_Hdd_Delete( REMOTE_FILENAME );
}
