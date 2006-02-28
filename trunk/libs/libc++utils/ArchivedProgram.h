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
#ifndef cpputils_archivedprogram_h
#define cpputils_archivedprogram_h

#include <stdlib.h>
#include "tapstring.h"
#include "tap.h"
#include "timeslot.h"
class stringarchive;

class ArchivedProgram
{
public:
	ArchivedProgram(const string& folderName, const string& fileName, dword dwStartCluster, dword dwClusterCount); 

	bool IsValid() const;
	const TimeSlot& GetTimeSlot() const;
	const string& GetTitle() const;
	const string& GetDescription() const;
	int GetChannelNum() const;
	string GetFileName() const;
	string Serialize() const;
	static ArchivedProgram DeSerialize(const string& data);
	bool Represents(const string& folderName, TYPE_File& file) const;

private:
	void Init();
	ArchivedProgram(stringarchive& ar);

	string m_sFileName;
	string m_sFolderName;
	bool m_bIsValid;

	string m_sTitle;
	string m_sDescription;
	string m_sChannel;
	word m_wServiceNum;
	TimeSlot m_TimeSlot;

	dword m_wClusterNum;
	dword m_wTotalCluster;


};
//static stringarchive& operator <<(stringarchive& ar, const ArchivedProgram& prog);
#endif