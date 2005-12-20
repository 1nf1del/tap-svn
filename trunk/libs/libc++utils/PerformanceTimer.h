#ifndef _performancetimer_libcpputils
#define _performancetimer_libcpputils
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
#endif