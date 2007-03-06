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
#include <FirmwareCalls.h>
#include <TAPExtensions.h>
#include <Logger.h>
#include <ProgressBox.h>
#include "LoadedTAPPage.h"
#include "AutoStartPage.h"
#include "Tapplication.h"
#include "ListColumn.h"
#include "FooterListItem.h"
#include "MessageBox.h"
#include "ConfigPage.h"

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


//-----------------------------------------------------------------------------
// array of event handler pointer of disabled TAPs
static TAP_EventHandlerFn disabledTAP[TAP_MAX];

// Generic handler substituted for other TAPs when they're disabled
// the real event handler address is stored in disabledTAP
static dword DisabledTAP_EventHandler(word event, dword param1, dword param2 )
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
static bool IsEnabled( unsigned int index )
{
	return disabledTAP[index] == NULL;
}


// Return true if the TAP at index it this TAP
static bool IsThisTAP( unsigned int index )
{
	TAPProcess* process = &tapProcess[index];
	return process->header && process->header->id == __tap_ud__;
}


static bool CanDisable( unsigned int index )
{
	if ( IsThisTAP( index ) )
		return false;
	TAPProcess* process = &tapProcess[index];
	if ( !process->header )
		return false;

	// don't disable Virtual Remote
	if (process->header->id == 0x810a0013)
		return false;

	return process->header->id != 0x814243a3 ; // Remote Extender mustn't be disabled
}


// Return true if the TAP at index is TSR Commander compatible
static TYPE_TSRCommander* IsTSRCommanderTAP( unsigned int index )
{
#ifdef WIN32
	return NULL;
#endif

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


//-----------------------------------------------------------------------------
dword LoadedTAPPage::TAPListItem::OnKey( dword key, dword extKey )
{
	switch ( key )
	{
	case RKEY_Ok:
	{
		if ( IsThisTAP( m_index ) )
		{
			Page* p = new ConfigPage();
			if ( p )
				p->Open();
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
		return 1;
	}
	case RKEY_NewF1:
		// we don't want to disable ourselves
		if ( CanDisable( m_index ) )
		{
			EnableTAP( m_index, !IsEnabled( m_index ) );
			m_theList->RedrawItem( m_index );
		}
		return 1;

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
		return 1;
	}
	case RKEY_Sat:
		if ( MessageBox::Show("Stop all TAPs?", "", "Yes\nNo" ) == 1 )
		{
			for ( int i = 0; i < TAP_MAX; ++i )
			{
				if ( !IsThisTAP( i ) && CanDisable( i ) )
				{
					TYPE_TSRCommander* tapBlock = IsTSRCommanderTAP( i );
					if ( tapBlock )
						tapBlock->ExitTAP = TRUE;
				}
			}
			m_theList->Close();
		}
		return 1;
	}

	return key;
}


void LoadedTAPPage::TAPListItem::DrawSubItem(short int iColumn, Rect rcBounds)
{
	TAPProcess* p = &tapProcess[m_index];
	char buffer[15];
	const char* text = 0;
	switch (iColumn)
	{
	case 0:
	{
		sprintf( buffer, "%d.", m_index+1 );
		text = buffer;
		break;
	}
	case 1:
		text = p->entryPoints && p->header ? p->header->name : "(empty)";
		break;
	case 2:
		text = p->entryPoints && p->header ? (IsEnabled(m_index) ? "enabled" : "disabled") : "";
		break;
	}
	if (text)
		DrawSubItemString(iColumn, rcBounds, text);
}


string LoadedTAPPage::TAPListItem::GetFooterText()
{
	TAPProcess* p = &tapProcess[m_index];
	if ( p->header )
	{
		string s;
		s.format("%s\n%s\n%s\nArchive=Reorder", p->header->description, p->header->authorName, p->header->etcStr );
		// we don't want to disable ourselves
		if ( CanDisable( m_index ) )
		{
			s += IsEnabled( m_index ) ? " | Red (Green)=Disable (all)" : " | Red (Green)=Enable (all)";
		}

		TYPE_TSRCommander* tapBlock = IsTSRCommanderTAP( m_index );
		if ( tapBlock )
		{
			if ( tapBlock->HasConfigDialog )
				s += " | OK=Options";
			s += " | White=Exit TAP";
		}
		return s;
	}

	return "";
}


//-----------------------------------------------------------------------------
// LoadedTAPPage
LoadedTAPPage::LoadedTAPPage() :
	ListPage( LF_SHOW_HEADER | LF_ROW_SEPARATORS | LF_SCROLLBAR | LF_COLUMN_SEPARATORS | LF_SHOW_FOOTER | LF_COLUMNS_IN_HEADER,
			  Rect(50,50,620,465), 32, 45, 100 )
{
    SetFontSizes(FNT_Size_1926, FNT_Size_1622, FNT_Size_1622);

    AddColumn(new ListColumn(this, 3));
    AddColumn(new ListColumn(this, 45, 0, "Running TAPs"));
	dword size, free, available;
	TAP_MemInfo( &size, &free, &available );
	available /= 104858; // approximately 1024*1024/10 - no floating point available
	char buffer[20];
	sprintf(buffer, "%d.%dMB", available/10, available % 10);
    AddColumn(new ListColumn(this, 7, 0, buffer));

	TAPProcess* p = tapProcess;
	for ( int i = 0; i < TAP_MAX; ++p, ++i )
	{
		if ( p->entryPoints && p->header )
			AddItem(new TAPListItem(this, i));
	}
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
		// pass Mute to the firmware
		return key;
	case RKEY_Menu:
	{
		Page* p = new ConfigPage();
		if (p)
			p->Open();
		return 0;
	}
	case RKEY_PlayList:
		// If the TAP_Hdd_Move isn't found then reordering on disk is not available
		if ( TAP_Hdd_Move_Available() )
			Replace(new AutoStartPage());
		return 0;
	case RKEY_F2:
	{
		unsigned int index = GetSelectedIndex();
		bool enable = !IsEnabled( index );
		// Toggle all
		for (unsigned int i = 0; i < m_items.size() && i < TAP_MAX; ++i )
		{
			// we don't want to disable ourselves
			if ( CanDisable( i ) )
			{
				EnableTAP( i, enable );
				RedrawItem( i );
			}
		}
		return 0;
	}
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
