#include "EPGLoadTask.h"

EPGLoadTask::EPGLoadTask(EPGdata* pTarget, DataSources dataSource, dword dwFlags, TaskSchedule* pSchedule) : 
	Task(TaskName(), pSchedule)
{
	m_dataSource = dataSource;
	m_dwFlags = dwFlags;
	m_pTarget = pTarget;
	m_iPercent = 0;
}

	string EPGLoadTask::TaskName()
	{
		return "EPGLoad";
	}

EPGLoadTask::~EPGLoadTask(void)
{
}

bool EPGLoadTask::BeginTask()
{
	return m_pTarget->BeginReading(m_dataSource, this, m_dwFlags);

}

bool EPGLoadTask::DoSomeWork(int iAmount)
{
	return m_pTarget->ReadSomeData(this, iAmount*25);
}

void EPGLoadTask::EndTask()
{
	m_pTarget->FinishReading(this);

}

int EPGLoadTask::GetPercentComplete()
{
	return m_iPercent;
}

void EPGLoadTask::Step(short iPercent)
{
	m_iPercent = iPercent;
}