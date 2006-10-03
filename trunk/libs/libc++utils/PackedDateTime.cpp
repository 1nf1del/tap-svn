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
#include "packeddatetime.h"
#include "tapstring.h"

PackedDateTime::PackedDateTime(void)
{
}

PackedDateTime::~PackedDateTime(void)
{
}

PackedDateTime::PackedDateTime(const string& sYMDHM)
{
	if (sYMDHM.size() != 12)
		return;
	int y = atoi(sYMDHM.substr(0,4));	
	int m = atoi(sYMDHM.substr(4,2));
	int d = atoi(sYMDHM.substr(6,2));
	int h = atoi(sYMDHM.substr(8,2));
	int min = atoi(sYMDHM.substr(10,2));

	m_wMJD = (word) TAP_MakeMjd((word) y,(byte) m,(byte) d);
	m_wTime = (word) ((h<<8) + min);

}

PackedDateTime::PackedDateTime(dword dwDateTime)
{
	m_wMJD = (word)(dwDateTime >> 16);
	m_wTime = (word)(dwDateTime & 0xFFFF);
}

PackedDateTime::PackedDateTime(word wMJD, word wTime)
{
	m_wMJD = wMJD;
	m_wTime = wTime;
}


string PackedDateTime::ShortDisplay() const
{
	word year;
	byte month, day, dofy;
	TAP_ExtractMjd(m_wMJD, &year, &month, &day, &dofy);

	string sResult;
	sResult.format("%s %d/%d %02d:%02d", DayOfWeek(true).c_str(), day, month, m_wTime>>8, m_wTime & 0xFF);

	return sResult;
}

string PackedDateTime::ShortDisplayRange(const PackedDateTime& end) const
{
	word year;
	byte month, day, dofy;
	TAP_ExtractMjd(m_wMJD, &year, &month, &day, &dofy);

	string sResult;
	sResult.format("%s%d %02d:%02d+%d", DayOfWeek(true).c_str(), day, m_wTime>>8, m_wTime & 0xFF, 
		end-*this);

	return sResult;
}

string PackedDateTime::LongDisplayRange(const PackedDateTime& end) const
{
	word year;
	byte month, day, dofy;
	TAP_ExtractMjd(m_wMJD, &year, &month, &day, &dofy);

	string sResult;
	sResult.format("%s %d/%d %02d:%02d-%02d:%02d", DayOfWeek(true).c_str(), day, month, m_wTime>>8, m_wTime & 0xFF, 
		end.m_wTime >> 8, end.m_wTime & 0xFF );

	return sResult;
}

char* ShortWeekDay(byte dofy)
{
	switch(dofy)
	{
	case 0:
		return "Mon";
	case 1:
		return "Tue";
	case 2:
		return "Wed";
	case 3:
		return "Thu";
	case 4:
		return "Fri";
	case 5:
		return "Sat";
	case 6:
		return "Sun";
	}
	return "???";
}

char* LongWeekDay(byte dofy)
{
	switch(dofy)
	{
	case 0:
		return "Monday";
	case 1:
		return "Tuesday";
	case 2:
		return "Wednesday";
	case 3:
		return "Thursday";
	case 4:
		return "Friday";
	case 5:
		return "Saturday";
	case 6:
		return "Sunday";
	}
	return "???";
}

string PackedDateTime::DayOfWeek(bool bShort) const
{
	word year;
	byte month, day, dofw;

	TAP_ExtractMjd(m_wMJD, &year, &month, &day, &dofw);

	return bShort? ShortWeekDay(dofw) : LongWeekDay(dofw);
}

string PackedDateTime::DisplayTimeRange(const PackedDateTime& end) const
{
	string sResult;
	sResult.format("%02d:%02d+%d", m_wTime>>8, m_wTime & 0xFF, 
		end-*this);
	return sResult;
}

PackedDateTime PackedDateTime::Now()
{
	word wMJD;
	byte hour, min, sec;
	TAP_GetTime(&wMJD, &hour, &min, &sec);
	return PackedDateTime(wMJD, (hour<<8) + min);
}

bool PackedDateTime::IsInPast() const
{
	 return (*this)<Now();
}

bool PackedDateTime::operator<(const PackedDateTime& right) const
{
	return this->AsDateTime() < right.AsDateTime();
}

bool PackedDateTime::operator<=(const PackedDateTime& right) const
{
	return this->AsDateTime() <= right.AsDateTime();
}

bool PackedDateTime::operator>(const PackedDateTime& right) const
{
	return this->AsDateTime() > right.AsDateTime();
}

bool PackedDateTime::operator>=(const PackedDateTime& right) const
{
	return this->AsDateTime() >= right.AsDateTime();
}

PackedDateTime PackedDateTime::operator +(short int duration) const
{
	int minutes = (m_wTime & 0xFF) + duration;
	int hours = (m_wTime >> 8) + (minutes / 60);
	minutes = minutes % 60;
	if (minutes<0)
	{
		hours--;
		minutes+=60;
	}
	word mjd = (word) (m_wMJD + hours / 24);
	hours = hours % 24;
	if (hours<0)
	{
		mjd--;
		hours+=24;
	}
	PackedDateTime result((mjd<<16) + (hours<<8) + minutes);
	return result;
}

bool PackedDateTime::operator==(const PackedDateTime& right) const
{
	return this->AsDateTime() == right.AsDateTime();
}

dword PackedDateTime::AsDateTime() const
{
	return (m_wMJD << 16) + m_wTime;
}

int PackedDateTime::operator-(const PackedDateTime& right) const
{
	int diff = (m_wMJD - right.m_wMJD) * 24 * 60;
	diff += ((m_wTime >> 8) - (right.m_wTime >> 8)) * 60;
	diff += (m_wTime & 0xFF) - (right.m_wTime & 0xFF);

	return diff;
}

PackedDateTime PackedDateTime::operator-(short int right) const
{
	return *this + (-right);
}

word PackedDateTime::GetMJD() const
{
	return m_wMJD;
}

void PackedDateTime::SetMJD(word mjd)
{
	m_wMJD = mjd;
}
