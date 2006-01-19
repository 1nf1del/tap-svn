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
#include ".\epgdata.h"
#include "epgchannel.h"
#include "epgevent.h"
#include "Globals.h"
#include "Channels.h"

EPGdata::EPGdata(void)
{
}

EPGdata::~EPGdata(void)
{
	for (unsigned int i=0; i<m_channels.size(); i++)
	{
		delete m_channels[i];
	}
}

void EPGdata::AddEvent(EPGevent* newEvent)
{
	if (Globals::GetChannels()->LogicalToToppy(newEvent->GetChannelNum())== -1)
	{
		delete newEvent;
		return;
	}
	FindChannelByNum(newEvent->GetChannelNum())->AddEvent(newEvent);
}

EPGchannel* EPGdata::FindChannelByNum(word channelNum) 
{
	EPGchannel* pChannel = GetChannel(channelNum);
	if (pChannel)
		return pChannel;

	EPGchannel* pNewChannel = new EPGchannel(channelNum);
	m_channels.push_back(pNewChannel);
	return pNewChannel;
}

const array<EPGevent*>& EPGdata::GetDataForChannel(int channelNum) const
{
	if (GetChannel(channelNum) == 0)
		return m_emptyData;

	return GetChannel(channelNum)->GetEvents();
}


void EPGdata::CheckForContinuedPrograms()
{
	for (unsigned int i = 0; i< m_channels.size(); i++)
	{
		m_channels[i]->ScanForSplitPrograms();
	}
}

array<string> EPGdata::GetGenres()
{
	array<string> results;
	for (unsigned int i = 0; i< m_channels.size(); i++)
	{
		const array<EPGevent*>& events = m_channels[i]->GetEvents();
		for (unsigned int j=0; j<events.size(); j++)
		{
			string genre = events[j]->GetGenre();
			if (!results.contains(genre))
				results.push_back(genre);
		}
	}
	return results;
}

const EPGchannel* EPGdata::GetChannel(int channelNum) const
{
	for (int i = m_channels.size()-1; i>=0; i--)
	{
		if (m_channels[i]->GetChannelNum() == channelNum)
			return m_channels[i];
	}

	return 0;

}

EPGchannel* EPGdata::GetChannel(int channelNum)
{
	for (int i = m_channels.size()-1; i>=0; i--)
	{
		if (m_channels[i]->GetChannelNum() == channelNum)
			return m_channels[i];
	}

	return 0;

}

const array<EPGchannel*> EPGdata::GetChannels() const
{
	return m_channels;
}

bool EPGdata::Visit(EPGVisitor* pVisitor) const
{
	for (unsigned int i = 0; i< m_channels.size(); i++)
	{
		if (!m_channels[i]->Visit(pVisitor))
			return false;
	}
	return true;
}
