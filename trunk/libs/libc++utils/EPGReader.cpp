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
#include "EPGreader.h"
#include "EPGevent.h"
#include "tapstring.h"
#include "taparray.h"
#include "EPGdata.h"
#include "globals.h"
#include "channels.h"

IEPGReader::~IEPGReader() 
{

}
bool IEPGReader::Read(EPGdata& epgdata, int maxRowsThisChunk)
{
	return false;
}
int IEPGReader::GetPercentDone()
{
	return 0;
}
bool IEPGReader::CanRead() 
{
	return false;
}


EPGReader::EPGReader(void)
{
	m_iTotalChan = Globals::GetChannels()->GetCount();
	m_iCurrentChan = 0;
}

EPGReader::~EPGReader(void)
{
}

bool EPGReader::CanRead()
{
	return true;
}

bool EPGReader::Read(EPGdata& epgdata, int maxRowsThisChunk)
{
	int iCount = 0;
	TYPE_TapEvent* pEvents = TAP_GetEvent(SVC_TYPE_Tv, m_iCurrentChan, &iCount);

	int iLogicalChan = Globals::GetChannels()->ToppyToLogical(m_iCurrentChan);

	for (int i=0; i<iCount; i++)
	{
		EPGevent* pNewEvent = new EPGevent(&pEvents[i], iLogicalChan);
		if (pNewEvent->GetEnd().IsInPast())
		{
			delete pNewEvent;
		}
		else
		{
			epgdata.AddEvent(pNewEvent);
		}
	}

	TAP_MemFree(pEvents);

	m_iCurrentChan++;

	maxRowsThisChunk-=iCount;

	if ((m_iCurrentChan<m_iTotalChan))
		return true;

	return false;
}

int EPGReader::GetPercentDone()
{
	return m_iCurrentChan * 100 / (m_iTotalChan+1);
}

