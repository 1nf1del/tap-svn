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
#include "DirectoryUtils.h"
#include "Logger.h"

bool CurrentDirStartCluster (dword* cluster)
{
	TYPE_File file;

	// '.' should be the first entry, so find it
	memset (&file, 0, sizeof (file));
	if (TAP_Hdd_FindFirst (&file))
	{
		while (strcmp (file.name, ".") && TAP_Hdd_FindNext (&file)) {};
	}

	// return starting cluster of this directory
	*cluster = file.startCluster;

	return (strcmp (file.name, ".") == 0);
}

string GetCurrentDirectory (void)
{
	dword cluster;
	string path;

	// while we have a '.' entry we work up the tree matching starting clusters
	while (CurrentDirStartCluster (&cluster))
	{
		// move into parent directory and look for a starting cluster match
		TAP_Hdd_ChangeDir ("..");

		TYPE_File file;
		memset (&file, 0, sizeof (file));
		if (TAP_Hdd_FindFirst (&file))
		{
			while ((cluster != file.startCluster) && TAP_Hdd_FindNext (&file)) {};
		}
		// if we have a match prepend it to the path
		if (cluster == file.startCluster)
		{
			string temp;

			temp = "/";
			temp += file.name;
			temp += path;
			path = temp;
		}
		else
		{
			// directory structure inconsistent, shouldn't get here
			// problem - we can't get back to our starting directory
			return "";
		}
	}

	if (path.size())
	{
		// finally we put ourselves back in our starting directory
		TAP_Hdd_ChangeDir (&path[1]);
	}
	else
	{
		path = "/";
	}

	return (path);
}

void MoveToRoot()
{
	dword cluster;
	while (CurrentDirStartCluster (&cluster))
	{
		// move into parent directory and look for a starting cluster match
		TAP_Hdd_ChangeDir ("..");
	}
}

bool ChangeDirectory(const string& newDirectory)
{
	if (newDirectory.size() == 0)
		return true; // nothing to do

	int iPos = 0;

	if (newDirectory.substr(0,1) == "/")
	{
//		TRACE("ChangeDirectory, path started with /, so moved to root\n");
		MoveToRoot();
		iPos = 1;
	}

//	TRACE1("ChangeDirectory, changing to %s\n", &newDirectory[iPos]);
	bool bResult = (TAP_Hdd_ChangeDir((char*)&newDirectory[iPos])!=0);
	if (!bResult)
		TRACE(bResult ? "ChangeDirectory Succeeded\n" : "ChangeDirectory Failed\n");

	return bResult;
}

int GetLastSlashPos(const string& path)
{
	int iSlashPos = -1;
	int iLastPos = -1;
	while ((iLastPos = path.find('/',iLastPos)) != -1)
	{
		iSlashPos = iLastPos;
	}

	return iSlashPos;
}

string GetFolderFromPath(const string& path)
{
	int iSlashPos = GetLastSlashPos(path);
	if (iSlashPos != -1)
	{
		return path.substr(0, iSlashPos);
	}
	return ".";
}

string GetFileFromPath(const string& path)
{
	int iSlashPos = GetLastSlashPos(path);
	if (iSlashPos == -1)
		return path;

	return path.substr(iSlashPos+1);
}

bool CreateDirectory(const string& directory)
{
	string dir = directory;
	if (dir.reverseFind('/') == dir.size()-1)
		dir= directory.substr(0,dir.size()-1);

	string baseFolder = GetFolderFromPath(dir);
	string newName = GetFileFromPath(dir);

	DirectoryRestorer dr(baseFolder);
	if (!dr.WasSuccesful())
	{
//		TRACE("failed to change to base folder");
		return false;
	}

	if (TAP_Hdd_Exist((char*)newName.c_str())) // already there?
	{
//		TRACE1("apparently %s already exists", newName.c_str());
		DirectoryRestorer dr(newName);
		return dr.WasSuccesful();
	}

	return (TAP_Hdd_Create((char*)newName.c_str(), ATTR_FOLDER) == 0);
}


UFILE* OpenFile(const string& sFileName, const char* szMode)
{
	string baseFolder = GetFolderFromPath(sFileName);
	string fileName = GetFileFromPath(sFileName);

	DirectoryRestorer dr(baseFolder);
	if (!dr.WasSuccesful())
		return 0;

	return fopen((char*)fileName.c_str(), (char*) szMode);
}

TYPE_File* OpenRawFile(const string& sFileName, const char* szMode)
{
	string baseFolder = GetFolderFromPath(sFileName);
	string fileName = GetFileFromPath(sFileName);

	DirectoryRestorer dr(baseFolder);
	if (!dr.WasSuccesful())
		return 0;

	char mode = *szMode;
	if (mode!='r' && mode!='w' && mode!='a')
		return NULL;

	if (mode=='w' || (mode=='a' && !FileExists(fileName)))
	{
		TAP_Hdd_Create((char*)fileName.c_str(), ATTR_NORMAL);
	}

	TYPE_File* result = TAP_Hdd_Fopen((char*)fileName.c_str());

	if (mode=='a' && result)
		TAP_Hdd_Fseek(result, 0, 2/*SEEK_END*/);

	return result;
}

DirectoryRestorer::DirectoryRestorer(const string& sChangeToDir)
{
	m_sSavedDir = GetCurrentDirectory();
	m_bWorked = ChangeDirectory(sChangeToDir);
}

DirectoryRestorer::~DirectoryRestorer()
{
	ChangeDirectory(m_sSavedDir);
}

bool DirectoryRestorer::WasSuccesful()
{
	return m_bWorked;
}


void GetDetailFolderContents(const string& sFolderName, array<TYPE_File>& results, const string& sExt, bool bFolders)
{
	DirectoryRestorer dr(sFolderName);
	if (!dr.WasSuccesful())
		return;

	TYPE_File file;
	int iCount = TAP_Hdd_FindFirst(&file);

	do
	{
		if ((file.attr == ATTR_FOLDER) != bFolders)
			continue;

		if (!bFolders)
		{
			string sName = file.name;
			sName = sName.tolower();
			if (sName.size()<=sExt.size())
				continue;
			if (sName.substr(sName.size() - sExt.size())!=sExt)
				continue;
		}

		results.push_back(file);
	} while (TAP_Hdd_FindNext(&file)<(dword)iCount);

	return;
}

array<string> GetFolderContents(const string& sFolderName, const string& sExt, bool bFolders)
{
	array<TYPE_File> files;
	GetDetailFolderContents(sFolderName, files, sExt, bFolders);

	array<string> results;

	for (unsigned int i=0; i<files.size(); i++)
	{
		TYPE_File& file = files[i];
		results.push_back(file.name);
	} 

	return results;
}

array<string> GetFilesInFolder(const string& sFolderName, const string& sExt)
{
	return GetFolderContents(sFolderName, sExt, false);
}

array<string> GetSubFolders(const string& sFolderName)
{
	return GetFolderContents(sFolderName, "", true);
}

bool FileExists(const string& file)
{
	string baseFolder = GetFolderFromPath(file);
	string fileName = GetFileFromPath(file);

	DirectoryRestorer dr(baseFolder);
	if (!dr.WasSuccesful())
		return false;

	return TAP_Hdd_Exist((char*) fileName.c_str());
}

bool DeleteFile(const string& file)
{
	if (!FileExists(file))
		return true;

	string baseFolder = GetFolderFromPath(file);
	string fileName = GetFileFromPath(file);

	DirectoryRestorer dr(baseFolder);
	if (!dr.WasSuccesful())
		return true;
	
	return TAP_Hdd_Delete((char*) fileName.c_str()) != 0;


}

