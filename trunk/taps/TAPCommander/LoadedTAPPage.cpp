/*
	Copyright (C) 2006 Simon Capewell
	Portions derived from original TSRCommander code by Judy

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
#include <string.h>
#include <tapmap.h>
#include <TAPExtensions.h>
#include <Logger.h>
#include <ProgressBox.h>
#include "LoadedTAPPage.h"
#include "AutoStartPage.h"
#include "Tapplication.h"
#include "ListColumn.h"
#include "FooterListItem.h"

typedef struct
{
	char	landingzone[256];
	char	signature[20];
	char	*TAPName;
	bool	ExitTAP;
	bool	HasConfigDialog;
	bool	ShowConfigDialog;
	dword	FutureExtensions[64];
} TYPE_TSRCommander;


// array of event handler pointer of disabled TAPs
static TAP_EventHandlerFn disabledTAP[TAP_MAX];

// Generic handler substituted for other TAPs when they're disabled
// the real event handler address is stored in disabledTAP
dword DisabledTAP_EventHandler(word event, dword param1, dword param2 )
{
	// Do not be tempted do anything in this function.
	// accessing globals or string literals will cause the Toppy to crash
	// due to $gp having the wrong value for TAPCommander
	return param1;
}


// Set the enabled state of the TAP at index
void EnableTAP( int index, bool enable )
{
	if ( tapProcess[index].entryPoints )
	{
		if ( enable && disabledTAP[index] )
		{
			tapProcess[index].entryPoints->TAP_EventHandler = disabledTAP[index];
			disabledTAP[index] = NULL;
		}
		else if ( !enable && disabledTAP[index] == NULL )
		{
			disabledTAP[index] = tapProcess[index].entryPoints->TAP_EventHandler;
			tapProcess[index].entryPoints->TAP_EventHandler = DisabledTAP_EventHandler;
		}
	}
}


// Return true if the TAP at index is enabled
bool IsEnabled( int index )
{
	return disabledTAP[index] == NULL;
}


// Return true if the TAP at index it this TAP
bool IsThisTAP( int index )
{
	TAPProcess* process = &tapProcess[index];
	return process->header && process->header->id == __tap_ud__;
}


// Return true if the TAP at index is TSR Commander compatible
TYPE_TSRCommander* IsTSRCommanderTAP( int index )
{
	TAPProcess* tap = tapProcess+index;
	if ( tap->entryPoints && tap->header )
	{
		for ( char* any = (char*)tap->header; any <= (char*)0x83FFFF00; )
		{
			if ( *any == 'J' )
			{
				for ( int i = 1 ; i < 258 ; i++ )
				{
					if ( *(any + i) != 'J' )
					{
						any = any + i - 1;
						break;
					}
				}
				// split the signature string or the original TSRCommander crashes when this TAP is loaded!
				if ( strncmp( any, "Judys_", 6 ) == 0 && strncmp( any+6, "Signature_X70E", 12 ) == 0 )
				{
					// success, found
					return (TYPE_TSRCommander*)(any-256);
				}
			}

			any += 256;
			// Exit loop if we've hit the next TAP
			if ( strncmp( any, "TFAPMIPS", 8 ) == 0 )
			{
				break;
			}
		}
	}
	return NULL;
}


// List item  specific list item for setting up skip distance
class TAPListItem :
	public SimpleTextListItem
{
public:
	TAPListItem(ListPage* pParentList, int index, dword dwFlags, const char* pszCol1) :
		SimpleTextListItem(pParentList, dwFlags, pszCol1),
		m_index(index)
	{
		Update();
	}
	virtual dword OnKey( dword key, dword extKey );
	virtual string GetFooterText();

private:
	void Update()
	{
		TAPProcess* p = &tapProcess[m_index];
		if ( p->entryPoints && p->header )
		{
			m_asItemText[1] = p->header->name;
			m_asItemText[2] = IsEnabled(m_index) ? "enabled" : "disabled";
		}
		else
		{
			m_asItemText[1] = "(empty)";
			m_asItemText[2] = "";
		}
	}

	int m_index;
};


dword TAPListItem::OnKey( dword key, dword extKey )
{
	switch ( key )
	{
	case RKEY_Ok:
	{
		if ( IsThisTAP( m_index ) )
		{
			// Configure
		}
		else
		{
			// Check whether this is a TSR Commander TAP with a config option
			TYPE_TSRCommander* tapBlock = IsTSRCommanderTAP( m_index );
			if ( tapBlock && tapBlock->HasConfigDialog )
			{
				m_theList->Close();
				tapBlock->ShowConfigDialog = TRUE;
			}
		}
		return 0;
	}
	case RKEY_NewF1:
		// we don't want to disable ourselves
		if ( !IsThisTAP( m_index ) )
		{
			EnableTAP( m_index, !IsEnabled( m_index ) );
			Update();
		}
		return 0;

	case RKEY_Stop:
	case RKEY_Ab:
	{
		// special case for this TAP so we don't need to add the TSRCommander stuff
		if ( IsThisTAP( m_index ) )
		{
			Tapplication::GetTheApplication()->Close();
			return 0;
		}
		TYPE_TSRCommander* tapBlock = IsTSRCommanderTAP( m_index );
		if ( tapBlock )
		{
			m_theList->Close();
			tapBlock->ExitTAP = TRUE;
		}
		return 0;
	}
	}

	return key;
}


string TAPListItem::GetFooterText()
{
	TAPProcess* p = &tapProcess[m_index];
	if ( p->header )
	{
		string s;
		s.format("%s\n%s\n%s\n", p->header->description, p->header->authorName, p->header->etcStr );
		// we don't want to disable ourselves
		if ( !IsThisTAP( m_index ) )
		{
			s += IsEnabled( m_index ) ? "Red = disable" : "Red = enable";
		}
		else
		{
			s += "White = Exit TAP";
		}

		TYPE_TSRCommander* tapBlock = IsTSRCommanderTAP( m_index );
		if ( tapBlock )
		{
			if ( tapBlock->HasConfigDialog )
				s += ", OK = Configure";
			s += ", White = Exit TAP";
		}
		return s;
	}

	return "";
}



LoadedTAPPage::LoadedTAPPage() :
	ListPage( LF_SHOW_HEADER | LF_ROW_SEPARATORS | LF_SCROLLBAR | LF_COLUMN_SEPARATORS | LF_SHOW_FOOTER | LF_COLUMNS_IN_HEADER,
			  Rect(50,50,620,465), 32, 45, 100 )
{
    SetFontSizes(FNT_Size_1926, FNT_Size_1622, FNT_Size_1622);

    AddColumn(new ListColumn(this, 3, 0, ""));
    AddColumn(new ListColumn(this, 45, 0, "Running TAPs"));
	dword size, free, available;
	TAP_MemInfo( &size, &free, &available );
	available /= 104858; // approximately 1024*1024/10 - no floating point available
	char buffer[20];
	sprintf(buffer, "%d.%dMB", available/10, available % 10);
    AddColumn(new ListColumn(this, 7, 0, buffer));

	TAPProcess* p = tapProcess;
	int count = 1;
	for ( int i = 0; i < TAP_MAX; ++p, ++i )
	{
		TAPProcess* p = &tapProcess[i];
		if ( p->entryPoints && p->header )
		{
			char str[15];
			sprintf(str, "%d.\n", count );
			AddItem(new TAPListItem(this, i, 0, str ));
			++count;
		}
	}

	AddItem(new FooterActionListItem(this, &ExitTapKeyPress, "Press OK to shut down TAP Commander", 0, "", "Exit TAP Commander"));
}


LoadedTAPPage::~LoadedTAPPage()
{
}


dword LoadedTAPPage::OnKey( dword key, dword extKey )
{
	switch ( key )
	{
	case RKEY_Exit:
		Close();
		return 0;
	case RKEY_Mute:
		return key;
	case RKEY_PlayList:
		if ( AutoStartPage::IsAvailable() )
		{
			AutoStartPage* p = new AutoStartPage();
			p->Open();
		}
		return 0;
	}
	ListPage::OnKey( key, extKey );

	return 0;
}


dword LoadedTAPPage::ExitTapKeyPress(ListPage* page, ListItem* item, dword key, dword extKey)
{
	if ( key == RKEY_Ok )
	{
		Tapplication::GetTheApplication()->Close();
		return 0;
	}
	return key;
}
