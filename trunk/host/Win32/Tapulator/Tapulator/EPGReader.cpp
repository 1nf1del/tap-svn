//Tapulator - An emulator for the Topfield PVR TAP API
//Copyright (C) 2006  Robin Glover
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
#include ".\epgreader.h"
#include "JagFileReader.h"
#include "MeiFileReader.h"
#include "MainFrm.h"
#include "Connfiguration.h"
#include "Heap.h"
EPGReader::EPGReader(CChannelList* pChannelList)
{
	m_pImpl = NULL;
	CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
	CConfiguration* pConfig = pFrame->GetConfig();
	CString sData = pConfig->GetEPGFile();
	sData = sData.MakeLower();

	if (sData.Right(3) == "dat")
		m_pImpl = new JagFileReader(pChannelList);
	else if (sData.Right(3) == "mei")
		m_pImpl = new MEIFileReader(pChannelList);
//	else...
}

EPGReader::~EPGReader(void)
{
	delete m_pImpl;
}

TYPE_TapEvent* EPGReader::GetEvent(int svcType, int svcNum, int *eventNum )
{
	if (m_pImpl)
		return m_pImpl->GetEvent(svcType, svcNum, eventNum);

	return NULL;
}


TYPE_TapEvent* EPGReader::GetCurrentEvent(int svcType, int svcNum)
{
	static TYPE_TapEvent eventInfo;
	int iCountEvents;
	TYPE_TapEvent* pData = GetEvent(svcType, svcNum, &iCountEvents);
	if (iCountEvents == 0)
	{
		ZeroMemory(&eventInfo, sizeof(TYPE_TapEvent));
	}
	else
	{
		memcpy(&eventInfo, pData, sizeof(TYPE_TapEvent));
	}
	Heap::GetTheHeap()->Release(pData);

	return &eventInfo;
}

