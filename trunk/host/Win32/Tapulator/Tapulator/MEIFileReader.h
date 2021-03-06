#pragma once
#include "EPGReader.h"
class CChannelList;

class MEIFileReader: public EPGImpl
{
public:
	MEIFileReader(CChannelList* pChannelList);
	~MEIFileReader(void);

	TYPE_TapEvent* GetEvent(int svcType, int svcNum, int *eventNum );
	virtual byte* GetExtInfo(TYPE_TapEvent *tapEvtInfo );

private:
	CChannelList* m_pChannelList;
	int BuildEvent(CString sDataLine, TYPE_TapEvent& result);
	void ReadAllData();

	int ToppyToLogical(int svcType, int svcNum);
	std::map<int, std::vector<TYPE_TapEvent> > m_AllEPGInfo;
	int m_iEventCount;
	std::vector<CString> m_vecExtInfo;

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

};

