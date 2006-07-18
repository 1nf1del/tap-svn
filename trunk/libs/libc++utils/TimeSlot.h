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
#ifndef cpputils_timeslot_h
#define cpputils_timeslot_h
#include <stdlib.h>
#include "PackedDateTime.h"
#include "tapstring.h"

class TimeSlot
{
public:
	TimeSlot(const PackedDateTime& start, const PackedDateTime& end);
	TimeSlot(const PackedDateTime& start, word duration);
	TimeSlot(dword startDateTime, word duration);
	TimeSlot(const TimeSlot& repeatingSlot, word wSpecificMJD);
	TimeSlot();
	~TimeSlot(void);

	const PackedDateTime& Start() const;
	const PackedDateTime& End() const;
	bool OverlapsWith(const TimeSlot& other) const;
	bool Contains(const TimeSlot& other) const;
	word Duration() const;
	word OverLapLength(const TimeSlot& other) const;
	void Pad(int start, int end);

	void SetStart(const PackedDateTime& start);
	void SetEnd(const PackedDateTime& end);
	void SetDuration(word duration);
	string Description() const;
	void SetRepeat(byte repeat);

	TimeSlot GetFirstInstance() const;

private:
	word GetDaysOfWeek() const;
	bool OverlapsBothRepeat(const TimeSlot& other) const;
	bool OverlapsOneRepeats(const TimeSlot& repeatingslot) const;
	bool SpansMidnight() const;
	PackedDateTime m_start;
	PackedDateTime m_end;
	TYPE_ReservationType m_repeat;
};
#endif