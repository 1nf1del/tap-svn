#pragma once
#include "task.h"
class Archive;

#pragma warning (disable : 4512)

class ArchiveLoadTask :
	public Task
{
public:
	ArchiveLoadTask(Archive*& pArchive, const string& sCacheFile, TaskSchedule* pSchedule);
	~ArchiveLoadTask(void);

	virtual bool DoSomeWork(int iAmount) ; // return true when more to do on this run of the task, false when done
	virtual bool BeginTask();
	virtual void EndTask();
	virtual int GetPercentComplete() ;
	static string TaskName();

private:
	Archive*& m_pTarget;
	int m_iPercent;
	string m_sCache;
};
