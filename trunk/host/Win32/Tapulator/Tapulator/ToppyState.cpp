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
#include ".\toppystate.h"
#include "ChannelList.h"
#include "JagFileReader.h"
#include <io.h>
#include "MJDUtil.h"

ToppyState::ToppyState(CChannelList* pChannelList, JagFileReader* pEPGSource)
{
	m_pChannelList = pChannelList;
	m_pEPGSource = pEPGSource;
	m_State = STATE_Normal;
	m_SubState = SUBSTATE_Normal;
	for (int i=0; i<= SYSOSD_ServiceStatus; i++)
		m_SysOSDCtl[i] = true;

	m_SysVars[SYSVAR_Vol] = 127;
	m_SysVars[SYSVAR_OsdLan] = LAN_English;
	m_SysVars[SYSVAR_SubLan] = LAN_English;
	m_SysVars[SYSVAR_AudLan] = LAN_English;
	m_SysVars[SYSVAR_TvType] = TVTYPE_Pal;
	m_SysVars[SYSVAR_VideoOut] = VIDEO_TYPE_Svhs;
	m_SysVars[SYSVAR_ScartType] = VIDEO_TYPE_Rgb;
	m_SysVars[SYSVAR_TvRatio] = SCREEN_RATIO_16_9;
	m_SysVars[SYSVAR_16_9_Display] = DISPLAY_MODE_CenterExtract;
	m_SysVars[SYSVAR_SoundMode] = SOUND_MODE_Stereo;
	m_SysVars[SYSVAR_UhfType] = 0;
	m_SysVars[SYSVAR_UhfCh] = 0;
	m_SysVars[SYSVAR_OsdAlpha] = 192;
	m_SysVars[SYSVAR_IBoxTime] = 10;
	m_SysVars[SYSVAR_IBoxPos] = 0; 
	m_SysVars[SYSVAR_Timeshift] = 1;
	m_bIsDirty = true;

	ZeroMemory(&m_recInfo, sizeof(TYPE_RecInfo)*2);
	ZeroMemory(&m_playInfo, sizeof(TYPE_PlayInfo));
}

ToppyState::~ToppyState(void)
{
}

void ToppyState::DrawScreen(CDC* pDC, CRect rcScreen, int iChannelNum, bool bMain)
{
	COLORREF crBack = bMain ? WinRGB(0,0,0) : WinRGB(201, 205, 44);
	COLORREF crFore = bMain ? WinRGB(20,20,44) : WinRGB(20, 20, 44);

	pDC->FillSolidRect(rcScreen, crBack);

	pDC->SetBkColor(crBack);
	pDC->SetTextColor(crFore);
	CFont theFont;
	theFont.CreatePointFont(rcScreen.Width() , "Arial", pDC);
	CFont* oldFont = pDC->SelectObject(&theFont);

	TYPE_TapChInfo chInfo;
	m_pChannelList->GetInfo(SVC_TYPE_Tv, iChannelNum, &chInfo);

	pDC->DrawText(chInfo.chName, rcScreen, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
	pDC->SelectObject(oldFont);
	m_bIsDirty = false;
	m_pChannelList->ClearDirtyFlag();
}

bool ToppyState::IsDirty()
{
	return m_bIsDirty || m_pChannelList->IsDirty();
}

void ToppyState::Draw(CDC* pDC)
{
	if (m_pChannelList->IsStarted(1))
		DrawScreen(pDC, m_pChannelList->GetChannelRect(1), m_pChannelList->GetCurrentChannel(1), true);
	if (m_pChannelList->IsStarted(0))
		DrawScreen(pDC, m_pChannelList->GetChannelRect(0), m_pChannelList->GetCurrentChannel(0), false);
	DrawState(pDC);
}

void ToppyState::DrawState(CDC* pDC)
{
	CRect rcInfo(0,0,720,20);
	pDC->SetTextColor(WinRGB(0,255,255));
	CFont theFont;
	theFont.CreatePointFont(140 , "Arial", pDC);
	CFont* oldFont = pDC->SelectObject(&theFont);

	pDC->SetBkMode(TRANSPARENT);
	pDC->DrawText(GetStateText(), rcInfo, DT_LEFT | DT_TOP);

	pDC->SelectObject(oldFont);
}

void ToppyState::Event(unsigned short event, unsigned long param1, unsigned long param2 )
{
	if (event != EVT_KEY)
		return;

	switch (param1)
	{
	case RKEY_Menu:
		if (m_State == STATE_Normal)
		{
			m_State = STATE_Menu;
			m_SubState = SUBSTATE_MainMenu;
			m_bIsDirty = true;
			break;
		}
		if (m_State == STATE_Menu)
		{
			switch (m_SubState)
			{
			case SUBSTATE_MainMenu:
				m_State = STATE_Normal;
				m_SubState = SUBSTATE_Normal;
				m_bIsDirty = true;
				break;
			}
			break;
		}
		break;
	case RKEY_VolDown:
		if (m_State == STATE_Normal)
		{
			SetSystemVar(SYSVAR_Vol, max(0, GetSystemVar(SYSVAR_Vol)-1));
		}
		break;
	case RKEY_VolUp:
		if (m_State == STATE_Normal)
		{
			SetSystemVar(SYSVAR_Vol, min(127, GetSystemVar(SYSVAR_Vol)+1));
		}
		break;
	case RKEY_ChDown:
		if (m_State == STATE_Normal)
		{
			m_pChannelList->DecrementCurrentChannel();
			m_bIsDirty = true;
		}
		break;
	case RKEY_ChUp:
		if (m_State == STATE_Normal)
		{
			m_pChannelList->IncrementCurrentChannel();
			m_bIsDirty = true;
		}
		break;

	}
}

DWORD ToppyState::GetState(DWORD *state, DWORD *subState )
{
	*state = m_State;
	*subState = m_SubState;
	return 1;

}

CString ToppyState::GetStateText()
{
	return DescribeState() + "," + DescribeSubState();
}
CString ToppyState::DescribeState()
{
	switch (m_State)
	{
	case STATE_Normal:
		return "Normal";
	case STATE_Menu:
		return "Menu";
	case STATE_Epg:
		return "Epg";
	case STATE_List:
		return "List";
	case STATE_Ttx:
		return "Ttx";
	case STATE_Game:
		return "Game";
	case STATE_FileList:
		return "FileList";
	case STATE_Tap:
		return "Tap";
	}
	return "???";
}


CString ToppyState::DescribeSubState()
{
	switch (m_SubState)
	{
	case SUBSTATE_MainMenu:
		return "MainMenu";
	case SUBSTATE_TimeMenu:
		return "TimeMenu";
	case SUBSTATE_TimeSettingMenu:
		return "TimeSettingMenu";
	case SUBSTATE_InstallationMenu:
		return "InstallationMenu";
	case SUBSTATE_TimerMenu:
		return "TimerMenu";
	case SUBSTATE_LanguageMenu:
		return "LanguageMenu";
	case SUBSTATE_ParentMenu:
		return "ParentMenu";
	case SUBSTATE_ParentLockMenu:
		return "ParentLockMenu";
	case SUBSTATE_AvMenu:
		return "AvMenu";
	case SUBSTATE_ChListMenu:
		return "ChListMenu";
	case SUBSTATE_FavChListMenu:
		return "FavChListMenu";
	case SUBSTATE_SearchMenu:
		return "SearchMenu";
	case SUBSTATE_LnbSettingMenu:
		return "LnbSettingMenu";
	case SUBSTATE_Diseqc12SettingMenu:
		return "Diseqc12SettingMenu";
	case SUBSTATE_USALSMenu:
		return "USALSMenu";
	case SUBSTATE_CiMenu:
		return "CiMenu";
	case SUBSTATE_Ci:
		return "Ci";
	case SUBSTATE_CiSlotMenu:
		return "CiSlotMenu";
	case SUBSTATE_SatSearch:
		return "SatSearch";
	case SUBSTATE_TpSearch:
		return "TpSearch";
	case SUBSTATE_SmatvSearch:
		return "SmatvSearch";
	case SUBSTATE_ChannelEditMenu:
		return "ChannelEditMenu";
	case SUBSTATE_InformationMenu:
		return "InformationMenu";
	case SUBSTATE_SystemMenu:
		return "SystemMenu";
	case SUBSTATE_GameMenu:
		return "GameMenu";
	case SUBSTATE_OtaMenu:
		return "OtaMenu";
	case SUBSTATE_IrdStatus:
		return "IrdStatus";
	case SUBSTATE_Carlendar:
		return "Carlendar";
	case SUBSTATE_PvrRecord:
		return "PvrRecord";
	case SUBSTATE_PvrReservation:
		return "PvrReservation";
	case SUBSTATE_ReservationList:
		return "ReservationList";
	case SUBSTATE_Normal:
		return "Normal";
	case SUBSTATE_Audio:
		return "Audio";
	case SUBSTATE_Subt:
		return "Subt";
	case SUBSTATE_Ttx:
		return "Ttx";
	case SUBSTATE_Fav:
		return "Fav";
	case SUBSTATE_Sat:
		return "Sat";
	case SUBSTATE_PvrTimeSearch:
		return "PvrTimeSearch";
	case SUBSTATE_PvrPlayingSearch:
		return "PvrPlayingSearch";
	case SUBSTATE_PvrRecSearch:
		return "PvrRecSearch";
	case SUBSTATE_PipList:
		return "PipList";
	case SUBSTATE_Multifeed:
		return "Multifeed";
	case SUBSTATE_List:
		return "List";
	case SUBSTATE_Epg:
		return "Epg";
	case SUBSTATE_Game:
		return "Game";
	case SUBSTATE_PvrList:
		return "PvrList";
	case SUBSTATE_ExeList:
		return "ExeList";
	case SUBSTATE_Exec:
		return "Exec";
	case SUBSTATE_TtxEmul:
		return "TtxEmul";
	}

	return "???";
}

void ToppyState::SysOsdControl(TYPE_TapSysOsdId osdId, bool ctrl )
{
	m_SysOSDCtl[osdId] = ctrl;
	m_bIsDirty = true;
}

int ToppyState::SetSystemVar(int varId, int value )
{
	m_SysVars[varId] = value;
	m_bIsDirty = true;
	return value;
}

int ToppyState::GetSystemVar(int varId )
{
	return m_SysVars[varId];
}

int ToppyState::WriteSystemVar(void )
{
	return 1;
}


int ToppyState::PlayTs(CString name )
{
	if (_taccess(name, 00) == 0)
	{
		m_playName = name;
	
		ZeroMemory(&m_playInfo, sizeof(TYPE_PlayInfo));

		TYPE_RecInfo recInfo;
		FILE* pFile = fopen(name, "r");
		fread(&m_playInfo.evtInfo, sizeof(TYPE_TapEvent), 1, pFile);
		fread(&recInfo, sizeof(TYPE_RecInfo), 1, pFile);
		fclose(pFile);

		m_playInfo.duration = recInfo.duration;
		m_playInfo.playMode = PLAYMODE_Playing;
		m_playInfo.totalBlock = 1000;
		m_playInfo.trickMode = TRICKMODE_Normal;
		m_playInfo.speed = 1;
		m_playInfo.svcNum = recInfo.svcNum;
		m_playInfo.svcType = recInfo.svcType;

		return 0;
	}
	return -1;
}

int ToppyState::StopTs(void)
{
	if (m_playName.IsEmpty())
		return -1;

	m_playName = "";
	return 0;
}

int ToppyState::PlayMp3(CString name )
{
	LogUnimplemented(__FUNCTION__ "(" __FUNCSIG__ ")");
	return 0;
}

int ToppyState::StopMp3(void )
{
	LogUnimplemented(__FUNCTION__ "(" __FUNCSIG__ ")");
	return 0;
}

bool ToppyState::StartRecord(CString destFolder )
{
	int iSlot = -1;
	if (m_recNames[0].IsEmpty())
		iSlot = 0;
	else if (m_recNames[1].IsEmpty())
		iSlot = 1;

	if (iSlot == -1)
		return false;

	int iChannelNum = m_pChannelList->GetCurrentChannel(1); 
	TYPE_TapEvent* pEvent =	m_pEPGSource->GetCurrentEvent(SVC_TYPE_Tv, iChannelNum);

	CString sName = pEvent->eventName;
	if (sName.IsEmpty())
		sName = "NoName";

	sName = destFolder + "\\" + sName;

	CString sRealName;
	for (int i=1; i<100; i++)
	{
		sRealName.Format("%s%d.rec", (LPCTSTR) sName, i);
		if (_taccess(sRealName, 00) != 0)
			break;

		sRealName = "";
	}

	if (sRealName.IsEmpty())
		return false;

	m_recNames[iSlot] = sRealName;

	strncpy(m_recInfo[iSlot].fileName, sRealName, 95);
	m_recInfo[iSlot].tuner = iSlot;
	m_recInfo[iSlot].recType = RECTYPE_Normal;
	m_recInfo[iSlot].recordedSec = 0;
	m_recInfo[iSlot].duration  = 30;
	m_recInfo[iSlot].svcNum = iChannelNum;
	m_recInfo[iSlot].svcType = SVC_TYPE_Tv;
	m_recInfo[iSlot].startTime = MJDUtil::DateTimeNowAsDWORD();

	FILE* pFile = fopen(sRealName, "w");
	fwrite(pEvent, sizeof(TYPE_TapEvent), 1, pFile);
	fwrite(&m_recInfo[iSlot], sizeof(TYPE_RecInfo), 1, pFile);
	fclose(pFile);

	
	return true;
}

bool ToppyState::StopRecord(BYTE recSlot )
{
	bool bResult = !m_recNames[recSlot].IsEmpty();
	m_recNames[recSlot] = "";
	return bResult;
}

bool ToppyState::GetRecInfo(BYTE recSlot, TYPE_RecInfo *recInfo )
{
	ZeroMemory(recInfo, sizeof(TYPE_RecInfo));
	if (m_recNames[recSlot].IsEmpty())
		return false;

	memcpy(recInfo, &m_recInfo[recSlot], sizeof(TYPE_RecInfo));
	return true;
}

bool ToppyState::GetPlayInfo(TYPE_PlayInfo *playInfo )
{
	if (m_playName.IsEmpty())
		return false;

	memcpy(playInfo, &m_playInfo, sizeof(TYPE_PlayInfo));

	return true;
}
