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
		MoveToRoot();
		iPos = 1;
	}

	return (TAP_Hdd_ChangeDir((char*)&newDirectory[iPos])!=0);
}

UFILE* OpenFile(const string& sFileName, char* szMode)
{
	int iSlashPos = -1;
	int iLastPos = -1;
	while ((iLastPos = sFileName.find('/',iLastPos)) != -1)
	{
		iSlashPos = iLastPos;
	}

	string sCurrentDir;
	if (iLastPos != -1)
	{
		sCurrentDir = GetCurrentDirectory();
		string sDir = sFileName.substr(0, iSlashPos);
		if (!ChangeDirectory(sDir))
		{
			ChangeDirectory(sCurrentDir);
			return 0;
		}
	}

	UFILE* result = fopen((char*)&sFileName[iLastPos+1], szMode);

	if (iLastPos != -1)
		ChangeDirectory(sCurrentDir);

	return result;
}





