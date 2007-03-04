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
#include <messagewin.h>
#include <Firmware.h>
#include <model.h>
#include <OpCodes.h>
#include "QuickAspectBlocker.h"
#include "TAPExtensions.h"
#include "Settings.h"


//-----------------------------------------------------------------------------
bool QuickAspectBlocker_Available = FALSE;
static dword aspectSwitchAddr = 0;


static dword QuickAspectBlockerSignature[] =
{
	0x0cFFFFFF,
	0x00000000,
	0x100002FF,
	0x8399FFFF,
	0x3c19ffff,
	0x0c05FFFF,
	0x02d92021,
	0x100002FF,
	0x8399FFFF,
	0x0c05FFFF,
	0x00000000,
	0x00021600,
	0x00021603,
	0x544002FF,
	0x8399FFFF,
	0x3c1eFFFF,
	0x93deFFFF,
	0x24180002
};


//-----------------------------------------------------------------------------
int QuickAspectBlocker_Init()
{
	// Look event table details up from the TAP_GetCurrentEvent code
	aspectSwitchAddr = FindFirmwareFunction( QuickAspectBlockerSignature, sizeof(QuickAspectBlockerSignature), 0x80140000, 0x80200000 );
	QuickAspectBlocker_Available = aspectSwitchAddr != 0;

	return QuickAspectBlocker_Available;
}


void QuickAspectBlocker_Enable()
{
	if ( !QuickAspectBlocker_Available || aspectSwitchAddr <= 0x80000000 )
		return;

	HackFirmware( (dword*)aspectSwitchAddr, NOP_CMD );
}
