/*
	Copyright (C) 2006 Simon Capewell

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

#include <stdlib.h>
#include <tap.h>
#include <Tapplication.h>
#include <TAPExtensions.h>
#include "LoadedTAPPage.h"
#include "AutoStartPage.h"


class TAPCommander : public Tapplication
{
public:
	TAPCommander()
	{
	}

	~TAPCommander()
	{
		for ( int i = 0; i < TAP_MAX; ++i )
			EnableTAP( i, true );
	}

	virtual bool Start();
	virtual dword OnKey( dword key, dword extKey );
	virtual bool OnConfigure();
};


bool TAPCommander::Start()
{
	// Initialise the firmware hack
	if ( !StartTAPExtensions() )
	{
		return false;
	}

	Tapplication::Start();

	return true;
}


dword TAPCommander::OnKey( dword key, dword extKey )
{
	if ( Tapplication::OnKey( key, extKey ) == 0 )
		return 0;

	if ( pageCount == 0 )
	{
		dword state, subState;
		TAP_GetState( &state, &subState );
		if ( state == STATE_Menu && subState == SUBSTATE_MainMenu && key == RKEY_Menu )
		{
			LoadedTAPPage* p = new LoadedTAPPage();
			p->Open();
			return 0;
		}
	}

#ifdef DEBUG
	if ( key == RKEY_Power )
	{
	//	return Close();
	}
#endif

	return key;
}


bool TAPCommander::OnConfigure()
{
    TAP_Print("Configure");
	return true;
}


DEFINE_APPLICATION(TAPCommander)
