#include "StdAfx.h"
#include ".\meifilereader.h"
#include "MainFrm.h"
#include "Connfiguration.H"
#include "ChannelList.h"
#include "Heap.h"
#include "MJDUtil.h"

MEIFileReader::MEIFileReader(CChannelList* pChannelList) : m_pChannelList(pChannelList)
{
	m_iEventCount = 0;
	ReadAllData();
}

MEIFileReader::~MEIFileReader(void)
{
}

int MEIFileReader::ToppyToLogical(int svcType, int svcNum)
{
	if (svcType != SVC_TYPE_Tv)
		return -1;

	TYPE_TapChInfo chInfo;

	if (m_pChannelList->GetInfo(svcType, svcNum, &chInfo) == 0)
		return -1;

	return chInfo.logicalChNum;



}


TYPE_TapEvent* MEIFileReader::GetEvent(int svcType, int svcNum, int *eventNum )
{
	*eventNum = 0;
	int iLogicalChan = ToppyToLogical(svcType, svcNum);
	if (iLogicalChan < 0)
		return NULL;

	*eventNum = m_AllEPGInfo[iLogicalChan].size();

	void* pData = Heap::GetTheHeap()->Allocate(sizeof(TYPE_TapEvent) * *eventNum);

	memcpy(pData, &m_AllEPGInfo[iLogicalChan][0], sizeof(TYPE_TapEvent) * *eventNum);

	return (TYPE_TapEvent*) pData;



}


void MEIFileReader::ReadAllData()
{
	CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
	CConfiguration* pConfig = pFrame->GetConfig();
	FILE* f = fopen(pConfig->GetEPGFile(), "rb");
	if (f == 0)
		return;

	char buf[4096];
	while (!feof(f))
	{
		if (fgets(buf, 4095, f)!=NULL)
		{
			TYPE_TapEvent result;
			int iChan = BuildEvent(buf, result);
			if (iChan>=0)
			{
				m_AllEPGInfo[iChan].push_back(result);
			}
		}
	}

	fclose(f);
}

DWORD ReadDateTime(CString sYMDHM)
{
	if (sYMDHM.GetLength() != 12)
		return 0;
	int y = atoi(sYMDHM.Mid(0,4));	
	int m = atoi(sYMDHM.Mid(4,2));
	int d = atoi(sYMDHM.Mid(6,2));
	int h = atoi(sYMDHM.Mid(8,2));
	int min = atoi(sYMDHM.Mid(10,2));

	WORD wMJD = (word) MJDUtil::MakeMJD((word) y,(byte) m,(byte) d);
	WORD wTime = (word) ((h<<8) + min);

	return (wMJD<<16) + wTime;
}

int MEIFileReader::BuildEvent(CString sMEIdata, TYPE_TapEvent& result )
{
	int iChannelNum = -1;
	ZeroMemory(&result, sizeof(TYPE_TapEvent));

	CString sDesc;
	CString sGenre;

	int iField = 0;
	int iPos = -1;
	int iNextPos = -1;
	bool bDone = false;
	while (!bDone)
	{
		bool bLast = false;
		iNextPos = sMEIdata.Find('|', iPos+1);
		if (iNextPos == -1)
		{
			iNextPos = sMEIdata.Find('\r', iPos+1);
			if (iNextPos == -1)
				iNextPos = sMEIdata.GetLength();
			bLast = true;
		}
		
		CString sSub = sMEIdata.Mid(iPos + 1, iNextPos - iPos - 1);
		switch (iField)
		{
		case lineLength:
			break;
		case startTime:
			result.startTime = ReadDateTime(sSub);
			break;
		case endTime:
			result.endTime = ReadDateTime(sSub);
			break;
		case channelNum:
			iChannelNum = (word) atoi(sSub);
			break;
		case title:
			strncpy(result.eventName, sSub, 128);
			break;
		case subtitle:
//			m_sSubtitle = sSub;
			break;
		case repeat:
//			m_bRepeat = (sSub == "TRUE");
			break;
		case newprog:
			break;
		case year:
//			m_wYear = (word) atoi(sSub);
			break;
		case filmpremiere:
//			m_bFilmPremiere = !sSub.empty();
			break;
		case lastchance:
			break;
		case description:
			sDesc = sSub;
			break;
		case genre:
			sGenre = sSub;
			break;
		case runningTime:
			result.duration = (word) atoi(sSub);
			break;
		case aspect:
//			m_bWideScreen = (sSub == "16:9");
			break;
		case blackandwhite:
//			m_bBlackAndWhite = !sSub.empty();
			break;
		case subtitles:
			//if (sSub.empty())
			//	m_Subtitles = noSubtitles;
			//else if (sSub == "teletext")
			//	m_Subtitles = teletextSubtitles;
			//else if (sSub == "onscreen")
			//	m_sSubtitle = onscreenSubtitles;
			//else
			//	m_Subtitles = noSubtitles;
			break;
		case episodenum:
//			m_sEpisodeNum = sSub;
			break;
		case episodesys:
			break;
		case ratingsys:
			break;
		case rating:
//			m_sAgeRating = sSub;
			break;
		case starrating:
////			m_iStars = -1;
//			if (sSub.size() > 0)
//				m_iStars = (signed char) atoi(sSub);
			break;
		}

		iPos = iNextPos;
		iField++;
		if (bLast)
			break;
	}
#ifdef WIN32
	_ASSERT(iField == end_of_fields);
#endif

	result.evtId = m_iEventCount++;
	m_vecExtInfo.push_back(sDesc);
	sDesc = "[" + sGenre + "]" + sDesc;
	strncpy(result.description, sDesc, 128);

	return iChannelNum;
}

byte* MEIFileReader::GetExtInfo(TYPE_TapEvent *tapEvtInfo )
{
	if (!tapEvtInfo)
		return 0;

	if (tapEvtInfo->evtId<0 || tapEvtInfo->evtId>=m_vecExtInfo.size())
		return 0;

	CString& data = m_vecExtInfo[tapEvtInfo->evtId];
	char* result = (char*)Heap::GetTheHeap()->Allocate(data.GetLength()+1);
	strcpy(result, data);
	return (byte*)result;
}
