/*
Copyright (C) 2006 Robin Glover

This file is part of the TAPs for Topfield PVRs project.
http://tap.berlios.de/

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/
#include "epgevent.h"
#include "globals.h"
#include "Channels.h"
#include "timers.h"
#include <ctype.h>
#include <string.h>
#ifdef WIN32
#include <crtdbg.h>
#endif
#include "DirectoryUtils.h"
#include "Logger.h"

dword EPGevent::sm_dwFlags = 0;

EPGevent::EPGevent(const string& sMEIdata) 
{
	Init();
	Parse(sMEIdata);
}

EPGevent::EPGevent(const string& sJagData, bool bJags)
{
	Init();
	bJags;
	ParseJags(sJagData);
}


EPGevent::EPGevent(TYPE_TapEvent* pEventData, int iChannelNum, char* pExtData) 
{
	Init();
	m_sTitle = pEventData->eventName;
	m_sDescription = pEventData->description;
	m_TimeSlot.SetStart(pEventData->startTime);
	m_TimeSlot.SetEnd(pEventData->endTime);
	m_wChannelNum = (short)iChannelNum;
	SetGenre();
	if (pExtData)
	{
		if (strncmp(pExtData, m_sDescription, m_sDescription.size()) == 0)
		{
			m_sDescription = pExtData;
		}
		else if (strncmp(pExtData, "crid", 4) == 0)
		{
			ReadCrids(pExtData);
		}
		else
		{
			m_sDescription += "\n";
			m_sDescription += pExtData;
		}
	}
	SetDescription();
}

void EPGevent::Init()
{
	m_pContinuesAs = NULL;
	m_bContinuation = false;
	m_wChannelNum = -1;
	m_wYear = -1;
	m_wRunningTime = 0;
	m_Subtitles = noSubtitles;
	m_bBlackAndWhite = false;
	m_bFilmPremiere = false;
	m_bRepeat = false;
	m_bWideScreen = false;
	m_iStars = 0;
}

EPGevent::~EPGevent(void)
{
}

void EPGevent::ParseJags(const string& sJagData)
{
	int iField = 0;
	int iPos = -1;
	int iNextPos = -1;
	bool bDone = false;
	while (!bDone)
	{
		bool bLast = false;
		iNextPos = sJagData.find('|', iPos);
		if (iNextPos == -1)
		{
			iNextPos = sJagData.find('\r', iPos);
			if (iNextPos == -1)
				iNextPos = sJagData.size();
			bLast = true;
		}

		string sSub = sJagData.substr(iPos + 1, iNextPos - iPos - 1);
		switch (iField)
		{
		case JCSVchanName:
			break;
		case JCSVstartTime:
			m_TimeSlot.SetStart(PackedDateTime(sSub));
			break;
		case JCSVendTime:
			m_TimeSlot.SetEnd(PackedDateTime(sSub));
			if (m_TimeSlot.End().IsInPast())
				return;
			break;
		case JCSVtimeOffset:
			break;
		case JCSVtitle:
			m_sTitle = sSub;
			break;
		case JCSVshortDescription:
			m_sDescription = sSub;
			break;
		case JCSVlongDescription:
			break;
		case JCSVcategory:
			break;
		case JCSVserviceNumber:
			m_wChannelNum = (word)(atoi(sSub)-1);
			break;
		case JCSVserviceName:
			break;
		case JCSVserviceId:
			break;
		}

		iPos = iNextPos;
		iField++;
		if (bLast)
			break;
	}
#ifdef WIN32
	_ASSERT(iField == JAGCSVend_of_fields);
#endif

	SetDescription();
	m_sGenre = "No Genre";
}

void EPGevent::Parse(const string& sMEIdata)
{
	int iField = 0;
	int iPos = -1;
	int iNextPos = -1;
	bool bDone = false;
	while (!bDone)
	{
		bool bLast = false;
		iNextPos = sMEIdata.find('|', iPos);
		if (iNextPos == -1)
		{
			iNextPos = sMEIdata.find('\r', iPos);
			if (iNextPos == -1)
				iNextPos = sMEIdata.size();
			bLast = true;
		}

		string sSub = sMEIdata.substr(iPos + 1, iNextPos - iPos - 1);
		switch (iField)
		{
		case lineLength:
			break;
		case startTime:
			m_TimeSlot.SetStart(PackedDateTime(sSub));
			break;
		case endTime:
			m_TimeSlot.SetEnd(PackedDateTime(sSub));
			if (m_TimeSlot.End().IsInPast())
				return;
			break;
		case channelNum:
			m_wChannelNum = (word) Globals::GetTheGlobals().GetChannels()->LogicalToToppy(atoi(sSub));
			break;
		case title:
			m_sTitle = sSub;
			break;
		case subtitle:
			m_sSubtitle = sSub;
			break;
		case repeat:
			m_bRepeat = (sSub == "TRUE");
			break;
		case newprog:
			m_bNewProgram = (sSub == "TRUE");
			break;
		case year:
			m_wYear = (word) atoi(sSub);
			break;
		case filmpremiere:
			m_bFilmPremiere = !sSub.empty();
			break;
		case lastchance:
			break;
		case description:
			m_sDescription = sSub;
			break;
		case genre:
			m_sGenre = sSub;
			if (m_sGenre.empty())
				m_sGenre = "No Genre";
			break;
		case runningTime:
			m_wRunningTime = (word) atoi(sSub);
			break;
		case aspect:
			m_bWideScreen = (sSub == "16:9");
			break;
		case blackandwhite:
			m_bBlackAndWhite = !sSub.empty();
			break;
		case subtitles:
			if (sSub.empty())
				m_Subtitles = noSubtitles;
			else if (sSub == "teletext")
				m_Subtitles = teletextSubtitles;
			else if (sSub == "onscreen")
				m_Subtitles = onscreenSubtitles;
			else
				m_Subtitles = noSubtitles;
			break;
		case episodenum:
			m_sEpisodeNum = sSub;
			break;
		case episodesys:
			break;
		case ratingsys:
			break;
		case rating:
			m_sAgeRating = sSub;
			break;
		case starrating:
			m_iStars = -1;
			if (sSub.size() > 0)
				m_iStars = (signed char) atoi(sSub);
			break;
		case crid31:
			m_sProgramId = sSub;
			break;
		case crid32:
			m_sSeriesId = sSub;
			break;
		}

		iPos = iNextPos;
		iField++;
		if (bLast)
			break;
	}
//#ifdef WIN32
//	_ASSERT(iField == end_of_fields);
//#endif
	SetDescription();
}

void EPGevent::SetDescription()
{
	if (m_sProgramId.size()>0)
		m_sDescription +="\nProgramID = " + m_sProgramId;
	if (m_sSeriesId.size()>0)
		m_sDescription += "\nSeriesID = " + m_sSeriesId;
	int iMaxLen = GetMaxDescriptionLength();
	if (m_sDescription.size() > iMaxLen)
	{
		string sBit = m_sDescription.substr(0, iMaxLen);
		m_sDescription.clear();
		m_sDescription = sBit;
	}
}

word EPGevent::GetChannelNum() const
{
	return m_wChannelNum;
}

const string& EPGevent::GetTitle() const
{
	return m_sTitle;
}


const string& EPGevent::GetDescription() const
{
	return m_sDescription;
}

const PackedDateTime& EPGevent::GetStart() const
{
	return m_TimeSlot.Start();
}

const PackedDateTime& EPGevent::GetEnd() const
{
	if (m_pContinuesAs)
		return m_pContinuesAs->GetEnd();

	return m_TimeSlot.End();
}

string RemoveStarsFromTitle(const string& title)
{
	string result = title;
	result.replace('*', ' ');
	return result.trim();
}

bool EPGevent::IsContinuationOf(const EPGevent* other) const
{
	if (m_bContinuation)
		return false;

	//if (m_wChannelNum >=80)
	//	return false;


	int gap = GetStart() - other->GetEnd();
	int duration = Duration() + other->Duration();

	if (gap>40)
		return false;

	if (duration <= gap * 2)
		return false;

	if (duration < 75)
		return false; // that's no film


	if (RemoveStarsFromTitle(m_sTitle) != RemoveStarsFromTitle(other->m_sTitle))
	{
		return false;
	}

	return true;
}

int EPGevent::Duration() const
{
	return GetEnd() - GetStart(); // now includes continuations correctly, but records all the stuff in between
}

void EPGevent::SetContinuation(EPGevent* pCont)
{
	m_pContinuesAs = pCont;
	pCont->m_bContinuation = true;
}

bool EPGevent::IsDuplicateShowingOf(const EPGevent* pEvent) const
{
	if (m_sTitle.compareNoCase(pEvent->m_sTitle)!=0)
		return false;

	if (m_sSubtitle.compareNoCase(pEvent->m_sSubtitle)!=0)
		return false;

	if ((m_sGenre.compareNoCase("film") == 0) && (pEvent->GetGenre().compareNoCase("film") == 0))
	{
		if (m_wYear!=-1)
			return m_wYear == pEvent->m_wYear;
	}

	if (m_sDescription.compareNoCase(pEvent->m_sDescription)!=0)
		return false;

	return true;
}

bool EPGevent::IsSplit() const
{
	return m_pContinuesAs != NULL;
}

bool EPGevent::IsContinuation() const
{
	return m_bContinuation;
}

const TimeSlot& EPGevent::GetTimeSlot() const
{
	return m_TimeSlot;
}

const string& EPGevent::GetGenre() const
{
	return m_sGenre;
}

const string& EPGevent::GetSubTitle() const
{
	return m_sSubtitle;
}

bool EPGevent::CanRecord() const
{
	if (m_TimeSlot.Start().IsInPast())
		return false;

	return Globals::GetTheGlobals().GetTimers()->CanRecord(this);
}

bool EPGevent::IsScheduledToRecord() const
{
	return (Globals::GetTheGlobals().GetTimers()->IsScheduled(this));
}

int EPGevent::Recordability(RecordabilityCalculationFlags flags) const
{
	if (flags & CareAboutProgramStarted)
	{
		if (m_TimeSlot.Start().IsInPast())
			return -100;
	}

	if (flags & CareIfScheduledToRecord)
	{
		if (IsScheduledToRecord())
			return 200;
	}

	int iCount = Globals::GetTheGlobals().GetTimers()->GetClashingTimers(this).size();

	int iScore = 0;

	if (flags & CareAboutClashingTimers)
		iScore = 100 - (iCount * 100);


	if (flags & PreferLateNightShowings)
	{
		int hour = (m_TimeSlot.Start().AsDateTime() & 0xFF00) >> 8;

		if (hour < 6) 
			iScore += 50;
	}

	return iScore;
}

int EPGevent::GetStarRating() const
{
	return m_iStars;
}

const string& EPGevent::GetEpisode() const
{
	return m_sEpisodeNum;
}

string EPGevent::GetFileName() const
{
	string sResult = m_sTitle;
	sResult.replace(':', ' ');
	sResult.replace('*', ' ');
	return sResult.trim();
}

void EPGevent::SetGenre()
{
	m_sGenre = "No Genre";
	if ((sm_dwFlags & EPGDATA_BUILTIN_GENRESINSQUAREBRACKETS)==0)
		return;

	int iPos1 = m_sDescription.find('[');
	int iPos2 = m_sDescription.find(']',iPos1);
	if (iPos1 > -1 && iPos2 > -1)
	{
		string sPosGenre = m_sDescription.substr(iPos1+1, iPos2-iPos1-1);
		if (sPosGenre.size() && isalpha(sPosGenre[0]))
			m_sGenre = sPosGenre;

		m_sDescription = m_sDescription.substr(0, iPos1) + m_sDescription.substr(iPos2+1);
	}
}

void EPGevent::SetFlags(dword dwNewFlags)
{
	sm_dwFlags = dwNewFlags;
}

unsigned short EPGevent::GetMaxDescriptionLength()
{
	int iVal = (sm_dwFlags & EPGDATA_DESCRIPTION_MAXLEN_MASK) >> 5;
	if (iVal == 0)
		iVal = 7;

	unsigned short iResult = (unsigned short) (32 << iVal);

	return iResult;
}

unsigned short EPGevent::GetDaysToLoad()
{
	int iVal = (sm_dwFlags & EPGDATA_DAYSTOLOAD_MASK) >> 1;
	if (iVal == 0)
		iVal = 15;

	return  (unsigned short) iVal;
}

bool EPGevent::IsNew() const
{
	return (m_bNewProgram || m_sEpisodeNum.substr(0,2) == "1/");
}

string EPGevent::GetCleanTitle() const
{
	string sTitle = m_sTitle;
	if (sTitle.substr(0,5) == "Film:")
		sTitle = sTitle.substr(5);
	if (sTitle.substr(0,5) == "Movie:")
		sTitle = sTitle.substr(5);

	while (sTitle.reverseFind('*') == sTitle.size()-1)
		sTitle = sTitle.substr(0, sTitle.size()-1);

	return sTitle.trim();

}

string EPGevent::GetControlTimerFolder()
{
	if (FileExists("/ProgramFiles/Settings/MyStuff"))
		return "/ProgramFiles/Settings/MyStuff/CTs";
	return "/ProgramFiles/MST";
}

bool EPGevent::WriteMyStuffControlTimer() const
{

	//	MST Format from BobD
	// Each field separated by ascii 124
	//
	//1) Version Number - currently 9
	//2) Keywords - as dispayed on CT edit screen eg "The Simpsons~Special" - but without the quotes!
	//3) Priority - 1 is highest
	//4) Ignore +1s - 1 => Yes ignore them, 0 => No don't ignore them
	//5) Record - 0 => Watch, 1=> Record, 2=> Off
	//6) Match - 1=> Exact, 2=> Anywhere, 3=> Start, 4=> End
	//7) LCN - -1 => All (when combined with field 4), otherwise lcn to match
	//8) - 0 => "All Days", 1=> "Mon-Fri", 2=> "Mon", "Tue","Wed","Thu","Fri","Sat","Sun", 9=> "Sat/Sun"
	//9) Start time - hhmm
	//10) Duration - Start 0000 duration 0 => 24 hours
	//11) Pre Padding - 200 => use defaults
	//12) Post Padding - 200 => use defaults
	//13) AutoMove path 

	// Notes from RWG:
	// * terminate with a zero char in the file - mst files created by mystuff have typical toppy
	//		junk padding to 512 bytes due to the firmware bugs associated with file writing/sizes
	// * fields 3 & 4 seem to be reversed between the above docs and real MST files
	// * should be OK just to drop the files (with any name) into the MST folder

	string sMSTText;
	sMSTText.format("9|%s|-1|1|1|1|-1|0|0000|0|200|200|", GetTitle().c_str());
	string sFileName;
	string sFolder = GetControlTimerFolder(); 
	CreateDirectory(sFolder);
	sFileName.format("%s/MeiSearch_%s.mst", sFolder.c_str(), GetCleanTitle().c_str());
	TYPE_File* pFile = OpenRawFile(sFileName, "w");
	if (pFile != NULL)
	{
		TAP_Hdd_Fwrite((void*)sMSTText.c_str(),sMSTText.size()+1, 1, pFile);
		TAP_Hdd_Fclose(pFile);
		return true;
	}
	else
	{
		return false;
	}
}

string EPGevent::GetYear() const
{
	string result;
	if (m_wYear > 0)
		result.format("%d", m_wYear);

	return result;

}

string EPGevent::GetStars() const
{
	if (m_iStars>0 && m_iStars<6)
	{
		string sStars;
		for (int i=0; i<m_iStars; i++)
			sStars+="*";

		if (m_sTitle.find(sStars) != m_sTitle.size() - m_iStars)
			return sStars;
	}
	return "";
}

const string& EPGevent::GetAgeRating() const
{
	return m_sAgeRating;
}

void EPGevent::ReadCrids(char* pExtData)
{
	while (strncmp(pExtData, "crid", 4) == 0)
	{
		string sKey = pExtData;
		pExtData += sKey.size()+1; 
		string sValue = pExtData;
		pExtData+=sValue.size()+1;
//		TRACE2("Read crid of key,value %s\n", sKey.c_str(), sValue.c_str());

		if (sKey.find("31")!=-1)
			m_sProgramId = sValue;

		if (sKey.find("32")!=-1)
			m_sSeriesId = sValue;

	}


}

const string& EPGevent::GetSeriesId() const
{
	return m_sSeriesId;
}

const string& EPGevent::GetProgramId() const
{
	return m_sProgramId;
}