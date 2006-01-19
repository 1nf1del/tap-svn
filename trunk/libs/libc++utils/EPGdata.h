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
#ifndef cpputils_epgdata_h
#define cpputils_epgdata_h
#include <stdlib.h>
#include "tap.h"
#include "taparray.h"
#include "tapstring.h"

class EPGevent;
class EPGchannel;
class EPGVisitor;

class EPGdata
{
public:
	EPGdata(void);
	~EPGdata(void);

	void AddEvent(EPGevent* newEvent);
	const array<EPGevent*>& GetDataForChannel(int channelNum) const;
	void CheckForContinuedPrograms();
	array<string> GetGenres();
	EPGchannel* GetChannel(int channelNum);
	const EPGchannel* GetChannel(int channelNum) const;
	const array<EPGchannel*> GetChannels() const;
	bool Visit(EPGVisitor* pVisitor) const;

private:
	EPGchannel* FindChannelByNum(word channelNum);
	array<EPGchannel*> m_channels;
	array<EPGevent*> m_emptyData;
};
#endif