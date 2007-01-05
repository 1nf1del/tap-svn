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


word (*realTAP_Hdd_ChangeDir)( char* dir ) = 0;
static string sCurrentDirectory;



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

void InternalMoveToRoot()
{
	dword cluster;
	while (CurrentDirStartCluster (&cluster))
	{
		// move into parent directory and look for a starting cluster match
		TAP_Hdd_ChangeDir ("..");
	}
}


string InternalGetCurrentDirectory (void)
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

void UpdateCurrentDirectory(const char* dir)
{
//	TRACE2("Current dir is %s, changing to %s", sCurrentDirectory.c_str(), dir);
	string sChange = dir;
	if (sChange == ".")
		return ;

	if (sChange == "..")
		sChange = "../";

	while (sChange.substr(0,3) == "../")
	{
		if (sCurrentDirectory == "/")
			return;

		int iTrim = sCurrentDirectory.reverseFind('/');
		if (iTrim >= 0)
			sCurrentDirectory = sCurrentDirectory.substr(0,iTrim);

		sChange = sChange.substr(3);		
	}

	if (sCurrentDirectory.empty())
		sCurrentDirectory="/";

	if (sChange.empty())
		return;


	if (sCurrentDirectory[sCurrentDirectory.size()-1]!='/')
		sCurrentDirectory += "/";
	
	sCurrentDirectory += sChange;
//	TRACE1("Current dir is %s", sCurrentDirectory.c_str());
	return ;
}

extern "C"
{
	word ourTAP_Hdd_ChangeDir(char* dir)
	{
		word result = realTAP_Hdd_ChangeDir(dir);
		if (result != 0)
			UpdateCurrentDirectory(dir);
		return result;
	}
}

void HookChangeDirFunc()
{
	if (realTAP_Hdd_ChangeDir)
		return;

//	Logger::SetDestination(Logger::Screen|Logger::Serial);
	TRACE("Hooking ChangeDir Function \n");

	// ctor the string as it will be ignored during load
	new (&sCurrentDirectory) string();

	sCurrentDirectory = InternalGetCurrentDirectory();
	TRACE1("Current directory initialized to %s\n", sCurrentDirectory.c_str());

	realTAP_Hdd_ChangeDir = TAP_Hdd_ChangeDir;
	TAP_Hdd_ChangeDir = ourTAP_Hdd_ChangeDir;
}

void MoveToRoot()
{
	while (sCurrentDirectory!="/")
	{
		if (TAP_Hdd_ChangeDir ("..") == 0)
			return; // bail out
	}
}

// Real functionality

string GetCurrentDirectory()
{
HookChangeDirFunc();
	return sCurrentDirectory;
}

bool ChangeDirectoryImpl(const string& newDirectory, bool bTrans)
{
	HookChangeDirFunc();
	if (newDirectory.size() == 0)
		return true; // nothing to do

	// this function is going to be transactional, even if the underlying TAP API isn't
	string sStartDirectory = sCurrentDirectory;

//	TRACE2("ChangeDirectory, starting in %s, going to %s", sCurrentDirectory.c_str(), newDirectory.c_str());
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
	{
		// due to the way TAP_Hdd_ChangeDir works when it fails, we are not
		// always in the directory we were in before - we need to recalc
		// the current directory - easier to do this in the failure case
		// than to add special handling in every changedir to cope with it
		sCurrentDirectory = InternalGetCurrentDirectory();
		TRACE2("ChangeDirectory Failed - couldn't get to %s now in %s\n", newDirectory.c_str(), sCurrentDirectory.c_str());
		if (bTrans)
		{
			if (ChangeDirectoryImpl(sStartDirectory, false))
			{
				TRACE1("ChangeDirectory has moved back to %s\n", sStartDirectory.c_str());
			}
			else
			{
				TRACE("ChangeDirectory unable to return to the starting directory - that's odd\n");
			}
		}
	}

	return bResult;
}

bool ChangeDirectory(const string& newDirectory)
{
	return ChangeDirectoryImpl(newDirectory, true);
}

bool CreateDirectory(const string& directory)
{
	HookChangeDirFunc();

//	TRACE1("Started CreateDirectory - param is %s", directory.c_str());
//	TRACE1("Started CreateDirectory - current dir  is %s", sCurrentDirectory.c_str());

	string dir = directory;
	if (dir.reverseFind('/') == dir.size()-1)
		dir= directory.substr(0,dir.size()-1);

	string baseFolder = GetFolderFromPath(dir);
	string newName = GetFileFromPath(dir);

//	TRACE2("Split input path into %s and %s\n", baseFolder.c_str(), newName.c_str());

	DirectoryRestorer dr(baseFolder);
	if (!dr.WasSuccesful())
	{
		TRACE("failed to change to base folder\n");
		return false;
	}

	if (TAP_Hdd_Exist((char*)newName.c_str())) // already there?
	{
		TRACE1("apparently %s already exists", newName.c_str());
		DirectoryRestorer dr(newName);
		bool bResult = dr.WasSuccesful();
		TRACE1("tried to change to target folder - result was %d", bResult);
		return bResult;
	}

	dword result = TAP_Hdd_Create((char*)newName.c_str(), ATTR_FOLDER);
	TRACE1("Result from Create was %d\n", result);
	return (FileExists(newName));
}


UFILE* OpenFile(const string& sFileName, const char* szMode)
{
	HookChangeDirFunc();

	string baseFolder = GetFolderFromPath(sFileName);
	string fileName = GetFileFromPath(sFileName);

	DirectoryRestorer dr(baseFolder);
	if (!dr.WasSuccesful())
		return 0;

	return fopen((char*)fileName.c_str(), (char*) szMode);
}

TYPE_File* OpenRawFile(const string& sFileName, const char* szMode)
{
	HookChangeDirFunc();

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

#define BLOCK_SIZE 4096

bool CopyFile(const string& sSource, const string& sDestination, bool bFailIfExists)
{
	if (FileExists(sDestination))
	{
		if (bFailIfExists)
			return false;

		DeleteFile(sDestination);

		if (FileExists(sDestination))
			return false;
	}

	TYPE_File* srcFile = OpenRawFile(sSource, "r");
	if (srcFile == NULL) 
		return false;

	TYPE_File* destFile = OpenRawFile(sDestination, "w");
	if (destFile == NULL)
	{
		TAP_Hdd_Fclose(srcFile);
		return false;
	}

	dword dwSize = TAP_Hdd_Flen(srcFile);
	char buf[BLOCK_SIZE];

	while (dwSize > 0)
	{
		dword dwBlock = min(dwSize, BLOCK_SIZE);
		TAP_Hdd_Fread(buf, dwBlock, 1, srcFile);
		TAP_Hdd_Fwrite(buf, dwBlock, 1, destFile);
		dwSize-=dwBlock;
	}

	TAP_Hdd_Fclose(srcFile);
	TAP_Hdd_Fclose(destFile);

	return true;
}

void GetDetailFolderContents(string sFolderName, array<TYPE_File>& results, const string& sExt, bool bFolders)
{
	if (sFolderName.reverseFind('/') == sFolderName.size()-1)
		sFolderName = sFolderName.substr(0, sFolderName.size()-1);

	DirectoryRestorer dr(sFolderName);
	if (!dr.WasSuccesful())
		return;

	TYPE_File file;
	int iCount = TAP_Hdd_FindFirst(&file);

	if (iCount == 0)
		return;

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

bool FileExists(string file)
{
	if (file.reverseFind('/') == file.size() -1)
	{
		file = file.substr(0,file.size()-1);
	}

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

