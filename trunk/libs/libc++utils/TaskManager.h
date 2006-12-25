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
#ifndef framework_taskmanager_h
#define framework_taskmanager_h

#include <stdlib.h>
#include "tapstring.h"
#include "taparray.h"
#include "tapmap.h"
#include "tap.h"
class Task;
#include "TaskSchedule.h"


class TaskManager
{
public:
	TaskManager(void);
	~TaskManager(void);

	void AddTask(Task* pTask);
	void CancelTask(const string& taskName);
	bool IsTaskComplete(const string& taskName);
	int GetTaskPercentComplete(const string& taskName);
	void Kick(const string& taskName);

	void OnIdleEvent();
private:
	Task* GetTaskFromMap(const string& taskName);
	void Schedule(Task* pTask);
	bool CountIdleEvents(dword dwThisTick);
	void AdjustWorkAmount();

	array<Task*> m_runList;
	map<string, Task*> m_taskMap;

	dword m_dwNextRunAtTick;
	dword m_dwNextResetTick;
	dword m_dwIdleCount;
	dword m_dwIdleFreq;
	int m_iAmount;
};

#endif