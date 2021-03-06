/************************************************************
		Part of the ukEPG project
	This module handles to loading, and saving of the configuration

Name	: log.c
Author	: sl8
Version	: 0.2
For	: Topfield TF5x00 series PVRs
Licence	:
Descr.	:
Usage	:
History	: v0.0 sl8:	16-02-06 	Inception date
	  v0.1 sl8:	09-03-06	Function added to log archive
	  v0.2 sl8:	11-04-06	Improvements to file handling
	  v0.3 sl8:	15-12-06	Settings/UkAuto folder


************************************************************/

#define LOG_FILENAME			"UkAutoLog.txt"
#define LOG_INFO			"UK Auto Scheduler Log File v1.0\r\n"
#define LOG_FILE_BLOCK_SIZE		512
#define LOG_FILE_MAX_LENGTH		(100 * LOG_FILE_BLOCK_SIZE)
#define LOG_EVENT_BUFFER_SIZE		256

static dword logInfoLength = 0, logBufferOffset = 0;
static bool logFileError = FALSE;

/*****************************************************************************/
/* logInitialise */
/*****************************************************************************/
void logInitialise(void)
{
#if LOG
	char	logBuffer[LOG_FILE_BLOCK_SIZE];
	char	*hddBuffer = NULL;
	byte	i = 0;
	static	bool logExitFL = FALSE;
	dword	fileLength = 0;

	TYPE_File*	fp = NULL;

	memset (logBuffer, 0, LOG_FILE_BLOCK_SIZE);

	GotoPath(SETTINGS_FOLDER);

	if (!TAP_Hdd_Exist (LOG_FILENAME))
	{
		TAP_Hdd_Create (LOG_FILENAME, ATTR_NORMAL);

		if (fp = TAP_Hdd_Fopen (LOG_FILENAME))
		{
			for (i = 0; i < LOG_FILE_MAX_LENGTH / LOG_FILE_BLOCK_SIZE; i++)
			{
				TAP_Hdd_Fwrite (logBuffer, LOG_FILE_BLOCK_SIZE, 1, fp);
			}

			TAP_Hdd_Fseek (fp, 0, 0);

			TAP_Hdd_Fwrite (LOG_INFO, sizeof(LOG_INFO), 1, fp);

			logBufferOffset = sizeof(LOG_INFO) - 1;

			TAP_Hdd_Fclose (fp);
		}
	}
	else
	{
		if (fp = TAP_Hdd_Fopen (LOG_FILENAME))
		{
			hddBuffer = (char *)TAP_MemAlloc( LOG_FILE_MAX_LENGTH );

			fileLength = TAP_Hdd_Flen( fp );
			if ( fileLength > LOG_FILE_MAX_LENGTH ) fileLength = LOG_FILE_MAX_LENGTH;

			TAP_Hdd_Fread (hddBuffer, fileLength, 1, fp);

			logBufferOffset = strlen(hddBuffer);

			TAP_Hdd_Fclose (fp);

			TAP_MemFree(hddBuffer);


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

		GotoPath(SETTINGS_FOLDER);

		if (fp = TAP_Hdd_Fopen (LOG_FILENAME))
		{
			TAP_Hdd_Fseek (fp, logBufferOffset, 0);

			TAP_Hdd_Fwrite (logBuffer, strlen(logBuffer), 1, fp);

			TAP_Hdd_Fclose (fp);

			logBufferOffset += strlen(logBuffer);

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




/*****************************************************************************/
/* logArchive */
/*****************************************************************************/
void logArchive(void)
{
#if LOG
	char	*logBuffer = NULL;
	char	nameBuffer[50];
	int	totalFileCount = 0, i = 0;
	TYPE_File	tempFile;
	word	year = 0;
	byte	month = 0, day = 0, weekday = 0;
	TYPE_File*	fp = NULL;


	if(logFileError == FALSE)
	{
		GotoDataFiles();
		totalFileCount = TAP_Hdd_FindFirst(&tempFile); 

		logBuffer = (char *)TAP_MemAlloc( totalFileCount * 50 );
		memset( logBuffer, 0, (totalFileCount * 50) );

		TAP_ExtractMjd( schTimeMjd, &year, &month, &day, &weekday);
		sprintf( logBuffer, "%02u:%02u:%02u %02u/%02u/%04u - Archive: %d\r\n", schTimeHour, schTimeMin, schTimeSec, day, month, year, totalFileCount );

		for ( i=1; i <= totalFileCount ; i++ )
		{
			memset(nameBuffer,0,50);
			sprintf( nameBuffer, "\t\t\t\t%s\r\n", tempFile.name );
			strcat( logBuffer, nameBuffer );

			TAP_Hdd_FindNext(&tempFile);
		}

		GotoPath(SETTINGS_FOLDER);

		if (fp = TAP_Hdd_Fopen (LOG_FILENAME))
		{
			TAP_Hdd_Fseek (fp, logBufferOffset, 0);

			TAP_Hdd_Fwrite (logBuffer, strlen(logBuffer), 1, fp);

			TAP_Hdd_Fclose (fp);

			logBufferOffset += strlen(logBuffer);

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
	
	
	TAP_MemFree(logBuffer);	
	
#endif
}




