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


dword fcDebugMethodMoveSignature[] =
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

dword fcDebugMethodMoveAddress = 0;

typedef dword (*TAP_Hdd_ApiMoveFn)(char *from_dir, char *to_dir, char *filename);
TAP_Hdd_ApiMoveFn TAP_Hdd_ApiMove;

enum
{
	oTAP_Hdd_unknown0 = 0x00,
	oTAP_Hdd_unknown1 = 0x01,
	oTAP_unknown2 = 0x02,
	oTAP_Hdd_unknown3 = 0x03,
	oTAP_Hdd_SetBookmark = 0x04,
	oTAP_Hdd_GotoBookmark = 0x05,
	oTAP_Hdd_ChangePlaybackPos = 0x06,
	oTAP_ControlEit = 0x07,
	oTAP_SetBk = 0x08,
	oTAP_EPG_UpdateEvent = 0x09,
	oTAP_EPG_DeleteEvent = 0x0a,
	oTAP_EPG_GetExtInfo = 0x0b,
	oTAP_Channel_IsStarted = 0x0c,
	oTAP_Vfd_GetStatus = 0x0d,
	oTAP_Vfd_Control = 0x0e,
	oTAP_Vfd_SendData = 0x0f,
	oTAP_Win_SetAvtice = 0x10,
	oTAP_Win_SetDrawItemFunc = 0x11,
	oTAP_SysOsdControl = 0x12,
	oTAP_Hdd_Move = 0x13,
	oTAP_Osd_unknown20 = 0x14,
};

extern void* (*TAP_GetSystemProc)( int );







bool fcCheckDebugMoveAvailability( void )
{
	fcDebugMethodMoveAddress = FindFirmwareFunction( fcDebugMethodMoveSignature, sizeof(fcDebugMethodMoveSignature), 0x800f8000, 0x80108000 );

	if ( !fcDebugMethodMoveAddress )
	{
		fcDebugMethodMoveAddress = FindFirmwareFunction( fcDebugMethodMoveSignature, sizeof(fcDebugMethodMoveSignature), 0x80230000, 0x80238000 );
	}

	if ( !fcDebugMethodMoveAddress )
	{
		return FALSE;
	}

	return TRUE;
}

bool TAP_Hdd_DebugMove( char* filename, char* destination )
{
	if ( fcDebugMethodMoveAddress )
	{
		if
		(
			(!filename)
			||
			(!destination)
		)
		{
			return FALSE;
		}

		CallFirmware( fcDebugMethodMoveAddress, (dword)filename, (dword)destination, 0,0 );

		return TRUE;
	}

	return FALSE;
}




bool fcCheckApiMoveAvailability( void )
{
	TAP_Hdd_ApiMove = (TAP_Hdd_ApiMoveFn)TAP_GetSystemProc(oTAP_Hdd_Move);

	if ( TAP_Hdd_ApiMove != NULL )
	{
		return TRUE;
	}

	return FALSE;
}

