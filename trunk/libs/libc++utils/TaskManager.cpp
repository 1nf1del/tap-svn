#include "TaskManager.h"
#include "Task.h"
#include "TaskSchedule.h"
#include "Logger.h"

#define IDLE_CHECK_TIMESLICE 50
#define MIN_IDLE_PER_SEC_TARGET 2
#define MAX_IDLE_PER_SEC_TARGET 10

TaskManager::TaskManager(void)
{
	m_dwNextRunAtTick = 0xFFFFFFFF;
	m_dwNextResetTick = TAP_GetTick() + IDLE_CHECK_TIMESLICE;
	m_dwIdleCount = 0;
	m_dwIdleFreq = 2;
	m_iAmount = 5; 
}

TaskManager::~TaskManager(void)
{
	for (unsigned int i=0; i<m_runList.size(); i++)
		delete m_runList[i];
}

void TaskManager::AddTask(Task* pTask)
{
	CancelTask(pTask->GetName()); // remove any existing copies;
	m_taskMap[pTask->GetName()] = pTask;
	Schedule(pTask);
	m_dwNextRunAtTick = m_runList[0]->GetSchedule()->GetNextRunTime();
}

void TaskManager::Schedule(Task* pTask)
{
	dword dwRunAtTick = pTask->GetSchedule()->GetNextRunTime();

	for (unsigned int i=0 ; i<m_runList.size(); i++)
	{
		if (dwRunAtTick<m_runList[i]->GetSchedule()->GetNextRunTime())
		{
			m_runList.resize(m_runList.size() + 1);
			for (unsigned int j=m_runList.size(); j>i; j--)
			{
				m_runList[j] = m_runList[j-1];
			}
			m_runList[i]=pTask;
			return;
		}

	}

	m_runList.push_back(pTask);
}

Task* TaskManager::GetTaskFromMap(const string& taskName)
{
	if (m_taskMap.Find(taskName) == NULL)
		return NULL; 
	
	Task* pTheTask = m_taskMap[taskName];

	return pTheTask;
}

void TaskManager::CancelTask(const string& taskName)
{
	Task* pTheTask = GetTaskFromMap(taskName);
	if (pTheTask == NULL)
		return;

	int iIndex = m_runList.find(pTheTask);
	m_runList.erase(iIndex);
	m_taskMap.Delete(taskName);
	delete pTheTask;
}

bool TaskManager::IsTaskComplete(const string& taskName)
{
	if (m_taskMap.Find(taskName) == NULL)
		return false; // we haven't seen this task, so it probably hasn't run

	Task* pTheTask = m_taskMap[taskName];

	if (pTheTask == NULL)
		return true;

	return false;
}

int TaskManager::GetTaskPercentComplete(const string& taskName)
{
	if (m_taskMap.Find(taskName) == NULL)
		return false; // we haven't seen this task, so it probably hasn't run

	Task* pTheTask = m_taskMap[taskName];

	if (pTheTask == NULL)
		return 100;

	return pTheTask->GetPercentComplete();
}

bool TaskManager::CountIdleEvents(dword dwThisTick)
{
	m_dwIdleCount++;
	if (dwThisTick>=m_dwNextResetTick)
	{
		m_dwIdleFreq = m_dwIdleCount * (100 / IDLE_CHECK_TIMESLICE);
		//if ((dwThisTick/100) % 5 == 0)
		//	TRACE1("Idle rate is %d per second\n", m_dwIdleFreq);
		m_dwIdleCount = 0;
		m_dwNextResetTick = dwThisTick + IDLE_CHECK_TIMESLICE;

		return true;
	}

	return false;
}

void TaskManager::OnIdleEvent()
{
	dword dwTick = TAP_GetTick();
	bool bUpdate = CountIdleEvents(dwTick);

	if (dwTick>=m_dwNextRunAtTick)
	{
		if (m_runList.size() == 0)
			return;

		Task* pTheTask = m_runList[0];

		if (pTheTask->GetSchedule()->GetNextRunTime() != m_dwNextRunAtTick)
		{
			m_dwNextRunAtTick = pTheTask->GetSchedule()->GetNextRunTime();
			return;
		}

		if (bUpdate)
			AdjustWorkAmount(pTheTask->GetSchedule()->BeenKicked());


		if (!pTheTask->DoWork(m_iAmount))
		{
			m_iAmount = 5;
			// task finished
			if (pTheTask->GetSchedule()->Reset())
			{
				pTheTask->Reset();
				Schedule(pTheTask);
				m_dwNextRunAtTick = m_runList[0]->GetSchedule()->GetNextRunTime();

			}
			else
			{
				m_taskMap[pTheTask->GetName()]=NULL;
				m_runList.erase(0);
				delete pTheTask;
			}
		}
	}
}

void TaskManager::Kick(const string& taskName)
{
	Task* pTheTask = GetTaskFromMap(taskName);
	if (pTheTask == NULL)
		return;

	pTheTask->GetSchedule()->Kick();
	int iIndex = m_runList.find(pTheTask);
	m_runList.erase(iIndex);

	Schedule(pTheTask);
	m_dwNextRunAtTick = m_runList[0]->GetSchedule()->GetNextRunTime();
}

void TaskManager::AdjustWorkAmount(bool bHighPriority)
{
	dword iFactor = bHighPriority ? 1 : 5;

	if (m_dwIdleFreq <=  MIN_IDLE_PER_SEC_TARGET * iFactor)
	{
		int newAmount =  m_iAmount * (m_dwIdleFreq) / (MIN_IDLE_PER_SEC_TARGET * iFactor);
		if (newAmount < m_iAmount / 4)
			newAmount = m_iAmount / 4;
		if (newAmount == m_iAmount)
			newAmount--;

		m_iAmount = newAmount;

		if (m_iAmount == 0)
			m_iAmount = 1; 

		TRACE1("Reducing work rate to %d\n", m_iAmount);
	}

	if (m_dwIdleFreq > MAX_IDLE_PER_SEC_TARGET * iFactor)
	{
		int newAmount = (m_iAmount * m_dwIdleFreq) / (MAX_IDLE_PER_SEC_TARGET * iFactor);
		if (newAmount > m_iAmount * 4)
			newAmount = m_iAmount * 4;

		if (newAmount == m_iAmount)
			newAmount++;
		m_iAmount = newAmount;

		TRACE1("Increasing work rate to %d\n", m_iAmount);
	}


	return;
}