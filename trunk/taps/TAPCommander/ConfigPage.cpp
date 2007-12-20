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

#include <stdlib.h>
#include <stdio.h>
#include <tap.h>
#include <libFireBird.h>
#include "AutoStartPage.h"
#include "Tapplication.h"
#include "Options.h"
#include "FooterListItem.h"
#include "ConfigPage.h"
#include "ListColumn.h"
#include "OptionListItem.h"
#include "MessageBox.h"


ConfigPage::ConfigPage() :
	ListPage( LF_SHOW_HEADER | LF_SHOW_FOOTER | LF_ROW_SEPARATORS | LF_SCROLLBAR | LF_COLUMN_SEPARATORS,
			 Rect(50,50,620,465), 32, 45, 100)
{
	SetFontSizes(FNT_Size_1926, FNT_Size_1622, FNT_Size_1622);
	AddColumn(new ListColumn(this, 70, LCF_CENTER_HEADER_TEXT, "TAP Commander Options"));
	AddColumn(new ListColumn(this, 40, LCF_CENTER_TEXT));

	const array<Option*>& options = Tapplication::GetTheApplication()->GetOptions()->GetAllOptions();
	for (unsigned int i=0; i<options.size(); i++)
		AddItem(new OptionListItem(this, options[i]));

	AddItem(new FooterActionListItem(this, &RebootKeyPress, "Press OK to reboot", 0, "Reboot"));
#ifdef DEBUG
	AddItem(new FooterActionListItem(this, &ShutdownKeyPress, "Press OK to shutdown", 0, "Shutdown"));
#endif
	AddItem(new FooterActionListItem(this, &ExitTapKeyPress, "Press OK to shut down TAP Commander", 0, "Exit TAP Commander"));
}


ConfigPage::~ConfigPage()
{
	DiscardItems(); // forces the optionlistitems to write their values to the ini file
	// and write out the inifile
	Options* options = Tapplication::GetTheApplication()->GetOptions();
	if (options)
		options->SaveValues();
}


dword ConfigPage::OnKey( dword key, dword extKey )
{
	switch ( key )
	{
	// Pass Mute to the firmware
	case RKEY_Mute:
		return key;
	// Save Changes
	case RKEY_Record:
		Close();
		return 0;
	}
	ListPage::OnKey( key, extKey );

	return 0;
}


dword ConfigPage::ExitTapKeyPress(ListPage* page, ListItem* item, dword key, dword extKey)
{
	if ( key == RKEY_Ok )
	{
		Tapplication::GetTheApplication()->Close();
		return 0;
	}
	return key;
}


dword ConfigPage::RebootKeyPress(ListPage* page, ListItem* item, dword key, dword extKey)
{
	if ( key == RKEY_Ok )
	{
		// Only allow reboot if no recordings are in progress
		if (!HDD_isAnyRecording())
			Reboot(false);
		else
			MessageBox::Show( "TAP Commander", "Recordings are in progress", "OK" );
		return 0;
	}
	return key;
}

#ifdef DEBUG
dword ConfigPage::ShutdownKeyPress(ListPage* page, ListItem* item, dword key, dword extKey)
{
	if ( key == RKEY_Ok )
	{
		// Only allow reboot if no recordings are in progress
		if (!HDD_isAnyRecording())
			Shutdown(TaskPower);
		else
			MessageBox::Show( "TAP Commander", "Recordings are in progress", "OK" );
		return 0;
	}
	return key;
}
#endif
