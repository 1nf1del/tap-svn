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

class IEPGReader;
class EPGevent;
class EPGchannel;
class EPGVisitor;
class ProgressNotification;

enum DataSources
{
	Auto,
	Mei,
	BuiltinEPG,
	JagsCSV,
	BuiltinExtendedEPG,
	FreeviewMEI,
};

#define EPGDATA_BUILTIN_GENRESINSQUAREBRACKETS	0x00000001
#define EPGDATA_DAYSTOLOAD_MASK					0x0000001E // 4 bits gives 0..15, treat 0 as 15 for backward compat
#define EPGDATA_DESCRIPTION_MAXLEN_MASK			0x000000E0 // 3 bits gives 0..7 1=64, 2=128, 3=256, 4=512, 5=1024, 6=2048, 7=0=4096

enum DescriptionMaxLength
{
	Len_Unlimited,
	Len_64,
	Len_128,
	Len_256,
	Len_512,
	Len_1024,
	Len_2048,
	Len_4096
};

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
	bool ReadData(DataSources dataSource, ProgressNotification* pProgress, dword dwFlags);
	bool HasData() const;	

	static dword BuildFlags(unsigned short iDaysToLoad, DescriptionMaxLength iMaxDescrLength);
	bool FinishReading(ProgressNotification* pProgress);
	bool BeginReading(DataSources dataSource, ProgressNotification* pProgress, dword dwFlags);
	bool ReadSomeData(ProgressNotification* pProgress, int iCount); 

private:
	bool TryReadingMei(ProgressNotification* pProgress);
	bool TryReadingBuiltin(ProgressNotification* pProgress);
	bool TryReadingJagsCSV(ProgressNotification* pProgress);
	bool TryReadingExtendedBuiltin(ProgressNotification* pProgress);
	bool TryReadingFreeViewMei(ProgressNotification* pProgress);

	EPGchannel* FindChannelByNum(word channelNum);
	array<EPGchannel*> m_channels;
	array<EPGevent*> m_emptyData;
	IEPGReader* m_pReader;
};
#endif