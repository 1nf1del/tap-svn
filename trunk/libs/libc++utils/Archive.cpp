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
#include "Logger.h"

Archive::Archive(const string& sCacheFile, bool bLoadInChunks)
{
	m_bLoadingInBackground = bLoadInChunks;
	m_sInitialFolder = GetCurrentDirectory();
	m_sCacheFile = sCacheFile;
	m_pDeletedPrograms = NULL;
	if (m_sCacheFile == "")
		m_sCacheFile = "/ProgramFiles/Archive.cache";

	LoadCache();
	ProcessFolder("/DataFiles");

	if (!m_bLoadingInBackground)
		Initialize();
}



void Archive::Initialize()
{

	while (DoSomeLoading())
	{

	}


	return;

}

Archive::Archive(const string& sDeletedCacheFile, array<const ArchivedProgram*>& deletedStuff)
{
	m_pDeletedPrograms = NULL;
	m_sCacheFile = sDeletedCacheFile;
	LoadCache();
	for (unsigned int i=0; i<m_cachedArchive.size(); i++)
	{
		m_theArchive.push_back(m_cachedArchive[i]);
	}
	m_cachedArchive.clear();
	for (unsigned int i=0; i<deletedStuff.size(); i++)
	{
		const ArchivedProgram* pProg = deletedStuff[i];
		if (m_theArchive.containsvalue(pProg))
		{
			delete pProg;
			continue;
		}
		m_theArchive.push_back(pProg);
	}
	SaveCache();
	Index();
}


Archive::~Archive()
{
	for (unsigned int i=0; i<m_theArchive.size(); i++)
		delete m_theArchive[i];

	for (unsigned int i=0; i<m_cachedArchive.size(); i++)
		delete m_cachedArchive[i];

	delete m_pDeletedPrograms;
}

void Archive::BuildDeletedArchive()
{
	string sDelCache = m_sCacheFile + ".deleted";
	m_pDeletedPrograms = new Archive(sDelCache, m_cachedArchive);
	m_cachedArchive.clear();
}


void Archive::LoadCache()
{
	IniFile ini;
	if (!ini.Load(m_sCacheFile))
		return;

	for (int i=0; i<ini.GetKeyCount(); i++)
	{
		string s = ini.GetKey(i);
		if (s.substr(0,7)=="Program")
		{
			s = ini.GetValue(i);
			m_cachedArchive.push_back(new ArchivedProgram(ArchivedProgram::DeSerialize(s)));
		}
	}

	TRACE1("Loaded cache %d items", ini.GetKeyCount());
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
	if (!ini.Save(m_sCacheFile))
	{
		TRACE("failed to save cache");	
		return;
	}

	TRACE1("Saved cache %d items", ini.GetKeyCount());
}

bool Archive::DoSomeLoading()
{
	if (!m_filesToDo.empty())
	{
		ProcessFile(m_filesToDo[0], m_totalClusters[0]);
		m_filesToDo.erase(0);
		m_totalClusters.erase(0);
		return true;
	}

	if (!m_foldersToDo.empty())
	{
		ProcessFolder(m_foldersToDo[0]);
		m_foldersToDo.erase(0);
		return true;
	}

	ChangeDirectory(m_sInitialFolder);

//	TRACE("About to save cache file\n");

	SaveCache();
	TRACE("About to update deleted program list\n");
	BuildDeletedArchive();
	TRACE("About to index the archive\n");
	Index();
	TRACE("Archive Load finished!\n");
	return false;


}


void Archive::ProcessFolder(string sFolderName)
{
	TRACE1("Processing folder %s\n", sFolderName.c_str());
	if (!m_bLoadingInBackground)
		ChangeDirectory(sFolderName);

	array<TYPE_File> files;
	GetDetailFolderContents(sFolderName, files, ".rec", false);
	for (unsigned int i=0; i<files.size(); i++)
	{
//		TRACE1("Queueing file %s\n", files[i].name);
		m_filesToDo.push_back(sFolderName + "/" + files[i].name);
		m_totalClusters.push_back(files[i].totalCluster);
	}


	array<string> folders = GetSubFolders(sFolderName);
//	TRACE1("Found %d folders to deal with \n", folders.size());
	for (unsigned int i=0; i<folders.size(); i++)
	{
//		TRACE2("About to Queue folder %d, string length is %d\n", i, folders[i].size());
		if (folders[i][0]!='.')
		{
//			TRACE1("Queueing folder %s\n", folders[i].c_str());
			m_foldersToDo.push_back(sFolderName + "/" + folders[i]);
//			TRACE("Queued\n");
		}
		else
		{
//			TRACE("Ignore folder starting with .\n");
		}
	}
//	TRACE1("Finished processing folder %s\n", sFolderName.c_str());
}

void Archive::ProcessFile(string fileName, dword dwTotalCluster)
{
	TRACE1("Checking file %s...", fileName.c_str());
	int iSplit = fileName.reverseFind('/');
	string sFolderName = fileName.substr(0, iSplit);
	string file = fileName.substr(iSplit+1);
	const ArchivedProgram* pProg = FindInCache(sFolderName, file, dwTotalCluster);
	if (pProg == NULL)
	{
		TRACE1("Adding Program from folder (%s)", sFolderName.c_str());
		if (!m_bLoadingInBackground)
			ChangeDirectory(sFolderName);
		pProg = new ArchivedProgram(sFolderName, file, 0, dwTotalCluster);
	}
	else
	{
		TRACE("Adding program from cache");
	}

	if (pProg->IsValid())
	{
		m_theArchive.push_back(pProg);
	}
	else
	{
		delete pProg;
	}
	TRACE("...done\n");

}




const array<const ArchivedProgram*>& Archive::GetPrograms()
{
	return m_theArchive;
}

bool Archive::Visit(ArchiveVisitor* pVisitor) const
{
	string sTitle = pVisitor->GetReducedTitle();
	const array<const ArchivedProgram*>& items = m_index[sTitle];

	for (unsigned int i=0; i<items.size(); i++)
	{
		if (!pVisitor->VisitProgram(items[i]))
			return false;

	}

	return true;
}



const array<const ArchivedProgram*>& Archive::GetDeletedPrograms()
{
	return m_pDeletedPrograms->GetPrograms();
}

bool Archive::VisitDeletedPrograms(ArchiveVisitor* pVisitor) const
{
	return m_pDeletedPrograms->Visit(pVisitor);
}


const ArchivedProgram* Archive::FindInCache(const string& folderName, const string& fileName, dword dwTotalCluster)
{
	for (unsigned int i=0; i<m_cachedArchive.size(); i++)
	{
		if (m_cachedArchive[i]->Represents(folderName, fileName, dwTotalCluster))
		{
			TRACE3("Found program in cache %s/%s,%d\n", folderName.c_str(), fileName.c_str(), dwTotalCluster);
			const ArchivedProgram* pResult = m_cachedArchive[i];
			m_cachedArchive.erase(i);
			return pResult;

		}
	}
	TRACE3("Program not in cache %s/%s,%d\n", folderName.c_str(), fileName.c_str(), dwTotalCluster);
	return NULL;
}

string RemovePrefix(const string& s)
{
	int iPos = s.find(':');

	if (iPos == -1)
		return s;

	string sSub = s.substr(iPos+1);

	while (sSub.size() && sSub[0] == ' ')
		sSub = sSub.substr(1);

	return sSub;
}

void Archive::Index()
{
	for (unsigned int i=0; i<m_theArchive.size(); i++)
	{
		string sTitle = RemovePrefix(m_theArchive[i]->GetTitle());
		m_index[sTitle].push_back(m_theArchive[i]);
	}

}