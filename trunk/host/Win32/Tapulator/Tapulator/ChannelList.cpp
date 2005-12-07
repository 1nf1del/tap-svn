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

static int LogicalChannelNums[] = {1,2,3,4,5,6,7,9,10,11,12,13,14,15,16,20,32,38,70,71,80,81,82,83};

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
	return sizeof(LogicalChannelNums)/sizeof(int);
}

int CChannelList::GetRadioCount()
{
	return 0;

}

int CChannelList::GetInfo(int svcType, int svcNum, TYPE_TapChInfo *chInfo )
{
	ZeroMemory(chInfo, sizeof(TYPE_TapChInfo));
	if (svcType == SVC_TYPE_Radio)
		return 0;

	if (svcNum >= GetTVCount())
		return 0;

	chInfo->logicalChNum = LogicalChannelNums[svcNum];
	strcpy (chInfo->chName, GetNameForChannel(chInfo->logicalChNum));
	chInfo->dolbyTrack = -1;
	chInfo->ttxAvailable = 1;
	return 1;
}

CString CChannelList::GetNameForChannel(int iChannelNum)
{
	CString sName;
	switch (iChannelNum)
	{
	case 1:
		sName="BBC ONE";
		break;
	case 2:
		sName="BBC TWO";
		break;
	case 3:
		sName="ITV1";
		break;
	case 4:
		sName="Channel 4";
		break;
	case 5:
		sName="five";
		break;
	case 6:
		sName="ITV2";
		break;
	case 7:
		sName="BBC THREE";
		break;
	case 8:
		sName="S4C";
		break;
	case 9:
		sName="BBC FOUR";
		break;
	case 10:
		sName="ITV3";
		break;
	case 11:
		sName="Sky Travel";
		break;
	case 12:
		sName="UKTV History";
		break;
	case 13:
		sName="More 4";
		break;
	case 14:
		sName="E4";
		break;
	case 15:
		sName="ABC1";
		break;
	case 16:
		sName="QVC";
		break;
	case 17:
		sName="UKTV Gold";
		break;
	case 18:
		sName="The Hits";
		break;
	case 19:
		sName="UKTV Br'tIdeas";
		break;
	case 20:
		sName="f tn";
		break;
	case 21:
		sName="TMF";
		break;
	case 22:
		sName="Ideal World";
		break;
	case 23:
		sName="Bid TV";
		break;
	case 24:
		sName="Price-Drop TV";
		break;
	case 25:
		sName="TCM";
		break;
	case 26:
		sName="UKTV Style";
		break;
	case 27:
		sName="Discovery";
		break;
	case 28:
		sName="DiscoveryRTime";
		break;
	case 29:
		sName="UKTV Food";
		break;
	case 32:
		sName="E4+1";
		break;
	case 33:
		sName="Eurosport";
		break;
	case 34:
		sName="Setanta Sports";
		break;
	case 36:
		sName="Xtraview";
		break;
	case 37:
		sName="Quiz Call";
		break;
	case 38:
		sName="Men & Motors";
		break;
	case 70:
		sName="CBBC Channel";
		break;
	case 71:
		sName="Cbeebies";
		break;
	case 72:
		sName="Cartoon Network";
		break;
	case 73:
		sName="Boomerang";
		break;
	case 74:
		sName="Toonami";
		break;
	case 80:
		sName="BBC News 24";
		break;
	case 81:
		sName="ITV News";
		break;
	case 82:
		sName="Sky News";
		break;
	case 83:
		sName="Sky Spts News";
		break;
	case 84:
		sName="Bloomberg";
		break;
	case 85:
		sName="BBC Parlmnt";
		break;
	case 86:
		sName="S4C2";
		break;
	case 87:
		sName="Community";
		break;
	case 88:
		sName="Teachers' TV";
		break;
	case 97:
		sName="Television X";
		break;
	case 98:
		sName="Red Hot";
		break;
	case 100:
		sName="Teletext";
		break;
	case 101:
		sName="Teletext TV Guide";
		break;
	case 102:
		sName="Teletext Holidays";
		break;
	case 103:
		sName="Teletext Cars";
		break;
	case 104:
		sName="Teletext on 4";
		break;
	case 105:
		sName="BBCi";
		break;
	case 106:
		sName="YooPlay";
		break;
	case 300:
		sName="4TV Interactive";
		break;
	case 301:
		sName="(BBCi)";
		break;
	case 302:
		sName="(BBCi)";
		break;
	case 303:
		sName="(BBCi)";
		break;
	}

	return sName;
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
