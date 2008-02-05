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
#include "PackedDateTime.h"

class EPGdata;
class EPGevent;

class IEPGReader
{
public:
	IEPGReader();

	virtual ~IEPGReader();
	virtual bool Read(EPGdata& epgdata, int maxRowsThisChunk = 0x7FFFFFFF);
	virtual short int GetPercentDone();
	virtual bool CanRead() ;
	virtual bool ReadChannel(EPGdata& epgdata, int channelNum); 

protected:
	bool IsEventWantedInEPG(EPGevent* pEvent);

	PackedDateTime m_pdtNow;
};

class EPGReader : public IEPGReader
{
public:
	EPGReader(bool bUseExtendedInfo = false, dword dwFlags = 0);
	~EPGReader(void);

	bool Read(EPGdata& epgdata, int maxRowsThisChunk = 0x7FFFFFFF);
	short int GetPercentDone();
	bool CanRead();
	bool ReadChannel(EPGdata& epgdata, int channelNum); 

private:
	EPGevent* BuildEvent(TYPE_TapEvent* pTapEvent, int iChannelNum);

	int m_iCurrentChan;
	int m_iTotalChan;
	bool m_bUseExtendedInfo;
	dword m_dwFlags;
};



#endif