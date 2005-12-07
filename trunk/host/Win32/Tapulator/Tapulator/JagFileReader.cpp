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
#include ".\jagfilereader.h"
#include "MJDUtil.h"
#include "ChannelList.h"
#include "Heap.h"
#include "MainFrm.h"
#include "Connfiguration.H"

JagFileReader::JagFileReader(CChannelList* pChannelList) : m_pChannelList (pChannelList)
{
}

JagFileReader::~JagFileReader(void)
{
}

bool JagToEpg(JagData& source, TYPE_TapEvent& dest, int iID, int iSvcId)
{
	ZeroMemory(&dest, sizeof(TYPE_TapEvent));
	WORD startDate = MAKEWORD(HIBYTE(source.startDate), LOBYTE(source.startDate));
	WORD startTime = MAKEWORD(HIBYTE(source.startTime), LOBYTE(source.startTime));
	WORD endDate = MAKEWORD(HIBYTE(source.endDate), LOBYTE(source.endDate));
	WORD endTime = MAKEWORD(HIBYTE(source.endTime), LOBYTE(source.endTime));
	WORD duration = MAKEWORD(HIBYTE(source.duration), LOBYTE(source.duration));
	if (startDate == 0)
		return false;
	dest.duration = duration;
	dest.startTime = MAKELONG(startTime, startDate);
	dest.endTime = MAKELONG(endTime, endDate);
	dest.evtId = iID;
	dest.svcId = iSvcId;
	char* pSplit = strstr(source.buffer, " ~ ");
	if (pSplit == NULL)
	{
		strcpy(dest.eventName, source.buffer);
	}
	else
	{
		int iFirstBit = pSplit - source.buffer;
		strncpy(dest.eventName, source.buffer, iFirstBit);
		strncpy(dest.description, source.buffer + iFirstBit + 3, 125 - iFirstBit);
	}

	return true;
}

char* JagFileReader::LoadJagFile()
{
	CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
	CConfiguration* pConfig = pFrame->GetConfig();
	FILE* f = fopen(pConfig->GetEPGFile(), "rb");
	if (f == 0)
		return 0;

	fseek(f, 0, SEEK_END);
	int iSize = ftell(f);
	fseek(f, 0, SEEK_SET);

	char* pData = (char*) malloc(iSize);
	fread(pData, iSize, 1, f);
	fclose(f);

	return pData;
}

JagData* JagFileReader::FindChannelData(int svcType, int iChannelNum, char* pData)
{
	int iPos = -1;

	TYPE_TapChInfo chInfo;
	m_pChannelList->GetInfo(svcType, iChannelNum, &chInfo);

	char* pChanName = pData + 0x29a2;
	for (int i=0; i< 100; i++)
	{
		if (strnicmp(pChanName, chInfo.chName, 24) == 0)
		{
			iPos = i;		
			break;
		}
		pChanName += 26;
	}

	if (iPos == -1)
		return 0;

	JagData* pItem = (JagData*)(pData + 0x218c + 0x8a00 * iPos);
	return pItem;
}

TYPE_TapEvent* JagFileReader::GetEvent(int svcType, int svcNum, int *eventNum )
{
	*eventNum = 0;

	char* pData = LoadJagFile();

	if (pData == NULL)
		return NULL;

	JagData* pItem = FindChannelData(svcType, svcNum, pData);
	if (pItem == 0)
		return 0;

	TYPE_TapEvent* pEvents = (TYPE_TapEvent*) Heap::GetTheHeap()->Allocate(sizeof(TYPE_TapEvent)*256);
	TYPE_TapEvent* pThisEvent = pEvents;
	TYPE_TapEvent thisEvent;

	WORD wdToday = MJDUtil::Today();
	SYSTEMTIME time;
	GetSystemTime(&time);
	WORD wdTime = (time.wHour << 8) + time.wMinute;

	for (int i=0; i<256; i++)
	{
		if (JagToEpg(*pItem++, thisEvent, i, svcNum))
		{
			DWORD wdThisDate = (((thisEvent.endTime) & 0xFFFF0000)>> 16);
			if ( wdThisDate < wdToday)
				continue; // in the past

			if (((thisEvent.endTime) & 0x0000FFFF) < wdTime)
				continue; // today, but finished

			memcpy(pThisEvent, &thisEvent, sizeof(TYPE_TapEvent));

			pThisEvent++;
			(*eventNum)++;
		}
		else
			break;

	}

	pEvents[0].runningStatus = 4;
	
	free(pData);

	return pEvents;
}

TYPE_TapEvent* JagFileReader::GetCurrentEvent(int svcType, int svcNum)
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
