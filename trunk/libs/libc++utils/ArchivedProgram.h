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
#ifndef cpputils_archivedprogram_h
#define cpputils_archivedprogram_h
#include "tapstring.h"
#include "tap.h"
#pragma pack (push)
#pragma pack (1)
struct RecHeader
{
	char magic[4];		//00   Id                : ARRAY 4 OF CHAR;     (* Note 1 *)  
	word version;		//04   Version           : CARD16;              (* Note 2 *)
	word reserved;		//06   resv              : CARD16;              (* Note 3 *)
	word duration;		//08   Duration          : CARD16;              (* Note 4 *)
	word service_num;	//0A   Service_Number    : CARD16;              (* Note 5 *)
	word service_type;	//0C   Service_Type      : CARD16;              (* Note 6 *)
	dword tuner_info;	//0E   Tuner_Info        : SET32;               (* Note 7 *)
						//(* Service information *)
	word service_id;	//12   Service_Id        : CARD16;
	word pmt_pid;		//14   PMT_PID           : CARD16;
	word pcr_pid;		//16   PCR_PID           : CARD16;
	word video_pid;		//18   VIDEO_PID         : CARD16;
	word audio_pid;		//1A   AUDIO_PID         : CARD16;              (* Note 8 *)
	char svc_name[24];	//1C   Service_Name      : ARRAY 24 OF CHAR;
	byte unk3[5];		//34   unk3              : ARRAY 5 OF CARD8;    (* Note 9 *)
						//(* Transmission information *)
	byte uhf_channel;	//39   UHFChannel        : CARD8;
	byte bandwidth;		//3A   Bandwidth         : CARD8;               (* Note 10 *)
	byte unk4;			//3B   unk4              : CARD8;               (* Note 11 *)
	dword frequency;	//3C   Frequency         : CARD32               (* Note 12 *)
	word transpt_stream;//40   TransportStream_ID: CARD16;
	word unk5;			//42   unk5              : CARD16;              (* Note 13 *)
	word orgnet_id;		//44   OrgNet_ID         : CARD16;              (* Note 14 *)
						//(* EPG information *)
	dword ukn6;			//46   unk6              : CARD32;              (* Note 15 *)
	byte duration_hour;	//4A   Duration_Hour     : CARD8;
	byte durantion_min;	//4B   Duration_Min      : CARD8;
	dword event_id;		//4C   Event_Id          : CARD32;
	word start_mjd;		//50   Start_Time_MJD    : CARD16;              (* Note 16 *)
	byte start_hour;	//52   Start_Time_Hour   : CARD8;
	byte start_min;		//53   Start_Time_Min    : CARD8;
	word end_mjd;		//54   End_Time_MJD      : CARD16;
	byte end_hour;		//56   End_Time_Hour     : CARD8;
	byte end_min;		//57   End_Time_Min      : CARD8;
	byte unk7;			//58   unk7              : CARD8;               (* Note 17 *)
	byte evt_name_len;	//59   Event_Name_Length : CARD8;
	byte parent_rating;	//5A   Parental_Rating   : CARD8;               (* Note 18 *)
	char evt_text[257];	//5B   Event_Text        : ARRAY 257 OF CHAR;   (* Note 19 *)
	byte unk8[18];		//15C   unk8              : ARRAY 18 OF CARD8;   (* Note 20 *)
						//(* Extended event information *)
	word extext_len;	//174   ExText_Length     : CARD16;
	dword extevt_id;	//176   ExEvent_Id        : CARD32;
	char extevt_text[1024];	//17A   ExEvent_Text      : ARRAY 1024 OF CHAR;  (* Note 21 *)
						//
	byte unk9[8];		//57A   unk9              : ARRAY 8 OF CARD8;    (* Note 22 *)
	dword bookmark[64];	//582   bookmark          : ARRAY 64 OF CARD32;  (* Note 23 *)
	dword resume;		//   resume            : CARD32;              (* Note 24 *)
	byte unk10[2048];	//   unk10             : ARRAY 2048 OF CARD8; (* Note 25 *)
		// END;
};
#pragma pack (pop)

class ArchivedProgram
{
public:
	ArchivedProgram(const string& fileName); 


};

#endif