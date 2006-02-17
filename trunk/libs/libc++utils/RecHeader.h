/*
	Copyright (C) 2006 Robin Glover

	This file is part of the TAPs for Topfield PVRs project.
		http://tap.berlios.de/

	This library is free software PACKED; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation PACKED; either
	version 2 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY PACKED; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with this library PACKED; if not, write to the Free Software
	Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/
#ifndef cpputils_recheader_h
#define cpputils_recheader_h

#ifdef WIN32
#define PACKED
#else
#define PACKED __attribute__((packed))
#endif

#pragma pack (push)
#pragma pack (1)
struct RecHeader
{
	char magic[4] PACKED;		//00   Id                : ARRAY 4 OF CHAR;     (* Note 1 *)  
	word version PACKED;		//04   Version           : CARD16;              (* Note 2 *)
	word reserved PACKED;		//06   resv              : CARD16;              (* Note 3 *)
	word duration PACKED;		//08   Duration          : CARD16;              (* Note 4 *)
	word service_num PACKED;	//0A   Service_Number    : CARD16;              (* Note 5 *)
	word service_type PACKED;	//0C   Service_Type      : CARD16;              (* Note 6 *)
	dword tuner_info PACKED;	//0E   Tuner_Info        : SET32;               (* Note 7 *)
						//(* Service information *)
	word service_id PACKED;	//12   Service_Id        : CARD16;
	word pmt_pid PACKED;		//14   PMT_PID           : CARD16;
	word pcr_pid PACKED;		//16   PCR_PID           : CARD16;
	word video_pid PACKED;		//18   VIDEO_PID         : CARD16;
	word audio_pid PACKED;		//1A   AUDIO_PID         : CARD16;              (* Note 8 *)
	char svc_name[24] PACKED;	//1C   Service_Name      : ARRAY 24 OF CHAR;
	byte unk3[5] PACKED;		//34   unk3              : ARRAY 5 OF CARD8;    (* Note 9 *)
						//(* Transmission information *)
	byte uhf_channel PACKED;	//39   UHFChannel        : CARD8;
	byte bandwidth PACKED;		//3A   Bandwidth         : CARD8;               (* Note 10 *)
	byte unk4 PACKED;			//3B   unk4              : CARD8;               (* Note 11 *)
	dword frequency PACKED;	//3C   Frequency         : CARD32               (* Note 12 *)
	word transpt_stream PACKED;//40   TransportStream_ID: CARD16;
	word unk5 PACKED;			//42   unk5              : CARD16;              (* Note 13 *)
	word orgnet_id PACKED;		//44   OrgNet_ID         : CARD16;              (* Note 14 *)
						//(* EPG information *)
	dword ukn6 PACKED;			//46   unk6              : CARD32;              (* Note 15 *)
	byte duration_hour PACKED;	//4A   Duration_Hour     : CARD8;
	byte durantion_min PACKED;	//4B   Duration_Min      : CARD8;
	dword event_id PACKED;		//4C   Event_Id          : CARD32;
	word start_mjd PACKED;		//50   Start_Time_MJD    : CARD16;              (* Note 16 *)
	byte start_hour PACKED;	//52   Start_Time_Hour   : CARD8;
	byte start_min PACKED;		//53   Start_Time_Min    : CARD8;
	word end_mjd PACKED;		//54   End_Time_MJD      : CARD16;
	byte end_hour PACKED;		//56   End_Time_Hour     : CARD8;
	byte end_min PACKED;		//57   End_Time_Min      : CARD8;
	byte unk7 PACKED;			//58   unk7              : CARD8;               (* Note 17 *)
	byte evt_name_len PACKED;	//59   Event_Name_Length : CARD8;
	byte parent_rating PACKED;	//5A   Parental_Rating   : CARD8;               (* Note 18 *)
	char evt_text[257] PACKED;	//5B   Event_Text        : ARRAY 257 OF CHAR;   (* Note 19 *)
	byte unk8[18] PACKED;		//15C   unk8              : ARRAY 18 OF CARD8;   (* Note 20 *)
						//(* Extended event information *)
	word extext_len PACKED;	//174   ExText_Length     : CARD16;
	dword extevt_id PACKED;	//176   ExEvent_Id        : CARD32;
	char extevt_text[1024] PACKED;	//17A   ExEvent_Text      : ARRAY 1024 OF CHAR;  (* Note 21 *)
						//
	byte unk9[8] PACKED;		//57A   unk9              : ARRAY 8 OF CARD8;    (* Note 22 *)
	dword bookmark[64] PACKED;	//582   bookmark          : ARRAY 64 OF CARD32;  (* Note 23 *)
	dword resume PACKED;		//   resume            : CARD32;              (* Note 24 *)
	byte unk10[2048] PACKED;	//   unk10             : ARRAY 2048 OF CARD8; (* Note 25 *)
		// END;
};
#pragma pack (pop)


#endif