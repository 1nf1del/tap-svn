#include "ArchiveLoadTask.h"
#include "Archive.h"
#include "Logger.h"

ArchiveLoadTask::ArchiveLoadTask(Archive*& pArchive, const string& sCacheFile, TaskSchedule* pSchedule) : Task(TaskName(), pSchedule), m_pTarget(pArchive)
{
	m_sCache = sCacheFile;
}

ArchiveLoadTask::~ArchiveLoadTask(void)
{
}

string ArchiveLoadTask::TaskName()
{
	return "ArchiveLoad";
}

bool ArchiveLoadTask::DoSomeWork(int iAmount)
{
	iAmount;
	return m_pTarget->DoSomeLoading();
	
}

bool ArchiveLoadTask::BeginTask()
{
	TRACE("Starting load of Archive\n");
	m_pTarget = new Archive(m_sCache, true);
	return true;
}

void ArchiveLoadTask::EndTask()
{
	TRACE("Finished load of Archive\n");

}

int ArchiveLoadTask::GetPercentComplete() 
{
	return 50;
}

