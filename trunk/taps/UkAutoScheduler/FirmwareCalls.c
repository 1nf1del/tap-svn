/*
	Copyright (C) 2005 Simon Capewell

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

#include <tap.h>
//#include "MessageWin.h"
#include "FirmwareCalls.h"
#include "Firmware.h"


dword moveSignature[] =
{
	0x27bdffd0,
	0xafb3001c,
	0xafb40020,
	0xafb50024,
	0xafb60028,
	0xafbe002c,
	0xafbf0018,
	0x0080f025,
	0x00a0b025,
	0x8f99ffff,
	0x17200005,
	0x00000000,
	0x2784ffff,
	0x00002825,
	0x0c02ffff,
	0x00000000,
	0x27a40010,
	0x00002825,
	0x0c02ffff,
	0x00000000,
	0x27a40014,
	0x00002825
};

dword moveAddress = 0;



bool Is_TAP_Hdd_Move_Available( void )
{
//	TAP_Print("Searching for firmware Move function...");
	moveAddress = FindFirmwareFunction( moveSignature, sizeof(moveSignature), 0x800f8000, 0x80108000 );
	if ( !moveAddress )
		moveAddress = FindFirmwareFunction( moveSignature, sizeof(moveSignature), 0x80230000, 0x80238000 );

	if ( !moveAddress )
	{
//		TAP_Print("not found\n");
		//ShowMessage( "Sorry, this firmware does not allow TAPs to move files.", 200 );
		return FALSE;
	}

//	TAP_Print("found at %08X\n", moveAddress);

	return TRUE;
}


bool TAP_Hdd_Move( char* filename, char* destination )
{
	if ( moveAddress )
	{
		if ( !filename || !destination )
			return FALSE;

		CallFirmware( moveAddress, (dword)filename, (dword)destination, 0,0 );

		return TRUE;
	}

	return FALSE;
}
