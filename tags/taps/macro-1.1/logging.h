#ifndef __LOGGING_H
#define __LOGGING_H

//------------------------------ Prototypes --------------------------------------
//

#define LOG_NONE	0x00000000
#define LOG_FATAL	0x00000001
#define LOG_ERROR	0x00000002
#define LOG_WARNING	0x00000004
#define LOG_INFO	0x00000008
#define LOG_DEBUG	0x00000010

#define LOG_ALL		0xFFFFFFFF

void openLogfile();
void appendToLogfile(const char* text);
void appendStringToLogfile(const char* fmt, const char* str);
void closeLogfile();
void logOutput( int ltype, char *fmt, ... );
char *getLoggingLevel(char *str);
int getLoggingLevelValue(char *str);
int setLoggingLevelValueFromString(char *str);
void setNextLoggingLevel(void);

#endif
