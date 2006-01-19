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
#include ".\epgchannel.h"
#include "epgevent.h"
#include "PackedDateTime.h"
#include "EPGvisitor.h"

EPGchannel::EPGchannel(word channelNum) : m_iChannelNum(channelNum)
{
}

EPGchannel::~EPGchannel(void)
{
	for (unsigned int i=0; i<m_Events.size(); i++)
	{
		delete m_Events[i];
	}
}

word EPGchannel::GetChannelNum()
{
	return m_iChannelNum;
}

const array<EPGevent*>& EPGchannel::GetEvents() const
{
	return m_Events;
}

void EPGchannel::AddEvent(EPGevent* newEvent)
{
	m_Events.push_back(newEvent);
}


void EPGchannel::ScanForSplitPrograms()
{
	for (unsigned int i=0; i+2<m_Events.size(); i++)
	{
		if (m_Events[i+2]->IsContinuationOf(m_Events[i]))
		{
			m_Events[i]->SetContinuation(m_Events[i+2]);
		}
	}
}

EPGevent* EPGchannel::FindEvent(const PackedDateTime& datetime) const
{
	for (unsigned int i=0; i<m_Events.size(); i++)
	{
		if ((m_Events[i]->GetStart()<=datetime) 
			&& (m_Events[i]->GetEnd()>datetime))
			return m_Events[i];
	}
	return 0;
}

bool EPGchannel::Visit(EPGVisitor* pVisitor) const
{
	for (unsigned int i=0; i<m_Events.size(); i++)
	{
		if (!pVisitor->VisitEvent(m_Events[i]))
			return false;
	}
	return true;
}
