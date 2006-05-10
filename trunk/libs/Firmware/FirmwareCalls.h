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


#ifndef __FIRMWARECALLS_H
#define __FIRMWARECALLS_H

#ifdef __cplusplus
extern "C" {
#endif

// Check for Move file function availability. Returns true if the firmware supports file move.
bool TAP_Hdd_Move_Available();

// Move a file. Returns true if parameters are valid and the firmware supports file move
// Does *not* return false if the file fails to move. You must check manually with TAP_Hdd_Exist
bool TAP_Hdd_Move( char* fromDir, char* toDir, char* filename );

// Shut down the unit
void TAP_Shutdown();

// Reboot the unit
// force=true to unconditionally shut down, terminating recordings
void TAP_Reboot( bool force );

// Generate a keypress using a hardware keycode
void TAP_GenerateKeypress( dword rawkey );

#ifdef __cplusplus
}
#endif

#endif
