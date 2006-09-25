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
#ifndef cpputils_epgevent_h
#define cpputils_epgevent_h

#include <stdlib.h>
#include "tap.h"
#include "tapstring.h"
#include "TimeSlot.h"


class EPGevent
{
public:
	EPGevent(const string& sMEIdata);
	EPGevent(const string& sJagData, bool bJags);
	EPGevent(TYPE_TapEvent* pEventData, int iChannelNum, char* pExtData);
	~EPGevent(void);

	enum subtitleType
	{
		noSubtitles,
		teletextSubtitles,
		onscreenSubtitles
	};

	word GetChannelNum() const;
	const string& GetTitle() const;
	string GetCleanTitle() const;
	const string& GetGenre() const;
	const string& GetSubTitle() const;
	const string& GetDescription() const;
	const string& GetEpisode() const;
	string GetYear() const;
	string GetStars() const;
	int GetStarRating() const;
	const PackedDateTime& GetStart() const;
	const PackedDateTime& GetEnd() const;
	const TimeSlot& GetTimeSlot() const;

	bool IsContinuationOf(const EPGevent* other) const;
	void SetContinuation(EPGevent* pCont);
	int Duration() const;
	bool IsDuplicateShowingOf(const EPGevent* pEvent) const;
	bool IsSplit() const;
	bool IsContinuation() const;


	bool CanRecord() const;
	bool IsScheduledToRecord() const;
	bool IsNew() const;

	enum RecordabilityCalculationFlags
	{
		CareAboutProgramStarted=0x01,
		CareAboutClashingTimers=0x02,
		PreferLateNightShowings=0x04,
		CareIfScheduledToRecord=0x08,

		DefaultRecordabilityFlags = 0x0F
	};

	int Recordability(RecordabilityCalculationFlags flags = DefaultRecordabilityFlags) const;
	string GetFileName() const;
	bool WriteMyStuffControlTimer() const;	

	static void SetFlags(dword dwNewFlags);
	static unsigned short GetDaysToLoad();

private:
	
	void Parse(const string& sMEIdata);
	void ParseJags(const string& sJagData);
	void Init();
	void SetGenre();
	void SetDescription();

	enum jagfieldIndex
	{
		JCSVchanName,
		JCSVstartTime,
		JCSVendTime,
		JCSVtimeOffset,
		JCSVtitle,
		JCSVshortDescription,
		JCSVlongDescription,
		JCSVcategory,
		JCSVserviceNumber,
		JCSVserviceName,
		JCSVserviceId,
		JAGCSVend_of_fields
	};

	enum fieldIndex
	{
		lineLength,
		startTime,
		endTime,
		channelNum,
		title,
		subtitle,
		repeat,
		newprog,
		year,
		filmpremiere,
		lastchance,
		description,
		genre,
		runningTime,
		aspect,
		blackandwhite,
		subtitles,
		episodenum,
		episodesys,
		ratingsys,
		rating,
		starrating,
		end_of_fields
	};


	TimeSlot m_TimeSlot;
	short int m_wChannelNum;
	string m_sTitle;
	string m_sSubtitle;
	bool m_bRepeat;
	// bool m_bNewProg
	short int m_wYear;
	bool m_bFilmPremiere;
	// bool m_bLastChance
	string m_sDescription;
	string m_sGenre;
	word m_wRunningTime;
	bool m_bWideScreen;
	bool m_bBlackAndWhite;
	bool m_bNewProgram;
	subtitleType m_Subtitles; //?
	string m_sEpisodeNum;
	// ?? episode sys
	// ?? rating sys
	string m_sAgeRating;
	signed char m_iStars;

	EPGevent* m_pContinuesAs;
	bool m_bContinuation;

	static unsigned short GetMaxDescriptionLength();

	static dword sm_dwFlags;
};

#endif