/************************************************************
				Part of the ukEPG project
		This module handles the log files

Name	: LogFile.c
Author	: kidhazy
Version	: 0.0
For		: Topfield TF5x00 series PVRs
Licence	:
Descr.	:
Usage	:
History	: v0.0 kidhazy: 27-09-05	Inception date.


**************************************************************/

#define LOGFILE_NAME "Archive.log"
#define SCREEN_DELAY 3
#define SCREEN_LOG_Y_MAX 500
#define SCREEN_LOG_Y_START 450


#if DEBUG == 1
// FileDescriptor for the logfile.
TYPE_File* logFD;
int logByteCount;
#endif

#if DEBUG == 4
word logRgn;
#endif

int screenLogY, screenLogLine;


// Create the logfile handling routines if the DEBUG variable has been set, otherwise create dummy definitions
#if DEBUG == 1
//
// Open the logfile.
//
void openLogfile()
{
	logByteCount = 0;
//	ChangeDirRoot();
//	TAP_SystemProc();
//	TAP_Hdd_ChangeDir("ProgramFiles");
	
	logFD = TAP_Hdd_Fopen(LOGFILE_NAME);
	if (logFD == NULL)
	{
		// No logfile, so create one.
		TAP_Hdd_Create(LOGFILE_NAME, ATTR_NORMAL);
		logFD = TAP_Hdd_Fopen(LOGFILE_NAME);
	}
	if (logFD != NULL)
	{
		// Go to the end of the file.
		TAP_Hdd_Fseek(logFD, 0, 2);
	}
}

//
// Write a string to the logfile.
//
void appendToLogfile(const char* text, int logLevel)
{
    word mjd; byte hour; byte min; byte sec; 
	word year; byte month; byte day; byte weekday;
    char str[20]; 
    dword tempTick;

    if (logLevel > LOGLEVEL) return;
	
	if (logFD != NULL)
	{
        TAP_GetTime( &mjd, &hour, &min, &sec);
        TAP_ExtractMjd(mjd, &year, &month, &day, &weekday);
        tempTick = TAP_GetTick();
        //sprintf(str,"%02d-%02d-%04d %02d:%02d:%02d - ", day, month, year, hour, min, sec);
        sprintf(str,"%02d:%02d:%02d %i - ", hour, min, sec, tempTick);
		TAP_Hdd_Fwrite((void*)str, strlen(str), 1, logFD);
		TAP_Hdd_Fwrite((void*)text, strlen(text), 1, logFD);
		TAP_Hdd_Fwrite((void*)"\r\n", strlen("\r\n"), 1, logFD);
		logByteCount += strlen(text) + strlen("\r\n");
	}
}

//
// Write an integer to the logfile.
//
void appendIntToLogfile(const char* fmt, int i, int logLevel)
{
	char buffer[600];
	sprintf(buffer, fmt, i);
	appendToLogfile(buffer, logLevel);		
}

//
// Write a long integer to the logfile.
//
void appendLongToLogfile(const char* fmt, long l, int logLevel)
{
	char buffer[600];
	sprintf(buffer, fmt, l);
	appendToLogfile(buffer, logLevel);		
}

//
// Write a string to the logfile.
//
void appendStringToLogfile(const char* fmt, const char* str, int logLevel)
{
	char buffer[600];
	sprintf(buffer, fmt, str);
	appendToLogfile(buffer, logLevel);		
}

//
// Close the logfile.
//
void closeLogfile()
{
	if (logFD != NULL)
	{
		// Make the end of the logfile stop on a 512 byte boundary.
		appendToLogfile("********** End of logfile **********", INFO);
		while ((logByteCount % 512) != 510)
		{
			TAP_Hdd_Fwrite(" ", 1, 1, logFD);
			logByteCount++;
		}
		TAP_Hdd_Fwrite((void*)"\r\n", strlen("\r\n"), 1, logFD);
		logByteCount += 2;

		TAP_Hdd_Fclose(logFD);
		logFD = NULL;
	}	
}
#endif




#if DEBUG == 2
#define closeLogfile()
//
// Open the logfile.
//
void openLogfile()
{
    screenLogY = SCREEN_LOG_Y_START; 
    screenLogLine = 1;
}    

//
// Write a string to the logfile.
//
void appendToLogfile(const char* text, int logLevel)
{
    char str[400]; 
	
    if (logLevel > LOGLEVEL) return;

    sprintf(str,"%d: %s",screenLogLine, text);
    TAP_Osd_PutStringAf1926( rgn, 50, screenLogY, 700, str, COLOR_White, COLOR_Black );
    TAP_Delay(SCREEN_DELAY);

    screenLogLine++;
    if (screenLogY < SCREEN_LOG_Y_MAX)
        screenLogY = screenLogY + 30;
    else    
        screenLogY = SCREEN_LOG_Y_START;
}

//
// Write an integer to the logfile.
//
void appendIntToLogfile(const char* fmt, int i, int logLevel)
{
	char buffer[600];
	sprintf(buffer, fmt, i);
	appendToLogfile(buffer, logLevel);		
}

//
// Write a long integer to the logfile.
//
void appendLongToLogfile(const char* fmt, long l, int logLevel)
{
	char buffer[600];
	sprintf(buffer, fmt, l);
	appendToLogfile(buffer, logLevel);		
}

//
// Write a string to the logfile.
//
void appendStringToLogfile(const char* fmt, const char* str, int logLevel)
{
	char buffer[600];
	sprintf(buffer, fmt, str);
	appendToLogfile(buffer, logLevel);		
}

#endif





#if DEBUG == 3
#define closeLogfile()
#define openLogfile()

//
// Write a string to the logfile.
//
void appendToLogfile(const char* text, int logLevel)
{
    if (logLevel > LOGLEVEL) return;
     
    TAP_Print("%s\r\n",text);
    TAP_Delay(0);
}

//
// Write an integer to the logfile.
//
void appendIntToLogfile(const char* fmt, int i, int logLevel)
{
	char buffer[600];
	sprintf(buffer, fmt, i);
	appendToLogfile(buffer, logLevel);		
}

//
// Write a long integer to the logfile.
//
void appendLongToLogfile(const char* fmt, long l, int logLevel)
{
	char buffer[600];
	sprintf(buffer, fmt, l);
	appendToLogfile(buffer, logLevel);		
}

//
// Write a string to the logfile.
//
void appendStringToLogfile(const char* fmt, const char* str, int logLevel)
{
	char buffer[600];
	sprintf(buffer, fmt, str);
	appendToLogfile(buffer, logLevel);		
}

#endif







#if DEBUG == 4
//
// Close the logfile.
//
void closeLogfile()
{
	TAP_Osd_Delete( logRgn );
}

//
// Open the logfile.
//
void openLogfile()
{
	logRgn     = TAP_Osd_Create( 0, 0, 720, 576, 0, FALSE );
}    

//
// Write a string to the logfile.
//
void appendToLogfile(const char* text, int logLevel)
{
    char str[400]; 

    if (logLevel > LOGLEVEL) return;

    sprintf(str,"%s", text);
    ShowMessageBox( logRgn, "Archive Log", str, "");

}

//
// Write an integer to the logfile.
//
void appendIntToLogfile(const char* fmt, int i, int logLevel)
{
	char buffer[600];
	sprintf(buffer, fmt, i);
	appendToLogfile(buffer, logLevel);		
}

//
// Write a long integer to the logfile.
//
void appendLongToLogfile(const char* fmt, long l, int logLevel)
{
	char buffer[600];
	sprintf(buffer, fmt, l);
	appendToLogfile(buffer, logLevel);		
}

//
// Write a string to the logfile.
//
void appendStringToLogfile(const char* fmt, const char* str, int logLevel)
{
	char buffer[600];
	sprintf(buffer, fmt, str);
	appendToLogfile(buffer,  logLevel);		
}

#endif




#if DEBUG == 0
#define openLogfile()
#define closeLogfile()
#define appendToLogfile(text, logLevel)
#define appendIntToLogfile(fmt, i, logLevel)
#define appendLongToLogfile(fmt, l, logLevel)
#define appendStringToLogfile(fmt,str, logLevel)

#endif
