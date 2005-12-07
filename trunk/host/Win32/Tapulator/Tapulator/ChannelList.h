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

#include "InfoLogger.h"
class CChannelList : InfoLogger
{
public:
	CChannelList(void);
	~CChannelList(void);

	int GetCurrentChannel(int mainSub);
	int IsTVOrRadio();

	int GetTVCount();
	int GetRadioCount();

	int GetInfo(int svcType, int svcNum, TYPE_TapChInfo *chInfo );

	void DecrementCurrentChannel();
	void IncrementCurrentChannel();

	int GetTotalNum(int *nTvSvc, int *nRadioSvc );
	int GetFirstInfo(int svcType, TYPE_TapChInfo *chInfo );
	int GetNextInfo(TYPE_TapChInfo *chInfo );
	int GetCurrent(int *tvRadio, int *svcNum );
	int GetCurrentSubTV(int *svcNum );
	int Start(int mainSub, int svcType, int chNum );
	int Scale(int mainSub, long x, long y, long w, long h, bool anim );
	int IsPlayable(int mainSub, int svcType, int chNum );
	int Move(int mainSub, int dir );
	int Stop(int mainSub );
	int GetTotalAudioTrack(void );
	char* GetAudioTrackName(int trkNum, char *trkName );
	int SetAudioTrack(int trkNum );
	int GetTotalSubtitleTrack(void );
	char* GetSubtitleTrackName(int trkNum, char *trkName );
	int SetSubtitleTrack(int trkNum );
	bool IsStarted(int mainSub );

	bool IsDirty();
	void ClearDirtyFlag();
	CRect GetChannelRect(int mainSub);

private:
	CString GetNameForChannel(int iChannelNum);

	int m_iCurrentChannel;
	int m_iCurrentSubChannel;
	int m_iFindNextPos;
	bool m_bStartMain;
	bool m_bStartSub;
	bool m_bIsDirty;
	CRect m_rcMain;
	CRect m_rcSub;
};
