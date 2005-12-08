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

#include "StdAfx.h"
#include ".\connfiguration.h"
#include "InfoLogger.h"

CConfiguration::CConfiguration(void)
{
	Load();
}

CConfiguration::~CConfiguration(void)
{
}

void CConfiguration::Save()
{
	CWinApp* pApp = AfxGetApp();
	pApp->WriteProfileString("Configuration", "FileSystemPath", m_sRootFolder);
	pApp->WriteProfileString("Configuration", "ChannelFile", m_sChannelFile);
	pApp->WriteProfileString("Configuration", "EPGFile", m_sEPGFile);
	pApp->WriteProfileString("Configuration", "TimerFile", m_sTimerFile);
	pApp->WriteProfileInt("Configuration", "LoggingFilter", m_dwLoggingFilter);
	pApp->WriteProfileInt("Configuration", "StartInAutoStart", m_iStartInAutoStart);
	pApp->WriteProfileInt("Configuration", "OSDTransparency", m_iOsdTransparency);
}

void CConfiguration::Load()
{
	CWinApp* pApp = AfxGetApp();
	m_sRootFolder = pApp->GetProfileString("Configuration", "FileSystemPath", "c:\\toppy");
	m_sChannelFile = pApp->GetProfileString("Configuration", "ChannelFile", "c:\\ChannelList.csv");
	m_sEPGFile = pApp->GetProfileString("Configuration", "EPGFile", "c:\\Jags_EPG.Buffer.DAT");
	m_sTimerFile = pApp->GetProfileString("Configuration", "TimerFile", "c:\\TimerList.txt");
	m_dwLoggingFilter = pApp->GetProfileInt("Configuration", "LoggingFilter", LOG_SEVERITY_CRIT		|
		   LOG_SEVERITY_ERROR		|
		   LOG_SEVERITY_UNIMPL		|
		   LOG_SEVERITY_WARNING		|
		   LOG_SEVERITY_INFO		|	
		   LOG_SEVERITY_USER		);
	m_iStartInAutoStart = pApp->GetProfileInt("Configuration", "StartInAutoStart", 0);
	m_iOsdTransparency = pApp->GetProfileInt("Configuration", "OSDTransparency", 10);
}

CString CConfiguration::GetRootFolderPath()
{
	return m_sRootFolder;
}

CString CConfiguration::GetChannelFile()
{
	return m_sChannelFile;
}

CString CConfiguration::GetTimerFile()
{
	return m_sTimerFile;
}

CString CConfiguration::GetEPGFile()
{
	return m_sEPGFile;
}

DWORD CConfiguration::GetLoggingFilter()
{
	return m_dwLoggingFilter;
}

int CConfiguration::GetStartInAutoStart()
{
	return m_iStartInAutoStart;
}

int CConfiguration::GetOsdTransparency()
{
	return m_iOsdTransparency;
}

void CConfiguration::SetLoggingFiler(DWORD dwVal)
{
	m_dwLoggingFilter = dwVal;
}

void CConfiguration::SetRootFolderPath(const CString& sVal)
{
	m_sRootFolder = sVal;
}

void CConfiguration::SetChannelFile(const CString& sVal)
{
	m_sChannelFile = sVal;
}

void CConfiguration::SetTimerFile(const CString& sVal)
{
	m_sTimerFile = sVal;
}

void CConfiguration::SetEPGFile(const CString& sVal)
{
	m_sEPGFile = sVal;
}

void CConfiguration::SetStartInAutoStart(int iVal)
{
	m_iStartInAutoStart = iVal;
}

void CConfiguration::SetOsdTransparency(int iVal)
{
	m_iOsdTransparency = iVal;
}
