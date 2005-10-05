#ifndef _UTILS_MAIN
#define _UTILS_MAIN

#include <stdarg.h>

#define MAX_STR_LEN 500
#define MAX_MESSAGE_SIZE 1024

extern int loggingLevel;

enum loggingFlags {
	_INFO	= 01,
	_ERROR	= 02,
	_FATAL	= 04
};

int initUtils( int );
int logMessage( int level, char* msg );
void SaveError(int iCode, char* error);
void AppendError(char* error);
void InitError();
int GetLastErrorCode();
char* GetLastErrorString();
int vsprintf(char *, const char *, va_list);

#endif  // _UTILS

