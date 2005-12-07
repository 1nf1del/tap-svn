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
#include ".\tapmodule.h"
#include "ToppyFramework.h"
#include "MAINfrm.h"
#include "Regions.h"
TapModule::TapModule(void)
{
	Reset();
}

TapModule::~TapModule(void)
{
}

bool TapModule::LoadModule(CString sModule)
{
	if (m_hModule != 0)
	{
		UnloadExistingTap();
	}

	m_hModule = LoadLibrary(sModule);
	if (m_hModule == NULL)
	{
		m_sError = "Could not load the specified TAP module - is it compiled for x86?";
		return false;
	}

	m_pFramework = (SetFrameworkFunc) GetProcAddress(m_hModule, "SetFramework");
	m_pEventHandler = (TAP_EventHandlerFunc) GetProcAddress(m_hModule, "TAP_EventHandler");
	m_pMain = (TAP_MainFunc) GetProcAddress(m_hModule, "TAP_Main");

	if ((m_pEventHandler == NULL) || (m_pMain == NULL) || (m_pFramework == NULL))
	{
		UnloadExistingTap();
		m_sError = "The module was missing some required entry points - is it really a tap? Did you remember the .def file?";
		return false;
	}

	LogInfo("Loaded Tap %s", (LPCSTR)sModule);
	return true;

}

void TapModule::UnloadExistingTap()
{
	LogInfo("Unloading TAP");
	FreeLibrary(m_hModule);
	Reset();
}

void TapModule::Reset()
{
	m_bTSR = false;
	m_hModule = 0;
	m_sError = "Unknown error";
	m_pEventHandler = NULL;
	m_pMain = NULL;
	m_pFramework = NULL;
}

CString TapModule::GetError()
{
	CString sTemp = m_sError;
	m_sError = "Unknown error";
	return sTemp;
}

void TapModule::SetFramework(IFramework* pFramework)
{
	m_pFramework(pFramework);
}

unsigned long TapModule::TAP_Event(unsigned short event, unsigned long param1, unsigned long param2)
{
	__try
	{
		long lResult =  m_pEventHandler(event, param1, param2);
		CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
		pFrame->UpdateStatusInfo();
		return lResult;
	}
	__except(Regions::GetFrameBuffer().ExceptionFilter(GetExceptionCode(), GetExceptionInformation()))
	{
		return 0;
	}
}

int TapModule::TAP_Main_SEH()
{
	__try
	{
		return m_pMain();
	} 
	__except(Regions::GetFrameBuffer().ExceptionFilter(GetExceptionCode(), GetExceptionInformation()))
	{
		return -1;
	}
}

int TapModule::TAP_Main()
{
	try
	{
		int iResult = TAP_Main_SEH();
		m_bTSR = (iResult == 1);
		return iResult;
	}
	catch (std::exception)
	{
		ExitProcess(0);// ugh
		return -1;
	}
}

long TapModule::RaiseKeyEvent(int keyCode, int hardwareKeyCode)
{
	CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
	try
	{
		int iResult = EVT_KEY;
		if (m_pEventHandler)
			iResult = TAP_Event(EVT_KEY, keyCode, hardwareKeyCode);

		if (iResult == EVT_KEY)
			pFrame->GetFramework()->RaiseEventToFirmware(EVT_KEY, keyCode, hardwareKeyCode);

		pFrame->RepaintIfNeeded();;
		return iResult;
	}
	catch (std::exception& e)
	{
		if (CString(e.what()) == CString("TAP_Exit"))
		{
			AfxMessageBox("TAP Exited");
			UnloadExistingTap();
			pFrame->RefreshFramework();
		}
	}
	catch (...)
	{
		AfxMessageBox("Exception thrown from Event Handler");
	}
	return 0;
}

bool TapModule::IsInTSRMode()
{
	if (m_hModule == NULL)
		return false;

	return m_bTSR;

}