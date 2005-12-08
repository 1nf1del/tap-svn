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
#include ".\channellist.h"
#include "MainFrm.h"
#include "Connfiguration.h"
#include "StringUtils.h"

CChannelList::CChannelList(void)
{
	m_iCurrentChannel = 0;
	m_iCurrentSubChannel = 1;
	m_iFindNextPos = -1;
	m_bStartMain = true;
	m_bStartSub = false;
	m_rcMain = CRect(0,0,720, 576);
	m_rcSub = CRect(450, 50, 690, 242);
	m_bIsDirty = false;

	CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
	CConfiguration* pConfig = pFrame->GetConfig();
	CStdioFile file;
	if ( file.Open( pConfig->GetChannelFile(), CFile::modeRead ) )
	{
		CString line;
		while ( file.ReadString( line ) )
		{
			CStringArray split;
			if ( Split( line, ",", split ) > 1 )
			{
				TYPE_TapChInfo chInfo;
				chInfo.logicalChNum = atoi( split[0] );
				strcpy(chInfo.satName, "");
				strcpy(chInfo.chName, split[1]);
				chInfo.dolbyTrack = -1;
				chInfo.ttxAvailable = 1;
				if ( split[2].CompareNoCase( "TV" ) == 0 )
					m_vecTV.push_back( chInfo );
				else
					m_vecRadio.push_back( chInfo );
			}
		}
	}
}

CChannelList::~CChannelList(void)
{
}


int CChannelList::GetCurrentChannel(int mainSub)
{
	return (mainSub == 1) ? m_iCurrentChannel : m_iCurrentSubChannel;
}

int CChannelList::IsTVOrRadio()
{
	return SVC_TYPE_Tv;
}

int CChannelList::GetTVCount()
{
	return m_vecTV.size();
}

int CChannelList::GetRadioCount()
{
	return m_vecRadio.size();
}


int CChannelList::GetInfo(int svcType, int svcNum, TYPE_TapChInfo *chInfo )
{
	ZeroMemory(chInfo, sizeof(TYPE_TapChInfo));
	std::vector<TYPE_TapChInfo>& v = svcType == SVC_TYPE_Radio ? m_vecRadio : m_vecTV;

	std::vector<TYPE_TapChInfo>::iterator p;
	int i = 0;
	for ( p = v.begin(); p != v.end(); ++p, ++i )
	{
		if ( i == svcNum )
		{
			*chInfo = *p;
			return 1;
		}
	}
	return 0;
}

void CChannelList::DecrementCurrentChannel()
{
	m_iCurrentChannel--;
	if (m_iCurrentChannel<0)
		m_iCurrentChannel = GetTVCount() - 1;
	m_bIsDirty = true;
}

void CChannelList::IncrementCurrentChannel()
{
	m_iCurrentChannel++;
	if (m_iCurrentChannel>=GetTVCount())
		m_iCurrentChannel = 0;
	m_bIsDirty = true;
}

int CChannelList::GetTotalNum(int *nTvSvc, int *nRadioSvc )
{
	*nTvSvc = GetTVCount();
	*nRadioSvc = GetRadioCount();
	return 1;
}

int CChannelList::GetFirstInfo(int svcType, TYPE_TapChInfo *chInfo )
{
	ZeroMemory(chInfo, sizeof(TYPE_TapChInfo));

	if (svcType == SVC_TYPE_Radio)
		return GetRadioCount();

	m_iFindNextPos = 0;
	GetInfo(svcType, m_iFindNextPos, chInfo);

	return GetTVCount();
}

int CChannelList::GetNextInfo(TYPE_TapChInfo *chInfo )
{
	if (m_iFindNextPos == -1)
		return 0;

	ZeroMemory(chInfo, sizeof(TYPE_TapChInfo));
	m_iFindNextPos++;
	GetInfo(SVC_TYPE_Tv, m_iFindNextPos, chInfo);

	if (m_iFindNextPos >= GetTVCount())
		return 0;

	return 1;
}

int CChannelList::GetCurrent(int *tvRadio, int *svcNum )
{
	*tvRadio = IsTVOrRadio();
	*svcNum =  GetCurrentChannel(1);
	return 0;
}

int CChannelList::GetCurrentSubTV(int *svcNum )
{
	*svcNum = m_iCurrentSubChannel;
	return 0;

}

int CChannelList::Start(int mainSub, int svcType, int chNum )
{
	if (svcType == SVC_TYPE_Radio)
		return -1;

	if (mainSub == 1) // main
	{
		m_bStartMain = true;
		m_iCurrentChannel = chNum;
	}
	else
	{
		m_bStartSub= true;
		m_iCurrentSubChannel = chNum;
	}
	m_bIsDirty = true;
	return 0;
}

int CChannelList::Scale(int mainSub, long x, long y, long w, long h, bool anim )
{
	if (mainSub == 1)
		m_rcMain = CRect(CPoint(x,y), CSize(w, h));
	else
		m_rcSub =  CRect(CPoint(x,y), CSize(w, h));
	m_bIsDirty = true;
	return 0;
}

int CChannelList::IsPlayable(int mainSub, int svcType, int chNum )
{
	return 0;
}

int CChannelList::Move(int mainSub, int dir )
{
	if (mainSub == 1)
	{
		m_iCurrentChannel = (m_iCurrentChannel + dir + GetTVCount()) % GetTVCount();
	}
	else
	{
		m_iCurrentSubChannel = (m_iCurrentSubChannel + dir + GetTVCount()) % GetTVCount();
	}
	m_bIsDirty = true;
	return 0;

}

int CChannelList::Stop(int mainSub )
{
	if (mainSub == 1)
	{
		m_bStartMain = false;
	}
	else
	{
		m_bStartSub = false;
	}
	m_bIsDirty = true;
	return 0;
}

int CChannelList::GetTotalAudioTrack(void )
{
	return 1;
}

char* CChannelList::GetAudioTrackName(int trkNum, char *trkName )
{
	static char* sName = "English";
	if (trkName)
		strcpy(trkName, sName);

	return sName;
}

int CChannelList::SetAudioTrack(int trkNum )
{
	m_bIsDirty = true;
	return (trkNum == 0);
}

int CChannelList::GetTotalSubtitleTrack(void )
{
	return 1;
}

char* CChannelList::GetSubtitleTrackName(int trkNum, char *trkName )
{
	static char* sName = "English";
	if (trkName)
		strcpy(trkName, sName);

	return sName;
}

int CChannelList::SetSubtitleTrack(int trkNum )
{
	m_bIsDirty = true;
	return (trkNum == 0);
}

bool CChannelList::IsStarted(int mainSub )
{
	return (mainSub == 1) ? m_bStartMain : m_bStartSub;
}

bool CChannelList::IsDirty()
{
	return m_bIsDirty;
}

void CChannelList::ClearDirtyFlag()
{
	m_bIsDirty = false;

}

CRect CChannelList::GetChannelRect(int mainSub)
{
	return (mainSub == 1) ? m_rcMain : m_rcSub;
}
