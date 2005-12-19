#pragma once
#include "tap.h"
#include "tapstring.h"
class PerformanceTimer
{
public:
	PerformanceTimer(const string& title);
	~PerformanceTimer(void);

private:
	string m_sTitle;
	dword m_dwStartTick;
};
