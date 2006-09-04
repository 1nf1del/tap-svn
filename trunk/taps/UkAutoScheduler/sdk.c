
void sdkParseXml(char*, int, char*, char*, char*, int, char*);
dword sdkParseTime(char*);
byte sdkConvertCharToNumber(char);
TYPE_TapEvent* TAP_GetEvent_SDK( byte, word, int* );
void TAP_EPG_GetExtInfo_SDK(word, int, byte*);

#define SDK_XML_FILENAME		"radiotimes.xml"
#define SDK_MAX_NUMBER_OF_CHANNELS	5
#define SDK_MAX_NUMBER_OF_EVENTS	150

struct extDataTag
{
	char	extDescription[256];
};

static struct	sdkEpgDataTag
{
	char	name[128];
	int	numbOfEvents;
	TYPE_TapEvent	eventData[SDK_MAX_NUMBER_OF_EVENTS];
	struct	extDataTag extData[SDK_MAX_NUMBER_OF_EVENTS];
}sdkEpgData[SDK_MAX_NUMBER_OF_CHANNELS];

dword	schXmlNumberOfChannels = 0;


bool sdkRetreiveXmlData(void)
{
	TYPE_File	*xmlFile = NULL;
	dword	fileLength = 0;
	dword	schXmlNumberOfProgrammes = 0;
	dword	startTimeInMins = 0, endTimeInMins = 0;
	dword	durationInMins = 0;
	word	startMjd = 0, endMjd = 0;
	byte	startHour = 0, endHour = 0;
	byte	startMin = 0, endMin = 0;
	int	bufferIndex = 0, numberOfTimers = 0;
	int	elementIndex = 0;
	int	i = 0, j = 0;
	char	*buffer = NULL;
	char	tempBuffer[1024];
	char	channelBuffer[128];
	char	buffer1[256];
	bool	found = FALSE;


	// Delete all timers

	numberOfTimers = TAP_Timer_GetTotalNum();

	if(numberOfTimers > 0)
	{
		for(i = 0; (i < numberOfTimers); i++ )
		{
			TAP_Timer_Delete(i);
		}
	}

	GotoProgramFiles();

	if ( TAP_Hdd_Exist( SDK_XML_FILENAME ) == FALSE )
	{
		return FALSE;
	}

	xmlFile = TAP_Hdd_Fopen( SDK_XML_FILENAME );

	if ( xmlFile == NULL ) return FALSE;

	fileLength = TAP_Hdd_Flen( xmlFile );

	buffer = (char *)TAP_MemAlloc( fileLength );

	TAP_Hdd_Fread( buffer, fileLength, 1, xmlFile );

	TAP_Hdd_Fclose( xmlFile );

// Find channel ID's

	memset(sdkEpgData, 0, (sizeof(struct sdkEpgDataTag)));
	
	schXmlNumberOfChannels = 0;
	schXmlNumberOfProgrammes = 0;

	for ( i = 0; i < fileLength; i++)
	{
		if
		(
			(strncmp(&buffer[i],"id=\"", 4) == 0)
			&&
			(schXmlNumberOfChannels < SDK_MAX_NUMBER_OF_CHANNELS)
			&&
			(schXmlNumberOfProgrammes == 0)
		)
		{
			elementIndex = 0;
			memset(sdkEpgData[schXmlNumberOfChannels].name, 0, 128);
			bufferIndex = i + 4;
	
			while
			(
				(buffer[bufferIndex] != '"')
				&&
				(elementIndex < 120)
				&&
				(bufferIndex <= fileLength)
			)
			{
				sdkEpgData[schXmlNumberOfChannels].name[(elementIndex)++] = buffer[(bufferIndex)++];
			}

			schXmlNumberOfChannels++;		
		}
		else if(strncmp(&buffer[i],"<programme", 10) == 0)
		{
			memset(channelBuffer, 0 , sizeof(channelBuffer));
			sdkParseXml(buffer, i, "channel=\"", "\"", channelBuffer, sizeof(channelBuffer), "[Not Available]");

			found = FALSE;
			for(j = 0; ((j < schXmlNumberOfChannels) && (found == FALSE)); j++)
			{
				if(strcmp(sdkEpgData[j].name, channelBuffer) == 0)
				{
					if(sdkEpgData[j].numbOfEvents < SDK_MAX_NUMBER_OF_EVENTS)
					{
						memset(tempBuffer, 0 , sizeof(tempBuffer));
						sdkParseXml(buffer, i, "<title>", "</title>", tempBuffer, 50, "[Not Available]");
						strncpy(sdkEpgData[j].eventData[sdkEpgData[j].numbOfEvents].eventName, tempBuffer, 50);

						memset(tempBuffer, 0 , sizeof(tempBuffer));
						sdkParseXml(buffer, i, "<desc lang=\"en\">", "</desc>", tempBuffer, 127, "[Not Available]");
						strncpy(sdkEpgData[j].eventData[sdkEpgData[j].numbOfEvents].description, tempBuffer, 127);
						strncpy(sdkEpgData[j].extData[sdkEpgData[j].numbOfEvents].extDescription, tempBuffer, 255);

						memset(tempBuffer, 0 , sizeof(tempBuffer));
						sdkParseXml(buffer, i, "start=\"", "\"", tempBuffer, sizeof(tempBuffer), "20000101000000 UTC");
						sdkEpgData[j].eventData[sdkEpgData[j].numbOfEvents].startTime = sdkParseTime(tempBuffer);

						memset(tempBuffer, 0 , sizeof(tempBuffer));
						sdkParseXml(buffer, i, "stop=\"", "\"", tempBuffer, sizeof(tempBuffer), "20000101000000 UTC");
						sdkEpgData[j].eventData[sdkEpgData[j].numbOfEvents].endTime = sdkParseTime(tempBuffer);

						startMjd = (sdkEpgData[j].eventData[sdkEpgData[j].numbOfEvents].startTime & 0xffff0000) >> 16;
						startHour = (sdkEpgData[j].eventData[sdkEpgData[j].numbOfEvents].startTime & 0xff00) >> 8;
						startMin = (sdkEpgData[j].eventData[sdkEpgData[j].numbOfEvents].startTime & 0xff);
						endMjd = (sdkEpgData[j].eventData[sdkEpgData[j].numbOfEvents].endTime & 0xffff0000) >> 16;
						endHour = (sdkEpgData[j].eventData[sdkEpgData[j].numbOfEvents].endTime & 0xff00) >> 8;
						endMin = (sdkEpgData[j].eventData[sdkEpgData[j].numbOfEvents].endTime & 0xff);

						startTimeInMins = (startMjd * 24 * 60) + (startHour * 60) + startMin;
						endTimeInMins = (endMjd * 24 * 60) + (endHour * 60) + endMin;

						durationInMins = endTimeInMins - startTimeInMins;

						sdkEpgData[j].eventData[sdkEpgData[j].numbOfEvents].duration = ((durationInMins / 60) << 8) + (durationInMins % 60);

						sdkEpgData[j].numbOfEvents++;
					}

					found = TRUE;
				}
			}

			schXmlNumberOfProgrammes++;
		}
		else
		{
		}
	}

	TAP_MemFree( buffer );

	return TRUE;
}


void sdkParseXml(char* buffer, int bufferIndex, char* startStr, char* endStr, char* resultBuffer, int resultBufferLength, char* notFoundStr)
{
	int	elementIndex = 0;
	int	fileLength = 0;
	char	buffer1[128];

	fileLength = strlen(buffer);

	memset(resultBuffer, 0, resultBufferLength);

	while
	(
		(strncmp(&buffer[bufferIndex],startStr, strlen(startStr)) != 0)
		&&
		(strncmp(&buffer[bufferIndex],"</programme>", 12) != 0)
		&&
		(bufferIndex < fileLength)
	)
	{
		bufferIndex++;
	}

	if
	(
		(bufferIndex < fileLength)
		&&
		(strncmp(&buffer[bufferIndex],"</programme>", 12) != 0)
	)
	{
		bufferIndex += strlen(startStr);

		elementIndex = 0;
		while
		(
			(strncmp(&buffer[bufferIndex],endStr, strlen(endStr)) != 0)
			&&
			(strncmp(&buffer[bufferIndex],"</programme>", 12) != 0)
			&&
			(elementIndex < resultBufferLength)
			&&
			(bufferIndex <= fileLength)
		)
		{
			resultBuffer[elementIndex++] = buffer[bufferIndex++];
		}

		if
		(
			(strncmp(&buffer[bufferIndex],"</programme>", 12) == 0)
			||
			(bufferIndex >= fileLength)
		)
		{
			memset(resultBuffer, 0, resultBufferLength);
			strcpy(resultBuffer,notFoundStr);
		}	
	}
	else
	{
		strcpy(resultBuffer,notFoundStr);
	}
}


dword sdkParseTime(char* buffer)
{
	word year = 2000;
	byte month = 1, day = 1;
	byte hour = 0, min = 0;

	if(strlen(buffer) >= 12)
	{
		year = sdkConvertCharToNumber(buffer[0]) * 1000;
		year += sdkConvertCharToNumber(buffer[1]) * 100;
		year += sdkConvertCharToNumber(buffer[2]) * 10;
		year += sdkConvertCharToNumber(buffer[3]) * 1;

		month = sdkConvertCharToNumber(buffer[4]) * 10;
		month += sdkConvertCharToNumber(buffer[5]) * 1;

		day = sdkConvertCharToNumber(buffer[6]) * 10;
		day += sdkConvertCharToNumber(buffer[7]) * 1;

		hour = sdkConvertCharToNumber(buffer[8]) * 10;
		hour += sdkConvertCharToNumber(buffer[9]) * 1;

		min = sdkConvertCharToNumber(buffer[10]) * 10;
		min += sdkConvertCharToNumber(buffer[11]) * 1;
	}

	return ((TAP_MakeMjd( year, month, day) << 16) + (hour << 8) + min);
}


byte sdkConvertCharToNumber(char character)
{

	if ((character >= '0') && (character <= '9' ))		// 0 to 9
	{
		return (character - '0');
	}

	return 0;
}



TYPE_TapEvent* TAP_GetEvent_SDK( byte schTvRadio, word schChannel, int* schEpgTotalEvents )
{
	if (schChannel < schXmlNumberOfChannels)
	{
		*schEpgTotalEvents = sdkEpgData[schChannel].numbOfEvents;

		return &(sdkEpgData[schChannel].eventData[0]);
	}
	else
	{
		*schEpgTotalEvents = 0;

		return &(sdkEpgData[0].eventData[0]);

	}
}


void TAP_Channel_GetTotalNum_SDK(int* totalTvSvc, int* totalRadioSvc)
{
	*totalTvSvc = schXmlNumberOfChannels;

	*totalRadioSvc = 0;
}

void TAP_EPG_GetExtInfo_SDK(word channel, int schEpgIndex, byte* schExtInfo)
{
//	strcpy(schExtInfo, sdkEpgData[channel].eventData[schEpgIndex].description);
//	return &(sdkEpgData[channel].eventData[schEpgIndex].description[0]);
}

int TAP_Timer_Add_SDK(TYPE_TimerInfo *newTimerInfo)
{
	int numberOfTimers = 0, i = 0, timerError = 0;
	bool conflict = FALSE;
	TYPE_TimerInfo setTimerInfo;
	TYPE_TimerInfo	copyInfo;
	dword	newTimerStartTimeInMins = 0, newTimerEndTimeInMins = 0;
	dword	setTimerStartTimeInMins = 0, setTimerEndTimeInMins = 0;
	char buffer1[256];


	newTimerStartTimeInMins = (((newTimerInfo->startTime >> 16) & 0xFFFF) * 24 * 60) + (((newTimerInfo->startTime >> 8) & 0xFF) * 60) + (newTimerInfo->startTime & 0xFF);
	newTimerEndTimeInMins = newTimerStartTimeInMins + (((newTimerInfo->duration >> 8) & 0xFF) * 60) + (newTimerInfo->duration & 0xFF);

	numberOfTimers = TAP_Timer_GetTotalNum();

	if(numberOfTimers > 0)
	{
		for(i = 0; ((i < numberOfTimers) && (conflict == FALSE)); i++ )
		{
			if(TAP_Timer_GetInfo(i, &setTimerInfo ))
			{
				setTimerStartTimeInMins = (((setTimerInfo.startTime >> 16) & 0xFFFF) * 24 * 60) + (((setTimerInfo.startTime >> 8) & 0xFF) * 60) + (setTimerInfo.startTime & 0xFF);
				setTimerEndTimeInMins = setTimerStartTimeInMins + (((setTimerInfo.duration >> 8) & 0xFF) * 60) + (setTimerInfo.duration & 0xFF);

				if
				(
					(setTimerInfo.svcNum == newTimerInfo->svcNum)
					&&
					(setTimerInfo.svcType == newTimerInfo->svcType)
					&&
					(
						(
							(newTimerStartTimeInMins > setTimerStartTimeInMins)
							&&
							(newTimerStartTimeInMins < setTimerEndTimeInMins)
						)
						||
						(
							(newTimerEndTimeInMins > setTimerStartTimeInMins)
							&&
							(newTimerEndTimeInMins < setTimerEndTimeInMins)
						)
					)
				)
				{
					conflict = TRUE;

					timerError = 0xFFFF0000 | i;
				}
			}
		}
	}

	if(conflict == FALSE)
	{
		copyInfo.isRec = newTimerInfo->isRec;
		copyInfo.tuner = 3;
		copyInfo.svcType = newTimerInfo->svcType;
		copyInfo.reserved = 0;	
		copyInfo.svcNum = newTimerInfo->svcNum;
		copyInfo.reservationType = 0;
		copyInfo.nameFix = 1;
		copyInfo.duration = newTimerInfo->duration;
		copyInfo.startTime = newTimerInfo->startTime;
		strcpy(copyInfo.fileName, newTimerInfo->fileName);

		timerError = TAP_Timer_Add(&copyInfo);
	}

//sprintf(buffer1,"timerError - %x\r\n",timerError);
//TAP_Print(buffer1);

	return timerError;
}



int TAP_Timer_Modify_SDK(int entryNum, TYPE_TimerInfo *newTimerInfo)
{
	TYPE_TimerInfo setTimerInfo;

	TAP_Timer_GetInfo(entryNum, &setTimerInfo );

	TAP_Timer_Delete(entryNum);

	if(TAP_Timer_Add_SDK(newTimerInfo) != 0)
	{
		TAP_Timer_Add_SDK(&setTimerInfo);
	}
}

