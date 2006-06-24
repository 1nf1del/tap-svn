/*
	Remote control codes for use with RemoteExtender

	Copyright (C) 2005 Rowan Moor

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

#ifndef __RAWKEY__
#define __RAWKEY__

// Pull in Topfield standard key definitions
#include <key.h>

// Additional key defines
#define RKEY_SubLeft		RKEY_Prev 
#define RKEY_SubRight		RKEY_Next 
#define RKEY_White		 	RKEY_Ab
#define RKEY_Red		 	RKEY_NewF1
#define RKEY_Green			RKEY_F2
#define RKEY_Yellow			RKEY_F3
#define RKEY_Blue			RKEY_F4
#define RKEY_Filelist		RKEY_PlayList


//	Standard Remote controller key codes
//  Note: These use the same naming convention as the RKEY codes, except the colour keys.
//  Where there are differences between the 5800 and 5000, 
//  the 5800 use is first and the 5000 use second in the name


#define RAWKEY_Front_ChUp		1
#define RAWKEY_Front_ChDown 	2
#define RAWKEY_Front_VolUp		3
#define RAWKEY_Front_VolDown	4
#define RAWKEY_Front_Ok 		5
#define RAWKEY_Front_Menu		6

#define RAWKEY_Up				0x100 
#define RAWKEY_Down				0x101 
#define RAWKEY_Right			0x102 
#define RAWKEY_Left				0x103 
#define RAWKEY_List_TVRad		0x104 
#define RAWKEY_Exit_AudioTrk	0x105
#define RAWKEY_Pause			0x106 
#define RAWKEY_ChUp_Subt		0x107
#define RAWKEY_TvSat			0x108 
#define RAWKEY_VolUp_Fav		0x109
#define RAWKEY_Mute	 			0x10c 
#define RAWKEY_Green			0x10d 
#define RAWKEY_Yellow			0x10e 
#define RAWKEY_Blue	 			0x10f 
#define RAWKEY_0	 			0x110 
#define RAWKEY_1	 			0x111 
#define RAWKEY_2	 			0x112 
#define RAWKEY_3	 			0x113 
#define RAWKEY_4	 			0x114 
#define RAWKEY_5	 			0x115 
#define RAWKEY_6	 			0x116 
#define RAWKEY_7	 			0x117 
#define RAWKEY_8	 			0x118 
#define RAWKEY_9	 			0x119 
#define RAWKEY_Menu	 			0x11a 
#define RAWKEY_Guide	 		0x11b 
#define RAWKEY_VolDown_Exit		0x11c
#define RAWKEY_Info	 			0x11d 
#define RAWKEY_Recall			0x11e 
#define RAWKEY_Ok	 			0x11f 
#define RAWKEY_TVRadio_Uhf		0x143 
#define RAWKEY_Opt_Sleep		0x144 
#define RAWKEY_Rewind			0x145 
#define RAWKEY_Play	 			0x146 
#define RAWKEY_ChDown_Teletext	0x147 
#define RAWKEY_Forward	 		0x148 
#define RAWKEY_Slow	 			0x149 
#define RAWKEY_Stop	 			0x14a 
#define RAWKEY_Record	 		0x14b 
#define RAWKEY_White_Ab	 		0x14c 
#define RAWKEY_Red	 			0x14d 
#define RAWKEY_PIP_Prev	 		0x150 
#define RAWKEY_PlayList	 		0x151
#define RAWKEY_Teletext_Next	0x152 
#define RAWKEY_PIPSwap_Sat	 	0x15e
#define RAWKEY_Power	 		0x1f0


//  Additional codes for Programmable Remotes
//  See RAWKET.txt for details of using these.

#define RAWKEY_40		 		0x128
#define RAWKEY_41		 		0x129
#define RAWKEY_42		 		0x12a
#define RAWKEY_43		 		0x12b
#define RAWKEY_44		 		0x12c
#define RAWKEY_45		 		0x12d
#define RAWKEY_46		 		0x12e
#define RAWKEY_47		 		0x12f
#define RAWKEY_48		 		0x130
#define RAWKEY_49		 		0x131


#endif
