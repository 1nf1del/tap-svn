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

#ifdef __cplusplus
extern "C" {
#endif


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
	char signature[32];
	dword id;
	byte padding1[28];
	char name[MAX_PROGRAM_NAME];
	char authorName[MAX_AUTHOR_NAME];
    char description[MAX_DESCRIPTION];
	char etcStr[MAX_ETCINFO_STR];
} TAPHeader;


typedef struct
{
	dword firmwaregp;
	dword gp;
	dword unknown1;
	TAPHeader* header;
	TAPEntryPoints* entryPoints;
	dword loadAddress;
	char* currentDirectory;
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

// Start a TAP in the current directory
int TAP_Start( char *pFilename );

typedef struct
{
  byte Attr;
  byte unused1 [7];     //MJD HH MM SS Offset for normal files
  dword DirectoryCluster;
  dword NrOfClusters;   //always 1 for directories
  dword NrOfFreeBytes;  //If you know the cluster size, you can calculate the location of the last directory entry
  char  FileName [64];  //. in this case
  byte unused2 [31];    //ServiceName for normal files
  byte unused3 [9];     //Several flags
  byte Checksum;
  byte unused4 [3];
} tDirStruct;


//tDirStruct* TAP_Hdd_GetCurrentDirectory();

#define EVT_TAPCLOSE		0x8000
#define EVT_TAPHASCONFIG	0x8001
#define EVT_TAPCONFIG		0x8002

#ifdef __cplusplus
}
#endif

#endif
