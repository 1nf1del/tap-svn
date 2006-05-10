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
#ifndef cpputils_meireader_h
#define cpputils_meireader_h
#include <stdlib.h>
#include "tap.h"
#include "file.h"
#include "EPGReader.h"
#include "tapstring.h"

class EPGdata;
class EPGevent;

class MEIReader : public IEPGReader
{
public:
	MEIReader(const string& filename = "");
	~MEIReader(void);

	bool Read(EPGdata& epgdata, int maxRowsThisChunk = 0x7FFFFFFF);
	short int GetPercentDone();
	bool CanRead();
private:
	bool Init();
	virtual string GetFileName();
	virtual EPGevent* ConstructEvent(const string& sdata);

	string m_sFileName;
	UFILE* m_pFile;
};
#endif