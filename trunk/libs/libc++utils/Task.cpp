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
#include "Task.h"
#include "TaskSchedule.h"


Task::Task(string name, TaskSchedule* pSchedule)
{
	m_sName = name;
	m_pSchedule = pSchedule;
	Reset();
}


Task::~Task()
{
	delete m_pSchedule;
}

bool Task::DoWork(int iAmount)
{
	if (!m_bBegun)
	{
		m_bBegun = true;
		return BeginTask();
	}

	if (!DoSomeWork(iAmount))
	{
		EndTask();
		return false;
	}

	return true;
}

void Task::Reset()
{
	m_bBegun = false;
}

string Task::GetName()
{
	return m_sName;
}

TaskSchedule* Task::GetSchedule()
{
	return m_pSchedule;
}

