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
#include "MessageWin.h"
#include "FirmwareCalls.h"
#include "Firmware.h"


dword debuggerMoveSignature[] =
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

dword moveAddressDebugger = 0;

typedef dword  (*TAP_Hdd_MoveFn)(char *from_dir, char *to_dir, char *filename);
TAP_Hdd_MoveFn  moveAddressAPI;

enum
{
    oTAP_Hdd_unknown0          = 0x00,  //hdd related, called by debug functions 'rs', 'rc'     -- read sector?
    oTAP_Hdd_unknown1          = 0x01,  //hdd related, called by debug functions 'ws', 'wc'     -- write sector?
    oTAP_unknown2              = 0x02,  //writes to eeprom? @ 0xa3ffffe0, 0xa3ffffe1
    oTAP_Hdd_unknown3          = 0x03,  //ata/dma related, ???
    oTAP_Hdd_SetBookmark       = 0x04,
    oTAP_Hdd_GotoBookmark      = 0x05,
    oTAP_Hdd_ChangePlaybackPos = 0x06,
    oTAP_ControlEit            = 0x07,
    oTAP_SetBk                 = 0x08,
    oTAP_EPG_UpdateEvent       = 0x09,
    oTAP_EPG_DeleteEvent       = 0x0a,
    oTAP_EPG_GetExtInfo        = 0x0b,
    oTAP_Channel_IsStarted     = 0x0c,
    oTAP_Vfd_GetStatus         = 0x0d,
    oTAP_Vfd_Control           = 0x0e,
    oTAP_Vfd_SendData          = 0x0f,
    oTAP_Win_SetAvtice         = 0x10,
    oTAP_Win_SetDrawItemFunc   = 0x11,
    oTAP_SysOsdControl         = 0x12,
    oTAP_Hdd_Move              = 0x13,
    oTAP_Osd_unknown20         = 0x14,  //osd related, ???
};

extern void* (*TAP_GetSystemProc)( int );


bool TAP_Hdd_Move_Available()
{
	if ( !moveAddressAPI && !moveAddressDebugger )
	{
		moveAddressAPI = (TAP_Hdd_MoveFn)TAP_GetSystemProc(oTAP_Hdd_Move);
		TAP_Print("API Move function %08X\n", moveAddressAPI);
	}

	if ( !moveAddressAPI && !moveAddressDebugger )
	{
		moveAddressDebugger = FindFirmwareFunction( debuggerMoveSignature, sizeof(debuggerMoveSignature), 0x800f8000, 0x80108000 );
		if ( !moveAddressDebugger )
			moveAddressDebugger = FindFirmwareFunction( debuggerMoveSignature, sizeof(debuggerMoveSignature), 0x80230000, 0x80238000 );

		TAP_Print("Debugger Move function %08X\n", moveAddressDebugger);
	}

	return moveAddressDebugger || moveAddressAPI;
}


bool TAP_Hdd_Move( char* fromDir, char* toDir, char* filename )
{
	if ( !CanMove() )
		return FALSE;

	if ( !fromDir || !toDir || !filename )
		return FALSE;

	if ( moveAddressAPI )
		moveAddressAPI( fromDir, toDir, filename );
	else if ( moveAddressDebugger )
	{
		// The debugger move function overwrites the inputs, so we need to copy to temporary buffers
		// it also requires 2 parameters rather than 3 and no preceeding / on paths to specify root
		char src[1000];
		char dest[1000];

		if ( fromDir[0] == '/' )
			++fromDir;
		strcpy( src, fromDir );
		strcat( src, "/" );
		strcat( src, filename );

		if ( toDir[0] == '/' )
			++toDir;
		strcpy( dest, toDir );
		CallFirmware( moveAddressDebugger, (dword)src, (dword)dest, 0,0 );
	}

	return TRUE;
}
