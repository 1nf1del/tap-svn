//------------------------------ Information --------------------------------------
//
// Logging module
//
//---------------------------------------------------------------------------------

#define LOGFILENAME "macro.log"

#include <string.h>	//strcmpi, strtok
#include <stdarg.h>	//va_list, va_args
#include <stdio.h>	//SEEK_ constants

#include "tap.h"
#include "macro.h"
#include "logging.h"

//------------------------------ Global Variables --------------------------------------
//

extern int _appl_version;	//firmware version

// FileDescriptor for the logfile.
TYPE_File* logFD = NULL;
int logByteCount;
dword _logSize;
static int loggingLevel = LOG_NONE;

//------------------------------ File Logging Tools --------------------------------------


//--------------------------------------------------------------------------------------
//	openLogFile - Open the TAP's logfile in the current directory
//--------------------------------------------------------------------------------------

void openLogfile()
{

	if (logFD != NULL)
	{
		TAP_Print("openLogfile: attempting to open file that's already open\r\n");
		return;
	}

	logFD = TAP_Hdd_Fopen((char *)LOGFILENAME);
	if (logFD == NULL)
	{
		//TAP_Print("File doesn't exist. Creating log file...\r\n");
		TAP_Hdd_Create((char *)LOGFILENAME, ATTR_NORMAL);
		logFD = TAP_Hdd_Fopen((char *)LOGFILENAME);
		//write the header stuff identifying this machine/firmware
		logByteCount = sizeof logByteCount;
		//TAP_Hdd_Fwrite((void *)logByteCount, logByteCount, 1, logFD);
		TAP_Hdd_Fwrite((void *)"4    ", logByteCount, 1, logFD);
		//TAP_Print("File marker written...\r\n");
	}
	else 
	{
		// goto the end of the file
		//TAP_Hdd_Fseek(logFD, 0, SEEK_END);
	}
}


//--------------------------------------------------------------------------------------
//	appendToLogfile - append the string to the logfile
//--------------------------------------------------------------------------------------

void appendToLogfile(const char* text)
{	
    word mjd; byte hour; byte min; byte sec; 
	word year; byte month; byte day; byte weekday;
    char str[20]; 

	//TAP_Print("Appending to log file...\r\n");
	openLogfile();

	if (logFD != NULL)
	{
		TAP_Hdd_Fseek(logFD, logByteCount, SEEK_SET);

        TAP_GetTime( &mjd, &hour, &min, &sec);
        TAP_ExtractMjd(mjd, &year, &month, &day, &weekday);
		sprintf(str,"%02d-%02d-%04d %02d:%02d:%02d - ", day, month, year, hour, min, sec);
		TAP_Hdd_Fwrite((void*)str, strlen(str), 1, logFD);
		TAP_Hdd_Fwrite((void*)text, strlen(text), 1, logFD);
		TAP_Hdd_Fwrite((void*)"\r\n", strlen("\r\n"), 1, logFD);
		logByteCount += strlen(str) + strlen(text) + strlen("\r\n");
		closeLogfile();
	}
	
}

//--------------------------------------------------------------------------------------
//	closeLogfile - close the logfile
//--------------------------------------------------------------------------------------

void closeLogfile(void)
{
	if (logFD != NULL)
	{
		char buff[512];
		int rem;

		memset(buff, 0, sizeof buff);
		// Space pad the end of the last 512 byte block as the Toppy writes in 512 bytes blocks
		TAP_Hdd_Fseek(logFD, logByteCount, SEEK_SET);
		strcpy(buff, "********** End Of File **********\r\n");
		TAP_Hdd_Fwrite((void*)buff, strlen(buff), 1, logFD);
		/*
		rem = logByteCount % 512;
		if (rem > 0)
		{
			memset(buff, 0, rem+1);
			TAP_Hdd_Fwrite(buff, rem, 1, logFD);
		}
		*/

		//TAP_Hdd_Fwrite((void*)"\r\n", strlen("\r\n"), 1, logFD);
		//logByteCount += 2;

		// goto the start of the file and write length marker
		//TAP_Hdd_Fseek(logFD, 0, SEEK_SET);
		//TAP_Hdd_Fwrite(&_logSig, sizeof _logSig, 1, logFD);

		//TAP_Hdd_Fseek(logFD, _logSig.logSize, SEEK_SET);
		//TAP_Hdd_Fwrite((void *)"************** end of file *************\n", 41, 1, logFD);

		TAP_Hdd_Fclose(logFD);
		logFD = NULL;
	}	
}

//--------------------------------------------------------------------------------------
//	logOutput - this is the function used externally to log some text if the current 
//		loggingLevel is set
//--------------------------------------------------------------------------------------
// 
// log output only if the corresponding loggingLevel is set
//
// logNone - no logging
// logSome - log F(atal) only
// logMore - log E(error) and F(fatal)
// logAll  - log I(nfo), W(arning), E and F
//

void logOutput( int ltype, char *fmt, ... )
{
	char buff[512];
	va_list args;

	switch (loggingLevel)
	{
	case _logNone: 		return; break;

	case _logSome:		//log F(atal) only
		if (LOG_FATAL & ltype) break; 
		return; break;

	case _logMore:		//log F(atal) and E(rror)
		if ((LOG_FATAL & ltype) || (LOG_ERROR & ltype)) break;
		return; break;

	case _logAll:
		if ((LOG_FATAL & ltype) || (LOG_ERROR & ltype) ||
		    (LOG_WARNING & ltype) || (LOG_INFO & ltype))
			break;
		return; break;

	default:	// unknown
		TAP_Print("Invalid log type: %d\r\n", ltype);
		return; break;
	}

	va_start(args, fmt);
	vsprintf(buff, fmt, args);  //magic happens here
	va_end(args);

	appendToLogfile(buff);
	strcat(buff,"\n");
	TAP_Print(buff);

	return;
}

//--------------------------------------------------------------------------------------
//	Decode the loggingLevel numeric to a readable string
//--------------------------------------------------------------------------------------

char *getLoggingLevel(char *str)
{
	switch (loggingLevel)
	{
	case _logNone: 		strcpy(str, "LogNone"); break;
	case _logSome:		strcpy(str, "LogSome"); break;
	case _logMore:		strcpy(str, "LogMore"); break;
	case _logAll:		strcpy(str, "LogAll"); break;
	default:  			strcpy(str, "Undefined"); break;
	}
	return str;

}

//--------------------------------------------------------------------------------------
//	Decode the loggingLevel string to a value
//--------------------------------------------------------------------------------------

int getLoggingLevelValue(char *str)
{

	if (strcmpi(str, "LogNone")==0)
		return _logNone;
	else if (strcmpi(str, "LogSome")==0)
		return _logSome;
	else if (strcmpi(str, "LogMore")==0)
		return _logMore;
	else if (strcmpi(str, "LogAll")==0)
		return _logAll;
	else return loggingLevel;  //return current

}


//--------------------------------------------------------------------------------------
//	Set the loggingLevel to a value based on a string - internal routine
//--------------------------------------------------------------------------------------

static int setInternalLoggingLevelValue(char *str)
{

	if (strcmpi(str, "LogNone")==0)
		return _logNone;
	else if (strcmpi(str, "LogSome")==0)
		return _logSome;
	else if (strcmpi(str, "LogMore")==0)
		return _logMore;
	else if (strcmpi(str, "LogAll")==0)
		return _logAll;
	else return loggingLevel;  //return current

}


//--------------------------------------------------------------------------------------
//	Set the loggingLevel to a value based on a string - external routine
//--------------------------------------------------------------------------------------

int setLoggingLevelValueFromString(char *str)
{
	loggingLevel = setInternalLoggingLevelValue(str);
	return loggingLevel;
}

//--------------------------------------------------------------------------------------
//	Used to increment the logging level (eg in the macro options menu)
//--------------------------------------------------------------------------------------

void setNextLoggingLevel()
{
	loggingLevel = (loggingLevel+1) % 4;  //should calc the number 4
}
