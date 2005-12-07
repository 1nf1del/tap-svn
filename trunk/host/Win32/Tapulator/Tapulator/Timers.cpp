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
#include ".\timers.h"
#include "MJDUtil.h"

template<typename T> void Zeromemory(T& thing)
{
	ZeroMemory(&thing, sizeof(T));
}

Timers::Timers(void)
{
	TYPE_TimerInfo info;
	Zeromemory(info);	
	info.duration = 30;
	strcpy(info.fileName,"Some Recording.rec");
	info.isRec = 1; // recording, not just programmed
	info.nameFix = false;
	info.reservationType = RESERVE_TYPE_Onetime;
	info.startTime = (MJDUtil::Today() << 16) + (23 << 8) + 30;
	info.svcType = SVC_TYPE_Tv;
	info.svcNum = 0;
	info.tuner = 3;

	Add(&info);
	++info.svcNum;
	strcpy(info.fileName,"The House of Tiny Tearaw_12-06.rec");
	Add(&info);
	++info.svcNum;
	Add(&info);
	++info.svcNum;
	Add(&info);
	++info.svcNum;
	Add(&info);
	++info.svcNum;
	Add(&info);
}

Timers::~Timers(void)
{
}

int Timers::GetTotalNum(void )
{
	return m_vecTimers.size();
}

bool Timers::GetInfo(unsigned int entryNum, TYPE_TimerInfo *info )
{
	if (entryNum<0)
		return false;

	if (entryNum >= m_vecTimers.size())
		return false;

	memcpy(info, &m_vecTimers[entryNum], sizeof(TYPE_TimerInfo));
	return true;
}

int Timers::Add(TYPE_TimerInfo *info )
{
	m_vecTimers.push_back(*info);
	return 0; // ought to check for overlap
}

int Timers::Modify(unsigned int entryNum, TYPE_TimerInfo *info )
{
	if (entryNum<0)
		return 1;

	if (entryNum >= m_vecTimers.size())
		return 1;

	m_vecTimers[entryNum] = *info;
	return 0; // ought to check for overlap/bad tuner
}

bool Timers::Delete(unsigned int entryNum )
{
	if (entryNum<0)
		return false;

	if (entryNum >= m_vecTimers.size())
		return false;

	m_vecTimers.erase(m_vecTimers.begin() + entryNum);
	return true;
}
