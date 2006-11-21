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
#include "logger.h"
#include <file.h>

Logger* Logger::m_pTheLogger = NULL;
bool Logger::m_bLogNoMore = false;

Logger::Logger(void)
{
	m_Destination = Serial;
	m_pFile = NULL;
	m_OSDRegion = 0;
}

Logger::~Logger(void)
{
	if (m_pFile)
		fclose(m_pFile);

	if (m_OSDRegion)
		TAP_Osd_Delete(m_OSDRegion);
}

void Logger::Log(const char* format, ...)
{
	if (GetLogger())
	{
		va_list arglist;
		va_start(arglist, format);
		GetLogger()->Logv(format, arglist);
		va_end(arglist);
	}
}

Logger* Logger::GetLogger()
{
	if ((m_pTheLogger == NULL) && !m_bLogNoMore)
		m_pTheLogger = new Logger();

	return m_pTheLogger;
}

void Logger::DoneWithLogger()
{
	TRACE("About to discard logger\n");
	delete m_pTheLogger;
	m_pTheLogger = NULL;
	m_bLogNoMore = true;
}

void Logger::SetDestination(int destination)
{
	GetLogger()->SetDest((Logger::LogDests)destination);
}

void Logger::SetDest(Logger::LogDests destination)
{
	if (destination & Screen)
	{
		if (m_OSDRegion == 0)
			m_OSDRegion = (word) TAP_Osd_Create(100,100,520,376,0,0);
		m_yOffs = 0;
	}
	else
	{
		if (m_OSDRegion != 0)
			TAP_Osd_Delete(m_OSDRegion);
	}
		
	if (destination & File)
	{
		if (m_pFile == NULL)
			m_pFile = fopen("LogFile", "a");
	}
	else
	{
		if (m_pFile != NULL)
			fclose(m_pFile);
	}
	m_Destination = destination;
}

void Logger::LogMemStats()
{
	dword heapSize, freeSize, availSize;
	TAP_MemInfo(&heapSize, &freeSize, &availSize);
	Log("AvailableMemory Stats : %d Total, %d Free, %d Avail\n", heapSize, freeSize, availSize);
}

void Logger::Logv(const char* format, const va_list &arglist)
{
	if (m_Destination == 0)
		return;

	char buf[2048];
	vsprintf(buf, format, arglist);


	if (m_Destination & File)
	{
		fputs(buf, m_pFile);
//		fflush(m_pFile);
	}

	buf[255] = 0; // TAP_Print, and probably Osd_PutString can only cope with 256 chars maximum
	// no point in trying to split - write shorter logging messages...

	if (m_Destination & Serial)
		TAP_Print(buf);

	if (m_Destination & Screen)
	{
		TAP_Osd_PutString1419(m_OSDRegion, 0, m_yOffs, 520, buf, COLOR_White, COLOR_Blue);
		TAP_Delay(25);
		m_yOffs += 20;
		if (m_yOffs > 350)
			m_yOffs = 0;
	}

}