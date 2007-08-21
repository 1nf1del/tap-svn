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
#include "epgdata.h"
#include "epgchannel.h"
#include "epgevent.h"
#include "Globals.h"
#include "Channels.h"
#include "MEIReader.h"
#include "EPGReader.h"
#include "ProgressNotification.h"
#include "Logger.h"
#include "JagCSVReader.h"

EPGdata::EPGVisitStatus::EPGVisitStatus()
{
	m_iNextChannel = 0;
}

EPGdata::EPGdata(void)
{
	m_pReader = NULL;
}

EPGdata::~EPGdata(void)
{
	for (unsigned int i=0; i<m_channels.size(); i++)
	{
		delete m_channels[i];
	}

	delete m_pReader;
}

void EPGdata::AddEvent(EPGevent* newEvent)
{
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
	array<string> lresults;
	for (unsigned int i = 0; i< m_channels.size(); i++)
	{
		const array<EPGevent*>& events = m_channels[i]->GetEvents();
		for (unsigned int j=0; j<events.size(); j++)
		{
			string genre = events[j]->GetGenre();
			if (!results.contains(genre))
			{
				string lgenre = genre.tolower();
				if (lresults.contains(lgenre))
					continue;
				results.push_back(genre);
				lresults.push_back(lgenre);
			}
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
	EPGVisitStatus status;
	while (PartVisit(pVisitor, status))
	{
	}
	return true;
}

bool EPGdata::PartVisit(EPGVisitor* pVisitor, EPGVisitStatus& status) const
{
	if (status.m_iNextChannel < m_channels.size())
	{
		if (!m_channels[status.m_iNextChannel++]->Visit(pVisitor))
			return false;
	}


	return status.m_iNextChannel < m_channels.size();
}

bool EPGdata::HasData() const
{
	return m_channels.size()>0;
}

bool EPGdata::ReadSomeData(ProgressNotification* pProgress, int iCount)
{
	if (m_pReader->CanRead())
	{

		if (m_pReader->Read(*this, iCount))
		{
			if (pProgress)
				pProgress->Step(m_pReader->GetPercentDone());

			return true;
		}

		return false;
	}

	return false;
}

bool EPGdata::FinishReading(ProgressNotification* pProgress)
{


	if (pProgress)
		pProgress->Step(100);

	CheckForContinuedPrograms();

	if (pProgress)
		pProgress->Finish();

	TRACE("Loaded EPG data\n");
	TRACE_MEMORY();

	delete m_pReader;
	m_pReader = NULL;

	return true;

}

bool EPGdata::TryReadingBuiltin(ProgressNotification* pProgress)
{
	delete m_pReader;
	m_pReader = new EPGReader(false);
	return ReadSomeData(pProgress,1);
}

bool EPGdata::TryReadingExtendedBuiltin(ProgressNotification* pProgress)
{
	delete m_pReader;
	m_pReader = new EPGReader(true);
	return ReadSomeData(pProgress,1);
}

bool EPGdata::TryReadingMei(ProgressNotification* pProgress)
{
	delete m_pReader;
	m_pReader = new MEIReader();
	return ReadSomeData(pProgress,1);
}

bool EPGdata::TryReadingFreeViewMei(ProgressNotification* pProgress)
{
	delete m_pReader;
	m_pReader = new MEIReader("/ProgramFiles/Freeview.mei");
	return ReadSomeData(pProgress,1);
}

bool EPGdata::TryReadingJagsCSV(ProgressNotification* pProgress)
{
	delete m_pReader;
	m_pReader = new JagCSVReader();
	return ReadSomeData(pProgress,1);
}


bool EPGdata::ReadData(DataSources dataSource, ProgressNotification* pProgress, dword dwFlags)
{
	if (!BeginReading(dataSource, pProgress, dwFlags))
		return false;

	while (ReadSomeData(pProgress, 250))
	{
	}

	return FinishReading(pProgress);
}


bool EPGdata::BeginReading(DataSources dataSource, ProgressNotification* pProgress, dword dwFlags)
{
	if (pProgress)
		pProgress->Start();

	EPGevent::SetFlags(dwFlags);
	switch (dataSource)
	{
	case BuiltinEPG:
		return TryReadingBuiltin(pProgress);
	case Mei:
		return TryReadingMei(pProgress);
	case JagsCSV:
		return TryReadingJagsCSV(pProgress);
	case BuiltinExtendedEPG:
		return TryReadingExtendedBuiltin(pProgress);
	case FreeviewMEI:
		return TryReadingFreeViewMei(pProgress);
	case Auto:
	default:
		if (TryReadingMei(pProgress))
			return true;
		if (TryReadingFreeViewMei(pProgress))
			return true;
		if (TryReadingJagsCSV(pProgress))
			return true;
		if (TryReadingBuiltin(pProgress))
			return true;
	}
	return false;
}

dword EPGdata::BuildFlags(unsigned short iDaysToLoad, DescriptionMaxLength iMaxDescrLength)
{
	if (iDaysToLoad>15)
		iDaysToLoad=15;

	return (iMaxDescrLength<< 5) + (iDaysToLoad << 1);

}
