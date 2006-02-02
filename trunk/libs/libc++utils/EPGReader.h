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
#ifndef libcpputils_epgreader_h
#define libcpputils_epgreader_h
#include <stdlib.h>
#include "tap.h"

class EPGdata;
class EPGevent;

class IEPGReader
{
public:
	virtual ~IEPGReader();
	virtual bool Read(EPGdata& epgdata, int maxRowsThisChunk = 0x7FFFFFFF);
	virtual int GetPercentDone();
	virtual bool CanRead() ;
};

class EPGReader : public IEPGReader
{
public:
	EPGReader(bool bUseExtendedInfo = false, dword dwFlags = 0);
	~EPGReader(void);

	bool Read(EPGdata& epgdata, int maxRowsThisChunk = 0x7FFFFFFF);
	int GetPercentDone();
	bool CanRead();
private:
	EPGevent* BuildEvent(TYPE_TapEvent* pTapEvent, int iLogicalChan);

	int m_iCurrentChan;
	int m_iTotalChan;
	bool m_bUseExtendedInfo;
	dword m_dwFlags;
};



#endif