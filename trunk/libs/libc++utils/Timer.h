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
#ifndef cpputils_timer_h
#define cpputils_timer_h
#include <stdlib.h>
#include "tap.h"
class EPGevent;
#include "Timeslot.h"

class Timer
{
	friend class Timers;
public:
	// these public methods for anyone to access
	// do not add/remove timers using this class, use the methods on Timers instead
	bool SchedulesEvent(const EPGevent* pEvent) const;
	bool SchedulesOtherEventsToo(const EPGevent* pEvent) const;
	const TimeSlot& GetTimeSlot() const;
	int GetLogicalChannelNum() const;
	string Description() const;
	bool IsRecording() const;

private:
	// these are public for the Timers class
	Timer();
	Timer(int iIndex, TYPE_TimerInfo* pInfo);
	~Timer(void);
	bool Merge(Timer* pOtherTimer);
	bool ShrinkToRemoveEvent(const EPGevent* pEvent);
	bool ExtendToCoverEvent(const EPGevent* pEvent);
	static bool ScheduleEvent(const EPGevent* pEvent, bool bPadStart, bool bPadEnd);
	bool UnSchedule();
	bool TrimToExclude(const TimeSlot& timeSlot);

	// these are private for internal use
	bool ExtendToCoverTime(const TimeSlot& timeSlot, bool bPad);
	bool UpdateFileName();
	static string GetFileName(const EPGevent* pEvent);
	EPGevent* GetFirstScheduledEvent() const;
	void UpdateTimerInfoTimes();
	bool SplitTimer(const TimeSlot& slotToRemove);
	static bool Schedule(int iLogicalChannelNum, const TimeSlot& timeSlot, const string& sFileName, bool bPadStart, bool bPadEnd);
	static bool Schedule(int iLogicalChannelNum, const TimeSlot& timeSlot);
	static EPGevent* GetFirstScheduledEvent(const TimeSlot& timeSlot, int iLogicalChannelNum);
	bool ReSchedule();
	bool Update();

	TYPE_TimerInfo m_TimerInfo;
	int m_iLogicalChannelNum;
	TimeSlot m_time;
	int m_iIndex;
	static short int GetDefaultStartPadding();
	static short int GetDefaultEndPadding();
	static short int GetMinProgramLength();

};
#endif