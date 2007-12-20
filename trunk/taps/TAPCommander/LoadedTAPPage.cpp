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
#include <libFireBird.h>
#include <Logger.h>
#include <ProgressBox.h>
#include "LoadedTAPPage.h"
#include "AutoStartPage.h"
#include "Tapplication.h"
#include "ListColumn.h"
#include "FooterListItem.h"
#include "MessageBox.h"
#include "ConfigPage.h"


extern bool menuActivates;
extern bool closeOnClose;


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


// Set the enabled state of the TAP at index
static void EnableTAP( int index, bool enable )
{
	HDD_TAP_Disable( HDD_TAP_GetIDByIndex(index), !enable );
}


// Return true if the TAP at index is enabled
static bool IsEnabled( unsigned int index )
{
	return !HDD_TAP_isDisabled( HDD_TAP_GetIDByIndex(index) );
}


// Return true if the TAP at index it this TAP
static bool IsThisTAP( unsigned int index )
{
	return HDD_TAP_GetIDByIndex( index ) == __tap_ud__;
}


static bool CanDisable( unsigned int index )
{
	if ( IsThisTAP( index ) )
		return false;

	dword id = HDD_TAP_GetIDByIndex( index );

	// don't disable Virtual Remote
	if (id == 0x810a0013)
		return false;

	return id != 0x814243a3 ; // Remote Extender mustn't be disabled
}


static bool CanStop( unsigned int index )
{
	dword id = HDD_TAP_GetIDByIndex( index );

	// don't stop Virtual Remote
	if (id == 0x810a0013)
		return false;

	return true;
}


// Return true if the TAP at index is TSR Commander compatible
static TYPE_TSRCommander* IsTSRCommanderTAP( unsigned int index )
{
#ifdef WIN32
	return NULL;
#endif

	tTAPTable* tap = ((tTAPTable*)TAP_TableAddress)+index;
	if ( tap->EntryPoints && tap->TAPHeader )
	{
		for ( char* any = (char*)tap->TAPHeader; any <= (char*)0x83FFFF00; )
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
void LoadedTAPPage::TAPListItem::EnableTAP( bool enable )
{
	if ( CanDisable( m_index ) )
	{
		::EnableTAP( m_index, enable );
		m_theList->RedrawItem(this);
	}
}


bool LoadedTAPPage::TAPListItem::IsEmpty()
{
	tTAPTableInfo info;
	HDD_TAP_GetInfo( m_index, &info );
	return info.LoadAddress == 0;
}


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
		EnableTAP( !IsEnabled( m_index ) );
		return 1;

	case RKEY_F2:
	{
		bool enable = !IsEnabled( m_index );
		// Toggle all
		for ( int i = 0; i < TAP_MAX; ++i )
		{
			if ( CanDisable( i ) )
				::EnableTAP( i, enable );
		}

		// Page redraws when returning 0
		return 0;
	}

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
			tapBlock->ExitTAP = TRUE;
			if ( closeOnClose )
				m_theList->Close();
			else
			{
				m_closing = true;
				m_theList->RedrawItem(this);
			}
		}
		return 1;
	}
	case RKEY_Sat:
		if ( MessageBox::Show("Stop all TAPs?", "", "Yes\nNo" ) == 1 )
		{
			for ( int i = 0; i < TAP_MAX; ++i )
			{
				if ( !IsThisTAP( i ) && CanStop( i ) )
				{
					TYPE_TSRCommander* tapBlock = IsTSRCommanderTAP( i );
					if ( tapBlock )
						tapBlock->ExitTAP = TRUE;
				}
			}
		}
		return 1;
	}

	return key;
}


void LoadedTAPPage::TAPListItem::DrawSubItem(short int iColumn, Rect rcBounds)
{
	tTAPTableInfo info;
	char buffer[15];
	const char* text = 0;

	HDD_TAP_GetInfo( m_index, &info );
	switch (iColumn)
	{
	case 0:
	{
		sprintf( buffer, "%d.", m_index+1 );
		text = buffer;
		break;
	}
	case 1:
		text = info.LoadAddress ? info.TAPName : "";
		break;
	case 2:
		if (m_closing)
			text = "closing";
		else if (info.LoadAddress)
			text = IsEnabled(m_index) ? "enabled" : "disabled";
		else
			text = "";
		break;
	}
	if (text)
		DrawSubItemString(iColumn, rcBounds, text);
}


string LoadedTAPPage::TAPListItem::GetFooterText()
{
	tTAPTable* tap = ((tTAPTable*)TAP_TableAddress)+m_index;
	if ( tap->LoadAddress )
	{
		tTAPHeader* header = tap->TAPHeader;
		string s;
		s.format("%s\n%s\n%s\nArchive=Reorder", header->Description, header->AuthorName, header->EtcStr );
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

	tTAPTable* p = (tTAPTable*)TAP_TableAddress;
	for ( int i = 0; i < TAP_MAX; ++p, ++i )
	{
		if ( p->LoadAddress && p->TAPHeader )
			AddItem(new TAPListItem(this, i));
	}
}


LoadedTAPPage::~LoadedTAPPage()
{
}


void LoadedTAPPage::OnIdle()
{
	// Remove any unloaded TAPs from the list
	bool erased = false;

	for (unsigned int i = 0; i < m_items.size() && i < TAP_MAX; )
	{
		TAPListItem* item = (TAPListItem*)m_items[i];
		if ( item->IsEmpty() )
		{
			RemoveItem(item);
			erased = true;
		}
		else
			++i;
	}
	if (erased)
	{
		// Some TAPs EnterNormal when they close. This is bad as TAP Commander loses key focus
		TAP_ExitNormal();
		Redraw();
	}
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
		if (menuActivates)
		{
			Close();
			return key;
		}
		Page* p = new ConfigPage();
		if (p)
			p->Open();
		return 0;
	}
	case RKEY_PlayList:
		Replace(new AutoStartPage());
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
