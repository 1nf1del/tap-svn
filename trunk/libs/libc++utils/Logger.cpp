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

#include "DirectoryUtils.h"
#include "PackedDateTime.h"
#include "logger.h"
#include <file.h>
#include <string.h>

Logger* Logger::m_pTheLogger = NULL;
bool Logger::m_bLogNoMore = false;

Logger::Logger()
{
	m_Destination = Serial;
	m_pFile = NULL;
	m_OSDRegion = 0;
	m_iFilePos = -1;
	m_bLogging = false;
}

Logger::~Logger(void)
{
	if (m_pFile)
		TAP_Hdd_Fclose(m_pFile);

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

void Logger::SetDestination(int destination, string logFileDir)
{
	GetLogger()->SetDest((Logger::LogDests)destination, logFileDir);
}

void Logger::SetDest(Logger::LogDests destination, string logFileDir)
{
	m_logFileDir = logFileDir;

//	if (destination & Screen)
	{
		if (m_OSDRegion == 0)
			m_OSDRegion = (word) TAP_Osd_Create(100,100,520,376,0,0);
		m_yOffs = 0;
	}
	//else
	//{
	//	if (m_OSDRegion != 0)
	//		TAP_Osd_Delete(m_OSDRegion);
	//	m_OSDRegion = 0;
	//}
		
	if (destination & File)
	{
		//if (m_pFile == NULL)
		//	m_pFile = OpenFile(m_logFileDir + "/LogFile", "a");
	}
	else
	{
		if (m_pFile != NULL)
			TAP_Hdd_Fclose(m_pFile);

		m_pFile = NULL;
	}
	m_Destination = destination;
}

void Logger::LogMemStats()
{
	dword heapSize, freeSize, availSize;
	TAP_MemInfo(&heapSize, &freeSize, &availSize);
	Log("AvailableMemory Stats : %d Total, %d Free, %d Avail\n", heapSize, freeSize, availSize);
}

void Logger::OpenFileIfNeeded()
{
	if (m_pFile == NULL)
	{
		m_pFile = OpenRawFile(m_logFileDir + "/LogFile.txt", "a");


		if (m_iFilePos == -1)
		{
			TAP_Hdd_Fseek(m_pFile, 0, 2);
			char buf[512];
			memset(buf, 32, 512);
			strcpy(buf, "=================================== MeiSearch Logging Started ==========================\r\n");
			TAP_Hdd_Fwrite(buf, 512, 1, m_pFile);


		}
		else
		{
			TAP_Hdd_Fseek(m_pFile, m_iFilePos, 0); 
		}
	}
}

void Logger::Logv(const char* format, const va_list &arglist)
{
	if (m_bLogging)
		return;

	if (m_Destination == 0)
		return;

	m_bLogging = true;



	char buf[2048];
	vsprintf(buf, format, arglist);


	if (m_Destination & File)
	{
		OpenFileIfNeeded();
		char timeBuf[32];
		PackedDateTime pdNow = PackedDateTime::Now();
		sprintf(timeBuf, "%s : ", pdNow.ShortDisplay().c_str());
		TAP_Hdd_Fwrite(timeBuf, strlen(timeBuf), 1, m_pFile);
		TAP_Hdd_Fwrite(buf, strlen(buf), 1, m_pFile);
		if (m_Destination & FlushFile)
		{
			m_iFilePos = TAP_Hdd_Ftell(m_pFile);

			if (m_iFilePos > 1023 && (m_iFilePos % 512) == 0)
				m_iFilePos += 512;

			TAP_Hdd_Fwrite((void*) "***END***", 9, 1, m_pFile);
			TAP_Hdd_Fclose(m_pFile);
			m_pFile = NULL;

		}
	}

	buf[255] = 0; // TAP_Print, and probably Osd_PutString can only cope with 256 chars maximum
	// no point in trying to split - write shorter logging messages...

	if (m_Destination & Serial)
		TAP_Print(buf);

	if (m_Destination & Screen)
	{
		LogToScreen(buf);
	}


	m_bLogging = false;
}

void Logger::LogToScreen(const char* buf)
{
	char myBuf[129];
	myBuf[0]=0;
	strncat(myBuf, buf, 128);

	int iLen = strlen(myBuf);
	for (int i=iLen; i<128; i++)
		strcat(myBuf, " ");

	myBuf[128]=0;

	TAP_Osd_PutString1419(m_OSDRegion, 0, (m_yOffs+20)>350 ? 0 : m_yOffs + 20 , 520, "________________________________________________________________", COLOR_White, COLOR_Blue);
	TAP_Osd_PutString1419(m_OSDRegion, 0, m_yOffs, 520, myBuf, COLOR_White, COLOR_Blue);
	TAP_Delay(50);
	m_yOffs += 20;
	if (m_yOffs > 350)
		m_yOffs = 0;

#ifdef WIN32
TAP_SystemProc(); // repaint on emulator so we can see what we have
#endif


}