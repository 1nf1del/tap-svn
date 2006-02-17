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
#include ".\timer.h"
#include "Channels.h"
#include "EPGevent.h"
#include "globals.h"
#include "EPGdata.h"
#include "EPGchannel.h"
#include "Timers.h"
#include <string.h>

Timer::Timer()
{
	memset(&m_TimerInfo, 0, sizeof(TYPE_TimerInfo));
	m_iIndex = 0;
}

Timer::Timer(int iIndex, TYPE_TimerInfo* pInfo)
{
	m_TimerInfo = *pInfo;
	m_time = TimeSlot(m_TimerInfo.startTime, m_TimerInfo.duration);
	m_time.SetRepeat(m_TimerInfo.reservationType);
	m_iIndex = iIndex;
}

Timer::~Timer(void)
{
}

bool Timer::SchedulesEvent(const EPGevent* pEvent) const
{
	if (m_TimerInfo.svcType != SVC_TYPE_Tv)
		return false;

	if (!Timers::m_bTreatVCRAsRecord && (m_TimerInfo.isRec == 0))
		return false;

	if (pEvent->GetChannelNum() != GetChannelNum())
		return false;

	if (m_time.Contains(pEvent->GetTimeSlot()))
		return true;

	return false;

}

const TimeSlot& Timer::GetTimeSlot() const
{
	return m_time;
}

int Timer::GetChannelNum() const
{
	return m_TimerInfo.svcNum;
}

bool Timer::SchedulesOtherEventsToo(const EPGevent* pEvent) const
{
	if (!SchedulesEvent(pEvent))
		return false;

	int iStartPadding = pEvent->GetStart() - m_time.Start();
	int iEndPadding = m_time.End() - pEvent->GetEnd();

	return (iStartPadding>(GetDefaultStartPadding()+GetMinProgramLength())) 
		|| (iEndPadding>(GetDefaultEndPadding()+GetMinProgramLength()));
}

bool Timer::ExtendToCoverTime(const TimeSlot& timeSlot, bool bPad)
{
	if (IsRepeatingTimer())
		return false;

	if (timeSlot.End()>m_time.End())
	{
		// we need to extend the timer at the end
		// assume overlap to get here
		m_time.SetEnd( timeSlot.End() + (bPad ? GetDefaultEndPadding() : 0));
		return Update();
	}
	else if (timeSlot.Start()<m_time.Start())
	{
		// we need to extend the timer at the start
		// assume overlap to get here
		m_time.SetStart(timeSlot.Start() - (bPad ? GetDefaultStartPadding() : 0));
		return Update();
	}
	else
		return false; // now we're confused
}


bool Timer::ExtendToCoverEvent(const EPGevent* pEvent)
{
	if (IsRepeatingTimer())
		return false;

	if (GetChannelNum() != pEvent->GetChannelNum())
		return false;

	return ExtendToCoverTime(pEvent->GetTimeSlot(), true);
}

bool Timer::Update()
{
	UpdateTimerInfoTimes();
	UpdateFileName();
	return (TAP_Timer_Modify(m_iIndex, &m_TimerInfo) == 0);
}

bool Timer::ShrinkToRemoveEvent(const EPGevent* pEvent)
{
	if (IsRepeatingTimer())
		return false;

	int iStartPadding = pEvent->GetStart() - m_time.Start();
	int iEndPadding = m_time.End() - pEvent->GetEnd();

	iStartPadding = max(0, iStartPadding - GetMinProgramLength());
	iEndPadding = max(0, iEndPadding - GetMinProgramLength());

	if (iStartPadding<GetDefaultStartPadding())
	{
		// cut off beginning from timer
		m_time.SetStart( pEvent->GetEnd()-GetDefaultStartPadding());
		return Update();
	}
	else if (iEndPadding<GetDefaultEndPadding())
	{
		// cut end beginning from timer
		m_time.SetEnd(pEvent->GetStart()+GetDefaultEndPadding());
		return Update();
	}
	else
	{
		return SplitTimer(pEvent->GetTimeSlot());
	}
}

short int Timer::GetDefaultEndPadding()
{
	return (short) Timers::m_postPadding;
}

short int Timer::GetDefaultStartPadding()
{
	return (short) Timers::m_prePadding;
}

short int Timer::GetMinProgramLength()
{
	return 4;
}

EPGevent* Timer::GetFirstScheduledEvent(const TimeSlot& timeSlot, int iChannelNum)
{
	EPGchannel* pChannel = Globals::GetEPGdata()->GetChannel(iChannelNum);
	if (pChannel)
	{
		EPGevent* pEvent = pChannel->FindEvent(timeSlot.Start() + GetDefaultStartPadding());
		return pEvent;
	}
	return NULL;
}


EPGevent* Timer::GetFirstScheduledEvent() const
{
	return GetFirstScheduledEvent(m_time, GetChannelNum());
}


string Timer::Description() const
{
	string result;
	EPGevent* pEvent = GetFirstScheduledEvent();

	if (pEvent != NULL)
		result = pEvent->GetTitle();

	if (result.size()>25)
		result = result.substr(0,20) + "...";

	result+= "(";
	result+= (GetChannelNum() == -1 ? string("????") : Globals::GetChannels()->NameForChannel(GetChannelNum()));
	result += ":" + m_time.Description() + ")";

	return result;
}

bool Timer::Schedule(int iChannelNum, const TimeSlot& timeSlot)
{
	string name;
	EPGevent* pEvent = GetFirstScheduledEvent(timeSlot, iChannelNum);
	if (pEvent)
	{
		name = GetFileName(pEvent);
	}
	else
	{
		name = "Unknown Program.rec";
	}
	return Schedule(iChannelNum, timeSlot, name, false, false);
}

bool Timer::Schedule(int iChannelNum, const TimeSlot& timeSlot, const string& sFileName, bool bPadStart, bool bPadEnd)
{
	TYPE_TimerInfo info;
	memset(&info, 0, sizeof(TYPE_TimerInfo));
	info.duration = (word) (timeSlot.Duration());
	info.duration += (bPadEnd ? GetDefaultEndPadding() : 0) + (bPadStart ? GetDefaultStartPadding() : 0);
	info.startTime = (timeSlot.Start() - (bPadStart ? GetDefaultStartPadding() : 0)).AsDateTime();
	info.isRec = 1;
	info.reservationType = RESERVE_TYPE_Onetime;
	info.svcNum = SVC_TYPE_Tv;
	info.tuner = 3;
	info.svcNum = (word) iChannelNum;
	info.nameFix = 1;
	strcpy(info.fileName, sFileName);

	int iResult = TAP_Timer_Add(&info);
	return iResult == 0;
}


bool Timer::ScheduleEvent(const EPGevent* pEvent, bool bPadStart, bool bPadEnd)
{
	return Schedule(pEvent->GetChannelNum(), pEvent->GetTimeSlot(), GetFileName(pEvent), bPadStart, bPadEnd);
}

string Timer::GetFileName(const EPGevent* pEvent)
{
	string result = pEvent->GetFileName().substr(0,95);
	result += ".rec";
	return result;
}


bool Timer::UpdateFileName()
{
	EPGevent* pEvent = GetFirstScheduledEvent();
	if (pEvent)
	{
		strcpy(m_TimerInfo.fileName, GetFileName(pEvent));
		return true;
	}
	return false;
}

void Timer::UpdateTimerInfoTimes()
{
	m_TimerInfo.startTime = m_time.Start().AsDateTime();
	m_TimerInfo.duration = m_time.Duration();

}

bool Timer::SplitTimer(const TimeSlot& slotToRemove)
{
	if (IsRepeatingTimer())
		return false;

	if (!TAP_Timer_Delete(m_iIndex))
		return false;

	TimeSlot slot1 = m_time;
	slot1.SetEnd(slotToRemove.Start() + GetDefaultEndPadding());

	TimeSlot slot2 = m_time;
	slot2.SetStart(slotToRemove.End() - GetDefaultStartPadding());

	if (!Schedule(GetChannelNum(), slot1))
	{
		ReSchedule();
		return false;
	}

	if (!Schedule(GetChannelNum(), slot2))
	{
		// TODO: - find and remove slot 1
		// Remove Slot 1;
		ReSchedule();
		return false;
	}

	return true;
}

bool Timer::ReSchedule()
{
	// assumes m_timerinfo is valid, but has been deleted
	int iResult = TAP_Timer_Add(&m_TimerInfo);
	return iResult == 0;

}

bool Timer::UnSchedule()
{
	if (IsRepeatingTimer())
		return false;
	// assumes m_timerinfo is valid, but has been deleted
	return TAP_Timer_Delete(m_iIndex);
}


bool Timer::Merge(Timer* pOtherTimer)
{
	if (IsRepeatingTimer() || pOtherTimer->IsRepeatingTimer())
		return false;

	if (GetChannelNum() != pOtherTimer->GetChannelNum())
		return false;

	if (!m_time.OverlapsWith(pOtherTimer->GetTimeSlot()))
		return false;

	if (m_time.Start() > pOtherTimer->GetTimeSlot().Start())
		return pOtherTimer->Merge(this);

	// now we are sure that this timer has the earlier start point;

	if (!pOtherTimer->UnSchedule())
		return false;

	return ExtendToCoverTime(pOtherTimer->GetTimeSlot(), false);

}

bool Timer::TrimToExclude(const TimeSlot& timeSlot)
{
	if (IsRepeatingTimer())
		return false;

	if (!timeSlot.OverlapsWith(m_time))
		return false;

	if (timeSlot.Start()>m_time.Start())
	{
// we need to trim our end point to match the timeslots start
		m_time.SetEnd(timeSlot.Start());
		return Update();
	}

	if (timeSlot.End() <m_time.End())
	{
// we need to trim our start point to match the timeslots end
		m_time.SetStart(timeSlot.End());
		return Update();
	}

	// ouch the timeslot starts before we do and ends after us
	// I suppose we should go away

	return UnSchedule();
}

bool Timer::IsRecording() const
{
	return m_TimerInfo.isRec != 0;
}

bool Timer::IsRepeatingTimer() const
{
	return m_TimerInfo.reservationType != RESERVE_TYPE_Onetime;

}
