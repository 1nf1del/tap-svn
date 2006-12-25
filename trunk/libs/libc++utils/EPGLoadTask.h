#pragma once
#include "task.h"
#include "EPGData.h"
#include "ProgressNotification.h"

class EPGLoadTask :
	public Task, public ProgressNotification
{
public:
	EPGLoadTask(EPGdata* pTarget, DataSources dataSource,  dword dwFlags, TaskSchedule* pSchedule);
	~EPGLoadTask(void);

	virtual bool DoSomeWork(int iAmount) ; // return true when more to do on this run of the task, false when done
	virtual bool BeginTask();
	virtual void EndTask();
	virtual int GetPercentComplete() ;
	virtual void Step(short int iPercent);
	static string TaskName();

private:
	DataSources m_dataSource;
	dword m_dwFlags;
	EPGdata* m_pTarget;
	int m_iPercent;
};
