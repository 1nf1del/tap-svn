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


#define LOG_SEVERITY_CRIT		0x00000001
#define LOG_SEVERITY_ERROR		0x00000002
#define LOG_SEVERITY_UNIMPL		0x00000004
#define LOG_SEVERITY_WARNING	0x00000008
#define LOG_SEVERITY_INFO		0x00000010
#define LOG_SEVERITY_VERBOSE	0x00008000
#define LOG_SEVERITY_USER		0x00010000

class InfoLogger
{
public:
	InfoLogger(void);
	virtual ~InfoLogger(void);

	void LogInfo(LPCTSTR sFmt, ...);
	void LogUnimplemented(LPCTSTR sMsg);
	void LogError(LPCTSTR sFmt, ...);
	void LogVerbose(LPCTSTR sFmt, ...);
	void LogUser(LPCTSTR sFmt, va_list argList);

private:
	DWORD GetFilter();
	void LogVarArgs(LPCTSTR sFmt, LPCTSTR sPrefix, va_list argList);
	void LogSomething(LPCTSTR sMessage);
};
