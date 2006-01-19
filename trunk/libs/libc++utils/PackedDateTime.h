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
#ifndef cpputils_packeddatetime_h
#define cpputils_packeddatetime_h
#include <stdlib.h>
#include "tap.h"
class string;

class PackedDateTime
{
public:
	PackedDateTime(void);
	~PackedDateTime(void);
	PackedDateTime(const string& sYMDHM);
	PackedDateTime(dword dwDateTime);

	string ShortDisplay() const;
	bool IsInPast() const;
	string ShortDisplayRange(const PackedDateTime& end) const;
	string DisplayTimeRange(const PackedDateTime& end) const;
	string DayOfWeek(bool bShort) const;

	bool operator<(const PackedDateTime& right) const;
	bool operator<=(const PackedDateTime& right) const;
	bool operator>(const PackedDateTime& right) const;
	bool operator>=(const PackedDateTime& right) const;
	bool operator==(const PackedDateTime& right) const;

	PackedDateTime operator+(short int duration) const;

	int operator-(const PackedDateTime& right) const;
	PackedDateTime operator-(short int right) const;
	dword AsDateTime() const;

	word GetMJD() const;
	void SetMJD(word mjd); 

private:


	word m_wMJD;
	word m_wTime;
};

#endif