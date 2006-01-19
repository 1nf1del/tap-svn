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
#include ".\meireader.h"
#include "EPGevent.h"
#include "tapstring.h"
#include "taparray.h"
#include "EPGdata.h"

MEIReader::MEIReader(void)
{
	m_pFile = fopen("MyStuff.mei", "r");
	if (m_pFile == NULL)
	{
		TAP_Hdd_ChangeDir("..");
		m_pFile = fopen("MyStuff.mei", "r");	
		TAP_Hdd_ChangeDir("Auto Start");
	}
}

MEIReader::~MEIReader(void)
{
	fclose(m_pFile);
}

bool MEIReader::CanRead()
{
	return m_pFile != 0;
}

bool MEIReader::Read(EPGdata& epgdata, int maxRowsThisChunk)
{
	if (m_pFile == NULL)
		return false;

	char buf[2048];
	string s;
	int iCount = 0;

	while(fgets(buf, 2048, m_pFile))
	{
		s = buf;
		if (!s.empty())
		{
			EPGevent* pNewEvent = new EPGevent(s);
			if (pNewEvent->GetEnd().IsInPast())
			{
				delete pNewEvent;
			}
			else
			{
				epgdata.AddEvent(pNewEvent);
			}
		}

		iCount++;
		if (iCount == maxRowsThisChunk)
		{
			return true;
		}
	}

	return false;
}

int MEIReader::GetPercentDone()
{
	int fpos = TAP_Hdd_Ftell(m_pFile->fd) / 1024;
	int fsize = m_pFile->size / 1024;

	return (fpos * 100) / fsize;
}
