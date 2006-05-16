/*
	Copyright (C) 2005-2006 Darkmatter

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

//	This is the include file for RemoteKeys.c

#ifndef __UkRemoteKeys__
#define __UkRemoteKeys__

#define MAX_NUMBER_OF_KEYS 256

//----------
// external structure
//
typedef struct
{
	dword	keyCode;
	char 	*ButtonText;
} key_Struct;


typedef struct
{
	int		count;
	key_Struct	*keys[MAX_NUMBER_OF_KEYS];						// an array of pointers
} keyCodes_Struct;

typedef enum
{
	TF5800,
	TF5000
} TYPE_ModelType;

//----------
// external procedures
//
void CreateTF5800Keys( keyCodes_Struct* );
void CreateTF5000Keys( keyCodes_Struct* );


char *GetButtonText( keyCodes_Struct*, dword );
int GetNumberOfKeys( keyCodes_Struct* );

dword GetNextKey( keyCodes_Struct*, dword );
dword GetPreviousKey( keyCodes_Struct*, dword );

bool IsKeyValid( keyCodes_Struct*, dword );


void KeyDestroy( keyCodes_Struct* );
#endif
