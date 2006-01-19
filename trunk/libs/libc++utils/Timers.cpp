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
#include ".\timers.h"
#include "Timer.h"
#include "EPGevent.h"
#include "Channels.h"
#include "Globals.h"

bool Timers::m_mergeFlag = false;
int Timers::m_prePadding = 0;
int Timers::m_postPadding = 0;
bool Timers::m_bTreatVCRAsRecord = false;

Timers::Timers(void)
{
	Refresh();
}

Timers::~Timers(void)
{
	Cleanup();
}


void Timers::Refresh()
{
	Cleanup();

	int iCount = TAP_Timer_GetTotalNum();
	for (int i=0; i<iCount; i++)
	{
		TYPE_TimerInfo info;
		TAP_Timer_GetInfo(i, &info);
		m_timers.push_back(new Timer(i,&info));
	}
}

void Timers::Cleanup()
{
	for (unsigned int i=0; i<m_timers.size(); i++)
		delete m_timers[i];

	m_timers.clear();
}

array<Timer*> Timers::GetTimers()
{
	return m_timers;
}

bool Timers::IsScheduled(const EPGevent* pEvent)
{
	for (unsigned int i=0; i<m_timers.size(); i++)
	{
		if (m_timers[i]->SchedulesEvent(pEvent))
			return true;
	}
	return false;
}

bool Timers::ScheduleEvent(const EPGevent* pEvent)
{
	array<Timer*> overlaps = GetOverlappingTimers(pEvent);
	for (unsigned int i=0; i<overlaps.size(); i++)
	{
		if ((overlaps[i]->GetLogicalChannelNum() == pEvent->GetChannelNum()) && ShouldMergePrograms())
		{
			if (overlaps[i]->ExtendToCoverEvent(pEvent))
			{
				Refresh();
				return true;
			}
		}
	}
#ifdef WIN32
	if (GetClashingTimers(pEvent).size()>=2)
		return false; // emulator will not return error on clash
#endif

	if (Timer::ScheduleEvent(pEvent))
	{
		Refresh();
		return true;
	}

	return false;
}

array<Timer*> Timers::GetOverlappingTimers(const EPGevent* pEvent) const
{
	array<Timer*> clashingTimers;

	TimeSlot paddedEventTimeSlot = pEvent->GetTimeSlot();
	paddedEventTimeSlot.Pad(m_prePadding, m_postPadding);

	for (unsigned int i=0; i<m_timers.size(); i++)
	{
		if (m_timers[i]->GetTimeSlot().OverlapsWith(paddedEventTimeSlot))
		{
			clashingTimers.push_back(m_timers[i]);
		}
	}

	return clashingTimers;
}

bool Timers::CancelEvent(const EPGevent* pEvent)
{
	for (unsigned int i=0; i<m_timers.size(); i++)
	{
		if (m_timers[i]->SchedulesEvent(pEvent))
		{
			bool bResult = false;
			if (m_timers[i]->SchedulesOtherEventsToo(pEvent))
				bResult = m_timers[i]->ShrinkToRemoveEvent(pEvent);
			else
				bResult = TAP_Timer_Delete(i);
			Refresh();
			return bResult;
		}
	}
	return false;

}

bool Timers::CanRecord(const EPGevent* pEvent) const
{
	array<Timer*> overlaps = GetClashingTimers(pEvent);

	if (overlaps.size()<2)
		return true;

	if (overlaps.size()>2)
		return false;

	if ((overlaps[0]->GetLogicalChannelNum() == pEvent->GetChannelNum()) && ShouldMergePrograms())
		return true;

	if ((overlaps[1]->GetLogicalChannelNum() == pEvent->GetChannelNum()) && ShouldMergePrograms())
		return true;

	return false;
}

array<Timer*> Timers::GetClashingTimers(const EPGevent* pEvent) const
{
	array<Timer*> overlaps = GetOverlappingTimers(pEvent);

	// if we have 0 or 1 overlaps, just return that - 
	// 0 means both tuners are free the whole period
	// 1 means that there is one free tuner or more for the whole period
	if (overlaps.size()<2)
		return overlaps; 

	array<Timer*> results;

	// if we have 2+ overlaps, we have to check if any of them overlap with
	// each other - as any such would use both tuners and stop the event
	// from being scheduled

	for (unsigned int i=0; i<overlaps.size(); i++)
	{
		for (unsigned int j=i+1; j<overlaps.size(); j++)
		{
			if (overlaps[i]->GetTimeSlot().OverlapsWith(overlaps[j]->GetTimeSlot()))
			{
				results.push_back(overlaps[i]);
				results.push_back(overlaps[j]);
				return results;
			}
		}
	}

	// nothing stops the recording, but put one timer in the results to
	// indicate that this timeslot may not be the best to record the programme
	// if more than one showing
	results.push_back(overlaps[0]);
	return results;
}

bool Timers::ShouldMergePrograms()
{
	return m_mergeFlag;
}

