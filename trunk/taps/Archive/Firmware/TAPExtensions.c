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
#include "TAPExtensions.h"
#include "OpCodes.h"


TAPProcess* tapProcess;
dword* currentTAPIndex;


// Initialise the TAP extensions. This must be called once before using any
// of the functions in this module
bool StartTAPExtensions()
{
	// Use one of the simpler TAP_ functions to locate the commands that address the TAP process table.
	// This contains
	dword* addr = (dword*)TAP_KbHit;
	int i;
	dword start = 0;
	dword end = 0;
	for ( i = 0 ; i < 15; ++i, ++addr )
	{
		if ( (*addr & 0xffff0000) == LUI_FP_CMD )
			end = (*addr & 0xffff) << 16;
		else if ( (*addr & 0xffff0000) == ADDIU_FP_CMD )
			end += (short)(*addr & 0xffff);
		else if ( (*addr & 0xffff0000) == LUI_S6_CMD )
			start = (*addr & 0xffff) << 16;
		else if ( (*addr & 0xffff0000) == ADDIU_S6_CMD )
			start += (short)(*addr & 0xffff);
	}
	
	if ( start == 0 || end == 0 )
		return FALSE;

	tapProcess = (TAPProcess*)start;
	currentTAPIndex = (dword*)end;

	return TRUE;
}


// Output diagnostic information
void DumpRunningTAPs()
{
	int i;
	TAPProcess* p = tapProcess;
	for ( i = 0 ; i < TAP_MAX; ++i, ++p )
	{
		if ( p->entryPoints )
		{
			TAP_Print( "*** TAP Info ***\n" );
			TAP_Print( "%s %08X\n", p->header->name, p->header->id );
			TAP_Print( "%s\n", p->header->description );
			TAP_Print( "%s\n", p->header->etcStr );
			TAP_Print( "firmwaregp %08X\n", p->firmwaregp );
			TAP_Print( "gp %08X\n", p->gp );
			TAP_Print( "unknown1 %08X\n", p->unknown1 );
			TAP_Print( "header %08X\n", p->header );
			TAP_Print( "entry %08X\n", p->entryPoints );
			TAP_Print( "load %08X\n", p->loadAddress );
			TAP_Print( "unknown2 %08X\n", p->unknown2 );
			TAP_Print( "z %08X\n", p->zero );
/*
			TAP_Print( "*** TAP Entry Points ***\n" );
			TAP_Print( "_initData %08X\n", p->entryPoints->_initData );
			TAP_Print( "TAP_Main %08X\n", p->entryPoints-> TAP_Main);
			TAP_Print( "_tap_gotStart %08X\n", p->entryPoints-> _tap_gotStart);
			TAP_Print( "_tap_gotEnd %08X\n", p->entryPoints-> _tap_gotEnd);
			TAP_Print( "_tap_textEnd %08X\n", p->entryPoints-> _tap_textEnd);
			TAP_Print( "_tap_fixupStart %08X\n", p->entryPoints-> _tap_fixupStart);
			TAP_Print( "_tap_fixupEnd %08X\n", p->entryPoints-> _tap_fixupEnd);
			TAP_Print( "TAP_EventHandler %08X\n", p->entryPoints-> TAP_EventHandler);
			TAP_Print( "_gp %08X\n", p->entryPoints-> _gp);
			TAP_Print( "_end %08X\n", p->entryPoints-> _end);
			TAP_Print( "\n" );
*/		}
	}
}


// Get the TAPProcess structure for a specific running TAP
TAPProcess* GetTAPProcess( dword tapID )
{
	int i;
	TAPProcess* p = tapProcess;
	for ( i = 0 ; i <TAP_MAX; ++i, ++p )
	{
		if ( p->entryPoints && p->header->id == tapID )
			return p;
	}

	return NULL;
}


// Get the position in the TAP process table of a specific running TAP
int GetTAPIndex( dword tapID )
{
	int i;
	TAPProcess* p = tapProcess;
	for ( i = 0 ; i < TAP_MAX; ++i, ++p )
	{
		if ( p->entryPoints && p->header && p->header->id == tapID )
			break;
	}

	return i;
}


// Promote a running TAP to the top of the TAP process list
bool PromoteTAP( dword tapID )
{
	int i;
	TAPProcess* p = tapProcess;
	for ( i = 0 ; i <TAP_MAX; ++i, ++p )
	{
		if ( p->entryPoints && p->header && p->header->id == tapID )
		{
			if ( i ) // No point promoting the first item
			{
				TAPProcess temp;
				memcpy( &temp, p, sizeof(TAPProcess) );
				memcpy( tapProcess+1, tapProcess, sizeof(TAPProcess)*i );
				memcpy( tapProcess, &temp, sizeof(TAPProcess) );
			}
			return TRUE;
		}
	}

	return FALSE;
}


static dword TAP_SendEventHelper( TAPProcess* tap, word event, dword param1, dword param2 )
{
#ifdef WIN32
#else       
	__asm__ __volatile__ (
		"addiu	$sp, -4\n"
		"sw		$31,0($sp)\n"		// save ra
		"lw		$28,4($04)\n"		// load gp with TAP gp value
		"lw		$08,16($04)\n"		// get TAP_EventHandler
		"lw		$09,28($08)\n"
		"addiu	$04,$05,0\n"
		"addiu	$05,$06,0\n"
		"addiu	$06,$07,0\n"
		"jalr	$09\n"
		"nop\n"
		"lw		$31,0($sp)\n"		// restore ra
		"addiu	$sp,4\n"
	);
#endif
}


// Send an event to a specific TAP using it's position in the TAP processes table
dword TAP_SendEvent( int index, word event, dword param1, dword param2 )
{
	TAPProcess* tap = tapProcess+index;
	if ( tap->entryPoints )
	{
		dword thisTAPIndex = *currentTAPIndex;
		// Get a copy of the value of gp for this TAP. For some reason reading the actual value
		// in the register results in a crash
		dword tapGP = tapProcess[thisTAPIndex].gp;

		// TAP_Print("Calling %s id=%08X ...", tap->header->name, tap->header->id );

		// any diagnostics must be placed *before* this line
		*currentTAPIndex = index;

		param1 = TAP_SendEventHelper( tap, event, param1, param2 );

		// restore gp for the calling TAP. This must be done within the loop so that
		// currentTAPIndex can be accessed
#ifdef WIN32
#else       
		asm volatile ( "or $28,%0,$00\n" : : "r"(tapGP));
		*currentTAPIndex = thisTAPIndex;
#endif
		// any diagnostics must be placed *after* this line
		// TAP_Print(" result %X\n", param1 );
	}
	return param1;
}


// Send an event to a specific TAP using it's id
dword TAP_SendEventByID( dword tapID, word event, dword param1, dword param2 )
{
	int index = GetTAPIndex( tapID );
	if ( index < TAP_MAX )
		param1 = TAP_SendEvent( index, event, param1, param2 );
	return param1;
}


// Broadcast an event to all running TAPs
dword TAP_BroadcastEvent( word event, dword param1, dword param2 )
{
	dword thisTAPIndex = *currentTAPIndex;
	int i;

	for ( i = 0 ; i < TAP_MAX; ++i )
	{
		if ( i != thisTAPIndex )	// don't call ourselves
		{
			dword tempParam1;
			tempParam1 = TAP_SendEvent( i, event, param1, param2 );

			// Zero return value should mean don't pass the value on to other TAPs
			// The firmware actually does, so make sure param2 is also zero in this case
			if ( tempParam1 == 0 )
			{
				param1 = 0;
				param2 = 0;
			}
		}
	}

	return param1;
}
