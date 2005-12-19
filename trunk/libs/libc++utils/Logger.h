#ifndef _logger_libcpputils
#define _logger_libcpputils

#include "tap.h"
#include <stdarg.h>

extern "C" 
{
	typedef struct _iobuf UFILE;
}

#define TRACE(str) Logger::Log(str)
#define TRACE1(str,x) Logger::Log(str,x)
#define TRACE2(str,x,y) Logger::Log(str,x,y)
#define TRACE3(str,x,y,z) Logger::Log(str,x,z)
#define TRACE4(str,x,y,z,t) Logger::Log(str,x,y,z,t)

class Logger
{
public:
	Logger(void);
	~Logger(void);

	enum LogDests
	{
		Serial=1,
		Screen=2,
		File=4
	};

	static void SetDestination(int destination);
	static void Log(const char* format, ...);

private:
	static Logger* GetLogger();
	static void DoneWithLogger();

	void Log(const char* format, const va_list& arglist);
	void SetDest(LogDests destination);

	static Logger* m_pTheLogger;

	LogDests m_Destination;
	word m_OSDRegion;
	int m_yOffs;
	UFILE* m_pFile;
};

#endif