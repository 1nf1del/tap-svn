//Tapulator - An emulator for the Topfield PVR TAP API
//Copyright (C) 2005  Robin Glover
//
//This program is free software; you can redistribute it and/or
//modify it under the terms of the GNU General Public License
//as published by the Free Software Foundation; either version 2
//of the License, or (at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program; if not, write to the Free Software
//Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
//
// You can contact the author via email at robin.w.glover@gmail.com

#pragma once

class CConfiguration
{
public:
	CConfiguration(void);
	~CConfiguration(void);

	void Save();
	void Load();

	CString GetRootFolderPath();
	CString GetChannelFile();
	CString GetTimerFile();
	CString GetEPGFile();
	DWORD GetLoggingFilter();
	int GetStartInAutoStart();
	int GetOsdTransparency();

	void SetLoggingFiler(DWORD dwVal);
	void SetRootFolderPath(const CString& sVal);
	void SetChannelFile(const CString& sVal);
	void SetTimerFile(const CString& sVal);
	void SetEPGFile(const CString& sVal);
	void SetStartInAutoStart(int iVal);
	void SetOsdTransparency(int iVal);

private:

	CString m_sRootFolder;
	CString m_sChannelFile;
	CString m_sTimerFile;
	CString m_sEPGFile;
	DWORD m_dwLoggingFilter;
	int m_iStartInAutoStart;
	int m_iOsdTransparency;
};
