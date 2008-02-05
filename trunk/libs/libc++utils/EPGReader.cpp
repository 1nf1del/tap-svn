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

IEPGReader::IEPGReader()
{
	m_pdtNow = PackedDateTime::Now();
}

IEPGReader::~IEPGReader() 
{

}
bool IEPGReader::Read(EPGdata& epgdata, int maxRowsThisChunk)
{
	epgdata;
	maxRowsThisChunk;
	return false;
}
short int IEPGReader::GetPercentDone()
{
	return 0;
}
bool IEPGReader::CanRead() 
{
	return false;
}

bool IEPGReader::ReadChannel(EPGdata& epgdata, int channelNum)
{
	epgdata;
	channelNum;
	return false;

}

bool IEPGReader::IsEventWantedInEPG(EPGevent* pEvent)
{
	if (pEvent->GetEnd().IsInPast())
		return false;

	if (pEvent->GetChannelNum() == 0xFFFF)
		return false; // for a channel that we don't have 

	PackedDateTime pdtEnd = m_pdtNow;
	pdtEnd.SetMJD(pdtEnd.GetMJD() + EPGevent::GetDaysToLoad());

	if (pEvent->GetStart()>pdtEnd)
		return false;

	return true;

}

EPGReader::EPGReader(bool bUseExtendedInfo, dword dwFlags)
{
	m_iTotalChan = Globals::GetTheGlobals().GetChannels()->GetCount();
	m_iCurrentChan = 0;
	m_bUseExtendedInfo = bUseExtendedInfo;
	m_dwFlags = dwFlags;
}

EPGReader::~EPGReader(void)
{
}

bool EPGReader::CanRead()
{
	return true;
}

EPGevent* EPGReader::BuildEvent(TYPE_TapEvent* pTapEvent, int iChannelNum)
{
	char* pExtData = (char*) (m_bUseExtendedInfo ? TAP_EPG_GetExtInfo(pTapEvent) : 0);
	EPGevent* pNewEvent = new EPGevent(pTapEvent, iChannelNum, pExtData);
	if (pExtData)
		TAP_MemFree(pExtData);
	return pNewEvent;
}

bool EPGReader::Read(EPGdata& epgdata, int maxRowsThisChunk)
{
	maxRowsThisChunk;
//	int iCount = 0;

	ReadChannel(epgdata, m_iCurrentChan);

	m_iCurrentChan++;

//	maxRowsThisChunk-=iCount;

	if ((m_iCurrentChan<m_iTotalChan))
		return true;

	return false;
}

bool EPGReader::ReadChannel(EPGdata& epgdata, int channelNum)
{
	int iCount = 0;
	TYPE_TapEvent* pEvents = TAP_GetEvent(SVC_TYPE_Tv, channelNum, &iCount);

	for (int i=0; i<iCount; i++)
	{
		EPGevent* pNewEvent = BuildEvent(&pEvents[i], channelNum);
		if (!IsEventWantedInEPG(pNewEvent))
		{
			delete pNewEvent;
		}
		else
		{
			epgdata.AddEvent(pNewEvent);
		}
	}

	TAP_MemFree(pEvents);

	return (iCount>0);
}

short int EPGReader::GetPercentDone()
{
	return (short int) ( m_iCurrentChan * 100 / (m_iTotalChan+1));
}

