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
#include <osd.h>
#include <Tapplication.h>
#include <TAPExtensions.h>
#include <morekeys.h>
#include "Options.h"
#include "SkinOption.h"
#include "YesNoOption.h"
#include "LoadedTAPPage.h"
#include "AutoStartPage.h"


class TAPCommander : public Tapplication
{
public:
	TAPCommander()
	{
		m_showStartPage = false;
		m_exitActivates = true;
	}

	~TAPCommander()
	{
		for ( int i = 0; i < TAP_MAX; ++i )
			EnableTAP( i, true );
	}

	virtual bool Start();
	virtual void OnIdle();
	virtual dword OnKey( dword key, dword extKey );
	virtual bool OnConfigure();
	virtual const char* GetIniName() const;

private:
	bool m_showStartPage;
	bool m_exitActivates;
};


const char* TAPCommander::GetIniName() const
{
	return "TAPCommander.ini";
}


bool TAPCommander::Start()
{
	// Initialise the firmware hack
	if ( !StartTAPExtensions() )
		return false;

	m_options->Add(new YesNoOption(m_options, "ExitActivates", true, "Exit Key Activates TAP Commander", "Allow Exit as an additional activation key. TAP Commander can always be activated with Menu+Menu", new OptionUpdateValueNotifier_bool(m_exitActivates)));

	Tapplication::Start();

	// if started manually then immediately show the running TAPs page
	// must be done in the idle loop or ExitNormal will not work
	if (TAP_GetTick() > 3000) // 30 seconds since startup
		m_showStartPage = true;

	return true;
}


void TAPCommander::OnIdle()
{
	if ( m_showStartPage )
	{
		m_showStartPage = false;
		LoadedTAPPage* p = new LoadedTAPPage;
		if (p)
			p->Open();
	}

	Tapplication::OnIdle();
}


dword TAPCommander::OnKey( dword key, dword extKey )
{
	if ( Tapplication::OnKey( key, extKey ) == 0 )
		return 0;

	if ( pageCount == 0 )
	{
		dword state, subState;
		TAP_GetState( &state, &subState );
		if ( (state == STATE_Menu && subState == SUBSTATE_MainMenu && key == RKEY_Menu) ||
			 (state == STATE_Normal && subState == SUBSTATE_Normal && m_exitActivates && key == RKEY_Exit && !OsdActive() ))
		{
			LoadedTAPPage* p = new LoadedTAPPage;
			if (p)
                p->Open();
			return 0;
		}
	}

#ifdef DEBUG
	if ( key == RKEY_Power )
	{
		TAP_Print("Exiting TAP Commander\n");
		return Close();
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
