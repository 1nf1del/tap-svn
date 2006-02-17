/************************************************************
		Part of the ukEPG project
	This module handles to loading, and saving of the configuration

Name	: log.c
Author	: sl8
Version	: 0.0
For	: Topfield TF5x00 series PVRs
Licence	:
Descr.	:
Usage	:
History	: v0.0 sl8:	16-02-06 	Inception date

************************************************************/

#define LOG_FILENAME			"UkAutoLog.txt"
#define LOG_INFO			"UK Auto Scheduler Log File v1.0\r\n"
#define LOG_FILE_BLOCK_SIZE		512
#define LOG_FILE_MAX_LENGTH		(100 * LOG_FILE_BLOCK_SIZE)
#define LOG_EVENT_BUFFER_SIZE		256

static dword logInfoLength = 0;
static word logBufferOffset = 0, logNumberOfEvents = 0;
static bool logFileError = FALSE;

/*****************************************************************************/
/* logInitialise */
/*****************************************************************************/
void logInitialise(void)
{
#if LOG
	char	logBuffer[LOG_FILE_BLOCK_SIZE];
	char	hddBuffer[1];
	byte	i = 0;
	static	bool logExitFL = FALSE;

	TYPE_File*	fp = NULL;

	memset (logBuffer, 0, LOG_FILE_BLOCK_SIZE);

	GotoTapDir();
	TAP_Hdd_ChangeDir( PROJECT_DIRECTORY );

	if (!TAP_Hdd_Exist (LOG_FILENAME))
	{
		TAP_Hdd_Create (LOG_FILENAME, ATTR_NORMAL);

		if (fp = TAP_Hdd_Fopen (LOG_FILENAME))
		{
			for (i = 0; i < LOG_FILE_MAX_LENGTH / LOG_FILE_BLOCK_SIZE; i++)
			{
				TAP_Hdd_Fwrite (logBuffer, 1, LOG_FILE_BLOCK_SIZE, fp);
			}

			TAP_Hdd_Fseek (fp, 0, 0);

			TAP_Hdd_Fwrite (LOG_INFO, 1, sizeof(LOG_INFO), fp);

			logBufferOffset = sizeof(LOG_INFO) - 1;

			logNumberOfEvents = 0;

			TAP_Hdd_Fclose (fp);
		}
	}
	else
	{
		if (fp = TAP_Hdd_Fopen (LOG_FILENAME))
		{
			logBufferOffset = 0;
			logExitFL = FALSE;

			while
			(
				(logExitFL == FALSE)
				&&
				(logBufferOffset < LOG_FILE_MAX_LENGTH)
			)
			{
				TAP_Hdd_Fseek (fp, logBufferOffset, 0);

				TAP_Hdd_Fread (hddBuffer, 1, 1, fp);

				if(hddBuffer[0] != 0)
				{
					logBufferOffset++;

					if(hddBuffer[0] == 0x0A)
					{
						logNumberOfEvents++;
					}
				}
				else
				{
					logExitFL = TRUE;
				}
			}

			logNumberOfEvents -= 2;

			TAP_Hdd_Fclose (fp);

			if (logBufferOffset >= (LOG_FILE_MAX_LENGTH - LOG_EVENT_BUFFER_SIZE))
			{
				logFileError = TRUE;

				return;
			}
		}
	}
#endif
}



/*****************************************************************************/
/* logStoreEvent */
/*****************************************************************************/
void logStoreEvent(char* logEvent)
{
#if LOG
	char	logBuffer[1024];
	word	year = 0;
	byte	month = 0, day = 0, weekday = 0;

	TYPE_File*	fp = NULL;

	if(logFileError == FALSE)
	{
		memset(logBuffer,0,1024);

		TAP_ExtractMjd( schTimeMjd, &year, &month, &day, &weekday);

		sprintf( logBuffer, "%02u:%02u:%02u %02u/%02u/%04u - %s\r\n", schTimeHour, schTimeMin, schTimeSec, day, month, year, logEvent );

		GotoTapDir();
		TAP_Hdd_ChangeDir( PROJECT_DIRECTORY );

		if (fp = TAP_Hdd_Fopen (LOG_FILENAME))
		{
			TAP_Hdd_Fseek (fp, logBufferOffset, 0);

			TAP_Hdd_Fwrite (logBuffer, 1, strlen(logBuffer), fp);

			TAP_Hdd_Fclose (fp);

			logBufferOffset += strlen(logBuffer);

			logNumberOfEvents++;

			if (logBufferOffset >= (LOG_FILE_MAX_LENGTH - LOG_EVENT_BUFFER_SIZE))
			{
				logFileError = TRUE;

				return;
			}
		}
		else
		{
			logFileError = TRUE;

			return;
		}
	}
#endif
}
