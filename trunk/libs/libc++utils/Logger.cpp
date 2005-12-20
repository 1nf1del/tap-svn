#include ".\logger.h"
#include <file.h>

Logger* Logger::m_pTheLogger = NULL;

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
	va_list arglist;
	va_start(arglist, format);
	GetLogger()->Log(format, arglist);
	va_end(arglist);
}

Logger* Logger::GetLogger()
{
	if (m_pTheLogger == NULL)
		m_pTheLogger = new Logger();

	return m_pTheLogger;
}

void Logger::DoneWithLogger()
{
	delete m_pTheLogger;
	m_pTheLogger = NULL;
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
			m_OSDRegion = TAP_Osd_Create(100,100,520,376,0,0);
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

void Logger::Log(const char* format, const va_list &arglist)
{
	if (m_Destination == 0)
		return;

	char buf[2048];
	vsprintf(buf, format, arglist);

	if (m_Destination & Serial)
		TAP_Print(buf);

	if (m_Destination & Screen)
	{
		TAP_Osd_PutString1419(m_OSDRegion, 0, m_yOffs, 520, buf, COLOR_White, COLOR_Blue);
		TAP_Delay(50);
		m_yOffs += 20;
		if (m_yOffs > 350)
			m_yOffs = 0;
	}

	if (m_Destination & File)
	{
		fputs(buf, m_pFile);
//		fflush(m_pFile);
	}
}