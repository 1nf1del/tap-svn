#include ".\performancetimer.h"
#include "logger.h"

PerformanceTimer::PerformanceTimer(const string& title) : m_sTitle(title)
{
	m_dwStartTick = TAP_GetTick();
}

PerformanceTimer::~PerformanceTimer(void)
{
	dword endTick = TAP_GetTick();
	TRACE2("%s timed at %d milliseconds\n", m_sTitle.c_str(), (endTick-m_dwStartTick) * 10);
}
