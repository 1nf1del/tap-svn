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
#include ".\epgevent.h"
#include "globals.h"
#include "Channels.h"
#include "timers.h"
#include <ctype.h>
#include <string.h>
#ifdef WIN32
#include <crtdbg.h>
#endif

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


EPGevent::EPGevent(TYPE_TapEvent* pEventData, int iLogicalChan, char* pExtData) 
{
	Init();
	m_sTitle = pEventData->eventName;
	m_sDescription = pEventData->description;
	m_TimeSlot.SetStart(pEventData->startTime);
	m_TimeSlot.SetEnd(pEventData->endTime);
	m_wChannelNum = (short)iLogicalChan;
	SetGenre();
	if (pExtData)
	{
		if (strncmp(pExtData, m_sDescription, m_sDescription.size()) == 0)
		{
			m_sDescription = pExtData;
		}
		else
		{
			m_sDescription += "\n";
			m_sDescription += pExtData;
		}
	}
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
			m_wChannelNum = (word)Globals::GetChannels()->ToppyToLogical(atoi(sSub)-1);
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
			m_wChannelNum = (word) atoi(sSub);
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
		}

		iPos = iNextPos;
		iField++;
		if (bLast)
			break;
	}
#ifdef WIN32
	_ASSERT(iField == end_of_fields);
#endif
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

bool EPGevent::IsContinuationOf(const EPGevent* other) const
{
	if (m_bContinuation)
		return false;

	if (m_wChannelNum >=80)
		return false;

	if (m_sTitle != other->m_sTitle)
		return false;

	int gap = GetStart() - other->GetEnd();
	int duration = Duration() + other->Duration();
		
	if (gap>40)
		return false;

	if (duration <= gap * 2)
		return false;

	if (duration < 75)
		return false; // that's no film

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

	return Globals::GetTimers()->CanRecord(this);
}

bool EPGevent::IsScheduledToRecord() const
{
	return (Globals::GetTimers()->IsScheduled(this));
}

int EPGevent::Recordability() const
{
	if (m_TimeSlot.Start().IsInPast())
		return -100;

	if (IsScheduledToRecord())
		return 200;

	int iCount = Globals::GetTimers()->GetClashingTimers(this).size();

	int iScore = 100 - (iCount * 100);

	int hour = (m_TimeSlot.Start().AsDateTime() & 0xFF00) >> 8;

	if (hour < 6) 
		iScore += 50;

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
