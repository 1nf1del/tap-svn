/*
Copyright (C) 2005 Robin Glover

This file is part of the TAPs for Topfield PVRs project.
http://tap.berlios.de/

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/
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
#define TRACE3(str,x,y,z) Logger::Log(str,x,y,z)
#define TRACE4(str,x,y,z,t) Logger::Log(str,x,y,z,t)
#define TRACE_MEMORY() Logger::LogMemStats()

class Logger
{
public:
	Logger(void);
	~Logger(void);

	enum LogDests
	{
		None=0,
		Serial=1,
		Screen=2,
		File=4
	};

	static void SetDestination(int destination);
	static void Log(const char* format, ...);
	static void DoneWithLogger();
	static void LogMemStats();

private:
	static Logger* GetLogger();

	void Logv(const char* format, const va_list& arglist);
	void SetDest(LogDests destination);

	static Logger* m_pTheLogger;
	static bool m_bLogNoMore;

	LogDests m_Destination;
	word m_OSDRegion;
	int m_yOffs;
	UFILE* m_pFile;
};

#endif