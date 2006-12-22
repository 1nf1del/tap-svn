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
#include <vtable.h>
#include "dlmalloc.h"
#include "Tapplication.h"
#include "Logger.h"
#include "tapapifix.h"

void (*_cached_TAP_Exit)() = NULL;
extern "C" void dummy_TAP_Exit()
{

}

extern "C" void cpp_TAP_Exit()
{
	TRACE("Flagging for exit of TAP due to TAP_Exit call\n");
	Tapplication* tap = Tapplication::GetTheApplication();
	if (tap)
	{
		TAP_Exit = &dummy_TAP_Exit; // prevent any calls to TAP_Exit in the cleanup from calling us again
		tap->m_isClosing = true;
	}
}


extern "C" 
dword TAP_Main()
{
//	Logger::SetDestination( Logger::Screen | Logger::Serial);
	TRACE("Starting Main\n");

	// Call the auto generated vtable fixer
	FixupVTables();
    TRACE("Fixed VTables\n");
	_cached_TAP_Exit = TAP_Exit;
	TAP_Exit = &cpp_TAP_Exit;
	InitTAPAPIFix();
	return 1;
}

Tapplication* CreateTapApplication()
{
	if (Tapplication::CreateTheApplication() == 0)
	{
		// the application failed to create or is non-tsr - this will be handled 
		// in the else in eventhandler
		return NULL;
	}
	return Tapplication::GetTheApplication();
}

int exiting = 0;

extern "C"
dword TAP_EventHandler( word event, dword param1, dword param2 )
{
	if (exiting != 0)
		return param1;

	Tapplication* tap = Tapplication::GetTheApplication();
	if (tap == NULL)
	{
		// do the creation in the first event, not on TAP_Main
		tap = CreateTapApplication();
	}
	if (tap)
	{
		param1 = tap->EventHandler(event, param1, param2);
		// Check the global application object. If null then we're to exit the TAP
		if ( tap->IsClosing() )
		{
			Tapplication::DiscardTheApplication();
			Logger::DoneWithLogger();
			dlmalloc_exit();
			exiting = 1;
			_cached_TAP_Exit();
			return 0;
		}
	}
	else
	{
		TRACE("Unexpected or non-TSR! No Tapplication object - exiting\n");
		dlmalloc_exit();
		exiting = 1;
		_cached_TAP_Exit();
		return 0;
	}

	return param1;
}



