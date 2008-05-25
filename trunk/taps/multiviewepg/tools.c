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

#include "tools.h"
#include "fontman.h"
#include "messagebox.h"

/******************************************************************************
Word Wrap string output.

str: The string to be printed.
x, y, w: print area starts at x pixels in, y pixels down and is w pixels wide.
fgCol, bgCol: foreground and background colours.
maxRows: defines the hight of the print area in rows of characters.
fntSize: one of FNT_Size_1419, FNT_Size_1622 and FNT_Size_1926.
lineSep: The extra pixels between rows (can be negative with bgCol = COLOR_None
for overprinting).

Control characters (cariage return, new line etc.) are not supported.
******************************************************************************/
int     WrapPutStr_StartPos = 0;  	//KH Starting position to print string from
int     LastWrapPutStr_Y;			//KH Tracks the last coord from the WrapPutStr routine.
int 	LastWrapPutStr_P;  			//KH Tracks the last string position from the WrapPutStr routine.
int 	LastWrapPutStr_Start;       //KH Tracks the start position of the last line from the WrapPutStr routine.

void WrapPutStrReset( void ){
	WrapPutStr_StartPos = 0;  	//KH Starting position to print string from
	LastWrapPutStr_Y = 0;			//KH Tracks the last coord from the WrapPutStr routine.
	LastWrapPutStr_P = 0;  			//KH Tracks the last string position from the WrapPutStr routine.
	LastWrapPutStr_Start = 0;       //KH Tracks the start position of the last line from the WrapPutStr routine.
}

void WrapPutStr(word windowRgn, char *str, int x, int y, int w, int fgCol, int bgCol, int maxRows, byte fntSize, int lineSep)
{
	int row=0,c=0,p=0;
	int done = 0;//, try = 5;
	int fontH;
	int avgFontW = 0;
	int len;
	int newStrW;
	char newStr[512]; //100 should be more than enough for a line [!!!????]

	switch (fntSize)
	{
		case FNT_Size_1419 : fontH = 19; avgFontW = 6; break;
		case FNT_Size_1622 : fontH = 22; avgFontW = 8; break;
		case FNT_Size_1926 : fontH = 26; avgFontW = 10; break;
		default : fontH = 19; break;
	}

	len = strlen(str);
	
	p = WrapPutStr_StartPos;  //KH Set the starting position to print string from.
	if (len == 0)  
    {
          LastWrapPutStr_P = p;      //KH Keep track of the last string position.
          return;
    }      

	for (row=0; row<maxRows;row++)   
	{
    	LastWrapPutStr_Start = p;  //KH Keep track of the position of the first character on each line.
		c = w/avgFontW;	// guess approx chars in line
		if(p + c > len) c = len - p;
		if(c > 99) c = 99;

		strncpy(newStr,str+p, c); // copy approx chars per line to new string
		newStr[c]= '\0'; //KH clean up the end of the new line.

		while(!done)
		{
			newStrW = TAP_Osd_GetW(newStr,0,fntSize); // see if it fits
			if (newStrW > w && c > 0)  // Too big
			{		 //remove some chars
				if(c > 4) c = c - 4;
				else c--;

				newStr[c]= '\0';
			}
			else
			{
				done = 1; // string short enough
			}
			
		}

		done = 0;

		while (TAP_Osd_GetW(newStr,0,fntSize)<w-avgFontW)    //while the width of the text is less than a lines length
		{
			if (p + c + 1 > len)   //if the counter is larger than the string length of the orig string, we have really finished here 
			{
				done=1;					
				break;
			}
			strncat(newStr,str+p+c,1); //copy 1 char from str at a time.
			c++;                 //get next char
		}


		if (!done ) //KH find wrap position unless finished
		{
			c--; //reduce c by 1 as it is one too much from last while statement.
			while (!isspace(newStr[c])&&!(newStr[c]=='-')&&(c>0))
		 	{   //look for a space or hyphen (or underflow).
				c--;
			}
			c++; //add 1 to keep hyphen on right line
		}
		newStr[c]='\0';   //terminate str (early at wrap point if not done).
		LastWrapPutStr_Y = y+((fontH+lineSep)*row); //KH Keep track of the Y coordinate of where we're printing.
		switch( fntSize ){	// Fix by jim. Basically the putstraf ignores the size param so using putstringafSIZE to fix
			case FNT_Size_1419:
				Font_Osd_PutString1419(windowRgn, x, LastWrapPutStr_Y, x+w, newStr, fgCol, bgCol);  //put multiple lines down if required.
				break;
			case FNT_Size_1622:
				Font_Osd_PutString1622(windowRgn, x, LastWrapPutStr_Y, x+w, newStr, fgCol, bgCol);  //put multiple lines down if required.
				break;
			case FNT_Size_1926:
				Font_Osd_PutString1926(windowRgn, x, LastWrapPutStr_Y, x+w, newStr, fgCol, bgCol);  //put multiple lines down if required.
				break;
		}
		p = p + c;   //store current position
		LastWrapPutStr_P = p; //KH Keep track of the last string position.
		newStr[0] = '\0';  //reset string.
    	if (done) //KH if we've printed all the text, break out of loop.
        {
            LastWrapPutStr_P = 0;  //KH Reset to start of string.
            break;
        }
	}    
	LastWrapPutStr_Y = LastWrapPutStr_Y + fontH + lineSep;  //KH Store the last Y position.
	WrapPutStr_StartPos = 0;  //KH Reset starting position to print string from
}

char* Channel_Name( int svcType, int svcNum ){	
 	TYPE_TapChInfo chInfo;
	static char channelName[128];
	memset( channelName, 0, sizeof channelName );
	TAP_Channel_GetInfo( svcType, svcNum, &chInfo );
	sprintf( channelName, "%s\0", chInfo.chName );
	return channelName;
}

TYPE_RecInfo Recording_GetRecInfo( int tuner ){
	if( tuner == 0 ){
		return ri_t1;
	} else {
		return ri_t2;
	}
}

bool State_Normal( void ){
	TAP_GetState( &mainstate, &substate );
	return ( mainstate == STATE_Normal && substate == SUBSTATE_Normal );
}

// No assumption is made that any handling of this buffer overflow
bool Update_RecordingInfo( int tuner ){
	TYPE_RecInfo ri_new;
	char overrun[128];
	int  overrunCheck;
	*overrun = 0;
	overrunCheck = 1;
	if(TAP_Hdd_GetRecInfo(tuner, &ri_new)){
		if( overrunCheck == 1 ){
			if( tuner == 0 ){
				ri_t1 = ri_new;
			} else {
				ri_t2 = ri_new;			
			}
			return TRUE;
		} else {
			*overrun = 0;
			overrunCheck = 1;
			if(TAP_Hdd_GetRecInfo(tuner, &ri_new)){
				if( overrunCheck == 1 ){
					if( tuner == 0 ){
						ri_t1 = ri_new;
					} else {
						ri_t2 = ri_new;			
					}
					return TRUE;
				} 
			}
		}
	}
	return FALSE;
}

// Roundabout way but the only way possible. Need to use the svcId and find the matching svcNum by scanning all channels
int GetSvcType( int svcId ){
	int tvNum, radNum;
	int i;
	int returnval = -1;
	TYPE_TapChInfo channel;
	TAP_Channel_GetTotalNum( &tvNum, &radNum );
	if( tvNum < 1 ) return -1;	// Sanity check
	for( i = 0; i < tvNum; i++ ){
		TAP_Channel_GetInfo( SVC_TYPE_Tv, i, &channel );
		if( channel.svcId == svcId ){
			returnval = SVC_TYPE_Tv;
			i = tvNum;
		}
	}
	if( returnval > -1 ) return returnval;
	for( i = 0; i < radNum; i++ ){
		TAP_Channel_GetInfo( SVC_TYPE_Radio, i, &channel );
		if( channel.svcId == svcId ){
			returnval = SVC_TYPE_Radio;
			i = radNum;
		}
	}
	return returnval;
}

// Roundabout way but the only way possible. Need to use the svcId and find the matching svcNum by scanning all channels
int GetSvcLCN( int svcId ){
	int tvNum, radNum;
	int i;
	int returnval = -1;
	TYPE_TapChInfo channel;
	TAP_Channel_GetTotalNum( &tvNum, &radNum );
	if( tvNum < 1 ) return -1;	// Sanity check
	for( i = 0; i < tvNum; i++ ){
		TAP_Channel_GetInfo( SVC_TYPE_Tv, i, &channel );
		if( channel.svcId == svcId ){
			returnval = channel.logicalChNum;
			i = tvNum;
		}
	}
	if( returnval > -1 ) return returnval;
	for( i = 0; i < radNum; i++ ){
		TAP_Channel_GetInfo( SVC_TYPE_Radio, i, &channel );
		if( channel.svcId == svcId ){
			returnval = channel.logicalChNum;
			i = radNum;
		}
	}
	return returnval;
}

// Roundabout way but the only way possible. Need to use the svcId and find the matching svcNum by scanning all channels
int GetSvcNum( int svcId ){
	int tvNum, radNum;
	int i;
	int returnval = -1;
	TYPE_TapChInfo channel;
	TAP_Channel_GetTotalNum( &tvNum, &radNum );
	if( tvNum < 1 ) return -1;	// Sanity check
	for( i = 0; i < tvNum; i++ ){
		TAP_Channel_GetInfo( SVC_TYPE_Tv, i, &channel );
		if( channel.svcId == svcId ){
			returnval = i;
			i = tvNum;
		}
	}
	if( returnval > -1 ) return returnval;
	// Allow for the radio
	for( i = 0; i < radNum; i++ ){
		TAP_Channel_GetInfo( SVC_TYPE_Radio, i, &channel );
		if( channel.svcId == svcId ){
			returnval = i;
			i = radNum;
		}
	}
	return returnval;
}

char* Parental_Value( byte value ){
	static char rating[10];
	switch( value ){
		case 7: sprintf(rating, "[C]\0" ); break;
		case 9:
		case 10:
			sprintf(rating, "[G]\0" ); break;
		case 11: 
		case 12:
			sprintf(rating, "[PG]\0" ); break;
		case 13: sprintf(rating, "[M]\0" ); break;
		case 15: sprintf(rating, "[MA]\0" ); break;
		case 18: sprintf(rating, "[R]\0" ); break;
		default: sprintf(rating, "\0" ); break;
	}
	return rating;
}

char* Endtime_HHMM( dword mjd, word duration ){
	static char time[10];
	int min;
	int hour;
	hour = (mjd&0xff00)>>8;				// extract the time
	min = (mjd&0xff);
	hour += (duration-(duration%60))/60;
	min += (duration%60);
	// Check for increase in the hour
	if( min >= 60 ){
		hour++;
		min -= 60;
	}
	// Check hour sanity
	if( hour >= 24 ) hour -= 24;
	TAP_SPrint(time, "%02d:%02d", hour, min);
	return time;
}

char* Time_HHMM2( dword mjd ){
	static char time[10];
	int min;
	int hour;
	hour = (mjd&0xff00)>>8;				// extract the time
	min = (mjd&0xff);
	TAP_SPrint(time, "%02d:%02d", hour, min);
	return time;
}

char* Time_HHMM( dword mjd ){
	static char time[10];
	int min;
	int hour;
	hour = (mjd&0xff00)>>8;				// extract the time
	min = (mjd&0xff);
	TAP_SPrint(time, "%02d:%02d", hour, min);
	return time;
}

int Endtime_MINS( dword mjd, word duration ){
	int min;
	int hour;
	hour = (mjd&0xff00)>>8;				// extract the time
	min = (mjd&0xff);
	hour += (duration-(duration%60))/60;
	min += (duration%60);
	// Check for increase in the hour
	if( min >= 60 ){
		hour++;
		min -= 60;
	}
	// Check hour sanity
	if( hour >= 24 ) hour -= 24;
	return (hour*60)+min;
}

int Time_MINS( dword mjd ){
	int min;
	int hour;
	hour = (mjd&0xff00)>>8;				// extract the time
	min = (mjd&0xff);
	return (hour*60)+min;
}

char* Time_DOWDDMM( dword mjd ){
	word year;
	byte month, day, weekDay;
	static char date[10];
	if( TAP_ExtractMjd( mjd, &year, &month, &day, &weekDay ) == 1 ){
		switch( weekDay ){
			case 6: sprintf(date, "Sun %02d/%02d\0", day, month ); break;
			case 0: sprintf(date, "Mon %02d/%02d\0", day, month ); break;
			case 1: sprintf(date, "Tue %02d/%02d\0", day, month ); break;
			case 2: sprintf(date, "Wed %02d/%02d\0", day, month); break;
			case 3: sprintf(date, "Thu %02d/%02d\0", day, month ); break;
			case 4: sprintf(date, "Fri %02d/%02d\0", day, month ); break;
			case 5: sprintf(date, "Sat %02d/%02d\0", day, month ); break;
		}
	} else {
		// Malformeed mjd format
		sprintf(date, "?\0" );
	}
	return date;
}

char* Time_DOWDDSpec( dword mjd ){
	word year;
	byte month, day, weekDay;
	static char date[10];
	if( TAP_ExtractMjd( mjd>>16, &year, &month, &day, &weekDay ) == 1 ){
		switch( weekDay ){
			case 6: sprintf(date, "Sun %2d\0", day ); break;
			case 0: sprintf(date, "Mon %2d\0", day ); break;
			case 1: sprintf(date, "Tue %2d\0", day ); break;
			case 2: sprintf(date, "Wed %2d\0", day ); break;
			case 3: sprintf(date, "Thu %2d\0", day ); break;
			case 4: sprintf(date, "Fri %2d\0", day ); break;
			case 5: sprintf(date, "Sat %2d\0", day ); break;
		}
	} else {
		// Malformeed mjd format
		sprintf(date, "?\0" );
	}
	return date;
}

char* Time_MNameSpec( dword mjd ){
	word year;
	byte month, day, weekDay;
	static char date[10];
	if( TAP_ExtractMjd( mjd>>16, &year, &month, &day, &weekDay ) == 1 ){
		switch( month ){
			case 1: sprintf(date, "Jan\0" ); break;
			case 2: sprintf(date, "Feb\0" ); break;
			case 3: sprintf(date, "Mar\0" ); break;
			case 4: sprintf(date, "Apr\0" ); break;
			case 5: sprintf(date, "May\0" ); break;
			case 6: sprintf(date, "Jun\0" ); break;
			case 7: sprintf(date, "Jul\0" ); break;
			case 8: sprintf(date, "Aug\0" ); break;
			case 9: sprintf(date, "Sep\0" ); break;
			case 10: sprintf(date, "Oct\0" ); break;
			case 11: sprintf(date, "Nov\0" ); break;
			case 12: sprintf(date, "Dec\0" ); break;
		}
	} else {
		// Malformeed mjd format
		sprintf(date, "?\0" );
	}
	return date;
}

char* Time_DOWDDMMSpec( dword mjd ){
	word year;
	byte month, day, weekDay;
	static char date[10];
	if( TAP_ExtractMjd( mjd>>16, &year, &month, &day, &weekDay ) == 1 ){
		switch( weekDay ){
			case 6: sprintf(date, "Sun %02d/%02d\0", day, month ); break;
			case 0: sprintf(date, "Mon %02d/%02d\0", day, month ); break;
			case 1: sprintf(date, "Tue %02d/%02d\0", day, month ); break;
			case 2: sprintf(date, "Wed %02d/%02d\0", day, month); break;
			case 3: sprintf(date, "Thu %02d/%02d\0", day, month ); break;
			case 4: sprintf(date, "Fri %02d/%02d\0", day, month ); break;
			case 5: sprintf(date, "Sat %02d/%02d\0", day, month ); break;
		}
	} else {
		// Malformeed mjd format
		sprintf(date, "?\0" );
	}
	return date;
}

char* Time_DOWDD( dword mjd ){
	word year;
	byte month, day, weekDay;
	static char date[10];
	if( TAP_ExtractMjd( mjd, &year, &month, &day, &weekDay ) == 1 ){
		switch( weekDay ){
			case 6: sprintf(date, "Sun %d\0", day ); break;
			case 0: sprintf(date, "Mon %d\0", day ); break;
			case 1: sprintf(date, "Tue %d\0", day ); break;
			case 2: sprintf(date, "Wed %d\0", day ); break;
			case 3: sprintf(date, "Thu %d\0", day ); break;
			case 4: sprintf(date, "Fri %d\0", day ); break;
			case 5: sprintf(date, "Sat %d\0", day ); break;
		}
	} else {
		// Malformeed mjd format
		sprintf(date, "?\0" );
	}
	return date;
}

char* Time_DDMMSpec( dword mjd, bool dow ){
	word year;
	byte month, day, weekDay;
	static char date[10];
	if( TAP_ExtractMjd( mjd>>16, &year, &month, &day, &weekDay ) == 1 ){
		if( dow ){
			switch( weekDay ){
				case 6: sprintf(date, "Sun\0" ); break;
				case 0: sprintf(date, "Mon\0" ); break;
				case 1: sprintf(date, "Tue\0" ); break;
				case 2: sprintf(date, "Wed\0" ); break;
				case 3: sprintf(date, "Thu\0" ); break;
				case 4: sprintf(date, "Fri\0" ); break;
				case 5: sprintf(date, "Sat\0" ); break;
			}
		} else {
			sprintf(date, "%02d/%02d\0", day, month );
		}
	} else {
		// Malformeed mjd format
		sprintf(date, "?\0" );
	}
	return date;
}

word GetSvcId( int svcType, int svcNum ){
	TYPE_TapChInfo channel;
	TAP_Channel_GetInfo( svcType, svcNum, &channel );
	return channel.svcId;
}

bool Update_Pi( void ){
	TYPE_PlayInfo pi_new;
	char overrun[128];
	int  overrunCheck;
	*overrun = 0;
	overrunCheck = 1;
	if(TAP_Hdd_GetPlayInfo(&pi_new)){
		if( overrunCheck == 1 ){
			pi = pi_new;
			return TRUE;
		} else {
			// Failed so retry once again
			*overrun = 0;
			overrunCheck = 1;
			if(TAP_Hdd_GetPlayInfo(&pi_new)){
				pi = pi_new;
				return TRUE;
			}
		}
	}
	return FALSE;
}

char* NumberToString( int number ){
	static char text[20];
	TAP_SPrint( text, "%d\0", number );
	return text;
}

char* SecsToString( int number ){
	static char text[20];
	TAP_SPrint( text, "%d Secs\0", number );
	return text;
}

char* MinsToString( int number ){
	static char text[20];
	TAP_SPrint( text, "%d Mins\0", number );
	return text;
}

int	GetInteger( char* sString ){
	int iValue, iNdx, iLen;
	bool	bNegate;
	bNegate = FALSE;
	iLen = strlen( sString );
	iValue = 0;
	for ( iNdx = 0; iNdx < iLen; iNdx += 1 )	{
		if ( sString[iNdx] == 45 ){
			bNegate = TRUE;
		} else {
			if ( sString[iNdx] < 48 || sString[iNdx] > 57 )	return 0;
			iValue = ( iValue * 10 ) + ( sString[iNdx] - 48 );
		}
	}
	if ( bNegate == TRUE ) iValue = iValue * -1;
	return iValue;
}

bool GetSetting( char* sIniFile, char* sSrcSection, char* sSrcLabel, char* sResult ){
	int	iNdx, iRecSize, ibNdx;
	char	sLabel[200], sParam[200], sBuff[200];
	bool	bEqual, bSectFound;

	memset( sLabel, 0, sizeof sLabel );
	memset( sParam, 0, sizeof sParam );
	memset( sResult, 0, sizeof sResult );
	memset( sBuff, 0, sizeof sBuff );

	iRecSize = strlen( sIniFile );
	bEqual = FALSE;
	bSectFound = FALSE;
	ibNdx = 0;

	for ( iNdx = 0; iNdx < iRecSize; iNdx += 1 )				
	{

		if (  sIniFile[iNdx]==0x0d || iNdx ==(iRecSize - 1) || sIniFile[iNdx] == 0x0a )	 //0x0d=CR
		{
			if ( bSectFound == FALSE )
			{
				if ( strcmp( sLabel, sSrcSection ) == 0 )
					bSectFound = TRUE;
			}
			else
			{
				if ( sLabel[0] == '[')
				{
					bSectFound = FALSE;
				}
				else
				{
					if ( strcmp( sLabel, sSrcLabel ) == 0 ) //i.e. they are equal
					{
					//	logf(4,"slabel:%s, srclabel:%s, sparam:%s",sLabel, sSrcLabel,sParam);
						strcpy( sResult, sParam );
						return TRUE;
					}
				}
			}

			memset (sLabel, 0, sizeof sLabel );
			memset (sParam, 0, sizeof sParam );
			bEqual = FALSE;
			ibNdx = 0;
		}
		else
		{
			if ( sIniFile[iNdx] != 0x0a ) //LF
			{
				if ( sIniFile[iNdx] == '=' )
				{
					ibNdx = 0;
					bEqual = TRUE;
				}
				else
				{
					if ( bEqual )
					{
						sParam[ibNdx] = sIniFile[iNdx];
					}
					else
					{
						sLabel[ibNdx] = sIniFile[iNdx];
					}
					if ( ibNdx < 200 )
						ibNdx += 1;		// Restrict to 200 characters
				}
			}
		}
	}	
	return FALSE;
}

char* RKEY_Name( dword param1 ){
	switch( param1 ){
		case RKEY_Power:
			return "Power";
			break;
		case RKEY_Mute:
			return "Mute";
			break;
		case RKEY_Uhf:
			return "Uhf";
			break;
		case RKEY_TvSat:
			return "TV/Stb";
			break;
		case RKEY_Sleep:
			return "Sleep";
			break;
		case RKEY_1:
			return "1";
			break;
		case RKEY_2:
			return "2";
			break;
		case RKEY_3:
			return "3";
			break;
		case RKEY_4:
			return "4";
			break;
		case RKEY_5:
			return "5";
			break;
		case RKEY_6:
			return "6";
			break;
		case RKEY_7:
			return "7";
			break;
		case RKEY_8:
			return "8";
			break;
		case RKEY_9:
			return "9";
			break;
		case RKEY_0:
			return "0";
			break;
		case RKEY_Recall:
			return "Recall";
			break;
		case RKEY_Info:
			return "Info";
			break;
		case RKEY_Menu:
			return "Menu";
			break;
		case RKEY_Guide:
			return "Guide";
			break;
		case RKEY_TvRadio:
			return "Tv/Radio";
			break;
		case RKEY_AudioTrk:
			return "AudioTrk";
			break;
		case RKEY_Subt:
			return "Subtitle";
			break;
		case RKEY_Teletext:
			return "Teletext";
			break;
		case RKEY_Exit:
			return "Exit";
			break;
		case RKEY_Fav:
			return "Fav";
			break;
		case RKEY_VolUp:
			return "VolUp";
			break;
		case RKEY_VolDown:
			return "VolDn";
			break;
		case RKEY_ChUp:
			return "ChUp";
			break;
		case RKEY_ChDown:
			return "ChDn";
			break;
		case RKEY_Ok:
			return "Ok";
			break;
		case RKEY_Rewind:
			return "RW";
			break;
		case RKEY_Forward:
			return "FF";
			break;
		case RKEY_Slow:
			return "SlowPlay";
			break;
		case RKEY_Stop:
			return "Stop";
			break;
		case RKEY_Record:
			return "Record";
			break;
		case RKEY_Pause:
			return "Pause";
			break;
		case RKEY_Prev:
			return "Prev";
			break;
		case RKEY_Next:
			return "Next";
			break;
		case RKEY_PlayList:
			return "FileList";
			break;
		case RKEY_NewF1:
			return "Red";
			break;
		case RKEY_Bookmark:
			return "Green";
			break;
		case RKEY_Goto:
			return "Yellow";
			break;
		case RKEY_Check:
			return "Blue";
			break;
		case RKEY_Ab:
			return "White";
			break;
		case RKEY_Sat:
			return "Grey";
			break;
		default:
			return "None";
			break;
	}
}

char* RKEY_NameShort( dword param1 ){
	switch( param1 ){
		case RKEY_Power:
			return "Pwr";
			break;
		case RKEY_Mute:
			return "Mut";
			break;
		case RKEY_Uhf:
			return "Uhf";
			break;
		case RKEY_TvSat:
			return "Stb";
			break;
		case RKEY_Sleep:
			return "Slp";
			break;
		case RKEY_1:
			return "1";
			break;
		case RKEY_2:
			return "2";
			break;
		case RKEY_3:
			return "3";
			break;
		case RKEY_4:
			return "4";
			break;
		case RKEY_5:
			return "5";
			break;
		case RKEY_6:
			return "6";
			break;
		case RKEY_7:
			return "7";
			break;
		case RKEY_8:
			return "8";
			break;
		case RKEY_9:
			return "9";
			break;
		case RKEY_0:
			return "0";
			break;
		case RKEY_Recall:
			return "Rcl";
			break;
		case RKEY_Info:
			return "Info";
			break;
		case RKEY_Menu:
			return "Menu";
			break;
		case RKEY_Guide:
			return "Guide";
			break;
		case RKEY_TvRadio:
			return "Tv/Rad";
			break;
		case RKEY_AudioTrk:
			return "AudTrk";
			break;
		case RKEY_Subt:
			return "Subt";
			break;
		case RKEY_Teletext:
			return "Tltxt";
			break;
		case RKEY_Exit:
			return "Exit";
			break;
		case RKEY_Fav:
			return "Fav";
			break;
		case RKEY_VolUp:
			return "VolUp";
			break;
		case RKEY_VolDown:
			return "VolDn";
			break;
		case RKEY_ChUp:
			return "ChUp";
			break;
		case RKEY_ChDown:
			return "ChDn";
			break;
		case RKEY_Ok:
			return "Ok";
			break;
		case RKEY_Rewind:
			return "RW";
			break;
		case RKEY_Forward:
			return "FF";
			break;
		case RKEY_Slow:
			return "Slow";
			break;
		case RKEY_Stop:
			return "Stop";
			break;
		case RKEY_Record:
			return "Rec";
			break;
		case RKEY_Pause:
			return "Pause";
			break;
		case RKEY_Prev:
			return "Prev";
			break;
		case RKEY_Next:
			return "Next";
			break;
		case RKEY_PlayList:
			return "File";
			break;
		case RKEY_NewF1:
			return "Red";
			break;
		case RKEY_Bookmark:
			return "Bkm";
			break;
		case RKEY_Goto:
			return "Goto";
			break;
		case RKEY_Check:
			return "Blue";
			break;
		case RKEY_Ab:
			return "Ab";
			break;
		case RKEY_Sat:
			return "Sat";
			break;
		default:
			return "None";
			break;
	}
}

// removes the ".rec" from the end of a file name
void StripFileName( char *name )
{
	int 	i, k, len;
	bool 	noREC;
	char	str[10];

	noREC = FALSE;
	len = strlen( name );
	memset( str, '\0', sizeof(str) );									// will catch any duff processing

	if ( len < 4 ) return;												// can't process if string too short
	else
	{
	    k=0;
		
		for ( i = len-4 ; i < len ; i++ )
		{
		    str[k] = name[i];
			k++;
		}

		if ( strcmp( str, ".rec" ) != 0 ) return;						// can't process if the string doesn't end in ".rec"
	}

	for ( i = len-4 ; i < len ; i++ )									// overwrite the ".rec"
	{
	    name[i]='\0';
	}
}

// Function to extract the episode title e.g. The Simpsons - Homer vs the Eighteenth Amendment
bool GetEpisodeName( char * episode, int seperator ){
	char str[128];
	char *result;
	int len;
	memset( str, '\0', sizeof(str) );	// Clear our temp array
	len = strlen(episode);				// determine the length of our episode name
	if( len < 1 || len > 128 ){			// invalid length so return and blank the details
		memset( episode, '\0', sizeof(episode) );
		return FALSE;	// Sanity check
	}
	result = strchr(episode, '[');
	if( result == NULL ) return FALSE;
	memset( str, '\0', sizeof(str) );	// Clear our temp array
	strncpy(str,episode,(result - episode));
	// Remove the spaces
	RTrim(str);
	memset( episode, '\0', sizeof(episode) );
	strcpy(episode,str);
	return TRUE;
}

//------------------------------ atoi --------------------------------------
//
// Copied from EPG Uploader source so all credit to tonymy01 for this stuff
int	atoi( const char* sString )
{
	bool bNegate=FALSE;
	int iValue=0; 
	int iNdx;
	int iLen=strlen( sString );;

	for ( iNdx = 0; iNdx < iLen; iNdx += 1 )				
	{
		if ( sString[iNdx] == '-' || sString[iNdx] == '+')
		{
			bNegate = (sString[iNdx] == '-');
		}
		else
		{
			if ( sString[iNdx] < '0' || sString[iNdx] > '9' )
				return 0;
			iValue = ( iValue * 10 ) + ( sString[iNdx] - '0' );
		}
	}

	if ( bNegate == TRUE )
		iValue = -iValue;
	return iValue;
}

char * MinsToHHMM2( int mins ){
	static char str[10];
	int min;
	int hour;
	if( mins < 0 ){
		sprintf(str, "Any\0");
	} else {
		min = mins % 60;
		hour = (mins-min)/60;
		sprintf(str, "%02d:%02d\0", hour, min);
	}
	return str;
}

char * MinsToHHMM( int mins ){
	static char str[10];
	int min;
	int hour;
	if( mins < 0 ){
		sprintf(str, "Any\0");
	} else {
		min = mins % 60;
		hour = (mins-min)/60;
		sprintf(str, "%02d:%02d\0", hour, min);
	}
	return str;
}

