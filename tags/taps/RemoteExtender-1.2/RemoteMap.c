/*
	Copyright (C) 2005 Simon Capewell

	This file is part of the TAPs for Topfield PVRs project.
		http://tap.berlios.de/

	This is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	The software is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this software; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include <tap.h>
#include <key.h>
#include "RemoteMap.h"


// This array maps hardware keycodes with the TF5000 RKEY values for TF5800 compatibility purposes
// as a result, the following keys have been swapped from the actual TF5000 remote
// RKEY_Fav  <==> RKEY_TvRadio
// RKEY_Exit <==> RKEY_AudioTrk
dword keys5000[] =
{
// 0			1				2			3				4				5				6				7
RKEY_ChUp,		RKEY_ChDown,	RKEY_VolUp,	RKEY_VolDown,	RKEY_Fav,		RKEY_Exit,		RKEY_Pause,		RKEY_Subt,
RKEY_TvSat,		RKEY_TvRadio,	0,			RKEY_PlayList,	RKEY_Mute,		RKEY_F2,		RKEY_F3,		RKEY_F4,	

RKEY_0,			RKEY_1,			RKEY_2,		RKEY_3,			RKEY_4,			RKEY_5,			RKEY_6,			RKEY_7,
RKEY_8,			RKEY_9,			RKEY_Menu,	RKEY_Guide,		RKEY_AudioTrk,	RKEY_Info,		RKEY_Recall,	RKEY_Ok,

0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,

0,				0,				0,			RKEY_Uhf,		RKEY_Sleep,		RKEY_Rewind,	RKEY_Play,		RKEY_Teletext,
RKEY_Forward,	RKEY_Slow,		RKEY_Stop,	RKEY_Record,	RKEY_Ab,		RKEY_NewF1,		0,				0,

RKEY_Prev,		RKEY_PlayList,	RKEY_Next,	0,				0,				0,				0,				0,
0,				0,				0,			0,				0,				0,				RKEY_Sat,		0,

0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
RKEY_Power,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};
