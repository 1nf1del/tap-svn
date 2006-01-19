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
#ifndef cpputils_timers_h
#define cpputils_timers_h
#include <stdlib.h>

class Timer;
#include "tap.h"
#include "taparray.h"
class EPGevent;

class Timers
{
public:
	Timers(void);
	~Timers(void);

	array<Timer*> GetTimers();
	bool IsScheduled(const EPGevent* pEvent);
	bool ScheduleEvent(const EPGevent* pEvent);
	bool CancelEvent(const EPGevent* pEvent);
	bool CanRecord(const EPGevent* pEvent) const;
	void Refresh();
	array<Timer*> GetClashingTimers(const EPGevent* pEvent) const;

	// public to allow options to attach to the variables by ref and change the values
	static int m_prePadding;
	static int m_postPadding;
	static bool m_mergeFlag;
	static bool m_bTreatVCRAsRecord;

private:

	array<Timer*> GetOverlappingTimers(const EPGevent* pEvent) const;
	void Cleanup();
	static bool ShouldMergePrograms();
	array<Timer*> m_timers;
};

#endif