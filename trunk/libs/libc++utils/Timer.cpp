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
	m_iLogicalChannelNum = 0;
}

Timer::Timer(int iIndex, TYPE_TimerInfo* pInfo)
{
	m_TimerInfo = *pInfo;
	m_time = TimeSlot(m_TimerInfo.startTime, m_TimerInfo.duration);
	m_time.SetRepeat(m_TimerInfo.reservationType);
	m_iLogicalChannelNum = Globals::GetChannels()->ToppyToLogical(m_TimerInfo.svcNum);
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

	if (pEvent->GetChannelNum() != m_iLogicalChannelNum)
		return false;

	if (m_time.Contains(pEvent->GetTimeSlot()))
		return true;

	return false;

}

const TimeSlot& Timer::GetTimeSlot() const
{
	return m_time;
}

int Timer::GetLogicalChannelNum() const
{
	return m_iLogicalChannelNum;
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

bool Timer::ExtendToCoverEvent(const EPGevent* pEvent)
{
	if (m_iLogicalChannelNum != pEvent->GetChannelNum())
		return false;

	if (pEvent->GetEnd()>m_time.End())
	{
		// we need to extend the timer at the end
		// assume overlap to get here
		PackedDateTime newEnd = pEvent->GetEnd() + GetDefaultEndPadding();
		m_TimerInfo.duration = (word) (newEnd - m_time.Start());
		return (TAP_Timer_Modify(m_iIndex, &m_TimerInfo) == 0);
	}
	else if (pEvent->GetStart()<m_time.Start())
	{
		// we need to extend the timer at the start
		// assume overlap to get here
		PackedDateTime newStart = pEvent->GetStart() - GetDefaultStartPadding();
		m_TimerInfo.startTime = newStart.AsDateTime();
		m_TimerInfo.duration = (word)( m_time.End() - newStart);
		return (TAP_Timer_Modify(m_iIndex, &m_TimerInfo) == 0);
	}
	else
		return false; // now we're confused


}

bool Timer::ShrinkToRemoveEvent(const EPGevent* pEvent)
{
	int iStartPadding = pEvent->GetStart() - m_time.Start();
	int iEndPadding = m_time.End() - pEvent->GetEnd();

	iStartPadding = max(0, iStartPadding - GetMinProgramLength());
	iEndPadding = max(0, iEndPadding - GetMinProgramLength());

	if (iStartPadding<GetDefaultStartPadding())
	{
		// cut off beginning from timer
		PackedDateTime newStart = pEvent->GetEnd()-GetDefaultStartPadding();
		m_TimerInfo.startTime = newStart.AsDateTime();
		m_TimerInfo.duration = (word) (m_time.End() - newStart);
		return (TAP_Timer_Modify(m_iIndex, &m_TimerInfo) == 0);
	}
	else if (iEndPadding<GetDefaultEndPadding())
	{
		// cut end beginning from timer
		PackedDateTime newEnd = pEvent->GetStart()+GetDefaultEndPadding();
		m_TimerInfo.duration = (word) (newEnd - m_time.Start());
		return (TAP_Timer_Modify(m_iIndex, &m_TimerInfo) == 0);
	}
	else
		return false; // we'd have to cut a section out of the middle.
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


string Timer::Description() const
{
	string result;
	EPGchannel* pChannel = Globals::GetEPGdata()->GetChannel(m_iLogicalChannelNum);
	if (pChannel)
	{
		EPGevent* pEvent = pChannel->FindEvent(m_time.Start() + GetDefaultStartPadding());
		if (pEvent != NULL)
			result = pEvent->GetTitle();

		if (result.size()>25)
			result = result.substr(0,20) + "...";
	}

	result+= "(";
	result+= (m_iLogicalChannelNum == -1 ? string("????") : Globals::GetChannels()->NameForLCN(m_iLogicalChannelNum));
	result += ":" + m_time.Description() + ")";

	return result;
}

bool Timer::ScheduleEvent(const EPGevent* pEvent)
{
	TYPE_TimerInfo info;
	memset(&info, 0, sizeof(TYPE_TimerInfo));
	info.duration = (word) (pEvent->Duration() + GetDefaultEndPadding() + GetDefaultStartPadding());
	info.startTime = (pEvent->GetStart() - GetDefaultStartPadding()).AsDateTime();
	info.isRec = 1;
	info.reservationType = RESERVE_TYPE_Onetime;
	info.svcNum = SVC_TYPE_Tv;
	info.tuner = 3;
	info.svcNum = (word) (Globals::GetChannels()->LogicalToToppy(pEvent->GetChannelNum()));
	info.nameFix = 1;
	strncpy(info.fileName, pEvent->GetFileName(), 95);
	strcat(info.fileName, ".rec");

	int iResult = TAP_Timer_Add(&info);
	return iResult == 0;
}