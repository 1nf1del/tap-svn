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
#ifndef cpputils_archive_h
#define cpputils_archive_h
#include <stdlib.h>
#include "ArchivedProgram.h"
#include "taparray.h"
#include "tapmap.h"
class ArchivedProgram;
class ArchiveVisitor;
class Archive
{
public:



	Archive(const string& sCacheFile, bool bLoadInChunks);
	~Archive();

	const array<const ArchivedProgram*>& GetPrograms();
	bool Visit(ArchiveVisitor* pVisitor) const;

	const array<const ArchivedProgram*>& GetDeletedPrograms();
	bool VisitDeletedPrograms(ArchiveVisitor* pVisitor) const;

	bool DoSomeLoading();

private:

	Archive(const string& sDeletedCacheFile, array<const ArchivedProgram*>& deletedStuff);
	void LoadCache();
	const ArchivedProgram* FindInCache(const string& folderName, const string& fileName, dword dwStartCluster);
	void SaveCache();
	void BuildDeletedArchive();
	void Index();
	void Initialize();
	void ProcessFolder(const string& sFolderName);
	void ProcessFile(const string& sFileName, dword dwStartCluster);

	array<const ArchivedProgram*> m_theArchive;
	array<const ArchivedProgram*> m_cachedArchive;
	mutable map<string, array<const ArchivedProgram*> > m_index;
	string m_sCacheFile;
	Archive* m_pDeletedPrograms;

	array<string> m_foldersToDo;
	array<string> m_filesToDo;
	array<dword>  m_totalClusters;
	bool m_bLoadingInBackground;
	string m_sInitialFolder;
};

#endif