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

class IFramework;
typedef unsigned long (*TAP_EventHandlerFunc) ( unsigned short event, unsigned long param1, unsigned long param2 );
typedef int (*TAP_MainFunc) (void);
typedef void (*SetFrameworkFunc) (IFramework* pFramework);
#include "InfoLogger.h"

class TapModule : InfoLogger
{
public:
	TapModule(void);
	~TapModule(void);

	bool LoadModule(CString sModule);

	CString GetError();

	void SetFramework(IFramework* pFramework);
	unsigned long TAP_Event(unsigned short event, unsigned long param1, unsigned long param2);
	int TAP_Main();
	long RaiseKeyEvent(int keyCode, int hardwareKeyCode);
	bool IsInTSRMode();

private:
	void UnloadExistingTap();
	void Reset();
	int TAP_Main_SEH();

	HMODULE m_hModule;
	CString m_sError;
	bool m_bTSR;

	TAP_EventHandlerFunc m_pEventHandler;
	TAP_MainFunc m_pMain;
	SetFrameworkFunc m_pFramework;
};
