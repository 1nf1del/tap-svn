/*
Copyright (C) 2006 Robin Glover

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
#include "globals.h"
#include "Timers.h"
#include "EPGData.h"
#include "Channels.h"
#include "Archive.h"
#include "Logger.h"
#include "EPGLoadTask.h"
#include "TaskManager.h"

Globals* Globals::m_pTheGlobals = NULL;

Globals& Globals::GetTheGlobals()
{
	if (m_pTheGlobals == NULL)
		m_pTheGlobals = new Globals();

	return *m_pTheGlobals;

}

Globals::Globals(void)
{
	m_pTheTimers = NULL;
	m_pEPGdata = NULL;
	m_pChannels = NULL;
	m_pArchive = NULL;
	m_pTaskManager = NULL;
	m_bWaitingForEPGLoad = false;
}

Globals::~Globals(void)
{
}

void Globals::Initialize(TaskManager* pTaskManager)
{
	m_pTaskManager = pTaskManager;
}

Timers* Globals::GetTimers()
{
	if (m_pTheTimers == 0)
		m_pTheTimers = new Timers();

	return m_pTheTimers;
}

Channels* Globals::GetChannels()
{
	if (m_pChannels == 0)
		m_pChannels = new Channels();

	return m_pChannels;
}

bool Globals::LoadEPGData(DataSources dataSource, ProgressNotification* pProgress, dword dwFlags)
{
	if (m_bWaitingForEPGLoad)
	{
		WaitForTaskComplete(pProgress, EPGLoadTask::TaskName(), m_bWaitingForEPGLoad);
		return true;
	}

	delete m_pEPGdata;
	m_pEPGdata = new EPGdata();
	return	m_pEPGdata->ReadData(dataSource, pProgress, dwFlags);
}

void Globals::ScheduleEPGLoad(int iSecondsDelay, DataSources dataSource, dword dwFlags)
{
	delete m_pEPGdata;
	m_pEPGdata = new EPGdata();
	EPGLoadTask* pLoader = new EPGLoadTask(m_pEPGdata, dataSource, dwFlags, new TaskSchedule(iSecondsDelay * 100));
	m_pTaskManager->AddTask(pLoader);
	m_bWaitingForEPGLoad = true;
}

EPGdata* Globals::GetEPGdata()
{
	if (m_bWaitingForEPGLoad)
	{
		WaitForTaskComplete(NULL, EPGLoadTask::TaskName(), m_bWaitingForEPGLoad);
	}

	if (m_pEPGdata == NULL)
		m_pEPGdata = new EPGdata();

	return m_pEPGdata;

}

void Globals::WaitForTaskComplete(ProgressNotification* pProgress, const string& taskName, bool& bWaitFlag)
{
	m_pTaskManager->Kick(taskName);

	if (pProgress)
		pProgress->Start();

	int iLastPercent = -1;

#pragma warning (disable : 4127)
	while (true)
	{
		TAP_SystemProc(); // allow idle events to be generated
		if (m_pTaskManager->IsTaskComplete(taskName))
		{
			bWaitFlag = false;
			if (pProgress)
				pProgress->Finish();
			return;
		}

		if (pProgress)
		{
			int iPercent = m_pTaskManager->GetTaskPercentComplete(taskName);
			if (iPercent != iLastPercent)
			{
				pProgress->Step((short)iPercent);
				iLastPercent = iPercent;
			}
		}

	}
}

void Globals::Cleanup()
{
	delete m_pTheTimers;
	m_pTheTimers = NULL;
	delete m_pEPGdata;
	m_pEPGdata = NULL;
	delete m_pChannels;
	m_pChannels = NULL;
	delete m_pArchive;
	m_pArchive = NULL;
}

Archive* Globals::GetArchive(const string& sCacheFile)
{
	if (m_pArchive == NULL)
	{
		//		TRACE("building archive");	
		m_pArchive = new Archive(sCacheFile);
	}

	return m_pArchive;
}


bool Globals::WaitingForEPGLoad()
{
	if (m_bWaitingForEPGLoad)
	{
		if (m_pTaskManager->IsTaskComplete(EPGLoadTask::TaskName()))
		{
			m_bWaitingForEPGLoad = false;
			return false;
		}
		return true;
	}
	return false;
}