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
#include "infologger.h"

class CChannelList;
class JagFileReader;

class ToppyState :
	public InfoLogger
{
public:
	ToppyState(CChannelList* pChannelList, JagFileReader* pEPGSource);
	~ToppyState(void);

	void Draw(CDC* pDC);
	void Event(unsigned short event, unsigned long param1, unsigned long param2 );
	DWORD GetState(DWORD *state, DWORD *subState );
	void SysOsdControl(TYPE_TapSysOsdId osdId, bool ctrl );
	int SetSystemVar(int varId, int value );
	int GetSystemVar(int varId );
	int WriteSystemVar(void );

	bool IsDirty();

	int PlayTs(CString name );
	int StopTs(void);
	int PlayMp3(CString name );
	int StopMp3(void );

	bool StartRecord(CString destFolder );
	bool StopRecord(BYTE recSlot );
	bool GetRecInfo(BYTE recSlot, TYPE_RecInfo *recInfo );
	bool GetPlayInfo(TYPE_PlayInfo *playInfo );


private:
	void DrawScreen(CDC* pDC, CRect rcScreen, int iChannelNum, bool bMain);
	void DrawState(CDC* pDC);
	CString GetStateText();
	CString DescribeState();
	CString DescribeSubState();


	CChannelList* m_pChannelList;
	JagFileReader* m_pEPGSource;
	DWORD m_State;
	DWORD m_SubState;
	bool m_SysOSDCtl[SYSOSD_ServiceStatus+1];
	int m_SysVars[SYSVAR_Timeshift + 1];
	bool m_bIsDirty;

	TYPE_RecInfo m_recInfo[2];
	CString m_recNames[2];
	TYPE_PlayInfo m_playInfo;
	CString m_playName;
};
