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
#include ".\timeslot.h"

TimeSlot::TimeSlot()
{
	m_repeat = RESERVE_TYPE_Onetime;
}

TimeSlot::TimeSlot(const PackedDateTime& start, const PackedDateTime& end)
{
	m_repeat = RESERVE_TYPE_Onetime;
	m_start = start;
	m_end = end;
	m_repeat = RESERVE_TYPE_Onetime;
}

TimeSlot::TimeSlot(const PackedDateTime& start, word duration)
{
	m_repeat = RESERVE_TYPE_Onetime;
	m_start = start;
	m_end = start + duration;
}

TimeSlot::TimeSlot(dword startDateTime, word duration) : m_start(startDateTime)
{
	m_repeat = RESERVE_TYPE_Onetime;
	m_end = m_start + duration;
}

TimeSlot::TimeSlot(const TimeSlot& repeatingSlot, word wSpecificMJD)
{
	*this = repeatingSlot;
	m_repeat = RESERVE_TYPE_Onetime;
	m_start.SetMJD(wSpecificMJD);
	m_end.SetMJD(wSpecificMJD + (repeatingSlot.SpansMidnight() ? 1 : 0));
}

TimeSlot::~TimeSlot(void)
{

}

const PackedDateTime& TimeSlot::Start() const
{
	return m_start;
}

const PackedDateTime& TimeSlot::End() const
{
	return m_end;
}

bool TimeSlot::OverlapsWith(const TimeSlot& other) const
{
	if ((m_repeat!=RESERVE_TYPE_Onetime) && (other.m_repeat!=RESERVE_TYPE_Onetime))
		return OverlapsBothRepeat(other);

	if (other.m_repeat!=RESERVE_TYPE_Onetime)
		return OverlapsOneRepeats(other);

	if (m_repeat != RESERVE_TYPE_Onetime)
		return other.OverlapsOneRepeats(*this);

	return ((Start()<other.End()) && (End()>other.Start()));
}

bool TimeSlot::Contains(const TimeSlot& other) const
{
	return ((Start()<=other.Start()) && (End()>=other.End()));
}

void TimeSlot::SetStart(const PackedDateTime& start)
{
	m_start = start;
}

void TimeSlot::SetEnd(const PackedDateTime& end)
{
	m_end = end;
}

void TimeSlot::SetDuration(word duration)
{
	m_end = m_start + duration;
}

word TimeSlot::Duration() const
{
	return (word) (m_end - m_start);
}

void TimeSlot::Pad(int start, int end)
{
	m_start = m_start + (short) (-start);
	m_end = m_end + (short) end;
}

word TimeSlot::OverLapLength(const TimeSlot& other) const
{
	PackedDateTime overlapStart = max(m_start, other.m_start);
	PackedDateTime overlapEnd = min(m_end, other.m_end);

	return (word) (overlapEnd - overlapStart);
}

string TimeSlot::Description() const
{
	switch (m_repeat)
	{
	case RESERVE_TYPE_Everyday:
		return "Daily "+m_start.DisplayTimeRange(m_end);		
	case RESERVE_TYPE_EveryWeekend:
		return "Weekends "+m_start.DisplayTimeRange(m_end);		
	case RESERVE_TYPE_WeekDay:
		return "Weekdays "+m_start.DisplayTimeRange(m_end);		
	case RESERVE_TYPE_Weekly:
		return m_start.DayOfWeek(false)+"s "+m_start.DisplayTimeRange(m_end);
	default:
	case RESERVE_TYPE_Onetime:
		return m_start.ShortDisplayRange(m_end);
	}

}	

word TimeSlot::GetDaysOfWeek() const
{
	switch (m_repeat)
	{
	case RESERVE_TYPE_Everyday:
		return 0x7F;
	case RESERVE_TYPE_EveryWeekend:
		return 0x60;
	case RESERVE_TYPE_WeekDay:
		return 0x1F;
	case RESERVE_TYPE_Onetime:
	case RESERVE_TYPE_Weekly:
		{
			word year;
			byte month, day, dofw;
			TAP_ExtractMjd(m_start.GetMJD(), &year, &month, &day, &dofw);
			return (1<<dofw);
		}
	}
	return 0;
}

bool TimeSlot::OverlapsBothRepeat(const TimeSlot& other) const
{
	// incomplete, but good enough... we assume that to get here we are dealing
	// with two slots that are both known to individuall overlap with a program
	// to be recorded. Therefore they must both occur on the same day, or just each side of midnight
	// it's sufficient to check the times for overlap
	// with the added caveat that if one overlaps midnight and the other
	// doesn't we must check the next days version of the non-midnight spanning
	// slot too....

	TimeSlot tempthis(*this, 0);
	TimeSlot tempother(other, 0);

	if (tempthis.OverlapsWith(tempother))
		return true;

	if (tempthis.SpansMidnight() && !tempother.SpansMidnight())
	{
		TimeSlot tempother2(tempother,1);
		return tempthis.OverlapsWith(tempother2);
	}
	if (!tempthis.SpansMidnight() && tempother.SpansMidnight())
	{
		TimeSlot tempthis2(tempthis,1);
		return tempthis2.OverlapsWith(tempother);
	}
	return false;
}

bool TimeSlot::OverlapsOneRepeats(const TimeSlot& repeatingslot) const
{
	word daysOther = repeatingslot.GetDaysOfWeek();
	word daysThis = GetDaysOfWeek();
	if (SpansMidnight())
	{
		word daysTomorrow = daysThis << 1;
		if (daysTomorrow & 0x80) 
			daysTomorrow |= 0x01;

		if (daysOther&daysThis)
		{
			// the repeating slot includes tomorrow - do we overlap with it?
			TimeSlot temp(repeatingslot,m_start.GetMJD() + 1);
			if (OverlapsWith(temp))
				return true;
		}
	}

	if ((daysOther&daysThis) == 0)
		return false; // occur on different days of week, cannot clash

	TimeSlot temp(repeatingslot, m_start.GetMJD());
	return OverlapsWith(temp);
}

bool TimeSlot::SpansMidnight() const
{
	return m_end.GetMJD()>m_start.GetMJD();
}

void TimeSlot::SetRepeat(byte repeat)
{
	m_repeat = (TYPE_ReservationType) repeat;
}