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

#ifndef __TAPEXTENSIONS_H
#define __TAPEXTENSIONS_H


typedef dword (*TAP_EventHandlerFn)( word event, dword param1, dword param2 );
typedef int (*TAP_MainFn)();

typedef struct
{
	// From libtap.a
	dword _initData;
	TAP_MainFn TAP_Main;
	dword _tap_gotStart;
	dword _tap_gotEnd;
	dword _tap_textEnd;
	dword _tap_fixupStart;
	dword _tap_fixupEnd;
	TAP_EventHandlerFn TAP_EventHandler;
	dword _gp;
	dword _end;
} TAPEntryPoints;


typedef struct
{
	byte signature[32];
	dword id;
	byte padding1[28];
	const char name[MAX_PROGRAM_NAME];
	const char authorName[MAX_AUTHOR_NAME];
    const char description[MAX_DESCRIPTION];
	const char etcStr[MAX_ETCINFO_STR];
} TAPHeader;


typedef struct
{
	dword firmwaregp;
	dword gp;
	dword unknown1;
	TAPHeader* header;
	TAPEntryPoints* entryPoints;
	dword loadAddress;
	dword unknown2;
	dword zero;
} TAPProcess;

#define TAP_MAX 16

extern TAPProcess* tapProcess;
extern dword* currentTAPIndex;

bool StartTAPExtensions();
int GetTAPIndex( dword tapID );
TAPProcess* GetTAPProcess( dword tapID );
bool PromoteTAP( dword tapID );
dword TAP_SendEvent( int index, word event, dword param1, dword param2 );
dword TAP_SendEventByID( dword tapID, word event, dword param1, dword param2 );
dword TAP_BroadcastEvent( word event, dword param1, dword param2 );

#define EVT_TAPCLOSE		0x8000
#define EVT_TAPHASCONFIG	0x8001
#define EVT_TAPCONFIG		0x8002


//typedef dword (*TAP_Launch)(TYPE_File* file);
//TAP_Launch tapLaunch = NULL;


#endif
