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
#include ".\infologger.h"
#include "MainFrm.h"
#include "Connfiguration.h"

InfoLogger::InfoLogger(void)
{
}

InfoLogger::~InfoLogger(void)
{
}

void InfoLogger::LogInfo(LPCTSTR sFmt, ...)
{
	if ((GetFilter() & LOG_SEVERITY_INFO) == 0)
		return;
	va_list argList;
	va_start(argList, sFmt);
	LogVarArgs(sFmt, "INFO: ", argList);
	va_end(argList);
}

void InfoLogger::LogError(LPCTSTR sFmt, ...)
{
	if ((GetFilter() & LOG_SEVERITY_ERROR) == 0)
		return;
	va_list argList;
	va_start(argList, sFmt);
	LogVarArgs(sFmt, "INFO: ", argList);
	va_end(argList);
}

void InfoLogger::LogVerbose(LPCTSTR sFmt, ...)
{
	if ((GetFilter() & LOG_SEVERITY_VERBOSE) == 0)
		return;
	va_list argList;
	va_start(argList, sFmt);
	LogVarArgs(sFmt, "INFO: ", argList);
	va_end(argList);
}

void InfoLogger::LogUser(LPCTSTR sFmt, va_list argList)
{
	if ((GetFilter() & LOG_SEVERITY_USER) == 0)
		return;
	LogVarArgs(sFmt, "USER: ", argList);
}


void InfoLogger::LogSomething(LPCTSTR sMessage)
{
	CMainFrame* pMainFrame = (CMainFrame*) AfxGetMainWnd();
	if (!pMainFrame->IsClosing())
		pMainFrame->LogEvent(sMessage);
}

void InfoLogger::LogUnimplemented(LPCTSTR sMsg)
{
	if ((GetFilter() & LOG_SEVERITY_UNIMPL) == 0)
		return;
	LogSomething(CString("UNIMPL: ")+sMsg);
//	ASSERT(false);
}

void InfoLogger::LogVarArgs(LPCTSTR sFmt, LPCTSTR sPrefix, va_list argList)
{
	char buf[2048];
	_vsnprintf(buf, 2048, (LPCTSTR)sFmt, argList);
	LogSomething(sPrefix+CString(buf));
}

DWORD InfoLogger::GetFilter()
{
	CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();

	if (pFrame == NULL)
		return 0;

	if (pFrame->IsClosing())
		return 0;

	CConfiguration* pConfig = pFrame->GetConfig();
	return pConfig->GetLoggingFilter();

}