/*
	Copyright (C) 2005-2007 Simon Capewell
	Portions copyright other authors

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

#ifndef __FIRMWARE_H
#define __FIRMWARE_H

#ifdef __cplusplus
extern "C" {
#endif

dword FindFirmwareFunction( dword* signature, size_t signatureSize, dword start, dword end );
dword GetFirmwareVariable( dword baseAddress, dword hwAddr, dword lwAddr );
dword CallFirmware( dword address, dword param1, dword param2, dword param3, dword param4 );
dword* CreateAPIWrapper( dword functionAddress );

dword HackFirmware( dword* address, dword value );
dword* FindAndHackFirmware( dword* start, dword length, dword oldOpCode, dword newOpCode );
void UndoFirmwareHacks();

dword FindEEPROM();

#ifdef __cplusplus
}
#endif

#endif
