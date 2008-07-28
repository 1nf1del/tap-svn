/*
	Copyright (C) 2007 Simon Capewell
	Portions copyright bdb

	This file is part of the TAPs for Topfield PVRs project.
		http://tap.berlios.de/

	This is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	The software is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this software; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include <tap.h>
#include <libFireBird.h>
#include <Firmware.h>
#include <model.h>
#include "MHEGState.h"
#include "TAPExtensions.h"
#include "Settings.h"


//-----------------------------------------------------------------------------
bool MHEGState_Available = FALSE;


static dword MHEGRegisterGroupSignature[] =
{
	0x53c0007c,                             //beqzl   $fp, XXXX
	0x8f99FFFF,                             //lw      $t9, -0x74D8($gp)
	0x8f99FFFF,                             //lw      $t9, -0x74F8($gp)
	0x57200079,                             //bnezl   $t9, XXXX
	0x8f99FFFF,                             //lw      $t9, -0x74D8($gp)
	0x8f83FFFF,                             //lw      $v1, -0x74F4($gp)         ; <---
	0x10600015,                             //beqz    $v1, XXXX
	0x00000000,                             //nop
	0x8c790008,                             //lw      $t9, 8($v1)
	0x8f220034,                             //lw      $v0, 0x34($t9)            ; register_group
	0x24180003,                             //li      $t8, 3                    ; register_group_3
	0x14580005,                             //bne     $v0, $t8, XXXX
	0x24190005,                             //li      $t9, 5                    ; register_group_5
	0x2bc10010,                             //slti    $1, $fp, 0x10
	0x5420006e,                             //bnezl   $1, XXXX
	0x8f99FFFF                              //lw      $t9, -0x74D8($gp)
};


dword GetMHEGRegisterGroup()
{
	__asm__ __volatile__ (
		"lui	$02,0x0\n"
		"or		$02,$02,0x0\n"
		"nop\n"
	);
}


//see  dtt mheg-5 spec. v1.06, section 3.6
//if register_group == 0 then no mheg is running
//if register_group == 3 then mheg is running, has claimed <text><red><green><yellow><blue>
//if register_group == 5 then mheg is running, has claimed group3 + <arrow keys> + <ok>
//if register_group == 4 then mheg is running, has claimed group3 + group5 + <number keys>
int GetMHEGMode()
{
	dword v0;
    int registerGroup = 0;

	v0 = *(dword*)GetMHEGRegisterGroup();
    if (v0)
	{
        dword v1 = *(dword*)(v0 + 0x8);
        if (v1)
		{
            registerGroup = *(dword*)(v1 + 0x34);
		}
	}

	return registerGroup;
}


// Call TAP_GetState
dword GetState( dword *state, dword *subState )
{
	__asm__ __volatile__ (
		"addiu	$8,0\n"
		"addiu	$8,0\n"
		"addiu	$8,0\n"
		"nop\n");
}


dword GetStateHook( dword *state, dword *subState )
{
	dword s, ss;
	// Call TAP_GetState
	dword result = GetState( &s, &ss );
	// If nothing else appears to be active, check for full screen MHEG
	if ( s == STATE_Normal && ss == SUBSTATE_Normal )
	{
		if ( GetMHEGMode() >= 4 )
		{
			s = STATE_Ttx;
			ss = SUBSTATE_Ttx;
		}
	}
	if (state)
		*state = s;
	if (subState)
		*subState = ss;
	return result;
}


//-----------------------------------------------------------------------------
int MHEGState_Init()
{
	dword addr;

	MHEGState_Available = (GetModel() == TF5800 && GetModel() == TF5810t || GetModel() == TF5800t) &&
		!PatchIsInstalled((dword*)0x80000000, "Mh");
	if ( !MHEGState_Available )
		return FALSE;
	// Look event table details up from the TAP_GetCurrentEvent code
	addr = FindFirmwareFunction( MHEGRegisterGroupSignature, sizeof(MHEGRegisterGroupSignature), 0x80000000, 0x80200000 );
	if ( !addr )
		return FALSE;
	// Register group variable is accessed by the instruction at offset 0x14
	addr = GetFirmwareVariable( addr, 0x16, 0x16 );
	if ( !addr )
		return FALSE;
	addr = tapProcess[*currentTAPIndex].firmwaregp + (0xffff0000 | addr);
	MHEGState_Available = TRUE;

	// We need to know various firmware specific addresses without needing to use $gp
	((word*)GetMHEGRegisterGroup)[1] = (addr >> 16) & 0xffff;
	((word*)GetMHEGRegisterGroup)[3] = addr & 0xffff;

	return TRUE;
}


void MHEGState_Enable()
{
	dword *fn;
	dword *addr;

	if ( !MHEGState_Available )
		return;

	// patch TAP_GetState to return STATE_Ttx and SUBSTATE_Ttx when full screen MHEG is active
	fn = (dword*)GetState;
	addr = (dword*)TAP_GetState;

	// save the original instructions to be executed prior to the hook code
	fn[0] = addr[0];
	fn[1] = addr[1];
	fn[2] = JMP_CMD | REL_ADDR(addr+2);
	fn[3] = NOP_CMD;
	HackFirmware( (dword)(addr), JMP_CMD | REL_ADDR(GetStateHook) );
	HackFirmware( (dword)(addr+1), NOP_CMD );
}
