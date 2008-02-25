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
#include <libFireBird.h>
#include <morekeys.h>
#include "Options.h"
#include "SkinOption.h"
#include "YesNoOption.h"
#include "LoadedTAPPage.h"
#include "AutoStartPage.h"
#include "TAPLists.h"


bool closeOnClose = true;
bool menuActivates = false;
bool showMemoryUse = false;
bool exitActivates = true;


class TAPCommander : public Tapplication
{
public:
	TAPCommander()
	{
		m_showStartPage = false;
	}

	~TAPCommander()
	{
		for ( int i = 0; i < TAP_MAX; ++i )
			HDD_TAP_Disable( HDD_TAP_GetIDByIndex(i), false );
	}

	virtual bool Start();
	virtual void OnIdle();
	virtual dword OnKey( dword key, dword extKey );
	virtual bool OnConfigure();
	virtual const char* GetIniName() const;

private:
	bool m_showStartPage;
};


const char* TAPCommander::GetIniName() const
{
	return "TAPCommander.ini";
}


bool TAPCommander::Start()
{
	// Initialise the firmware hack
	if ( !InitTAPex() )
		return false;

	if (InitTAPMonitor())
		m_options->Add(new YesNoOption(m_options, "ShowMemoryUse", true, "Show memory use in Running TAPs screen", "Shows the total memory used by each TAP", new OptionUpdateValueNotifier_bool(showMemoryUse)));
	else
		showMemoryUse = false;
	m_options->Add(new YesNoOption(m_options, "CloseOnTAPClose", true, "Auto close Running TAPs screen", "Closes the Running TAPs screen when closing another TAP", new OptionUpdateValueNotifier_bool(closeOnClose)));
	m_options->Add(new YesNoOption(m_options, "MenuActivates", false, "Menu key activates TAP Commander first", "Allow Menu to activate TAP Commander before the Topfield Main Menu", new OptionUpdateValueNotifier_bool(menuActivates)));
	m_options->Add(new YesNoOption(m_options, "ExitActivates", true, "Exit key activates TAP Commander", "Allow Exit as an additional activation key. TAP Commander can always be activated with Menu+Menu", new OptionUpdateValueNotifier_bool(exitActivates)));

	if (!Tapplication::Start())
		return false;

	// if TAP not launched from Auto Start then immediately show the running TAPs page
	// must be done in the idle loop or ExitNormal will not work
	if (startupDirectory != "/ProgramFiles/Auto Start")
		m_showStartPage = true;

	return true;
}


void TAPCommander::OnIdle()
{
	if ( m_showStartPage )
	{
		m_showStartPage = false;
		Page* p = new LoadedTAPPage();
		if (p)
			p->Open();
	}

	Tapplication::OnIdle();
}


dword TAPCommander::OnKey( dword key, dword extKey )
{
	if ( pageCount == 0 )
	{
		dword state, subState;

		if ( key == RKEY_Menu )
		{
			TAP_GetState( &state, &subState );
			// Allow the firmware to receive the keypress so it hides its menu.
			if ( state == STATE_Menu && subState == SUBSTATE_MainMenu )
				m_showStartPage = true;
		}
		if ( (key == RKEY_Exit && exitActivates) || (key == RKEY_Menu && menuActivates) )
		{
			TAP_GetState( &state, &subState );
			if ( state == STATE_Normal && subState == SUBSTATE_Normal && !OsdActive() )
			{
				m_showStartPage = true;
				return 0;
			}
		}
	}

#ifdef DEBUG
	if ( key == RKEY_Power )
	{
		TAP_Print("Exiting TAP Commander\n");
		return Close();
	}
#endif

	if ( Tapplication::OnKey( key, extKey ) == 0 )
		return 0;

	return key;
}


bool TAPCommander::OnConfigure()
{
    TAP_Print("Configure");
	return true;
}


DEFINE_APPLICATION(TAPCommander)
