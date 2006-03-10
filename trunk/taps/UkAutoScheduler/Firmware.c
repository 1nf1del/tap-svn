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
#include "Firmware.h"
#include "TAPExtensions.h"
#include "OpCodes.h"

word* sysID = (word*)0xa3fffffa;

// Return the Topfield model number
Model GetModel()
{
	switch ( *sysID )
	{
	case 406:	return TF5000_5500;
	case 416:	return TF5000t;
	case 436:	return TF5010_5510;
	case 446:	return TF5000_BP_WP;
	case 456:	return TF5800t;
	case 466:	return TF5000t_BP_WP;
	case 486:	return TF5010_SE;
	case 501:	return TF5000CI_ME;
	case 1416:	return TF5000_MP;
	case 1426:	return TF5000t_MP;
	case 1456:	return TF5010_MP;
	case 1501:	return TF5000CI_EUR;
	case 10416:	return PC5101c_5102c;
	case 10426:	return PC5101t_5102t;
	case 10446:	return TF5200c;
	case 12406:	return TF5100c;
	case 13406:	return TF5100;
	}
	return TFOther;
}


// Locate the address of a firmware function using a signature
dword FindFirmwareFunction( dword* signature, size_t signatureSize, dword start, dword end )
{
	byte* p;
	for ( p = (byte*)start; p < (byte*)end; ++p )
	{
		dword address = (dword)p;
		byte* sig = (byte*)signature;
		int i;
		for ( i = 0; i < signatureSize && p < (byte*)end && (*sig==*p || *sig==0xff); ++i )
		{
			++sig;
			++p;
		}
		if ( i >= signatureSize )
			return address;
	}

	return 0;
}


// Internal function based on TAP_GetTime to call the firmware with the correct value in $gp
static dword CallFirmwareInternal( dword a0, dword a1, dword a2, dword a3, dword address, TAPProcess* tapTable, dword* tapIndex )
{
	__asm__ __volatile__ (
		"lw		$08,16($29)\n"
		"lw		$09,20($29)\n"
		"lw		$10,24($29)\n"
		"addiu	$29,-48\n"
		"sw		$30,44($29)\n"
		"nop\n"
		"sw		$31,32($29)\n"
		"sw		$22,40($29)\n"
		"move	$30,$10\n"
		"sw		$21,36($29)\n"
		"lw		$15,0($30)\n"
		"nop\n"
		"move	$21,$09\n"
		"sll	$15,5\n"
		"addu	$15,$21\n"
		"lw		$22,0($15)\n"
		"nop\n"
		"nop\n"
		"move	$02,$28\n"
		"nop\n"
		"move	$28,$22\n"
		"nop\n"
		"jalr	$08\n"
		"nop\n"
		"lw		$25,0($30)\n"
		"addu	$22,$02,0\n"
		"sll	$25,5\n"
		"addu	$25,$21\n"
		"lw		$30,4($25)\n"
		"nop\n"
		"nop\n"
		"move	$02,$28\n"
		"nop\n"
		"nop\n"
		"move	$28,$30\n"
		"lw		$31,32($29)\n"
		"move	$02,$22\n"
		"lw		$21,36($29)\n"
		"lw		$22,40($29)\n"
		"lw		$30,44($29)\n"
		"addiu	$29,48\n"
	);
}


// Call a firmware function from a TAP
dword CallFirmware( dword address, dword param1, dword param2, dword param3, dword param4 )
{
	if ( !tapProcess || !currentTAPIndex )
		return 0;
	if ( address < 0x80000000 || address > 0x80400000 )
		return 0;
	return CallFirmwareInternal( param1, param2, param3, param4, address, tapProcess, currentTAPIndex );
}


// Create a code wrapper based on TAP_GetTime. The function allocates a buffer and copies 
// TAP_GetTime into it except for the actual call of the getTime() firmware function. 
// The address of getTime() is replaced with the provided address.
// The wrapped function call supports up to 4 parameters and returns a short value.
// Parameters:
//      functionAddress - address of the FW function to be wrapped
// Returns:
//		Address of the new wrapper
dword* CreateAPIWrapper( dword functionAddress )
{
	int i;
	// TAP_GetTime provides a template for wrapper code. The only thing to be
	// changed is the address of the wrapped function.
	dword *src = (dword*)TAP_GetTime;
	dword *buffer;
	int bufferSize = 0;

	// find the return statement
	while ( src[bufferSize] != JR_CMD )
	{
		++bufferSize;
	}

	// the buffer should include the return statement and the command following
	// the return statement
	bufferSize += 2;

	// allocate memory
	buffer = malloc( sizeof(dword) * bufferSize );
	if ( buffer == NULL )
	{
//		TAP_Print( "malloc failed\n" );
		return NULL;
	}

	for ( i = 0; i < bufferSize; ++i, ++src )
	{
		if ( (*src & JAL_MASK) == JAL_CMD )
		{
			// this is the wrapped call to the actual firmware implementation
			// replace it with the call to the provided address
			buffer[i] = JAL(functionAddress);
		}
		else
		{
			buffer[i] = *src;
		}
	}

	return buffer;
}


// structure for tracking changes to the firmware
typedef struct
{
	dword* address;
	dword oldValue;
} FirmwareChange;

static FirmwareChange firmwareHacks[100];
static int hackCount = 0;


// Replace a single instruction with another instruction
dword HackFirmware( dword* address, dword value )
{
	if ( (dword)address < 0x80000000 || (dword)address > 0x83ffffff )
	{
//		TAP_Print( "HackFirmware address %08X out of range.\n", address );
		return;
	}
//	TAP_Print( "%d: %08X changed from %08X to %08X\n", hackCount, address, *address, value );
	firmwareHacks[hackCount].address = address;
	firmwareHacks[hackCount].oldValue = *address;
	++hackCount;
	*address = value;

	return firmwareHacks[hackCount].oldValue;
}


// Find and replace a single instruction within an address range or function
dword* FindAndHackFirmware( dword* start, dword length, dword oldOpCode, dword newOpCode )
{
	int i;
	for ( i = 0; i < length && *start != JR_CMD; ++i )
	{
		if ( *start == oldOpCode )
		{
			HackFirmware( start, newOpCode );
			return start;
		}
		++start;
	}
	return NULL;
}


// Undo hacks
void UndoFirmwareHacks()
{
	while ( hackCount > 0 )
	{
		--hackCount;
//		TAP_Print( "%d: %08X set to %08X\n", hackCount, firmwareHacks[hackCount].address, firmwareHacks[hackCount].oldValue );
		*firmwareHacks[hackCount].address = firmwareHacks[hackCount].oldValue;
	}
}
