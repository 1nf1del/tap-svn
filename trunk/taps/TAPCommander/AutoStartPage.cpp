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
#include <stdio.h>
#include <tap.h>
#include <string.h>
#include <tapmap.h>
#include <TAPExtensions.h>
#include <Logger.h>
#include <ProgressBox.h>
#include "AutoStartPage.h"
#include "Tapplication.h"
#include "ListColumn.h"
#include "FooterListItem.h"


typedef dword  (*TAP_Hdd_MoveFn)(char *from_dir, char *to_dir, char *filename);
TAP_Hdd_MoveFn  TAP_Hdd_Move; 

enum
{
    oTAP_Hdd_unknown0          = 0x00,  //hdd related, called by debug functions 'rs', 'rc'     -- read sector?
    oTAP_Hdd_unknown1          = 0x01,  //hdd related, called by debug functions 'ws', 'wc'     -- write sector?
    oTAP_unknown2              = 0x02,  //writes to eeprom? @ 0xa3ffffe0, 0xa3ffffe1
    oTAP_Hdd_unknown3          = 0x03,  //ata/dma related, ???
    oTAP_Hdd_SetBookmark       = 0x04,
    oTAP_Hdd_GotoBookmark      = 0x05,
    oTAP_Hdd_ChangePlaybackPos = 0x06,
    oTAP_ControlEit            = 0x07,
    oTAP_SetBk                 = 0x08,
    oTAP_EPG_UpdateEvent       = 0x09,
    oTAP_EPG_DeleteEvent       = 0x0a,
    oTAP_EPG_GetExtInfo        = 0x0b,
    oTAP_Channel_IsStarted     = 0x0c,
    oTAP_Vfd_GetStatus         = 0x0d,
    oTAP_Vfd_Control           = 0x0e,
    oTAP_Vfd_SendData          = 0x0f,
    oTAP_Win_SetAvtice         = 0x10,
    oTAP_Win_SetDrawItemFunc   = 0x11,
    oTAP_SysOsdControl         = 0x12,
    oTAP_Hdd_Move              = 0x13,
    oTAP_Osd_unknown20         = 0x14,  //osd related, ???
};

extern void* (*TAP_GetSystemProc)( int );


// If the TAP_Hdd_Move isn't found then reordering on disk is not available
bool AutoStartPage::IsAvailable()
{
    TAP_Hdd_Move = (TAP_Hdd_MoveFn)TAP_GetSystemProc(oTAP_Hdd_Move);
	if ( TAP_Hdd_Move == NULL )
	    TAP_Hdd_Move = (TAP_Hdd_MoveFn)TAP_GetSystemProc(oTAP_Hdd_Move);

	return TAP_Hdd_Move != NULL;
}


AutoStartPage::AutoStartPage() :
	ListPage( LF_SHOW_HEADER | LF_ROW_SEPARATORS | LF_SCROLLBAR | LF_COLUMN_SEPARATORS | LF_SHOW_FOOTER | LF_COLUMNS_IN_HEADER,
			  Rect(50,50,620,465), 32, 45, 100 )
{
    SetFontSizes(FNT_Size_1926, FNT_Size_1622, FNT_Size_1622);

    AddColumn(new ListColumn(this, 3, 0, ""));
    AddColumn(new ListColumn(this, 45, 0, "Auto Start TAPs"));
    AddColumn(new ListColumn(this, 7, 0, ""));

}


AutoStartPage::~AutoStartPage()
{
}


void AutoStartPage::OnOpen()
{
	ListPage::OnOpen();

	// Get a list of TAPs in the current Auto Start
	TAP_Hdd_ChangeDir("Auto Start");
	TYPE_File file;
	dword totalEntry = TAP_Hdd_FindFirst( &file );
	int index = 0;
	while ( totalEntry-- )
	{
		if ( file.attr == ATTR_NORMAL )
		{
			int len = strlen(file.name);
			// ensure we're only looking at .tap files
			if ( len > 4 && stricmp( file.name+len-4, ".tap" ) == 0 )
			{
				// that we can open
				TYPE_File* f = TAP_Hdd_Fopen( file.name );
				if ( f )
				{
					// read the TAP header
					TAPHeader header;
					memset( &header, 0, sizeof(header) );
					TAP_Hdd_Fread( &header, sizeof(header), 1, f );
					TAP_Hdd_Fclose( f );
					// ensure that it's actually a TAP format file
					if ( strncmp( header.signature, "TFAPMIPS", 8 ) == 0 )
					{
						// store the filename in the order array
						m_filenames.push_back( file.name );
						string s;
						// and generate the text that will be shown in the footer
						s.format( "%s\n%s\n%s\n", header.description, header.authorName, header.etcStr );
						char buffer[10];
						sprintf( buffer, "%d.", ++index );
						AddItem(new FooterTextListItem(this, s, 0, buffer, header.name ));
					}
				}
			}
		}

		if ( totalEntry==0 || TAP_Hdd_FindNext( &file ) == 0 )
		{
			break;
		}
	}

	AddItem(new FooterActionListItem(this, &Commit, "Press OK to save the new TAP order to disk\nTAPs will be loaded in this order next startup", 0, "", "Save Changes"));
	AddItem(new FooterActionListItem(this, &Cancel, "Press OK to discard changes", 0, "", "Cancel Changes"));

	Redraw();
}


dword AutoStartPage::OnKey( dword key, dword extKey )
{
	switch ( key )
	{
	case RKEY_VolUp:
	{
		int index = GetSelectedIndex();
		if ( index < m_filenames.size()-1 )
			if ( MoveItem( index, true ) )
			{
				// swap the filenames
				string s = m_filenames[index];
				m_filenames[index] = m_filenames[index+1];
				m_filenames[index+1] = s;
				// rename the first (index) column of the moved rows
				((SimpleTextListItem*)m_items[index])->m_asItemText[0].format( "%d.", index+1 );
				((SimpleTextListItem*)m_items[index+1])->m_asItemText[0].format( "%d.", index+2 );
				// move the selection
				MoveSelection( 1, false );
			}
		return 0;
	}
	case RKEY_VolDown:
	{
		int index = GetSelectedIndex();
		if ( index > 0 && index < m_filenames.size() )
			if ( MoveItem( index, false ) )
			{
				// swap the filenames
				string s = m_filenames[index];
				m_filenames[index] = m_filenames[index-1];
				m_filenames[index-1] = s;
				// rename the first (index) column of the moved rows
				((SimpleTextListItem*)m_items[index-1])->m_asItemText[0].format( "%d.", index );
				((SimpleTextListItem*)m_items[index])->m_asItemText[0].format( "%d.", index+1 );
				// move the selection
				MoveSelection( -1, false );
			}
		return 0;
	}
	case RKEY_Exit:
		Close();
		return 0;

	case RKEY_Mute:
		return key;
	}
	ListPage::OnKey( key, extKey );

	return 0;
}

dword AutoStartPage::Cancel(ListPage* page, ListItem* item, dword key, dword extKey)
{
	if ( key == RKEY_Ok )
	{
		page->Close();
		return 0;
	}
	return key;
}


dword AutoStartPage::Commit(ListPage* page, ListItem* item, dword key, dword extKey)
{
	if ( key != RKEY_Ok )
		return key;

	array<string>& filenames = ((AutoStartPage*)page)->m_filenames;

	// Set current directory to ProgramFiles
	TAP_Hdd_ChangeDir("..");
	TAP_Hdd_ChangeDir("..");
	TAP_Hdd_ChangeDir("..");
	TAP_Hdd_ChangeDir("ProgramFiles");

	// display a progress box
	ProgressBox progress( "Reordering TAPs", "Reordering Auto Start TAPs", "Getting list of TAPs" );
	progress.CreateDialog();


	int errors = 0;
	if ( filenames.size() > 0 )
	{
		// Create a temporary Auto Start and move all TAPs there
		TAP_Hdd_Create( "Temp Auto Start", ATTR_FOLDER );
		if ( !TAP_Hdd_Exist("Temp Auto Start") )
		{
			TRACE("Failed to create Temp Auto Start");
			progress.UpdateProgress( 100, "Failed to create Temp Auto Start", "No TAPs have been reordered" );
			TAP_Delay(500);
			progress.DestroyDialog();
			return 0;
		}

		// Move the TAPs to the Temporary Auto Start directory
		TAP_Hdd_ChangeDir("Temp Auto Start");
		progress.SetLine1("Preparing TAP:");
		for ( int i = 0; i < filenames.size(); ++i )
		{
			TRACE1("Moving %s\n",(char*)filenames[i].c_str());
			progress.UpdateProgress( 10+(45*i)/filenames.size(), NULL, filenames[i] );
			TAP_Hdd_Move("/ProgramFiles/Auto Start", "/ProgramFiles/Temp Auto Start", (char*)filenames[i].c_str());
			if ( !TAP_Hdd_Exist( (char*)filenames[i].c_str() ) )
			{
				TRACE1("Failed to move %s\n", (char*)filenames[i].c_str() );
				++errors;
			}
		}
		TAP_Hdd_ChangeDir("..");
		TRACE("Done moving\n");

		// Now move them back into Auto Start in the order the user has requested
		progress.SetLine1("Reordering TAP:");
		TAP_Hdd_ChangeDir("Auto Start");
		int count = 0;
		for ( int i = 0; i < filenames.size(); ++i )
		{
			if ( filenames[i].size() > 0 )
			{
				TRACE1("Moving TAP %s\n",(char*)filenames[i].c_str());
				progress.UpdateProgress( 55+(45*count)/filenames.size(), NULL, filenames[i] );
				TAP_Hdd_Move("/ProgramFiles/Temp Auto Start", "/ProgramFiles/Auto Start", (char*)filenames[i].c_str());
				if ( !TAP_Hdd_Exist( (char*)filenames[i].c_str() ) )
				{
					TRACE1("Failed to move %s\n", (char*)filenames[i].c_str() );
					++errors;
				}
				++count;
			}
		}
		TAP_Hdd_ChangeDir("..");
		TRACE("Done moving\n");
	}

	// report any errors
	if ( errors )
	{
		progress.UpdateProgress( 100, "Some TAPs could not be moved", "Check the Temp Auto Start folder" );
		TAP_Delay(500);
	}
	progress.DestroyDialog();

	page->Close();

	return 0;
}
