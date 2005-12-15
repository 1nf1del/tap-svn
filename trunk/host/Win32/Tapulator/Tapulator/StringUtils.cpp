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

#include "stdafx.h"
#include "StringUtils.h"


int Split(const CString& str, LPCTSTR delimiter, CStringArray& strArray)
{
	int start = 0;
	int delim = str.Find(delimiter);
	int delimLen = _tcslen(delimiter);
	int elemCnt = 1; // the ACTUAL number of items, there'll be at least 1

	// counting the elements, setting the size and then filling the array
	// is much faster than doing .Add for each new element
	while (delim > -1)
	{
		elemCnt++;
		start = delim + delimLen;
		delim = str.Find(delimiter, start);
	}

	// manually going through and finding each delimiter again is faster than
	// keeping track of the positions from the last loop.. because doing .Add over and over
	// to the position array would be such a bottleneck
	start = 0;
	delim = str.Find(delimiter);
	strArray.SetSize(elemCnt); // now we don't have to use .Add, saving tons of cpu cycles
	elemCnt = -1;

	while (delim > -1)
	{
		elemCnt++;
		strArray[elemCnt] = str.Mid(start, delim-start);
		start = delim + delimLen;
		delim = str.Find(delimiter, start);
	}

	if (start < str.GetLength())
		strArray[elemCnt+1] = str.Mid(start);
	else
		strArray[elemCnt+1] = "";

	return elemCnt+1;
}
