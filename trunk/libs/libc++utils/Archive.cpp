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
#include "Archive.h"
#include "DirectoryUtils.h"
#include "ArchiveVisitor.h"
#include "inifile.h"

Archive::Archive(const string& sCacheFile)
{
	m_sCacheFile = sCacheFile;
	if (m_sCacheFile == "")
		m_sCacheFile = "/ProgramFiles/Archive.cache";
	Populate();
}

Archive::~Archive()
{
	for (unsigned int i=0; i<m_theArchive.size(); i++)
		delete m_theArchive[i];

	for (unsigned int i=0; i<m_cachedArchive.size(); i++)
		delete m_cachedArchive[i];

}

void Archive::Populate()
{
	LoadCache();
	PopulateFromFolder("/DataFiles");
	SaveCache();
}

void Archive::LoadCache()
{
	IniFile ini;
	ini.Load(m_sCacheFile);

	for (int i=0; i<ini.GetKeyCount(); i++)
	{
		string s = ini.GetKey(i);
		if (s.substr(0,7)=="Program")
		{
			s = ini.GetValue(i);
			m_cachedArchive.push_back(new ArchivedProgram(ArchivedProgram::DeSerialize(s)));
		}
	}
}

void Archive::SaveCache()
{
	IniFile ini;
	for (unsigned int i=0; i<m_theArchive.size(); i++)
	{
		string sKey;
		sKey.format("Program%d", i);
		ini.SetValue(sKey, m_theArchive[i]->Serialize());
	}
	ini.Save(m_sCacheFile);
}

void Archive::PopulateFromFolder(const string& sFolderName)
{
	DirectoryRestorer dr(sFolderName);

	array<TYPE_File> files;
	GetDetailFolderContents(sFolderName, files, ".rec", false);
	for (unsigned int i=0; i<files.size(); i++)
	{
		TYPE_File& file = files[i];
		const ArchivedProgram* pProg = FindInCache(sFolderName, file);
		if (pProg == NULL)
		{
			pProg = new ArchivedProgram(sFolderName, file.name);
		}

		if (pProg->IsValid())
		{
			m_theArchive.push_back(pProg);
		}
		else
		{
			delete pProg;
		}
	}


	array<string> folders = GetSubFolders(sFolderName);
	for (unsigned int i=0; i<folders.size(); i++)
	{
		if (folders[i][0]!='.')
			PopulateFromFolder(sFolderName + "/" + folders[i]);
	}
}

const array<const ArchivedProgram*>& Archive::GetPrograms()
{
	return m_theArchive;
}

bool Archive::Visit(ArchiveVisitor* pVisitor) const
{
	for (unsigned int i=0; i<m_theArchive.size(); i++)
	{
		if (!pVisitor->VisitProgram(m_theArchive[i]))
			return false;

	}

	return true;
}

const ArchivedProgram* Archive::FindInCache(const string& folderName, TYPE_File& file)
{
	for (unsigned int i=0; i<m_cachedArchive.size(); i++)
	{
		if (m_cachedArchive[i]->Represents(folderName, file))
		{
			const ArchivedProgram* pResult = m_cachedArchive[i];
			m_cachedArchive.erase(i);
			return pResult;

		}
	}
	return NULL;
}
