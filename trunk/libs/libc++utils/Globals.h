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
#ifndef cpputils_globals_h
#define cpputils_globals_h

class Timers;
class EPGdata;
class Channels;
class ProgressNotification;
class Archive;
class TaskManager;
#include "EPGdata.h"

class Globals
{
public:
	Globals(void);
	~Globals(void);

	void Cleanup();
	Timers* GetTimers();
	EPGdata* GetEPGdata();
	Channels* GetChannels();
	bool LoadEPGData(DataSources dataSource, ProgressNotification* pProgress = 0, dword dwFlags = 0);
	Archive* GetArchive(const string& sCacheFile);
	void ScheduleEPGLoad(int iSecondsDelay, DataSources dataSource, dword dwFlags);
	void ScheduleArchiveLoad(int iSecondsDelay, const string& sCacheFile);
	void Initialize(TaskManager* pTaskManager);
	bool WaitingForEPGLoad();

	static Globals& GetTheGlobals();
private:
	void WaitForTaskComplete(ProgressNotification* pProgress, const string& taskName, bool& bWaitFlag);

	static Globals* m_pTheGlobals;

	Timers* m_pTheTimers;
	EPGdata* m_pEPGdata;
	Channels* m_pChannels;
	Archive* m_pArchive;
	TaskManager* m_pTaskManager;
	bool m_bWaitingForEPGLoad;
	bool m_bWaitingForArchiveLoad;
};
#endif