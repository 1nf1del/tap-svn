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
#ifndef libcpputils_directoryutils
#define libcpputils_directoryutils
#include <string.h>
#include <stdlib.h>
#include "tap.h"
#include "tapstring.h"
#include "file.h"
#include "taparray.h"

class DirectoryRestorer
{
public:
	DirectoryRestorer(const string& sChangeToDirectory);
	~DirectoryRestorer();
	bool WasSuccesful();

private:
	bool m_bWorked;
	string m_sSavedDir;

};

string GetCurrentDirectory();
bool ChangeDirectory(const string& newDirectory);
UFILE* OpenFile(const string& sFileName, const char* szMode);
bool CreateDirectory(const string& directory);
bool FileExists(string file);
bool DeleteFile(const string& file);
TYPE_File* OpenRawFile(const string& sFileName, const char* szMode);

array<string> GetFilesInFolder(const string& sFolderName, const string& sExt);
array<string> GetSubFolders(const string& sFolderName);
void GetDetailFolderContents(const string& sFolderName, array<TYPE_File>& results, const string& sExt, bool bFolders);

#endif