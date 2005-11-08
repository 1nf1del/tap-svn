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

#ifndef __FIRMWARE_H
#define __FIRMWARE_H


extern dword _appl_version;
extern word* sysID;

typedef enum
{
	TFOther,
	TF5000_5500,
	TF5000t,
	TF5010_5510,
	TF5000_BP_WP,
	TF5800t,
	TF5000t_BP_WP,
	TF5010_SE,
	TF5000CI_ME,
	TF5000_MP,
	TF5000t_MP,
	TF5010_MP,
	TF5000CI_EUR,
	TF5200c,
	TF5100c
} Model;

Model GetModel();

dword FindFirmwareFunction( dword* signature, size_t signatureSize, dword start, dword end );
dword* CreateAPIWrapper( dword functionAddress );

dword HackFirmware( dword* address, dword value );
dword* FindAndHackFirmware( dword* start, dword length, dword oldOpCode, dword newOpCode );
void UndoFirmwareHacks();


#endif
