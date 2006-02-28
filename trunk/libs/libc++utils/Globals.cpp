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
#include ".\globals.h"
#include "Timers.h"
#include "EPGData.h"
#include "Channels.h"
#include "Archive.h"
#include "Logger.H"

Timers* Globals::m_pTheTimers = NULL;
EPGdata* Globals::m_pEPGdata = NULL;
Channels* Globals::m_pChannels = NULL;
Archive* Globals::m_pArchive = NULL;

Globals::Globals(void)
{
}

Globals::~Globals(void)
{
}

Timers* Globals::GetTimers()
{
	if (m_pTheTimers == 0)
		m_pTheTimers = new Timers();

	return m_pTheTimers;
}

Channels* Globals::GetChannels()
{
	if (m_pChannels == 0)
		m_pChannels = new Channels();

	return m_pChannels;
}

bool Globals::LoadEPGData(DataSources dataSource, ProgressNotification* pProgress, dword dwFlags)
{
	delete m_pEPGdata;
	m_pEPGdata = new EPGdata();
	return	m_pEPGdata->ReadData(dataSource, pProgress, dwFlags);
}

EPGdata* Globals::GetEPGdata()
{
	if (m_pEPGdata == NULL)
		m_pEPGdata = new EPGdata();

	return m_pEPGdata;

}

void Globals::Cleanup()
{
	delete m_pTheTimers;
	m_pTheTimers = NULL;
	delete m_pEPGdata;
	m_pEPGdata = NULL;
	delete m_pChannels;
	m_pChannels = NULL;
	delete m_pArchive;
	m_pArchive = NULL;
}

Archive* Globals::GetArchive(const string& sCacheFile)
{
	if (m_pArchive == NULL)
	{
//		TRACE("building archive");	
		m_pArchive = new Archive(sCacheFile);
	}

	return m_pArchive;
}
