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
#include "Tapplication.h"
#include "Logger.h"

extern "C" 
dword TAP_Main()
{
//	Logger::SetDestination( Logger::Screen | Logger::Serial);
	TRACE("Starting Main\n");

	// Call the auto generated vtable fixer
	FixupVTables();
    TRACE("Fixed VTables\n");

	return Tapplication::CreateTheApplication();
}


extern "C"
dword TAP_EventHandler( word event, dword param1, dword param2 )
{
	if (Tapplication::GetTheApplication())
		return Tapplication::GetTheApplication()->EventHandler(event, param1, param2);

	return param1;
}
