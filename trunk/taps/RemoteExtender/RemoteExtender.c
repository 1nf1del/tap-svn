/*
	Copyright (C) 2005 Simon Capewell

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
#include <messagewin.h>
#include <Firmware.h>
#include <TAPExtensions.h>
#include <model.h>
#include <OpCodes.h>
#include <morekeys.h>

//#define MHEG 1
//#define TOPPY2 1
//#define DIAGNOSTIC 1

#ifdef MHEG
#define MHEG_TEXT "MHEG "
#else
#define MHEG_TEXT ""
#endif

#ifdef TOPPY2
#define TOPPY2_TEXT "Toppy2 " 
#else
#define TOPPY2_TEXT ""
#endif


TAP_ID(0x814243a3);
TAP_PROGRAM_NAME("Remote Extender " TOPPY2_TEXT MHEG_TEXT "1.1");
TAP_AUTHOR_NAME("Simon Capewell");
TAP_DESCRIPTION("Makes extra remote keys accessible to other TAPs");
TAP_ETCINFO(__DATE__);

#include <TSRCommander.h>

dword exitCount = 0;
dword lastKey = 0;
#ifdef DIAGNOSTIC
int rgn = 0;
#endif

//-----------------------------------------------------------------------------
// ReceiveKeyHook() stores the RC code received from the front panel processor to the
// memory specified in $t1 ($09).
// The value loaded in $t1 has to be set on TAP start. It has to
// be an absolute value. The hook function is to be inserted into
// the function retrieving the RC code from the front panel processor.
// The first two NOPs can be replaced with the instructions overwritten
// by the installed hook.
void ReceiveKeyHook()
{
	__asm__ __volatile__ (
		"nop\n"								// 00 				
		"nop\n"								// 04
		"lui	$09,0x0\n"					// 08 $t1
		"or		$09,$09,0x0\n"				// 0c $t1
		"nop\n"	
		"ori	$08,$00,0xe300\n"			// 0C $t0
		"bne	$21,$08,end\n");			// 

#ifdef TOPPY2
	__asm__ __volatile__ (
		"srl	$10,$15,16\n"
		"beqz	$10,store\n"				// continue to allow front panel codes through
		"and	$10,$15,0xff\n"
		// if $15 < 0x20
		"slti	$08,$10,0x20\n"
		"bnez	$08,filter\n"
		// if $10 >= 0x40
		"slti	$08,$10,0x40\n"
		"beqz	$08,test14\n"
		"addiu	$15,$15,-0x20\n"
		"b		store\n"

		"test14:\n"
		"slti	$08,$10,0x53\n"
		"bnez	$08,filter\n"
		"li		$08,0x5e\n"
		"beq	$08,$10,filter\n"

		"addiu	$15,$15,-0x10\n"
		"b		store\n"

		"filter:\n"
		"ori	$15,0xffff\n"
		"store:\n"
		"sw		$15,0($30)\n");				// *fp=t7
#endif

	__asm__ __volatile__ (
		"sw		$15,0($09)\n"				// *t9=t7
		"end:\n"
		"nop\n");
}


//-----------------------------------------------------------------------------
dword remoteReceiveSignature[] =
{
	0x27bdffe0,
	0xafb50014,
	0xafb60018,
	0xafbe001c,
	0xafbf0010,
	0x0080f025,
	0x0000a825,
	0x0c0000ff,
	0x00000000,
	0x0040b025,
	0x3c1980ff,
	0x8f39ffff,
	0x3c1880ff,
	0x8f18ffff,
	0x13380018,
	0x00000000,
	0x3c1980ff,
	0x2739ffff,
	0x27390008,
	0x3c1880ff,
	0x8f18ffff,
	0x0018c040,
	0x0338c821,
	0x97350000
};

bool HookReceiveKeyFunction()
{
	dword* hookFn;
	dword* addr;
	
	addr = (dword*)FindFirmwareFunction( remoteReceiveSignature, sizeof(remoteReceiveSignature), 0x80004000, 0x80007000 );
	if ( !addr )
	{
		ShowMessage( "Remote Extender\nReceive key function not found\n", 200 );
		return FALSE;
	}

	if ( (addr[0x20] & 0xffffff00) != 0xafcf0000 || (addr[0x21] & 0xffffff00) != 0x3c198000 )
	{
		ShowMessage( "Remote Extender\nFirmware receive function has an unexpected structure\n", 200 );
		return FALSE;
	}

	hookFn = (dword*)ReceiveKeyHook;
	// Validate receive hook function
	if (hookFn[2] != LUI_T1_CMD || hookFn[3] != OR_T1_CMD)
	{
		ShowMessage( "Remote Extender\nTAP receive key hook function has an unexpected structure\n", 200 );
		return FALSE;
	}

	// save the original instructions to be executed prior to the hook code
	hookFn[0] = addr[0x20];
	hookFn[1] = addr[0x21];

	// initialize memory address
	hookFn[2] = LUI_T1_CMD | (((dword)&lastKey >> 16) & 0xffff);
	hookFn[3] = OR_T1_CMD | ((dword)&lastKey & 0xffff);

	// install the hook
	HackFirmware( addr+0x20, JAL(ReceiveKeyHook) );
	HackFirmware( addr+0x21, NOP_CMD );

	return TRUE;
}


dword dispatchKeySignature[] =
{
	0x8eff001c,
	0x8fff0000,
	0x8eff0004,
	0x03c07825,
	0x00000000,
	0x00000000,
	0x03801025,
	0x00000000,
	0x00000000,
	0x02ffe025,
	0x24040100,
	0x02ff2825,
	0x02fff809,
	0x00003025,
	0xafc20000,
	0x8eff0000,
	0x03c0c025,
	0x00000000,
	0x00000000,
	0x03801025,
	0x02ffe025,
	0x26ff0020,
	0x26ff0001,
	0x2eff0010
};

// Change the firmware key dispatch to send 0xFF rather than 0x100
bool AdjustDispatchKeyFunction()
{
	dword* addr;
	
	addr = (dword*)FindFirmwareFunction( dispatchKeySignature, sizeof(dispatchKeySignature), 0x80128000, 0x80148000 );
	if ( !addr )
	{
		ShowMessage( "Remote Extender\nDispatch key function not found\n", 200 );
		return FALSE;
	}

	if ( addr[10] != 0x24040100 )
	{
		ShowMessage( "Remote Extender\nDispatch key function has an unexpected structure\n", 200 );
		return FALSE;
	}

	HackFirmware( addr+10, 0x240400ff );

	return TRUE;
}


//-----------------------------------------------------------------------------
dword TAP_EventHandler( word event, dword param1, dword param2 )
{
#ifdef DEBUG
	if ( event == EVT_KEY || event == EVT_KEY-1 ) 
	{
		TAP_Print("RemoteExtender: key=%X lastKey=%X\n", param1, lastKey);
		if ( param1 == RKEY_Recall )
		{
#ifdef DIAGNOSTIC
			if ( rgn )
				TAP_Osd_Delete(rgn);
#endif
			UndoFirmwareHacks();
			TAP_Exit();
			TAP_Print("Exiting Remote Extender TAP\n");
			return 0;
		}
	}
#endif

	// Capture the reassigned remote keypress event
	if ( event == EVT_KEY-1 )
	{
		int i;

#ifdef DIAGNOSTIC
		// display key codes on screen
		{
			char buffer[256];
			TAP_Osd_FillBox( rgn, 50,250, 130,50, COLOR_DarkGray );
			sprintf(buffer, "key=%X", param1);
			TAP_Osd_PutS( rgn, 50,250, -1, buffer, COLOR_White, COLOR_DarkGray, 0, FNT_Size_1622, FALSE, ALIGN_LEFT );
			sprintf(buffer, "rawkey=%X", lastKey);
			TAP_Osd_PutS( rgn, 50,275, -1, buffer, COLOR_White, COLOR_DarkGray, 0, FNT_Size_1622, FALSE, ALIGN_LEFT );
		}
#endif

#ifdef TOPPY2
		// Don't pass on filtered Toppy2 keypresses
		if ( (lastKey & 0xffff) == 0xffff )
		{
			lastKey = 0;
			return 0;
		}
#endif

		// provide an emergency shut down - press front panel OK button 5 times
		if ( lastKey != RAWKEY_Front_Ok )
			exitCount = 0;
		else if ( exitCount < 4 )
			++exitCount;
		else
		{
#ifdef DIAGNOSTIC
			if ( rgn )
				TAP_Osd_Delete(rgn);
#endif
			UndoFirmwareHacks();
			TAP_Exit();
			ShowMessage("Remote Extender TAP shut down\n"
				"Front Panel OK button was pressed 5 times.", 200);
			return 0;
		}

		// lastkey must NOT be referenced within the for loop due to possible re-entrancy
		// e.g. another TAP calling TAP_SystemProc from their TAP_EventHandler
		param2 = lastKey < 0x10000 ? lastKey : 0x100 | (lastKey & 0xff);
		// therefore we clear it here
		lastKey = 0;

		// TAP_Print("Sending %04X %08X %02X\n", event, param1, param2 );
		for ( i = 0 ; i < TAP_MAX; ++i )
		{
			if ( i != *currentTAPIndex )	// don't call ourselves
			{
				dword tempParam1;

				// TAP_Print("Sending %04X %08X %02X\n", EVT_KEY, param1, param2 );
				tempParam1 = TAP_SendEvent( i, EVT_KEY, param1, param2 );

				// Zero return value should mean don't pass the value on to other TAPs
				// The firmware actually does, so make sure param2 is also zero in this case
				if ( tempParam1 == 0 )
				{
					param1 = 0;
					param2 = 0;
				}
			}
		}

		// Check keypresses that originate from a programmable remote rather than TAP_GenerateEvent 
		// for potentially disk formatting Topfield test codes
		// RKEY_Cmd_6 jumps to the start of the timeshift buffer
		if ( param2 != 0 && param1 >= RKEY_Cmd_0 && param1 <= RKEY_Cmd_f && param1 != RKEY_Cmd_6 )
		{
			char buffer[200];
			sprintf( buffer, "Test key %X detected. It is advisable to avoid\n"
				"this keycode as it may cause disk corruption\n"
				"when RemoteExtender is not running." );
			ShowMessage( buffer, 500 );
			// We definately don't want the firmware to receive these keypresses!
			return 0;
		}

		return param1;
	}

	return param1;
}


typedef struct {
	Model	model;
    int		firmwareVersion;
	dword	registerGroup;
} FirmwareDetail;


FirmwareDetail firmware[] = 
{
	// Model		FW version,	registerGroup
	TF5800t,		0x1288,		0x804cb11c,		// 14 July 2006
	TF5800t,		0x1225,		0x80427bfc		// 08 Dec 2005
};


FirmwareDetail* GetFirmwareDetail()
{
	__asm__ __volatile__ (
		"lui	$02,0x0\n"
		"or		$02,$02,0x0\n"
		"nop\n"
		);
}

void FW_Print(const void *fmt, ...)
{
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

	v0 = *(dword*)GetFirmwareDetail()->registerGroup;
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
void TSRCommanderConfigDialog()
{
}


bool TSRCommanderExitTAP()
{
	UndoFirmwareHacks();
	TAP_Exit();
}


void ShowUnsupportedMessage()
{
	// Try and get some helpful information of users with unsupported firmware
	char buffer[300];
	sprintf( buffer, "Remote Extender is not compatible with your firmware\n"
		"For an update, please contact the author quoting\n"
		"%d and %04X", *sysID, _appl_version );
	ShowMessage( buffer, 750 );
}


int TAP_Main()
{
	bool supported = FALSE;
	int index;

	TAP_Print( "Starting Remote Extender TAP\n" );

#ifdef MHEG
	// Look up the current firmware
	for ( index=0; index<sizeof(firmware)/sizeof(FirmwareDetail); ++index )
	{
		if ( GetModel() == firmware[index].model && _appl_version == firmware[index].firmwareVersion )
		{
			supported = TRUE;
			break;
		}
	}
	if ( !supported )
	{
		ShowUnsupportedMessage();
		return 0;
	}
	((dword*)FW_Print)[0] = J(TAP_Print);
	((dword*)FW_Print)[1] = NOP_CMD;

	// We need to know various firmware specific addresses without needing to use $gp
	((word*)GetFirmwareDetail)[1] = ((dword)(firmware+index) >> 16) & 0xffff;
	((word*)GetFirmwareDetail)[3] = (dword)(firmware+index) & 0xffff;
#endif

	if ( !StartTAPExtensions() )
	{
		ShowUnsupportedMessage();
		return 0;
	}

	if ( !HookReceiveKeyFunction() )
	{
		UndoFirmwareHacks();
		ShowUnsupportedMessage();
		return 0;
	}

	if ( !AdjustDispatchKeyFunction() )
	{
		UndoFirmwareHacks();
		ShowUnsupportedMessage();
		return 0;
	}


	// patch TAP_GetState to return STATE_Ttx and SUBSTATE_Ttx when full screen MHEG is active
#ifdef MHEG
	{
		dword* fn = (dword*)GetState;
		dword* addr = (dword*)TAP_GetState;

		// save the original instructions to be executed prior to the hook code
		fn[0] = addr[0];
		fn[1] = addr[1];
		fn[2] = J(addr+2);
		fn[3] = NOP_CMD;
		HackFirmware( addr, J(GetStateHook) );
		HackFirmware( addr+1, NOP_CMD );
	}
#endif

#ifdef DIAGNOSTIC
	rgn = TAP_Osd_Create(0, 0, 720, 576, 0, FALSE); // Define a region that covers the whole screen
#endif

	return 1;
}
