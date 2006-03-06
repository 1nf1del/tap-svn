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
#include "DirectoryUtils.h"

MEIReader::MEIReader(const string& filename)
{
	m_sFileName = filename;
	if (m_sFileName.empty())
		m_sFileName = "/ProgramFiles/MyStuff.mei";
	m_pFile = 0;
}

MEIReader::~MEIReader(void)
{
	fclose(m_pFile);
}

bool MEIReader::Init()
{
	if (m_pFile)
		return true;

	m_pFile = OpenFile(GetFileName(), "r");

	return (m_pFile != 0);
}

bool MEIReader::CanRead()
{
	return Init();
}

bool MEIReader::Read(EPGdata& epgdata, int maxRowsThisChunk)
{
	if (!Init())
		return false;

	char buf[2048];
	string s;
	int iCount = 0;

	while(fgets(buf, 2048, m_pFile))
	{
		s = buf;
		if (!s.empty())
		{
			EPGevent* pNewEvent = ConstructEvent(s);
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

string MEIReader::GetFileName()
{
	return m_sFileName;
}

EPGevent* MEIReader::ConstructEvent(const string& sdata)
{
	return new EPGevent(sdata);
}
