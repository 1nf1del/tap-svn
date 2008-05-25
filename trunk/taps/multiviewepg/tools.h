//
//	  MultiViewEPG jim16 Tue 1 Apr 12:12:00
//
//	  Copyright (C) 2008 Jim16
//
//	  This is free software; you can redistribute it and/or modify
//	  it under the terms of the GNU General Public License as published by
//	  the Free Software Foundation; either version 2 of the License, or
//	  (at your option) any later version.
//
//	  The software is distributed in the hope that it will be useful,
//	  but WITHOUT ANY WARRANTY; without even the implied warranty of
//	  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//	  GNU General Public License for more details.
//
//	  You should have received a copy of the GNU General Public License
//	  along with this software; if not, write to the Free Software
//	  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

#ifndef __TOOLSSTUFF__
  #define __TOOLSSTUFF__

#define __TOOLSSTUFF_VERSION__ "2008-01-15"

static TYPE_RecInfo ri_t1;
static TYPE_RecInfo ri_t2;
static TYPE_PlayInfo pi;
static dword mainstate, substate;

void WrapPutStr(word, char *, int, int, int, int, int, int, byte, int);
char* Channel_Name( int, int );
TYPE_RecInfo Recording_GetRecInfo( int );
bool State_Normal( void );
bool Update_RecordingInfo( int );
int GetSvcNum( int );
char* Parental_Value( byte );
char* Endtime_HHMM( dword, word );
char* Time_HHMM( dword );
char* Time_DOWDDMM( dword );
char* Time_DOWDDMMSpec( dword );
char* Time_DDMMSpec( dword, bool );
word GetSvcId( int svcType, int svcNum );
bool Update_Pi( void );
char* NumberToString( int );
bool GetSetting( char *, char *, char *, char * );
int	GetInteger( char * );
char* RKEY_Name( dword );
char* Time_DOWDD( dword );
char* RKEY_NameShort( dword );
void StripFileName( char * );
bool GetEpisodeName( char *, int );
int GetSvcType( int );
char * MinsToHHMM( int );
char * MinsToHHMM2( int );
int	atoi( const char* );
int GetSvcLCN( int svcId );
int Time_MINS( dword mjd );
int Endtime_MINS( dword mjd, word duration );
void WrapPutStrReset( void );
char* MinsToString( int number );
char* SecsToString( int number );
char* Time_MNameSpec( dword mjd );

#endif
