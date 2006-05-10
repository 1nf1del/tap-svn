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
#include "archivedprogram.h"
#include "file.h"
#include <string.h>
#include "RecHeader.h"
#include "stringarchive.h"
#include "logger.h"

ArchivedProgram::ArchivedProgram(const string& folderName, const string& fileName, dword dwStartCluster, dword dwClusterCount)
{
	m_sFolderName = folderName;
	m_sFileName = fileName;

	m_bIsValid = false;
	m_wClusterNum = dwStartCluster;
	m_wTotalCluster = dwClusterCount;
	Init();
}

string BuildStringFromChars(const char* pData, int iMaxLen)
{
	string result;
	strncpy(result.getbuffer(iMaxLen), pData, iMaxLen);
	result.releasebuffer();
	return result;
}

void ArchivedProgram::Init()
{
	UFILE* f = fopen((char*) m_sFileName.c_str(), "r");
	if (f == NULL)
		return;


	RecHeader hdr;
	read(f->fd, &hdr, sizeof(RecHeader));
	fclose(f);

#ifdef WIN32
	hdr.service_num = _byteswap_ushort(hdr.service_num);
	hdr.start_mjd = _byteswap_ushort(hdr.start_mjd);
	hdr.end_mjd = _byteswap_ushort(hdr.end_mjd);
#endif


	if (hdr.service_type != SVC_TYPE_Tv)
		return; // not interested

	m_wServiceNum = hdr.service_num;
	m_sChannel = BuildStringFromChars(hdr.svc_name, 24);
	m_sTitle = BuildStringFromChars(hdr.evt_text, hdr.evt_name_len);
	m_sDescription = BuildStringFromChars(hdr.evt_text + hdr.evt_name_len, 257-hdr.evt_name_len);

	dword dwStart = hdr.start_mjd;
	dwStart<<=16;
	dwStart|=(hdr.start_hour<<8);
	dwStart|=(hdr.start_min);

	dword dwEnd = hdr.end_mjd;
	dwEnd<<=16;
	dwEnd|=(hdr.end_hour<<8);
	dwEnd|=(hdr.end_min);
	m_TimeSlot.SetStart(PackedDateTime(dwStart));
	m_TimeSlot.SetEnd(PackedDateTime(dwEnd));

	m_bIsValid = true;

}

bool ArchivedProgram::IsValid() const
{
	return m_bIsValid;
}

const TimeSlot& ArchivedProgram::GetTimeSlot() const
{
	return m_TimeSlot;
}

const string& ArchivedProgram::GetTitle() const
{
	return m_sTitle;
}
const string& ArchivedProgram::GetDescription() const
{
	return m_sDescription;
}
int ArchivedProgram::GetChannelNum() const
{
	return m_wServiceNum;
}

string ArchivedProgram::GetFileName() const
{
	return m_sFolderName + "/" + m_sFileName;
}


string ArchivedProgram::Serialize() const
{
	stringarchive sa;
	sa.insert("ArcPrg");
	sa << m_sFolderName << m_sFileName << m_sTitle << m_sDescription << m_sChannel << m_wServiceNum;
	sa << m_TimeSlot.Start().AsDateTime() << m_TimeSlot.End().AsDateTime() << m_wClusterNum << m_wTotalCluster;
	return sa.toString();
}


ArchivedProgram ArchivedProgram::DeSerialize(const string& sData)
{
	stringarchive sa(sData);
	return ArchivedProgram(sa);
}

ArchivedProgram::ArchivedProgram(stringarchive& sa)
{
	dword dwStart, dwEnd;
	sa.expect("ArcPrg");
	sa >> m_sFolderName >> m_sFileName >> m_sTitle >> m_sDescription >> m_sChannel >> m_wServiceNum;
	sa >> dwStart >> dwEnd >> m_wClusterNum >> m_wTotalCluster;
	m_TimeSlot.SetStart(PackedDateTime(dwStart));
	m_TimeSlot.SetEnd(PackedDateTime(dwEnd));
}


bool ArchivedProgram::Represents(const string& folderName, TYPE_File& file) const
{
	if (m_sFolderName.compareNoCase(folderName)!=0)
		return false;

	if (m_sFileName.compareNoCase(file.name)!=0)
		return false;

	if (m_wClusterNum != file.startCluster)
		return false;

	//if (m_wTotalCluster != file.totalCluster)
	//	return false;

	return true;

}

bool ArchivedProgram::operator==(const ArchivedProgram& other) const
{
	return (m_sFileName == other.m_sFileName) &&
		(m_sFolderName == other.m_sFolderName) &&
		(m_sTitle == other.m_sTitle) &&
		(m_sDescription == other.m_sDescription);
}