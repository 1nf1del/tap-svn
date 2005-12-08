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
#include ".\timers.h"
#include "MainFrm.h"
#include "MJDUtil.h"
#include "Connfiguration.h"
#include "ChannelList.h"


template<typename T> void Zeromemory(T& thing)
{
	ZeroMemory(&thing, sizeof(T));
}

Timers::Timers( CChannelList* channelList ) : m_ChannelList( channelList )
{
	// Load the timers from a tab delimited text file
	CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
	CConfiguration* pConfig = pFrame->GetConfig();
	CStdioFile file;
	if ( file.Open( pConfig->GetTimerFile(), CFile::modeRead ) )
	{
		if ( file.ReadString( m_line ) )
		{
			while ( file.ReadString( m_line ) )
			{
				ProcessTimerLine();
			}
		}
	}
}

Timers::~Timers(void)
{
}

int Timers::GetTotalNum(void )
{
	return m_vecTimers.size();
}

bool Timers::GetInfo(unsigned int entryNum, TYPE_TimerInfo *info )
{
	if (entryNum<0)
		return false;

	if (entryNum >= m_vecTimers.size())
		return false;

	memcpy(info, &m_vecTimers[entryNum], sizeof(TYPE_TimerInfo));
	return true;
}

int Timers::Add(TYPE_TimerInfo *info )
{
	m_vecTimers.push_back(*info);
	return 0; // ought to check for overlap
}

int Timers::Modify(unsigned int entryNum, TYPE_TimerInfo *info )
{
	if (entryNum<0)
		return 1;

	if (entryNum >= m_vecTimers.size())
		return 1;

	m_vecTimers[entryNum] = *info;
	return 0; // ought to check for overlap/bad tuner
}

bool Timers::Delete(unsigned int entryNum )
{
	if (entryNum<0)
		return false;

	if (entryNum >= m_vecTimers.size())
		return false;

	m_vecTimers.erase(m_vecTimers.begin() + entryNum);
	return true;
}


// Timer loading functions lifted from UKTimers
CString Timers::ReadTimerField()
{
	int index = m_line.Find("\t", 0);
	if ( index == -1 )
		return "";

	CString result = m_line.Left( index );
	m_line = m_line.Mid( index+1 );
	return result;
}


word Timers::ExtractServiceID( char *LCN_str, char *serviceName, byte serviceType )
{
	TYPE_TapChInfo	chInfo;
	int countTvSvc, countRadioSvc, count, i, result;
	word extracted_LCN, last_match;

	extracted_LCN = atoi( LCN_str );									// convert the string to an integer (atoi is a built in function)
    last_match = 0;
	
	m_ChannelList->GetTotalNum( &countTvSvc, &countRadioSvc );				// how many channels are installed
	if ( serviceType == 0 ) count = countTvSvc; else count = countRadioSvc;

	for ( i=0 ; i<count ; i++ )											// for all services check the LCN against the value read from file
	{
		result = m_ChannelList->GetInfo( serviceType, i, &chInfo );		// get the LCN info for this service

	    if ( chInfo.logicalChNum == extracted_LCN )						// record any matches
	    {
		    last_match = i;												// (TOPPY may have more than 1 serivce with same LCN)
			if ( strcmp( serviceName, chInfo.chName) == 0) break;		// no need to look further if names match
		}
	}

	return last_match;
}

dword Timers::ReadTimerStart()
{
    char date_str[256], time_str[256];
	byte	month, day;
    word	year, mjd;
	int 	min, hour;
	dword	startime;
	
	strcpy( date_str, ReadTimerField() );								// Read the Date
	strcpy( time_str, ReadTimerField() );								// Read the Time

	day = ( ((date_str[0] - '0') * 10) + (date_str[1] - '0') );
	month = ( ((date_str[3] - '0') * 10) + (date_str[4] - '0') );
	year = ( ((date_str[6] - '0') * 1000) + ((date_str[7] - '0') * 100) + ((date_str[8] - '0') * 10) + (date_str[9] - '0') );

	hour = ( ((time_str[0] - '0') * 10) + (time_str[1] - '0') );
	min = ( ((time_str[3] - '0') * 10) + (time_str[4] - '0') );
	
	mjd = MJDUtil::MakeMJD( year, month, day );

	startime = (mjd<<16) | (hour<<8) | (min);							// convert the startime to Toppy format
	return startime;
}

word Timers::ReadTimerDuration()
{
    char 	str[256];
	int 	min, hour;
	word	duration;
	
	strcpy( str, ReadTimerField() );									// Read the Duration

	hour = ( ((str[0] - '0') * 10) + (str[1] - '0') );
	min = ( ((str[3] - '0') * 10) + (str[4] - '0') );
	
	duration = (hour * 60) + min;										// Convert the duration to minutes
	return duration;
}


bool Timers::ProcessTimerLine()
{
	char str[256], tmp[10], svcName[256];


	strcpy( str, ReadTimerField() );
	strcpy( tmp, str );
	if ( str[0] == '\0' ) return FALSE;

	TYPE_TimerInfo timerInfo;
	Zeromemory(timerInfo);

	strcpy( str, ReadTimerField() );
	if ( strcmp( str, "VCR" ) == 0 ) { timerInfo.isRec = 0; goto Tuner; }
	if ( strcmp( str, "Rec" ) == 0 ) { timerInfo.isRec = 1; goto Tuner; }
	timerInfo.isRec = 0;		  										// default = VCR

Tuner:
	strcpy( str, ReadTimerField() );
	if ( strcmp( str, "1" ) == 0 ) { timerInfo.tuner = 0; goto Type; }
	if ( strcmp( str, "2" ) == 0 ) { timerInfo.tuner = 1; goto Type; }
	if ( strcmp( str, "4" ) == 0 ) { timerInfo.tuner = 3; goto Type; }
	timerInfo.tuner = 3;		  										// default = tuner 4
	
Type:
	strcpy( str, ReadTimerField() );
	if ( strcmp( str, "TV" ) == 0 ) { timerInfo.svcType = 0; goto LCN; }
	if ( strcmp( str, "Radio" ) == 0 ) { timerInfo.svcType = 1; goto LCN; }
	timerInfo.svcType = 0;		  										// default = TV

LCN:
	strcpy( str, ReadTimerField() );									// LCN
	strcpy( svcName, ReadTimerField() );									// Service Name
	timerInfo.svcNum = ExtractServiceID( str, svcName, timerInfo.svcType );	// extract service number from above 2 fields

	strcpy( str, ReadTimerField() );
	if ( strcmp( str, "One time" ) == 0 ) { timerInfo.reservationType = RESERVE_TYPE_Onetime; goto StartTime; }
	if ( strcmp( str, "Everyday" ) == 0 ) { timerInfo.reservationType = RESERVE_TYPE_Everyday; goto StartTime; }
	if ( strcmp( str, "Every Weekend" ) == 0 ) { timerInfo.reservationType = RESERVE_TYPE_EveryWeekend; goto StartTime; }
	if ( strcmp( str, "Weekly" ) == 0 ) { timerInfo.reservationType = RESERVE_TYPE_Weekly; goto StartTime; }
	if ( strcmp( str, "Weekday" ) == 0 ) { timerInfo.reservationType = RESERVE_TYPE_WeekDay; goto StartTime; }
	timerInfo.reservationType = RESERVE_TYPE_Onetime;					// default = One time

StartTime:
	timerInfo.startTime = ReadTimerStart();								// Date & Time
	timerInfo.duration = ReadTimerDuration();							// Duration

	strcpy ( timerInfo.fileName, ReadTimerField() );					// File Name

	strcpy(str, ReadTimerField() );
	if ( strcmp( str, "0" ) == 0 ) { timerInfo.nameFix = 0; goto DayNo; }
	if ( strcmp( str, "1" ) == 0 ) { timerInfo.nameFix = 1; goto DayNo; }
	timerInfo.nameFix = 0;		  										// default = name not fixed

DayNo:
	ReadTimerField();													// Day No
	ReadTimerField();													// Day Name

	Add( &timerInfo );

	return TRUE;
}
