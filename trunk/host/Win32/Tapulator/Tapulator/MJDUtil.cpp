//Tapulator - An emulator for the Topfield PVR TAP API
//Copyright (C) 2005  Robin Glover
//
//This program is free software; you can redistribute it and/or
//modify it under the terms of the GNU General Public License
//as published by the Free Software Foundation; either version 2
//of the License, or (at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program; if not, write to the Free Software
//Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
//
// You can contact the author via email at robin.w.glover@gmail.com

#include "StdAfx.h"
#include ".\mjdutil.h"

MJDUtil::MJDUtil(void)
{
}

MJDUtil::~MJDUtil(void)
{
}


bool MJDUtil::ExtractMJD(WORD mjd, WORD& year, BYTE& month, BYTE& day, BYTE& weekday)
{
	int a = (int) (mjd+2400000.5) + 32044;
	int b = (4*a+3)/146097;
	int c = a - (b*146097)/4;

	int d = (4*c+3)/1461;
	int e = c - (1461*d)/4;
	int m = (5*e+2)/153;

	day   = e - (153*m+2)/5 + 1;
	month = m + 3 - 12*(m/10);
	year  = b*100 + d - 4800 + m/10;

	CTime tm(year, month, day, 0, 0, 0);
	weekday = (tm.GetDayOfWeek() + 5) % 7;

	return true;
}

WORD MJDUtil::MakeMJD(WORD year, BYTE month, BYTE day)
{
	int a = (14-month)/12;
	int y = year+4800-a;
	int m = month + 12*a - 3;

	//  For a date in the Gregorian calendar:
	int JD = day + (153*m+2)/5 + y*365 + y/4 - y/100 + y/400 - 32045;

	return JD - 2400000;
}

WORD MJDUtil::Today()
{
	SYSTEMTIME time;
	GetSystemTime(&time);
	return MakeMJD(time.wYear, (BYTE)time.wMonth, (BYTE)time.wDay);
}


DWORD MJDUtil::DateTimeNowAsDWORD()
{
	SYSTEMTIME time;
	GetSystemTime(&time);

	DWORD dwResult = (Today() << 16) + (time.wHour << 8) + time.wMinute;

	return dwResult;


}
