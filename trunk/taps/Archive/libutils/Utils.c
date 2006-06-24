/*
Utils.c

Do all init stuff here. The calling tap should call this first to init the library

*/

#include <string.h>
#include <tap.h>
#define printf TAP_Print
#include "Utils.h"


int loggingLevel;

int initUtils(int fl)
{

	InitError();

	//store the logging level
	if ((fl & (_INFO | _ERROR | _FATAL)) == 0)
		return 0;

	loggingLevel = fl;

	return 1;

}

int logMessage( int level, char* msg )
{
	if (level >= loggingLevel)
	{
		char loggingChar;
		switch (level)
		{
		case _INFO:
			loggingChar = 'I';
			break;
		case _ERROR:
			loggingChar = 'E';
			break;
		case _FATAL:
			loggingChar = 'F';
			break;
		default:
			loggingChar = '?';
		}

		printf("-%c-, %s\n", loggingChar, msg);
		return 0;
	} else
		return 1;
}

/* error stuff */

static	int		code;
static	char	errorString[MAX_STR_LEN];

void SaveError(int iCode, char* error)
{
	code = iCode;
	strcpy(errorString, error);
}

void AppendError(char* error)
{
	strcat(errorString, error);
}

void InitError()
{
	code = 0;
	errorString[0] = '\0';
}

int GetLastErrorCode()
{
	return code;
}

char* GetLastErrorString()
{
	return &errorString[0];
}

