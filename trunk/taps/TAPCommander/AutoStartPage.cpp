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
#include <string.h>
#include <tapmap.h>
#include <DirectoryUtils.h>
#include <libFireBird.h>
#include <Logger.h>
#include <ProgressBox.h>
#include "AutoStartPage.h"
#include "LoadedTAPPage.h"
#include "Tapplication.h"
#include "ListColumn.h"
#include "FooterListItem.h"
#include "MessageBox.h"
#include "IniFile.h"
#include "Options.h"
#include "ConfigPage.h"


extern bool menuActivates;


static const char* messageBoxTitle = "Reorder TAPs";
// using constants causes crashes in TAP_Hdd_ChangeDir
#define programFilesPath "/ProgramFiles"
#define autoStartPath "/ProgramFiles/Auto Start"
#define tempAutoStartName "Temp Auto Start"


void AutoStartPage::TAPListItem::DrawSubItem(short int iColumn, Rect rcBounds)
{
	AutoStartTAP& tap = ((AutoStartPage*)m_theList)->m_taps[m_index];
	char buffer[15];
	const char* text = 0;
	switch (iColumn)
	{
	case 0:
	{
		// Column 1: TAP index
		sprintf( buffer, "%d.", m_index+1 );
		text = buffer;
		break;
	}
	case 1:
		// Column 2: TAP name
		text = tap.name;
		break;
	case 2:
		// Column 3: Enabled state
		text = tap.enabled ? "enabled" : "disabled";
		break;
	}
	if (text)
		DrawSubItemString(iColumn, rcBounds, text);
}


string AutoStartPage::TAPListItem::GetFooterText()
{
	AutoStartTAP& tap = ((AutoStartPage*)m_theList)->m_taps[m_index];
	const char* enabled = tap.enabled ? "Disable" : "Enable";
	string s;
	s.format("%s\nArchive=Running | Record=Save | Recall=Discard | Red (Green)=%s (all)", tap.footer.c_str(), enabled);
	return s;
}


//-----------------------------------------------------------------------------
// AutoStartPage
AutoStartPage::AutoStartPage() :
	ListPage( LF_SHOW_HEADER | LF_ROW_SEPARATORS | LF_SCROLLBAR | LF_COLUMN_SEPARATORS | LF_SHOW_FOOTER | LF_COLUMNS_IN_HEADER,
			  Rect(50,50,620,465), 32, 45, 100 ),
	m_dirty(false)
{
    SetFontSizes(FNT_Size_1926, FNT_Size_1622, FNT_Size_1622);

    AddColumn(new ListColumn(this, 3));
    AddColumn(new ListColumn(this, 45, 0, "Auto Start TAPs"));
    AddColumn(new ListColumn(this, 7));
	AddItem(new SimpleTextListItem(this, 0, "", "Loading..."));
}


AutoStartPage::~AutoStartPage()
{
}


void AutoStartPage::OnOpen()
{
	ListPage::OnOpen();
	PopulateList();
}


void AutoStartPage::PopulateList()
{
	m_taps.clear();

	// Get a list of TAPs in the current Auto Start
	TAP_Hdd_ChangeDir( autoStartPath );

	TYPE_File file;

	int index = 0;
	for ( dword totalEntry = TAP_Hdd_FindFirst( &file ); totalEntry > 0; )
	{
		if ( file.attr == ATTR_NORMAL )
		{
			char* ext = strrchr(file.name, '.');
			if ( ext )
			{
				// ensure we're only looking at .tap files
				++ext;
				bool enabled = stricmp( ext, "tap" ) == 0;
				if ( enabled || stricmp( ext, "nap" ) == 0 )
				{
					// that we can open
					TYPE_File* f = TAP_Hdd_Fopen( file.name );
					if ( f )
					{
						// read the TAP header
						tTAPHeader header;
						memset( &header, 0, sizeof(header) );
						TAP_Hdd_Fread( &header, sizeof(header), 1, f );
						TAP_Hdd_Fclose( f );
						// ensure that it's actually a TAP format file
						if ( strncmp( header.Signature, "TFAPMIPS", 8 ) == 0 )
						{
							m_taps.push_back(AutoStartTAP());
							AutoStartTAP& t = m_taps.back();
							// store the filename with the extension .tap in the order array
							*ext = 't';
							t.index = index;
							t.filename = file.name;
							t.enabled = enabled;
							t.id = header.TAPID;
							t.name = header.Name;
							// and generate the text that will be shown in the footer
							t.footer.format( "%s\n%s\n%s\n", header.Description, header.AuthorName, header.EtcStr );
							++index;
						}
					}
				}
			}
		}

		dword currentEntry = TAP_Hdd_FindNext( &file );
		if ( totalEntry == currentEntry || currentEntry == 0 )
			break;
	}

	// Remove the Loading message and redraw the window contents
	DiscardItems();
	for (unsigned int i=0; i < m_taps.size(); ++i)
		AddItem(new TAPListItem(this, i));
	AddItem(new FooterActionListItem(this, &Commit, "Press OK to save the new TAP order\nTAPs will be loaded in this order next time the unit wakes from standby", 0, "", "Save Changes"));
	AddItem(new FooterActionListItem(this, &Backup, "Press OK to backup the TAP order and enabled state as listed above", 0, "", "Backup"));
	AddItem(new FooterActionListItem(this, &Restore, "Press OK to restore the previous backup to the list above\nThis does not save changes to disk", 0, "", "Restore"));

	Draw();
}

bool AutoStartPage::CanExit()
{
	if (m_dirty)
	{
		switch (MessageBox::Show("Reorder TAPs", "You have unsaved changes", "Save\nDiscard\nCancel"))
		{
		case 1:
			Save();
			break;
		case 0:
		case 3:
			return false;
		}
	}
	return true;
}


dword AutoStartPage::OnKey( dword key, dword extKey )
{
	switch ( key )
	{
	case RKEY_Exit:
		if ( CanExit() )
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
		if ( CanExit() )
			Replace(new LoadedTAPPage());
		return 0;
	case RKEY_VolUp:
	{
		int index = GetSelectedIndex();
		if ( index < (int)m_taps.size()-1 )
		{
			// swap the filenames
			AutoStartTAP t = m_taps[index];
			m_taps[index] = m_taps[index+1];
			m_taps[index+1] = t;
			// move the selection
			MoveSelection( 1, false );
			m_dirty = true;
		}
		return 0;
	}
	case RKEY_VolDown:
	{
		int index = GetSelectedIndex();
		if ( index > 0 && index < (int)m_taps.size() )
		{
			// swap the filenames
			AutoStartTAP t = m_taps[index];
			m_taps[index] = m_taps[index-1];
			m_taps[index-1] = t;
			// move the selection
			MoveSelection( -1, false );
			m_dirty = true;
		}
		return 0;
	}
	case RKEY_NewF1:
	{
		// enable/disable TAP
		int index = GetSelectedIndex();
		EnableTAP( index, !m_taps[index].enabled );
		return 0;
	}
	case RKEY_F2:
	{
		int index = GetSelectedIndex();
		if ( index < (int)m_taps.size() )
		{
			bool enable = !m_taps[index].enabled;
			for ( unsigned int i = 0; i < m_taps.size(); ++i )
				EnableTAP( i, enable );
		}
		return 0;
	}
	// Discard changes
	case RKEY_Recall:
		// get rid of the list items, then the 
		DiscardItems();
		AddItem(new SimpleTextListItem(this, 0, "", "Loading..."));
		Draw();
		PopulateList();
		m_dirty = false;
		return 0;
	// Save Changes
	case RKEY_Record:
		Save();
		Close();
		return 0;
	}
	ListPage::OnKey( key, extKey );

	return 0;
}


void AutoStartPage::EnableTAP( unsigned int index, bool enable )
{
	if ( index < m_taps.size() && m_taps[index].id != __tap_ud__ )
	{
		m_dirty = true;
		m_taps[index].enabled = enable;
		RedrawItem(index);
	}
}


// Save the current order and enabled state to the ini file
dword AutoStartPage::Backup( ListPage* page, ListItem* item, dword key, dword extKey )
{
	if ( key != RKEY_Ok )
		return key;
	
	array<AutoStartTAP>& taps = ((AutoStartPage*)page)->m_taps;

	DialogBox progress( "Please wait", "Backing up..." );
	progress.OnOpen();

	string backup;
	for ( unsigned int i = 0; i < taps.size(); ++i )
	{
		if ( i > 0 )
			backup += ",";
		string s = taps[i].filename;
		s[taps[i].filename.size()-3] = taps[i].enabled ? 't' : 'n';
		backup += s;
	}

	Options* options = Tapplication::GetTheApplication()->GetOptions();
	IniFile* file = options->GetIniFile();
	if ( file )
	{
		file->SetValue( "Backup", backup );
		options->SaveValues();
	}

	progress.OnClose();

	return 0;
}


// Load the order and enabled state from the ini file
dword AutoStartPage::Restore( ListPage* page, ListItem* item, dword key, dword extKey )
{
	if ( key != RKEY_Ok )
		return key;

	AutoStartPage* ths = (AutoStartPage*)page;
	array<AutoStartTAP>& taps = ths->m_taps;

	DialogBox progress( "Please wait", "Restoring..." );
	progress.OnOpen();

	IniFile* file = Tapplication::GetTheApplication()->GetOptions()->GetIniFile();
	if ( file )
	{
		string s;
		if ( file->GetValue( "Backup", s ) )
		{
			// split the backup string into a list of filenames
			array<string> tapList = s.split(",");
			for ( unsigned int i = 0; i < taps.size(); ++i )
				taps[i].spare = i;
			// run through the list finding each filename in the current tap list
			int count = 0;
			for ( unsigned int i = 0; i < tapList.size(); ++i )
			{
				string findName = tapList[i].trim();
				int extOffset = findName.reverseFind( '.' );
				if ( ++extOffset == 0 )
					continue;
				bool enabled = findName[extOffset] != 'n';
				findName[extOffset] = 't';
				for ( unsigned int j = count; j < taps.size(); ++j )
				{
					if ( findName.compareNoCase(taps[j].filename) == 0)
					{
						taps[j].enabled = enabled;
						if ( count != j )
						{
							AutoStartTAP t = taps[count];
							taps[count] = taps[j];
							taps[j] = t;
						}
						++count;
						break;
					}
				}
			}
			if (count)
			{
				taps.sort(count);
				ths->Draw();
				ths->m_dirty = true;
			}
		}

		progress.OnClose();
	}

	return 0;
}


dword AutoStartPage::Commit( ListPage* page, ListItem* item, dword key, dword extKey )
{
	if ( key == RKEY_Ok )
	{
		((AutoStartPage*)page)->Save();
		page->Close();
	}
	return key;
}


void AutoStartPage::Save()
{
	if ( m_taps.size() == 0 )
	{
		MessageBox::Show( messageBoxTitle, "No TAPs are installed in Auto Start", "OK" );
		return;
	}

	// Set current directory to ProgramFiles
	if ( !TAP_Hdd_ChangeDir( programFilesPath ) )
	{
		MessageBox::Show( messageBoxTitle, "ProgramFiles directory could not be found", "OK" );
		return;
	}

	// display a progress box
	ProgressBox progress( "Please wait", "Reordering Auto Start TAPs" );
	progress.OnOpen();

	string errors;
	string warnings;

	bool reorder = false;
	// Get the current filenames of each of the TAPs
	if ( !TAP_Hdd_ChangeDir( autoStartPath ) )
	{
		progress.OnClose();
		MessageBox::Show("Reorder TAPs", "Failed to find Auto Start folder", "OK");
		return;
	}
	for ( unsigned int i = 0; i < m_taps.size(); ++i )
	{
		if ( m_taps[i].index != i )
			reorder = true;
		if ( !TAP_Hdd_Exist( (char*)m_taps[i].filename.c_str() ) )
		{
			int extOffset = m_taps[i].filename.size()-3;
			if ( extOffset > 0)
			{
				m_taps[i].filename[extOffset] = m_taps[i].filename[extOffset] == 't' ? 'n' : 't';
			}
		}
	}

	if (reorder)
	{
		TAP_Hdd_ChangeDir("..");
		// Create a temporary Auto Start and move all TAPs there
		TAP_Hdd_Create( tempAutoStartName, ATTR_FOLDER );
		if ( !TAP_Hdd_ChangeDir( tempAutoStartName ) )
		{
			progress.OnClose();
			MessageBox::Show( messageBoxTitle, "Failed to create Temp Auto Start folder", "OK");
			return;
		}
		// Move the TAPs to the Temporary Auto Start directory
		short int stepSize = 50/m_taps.size();
		for ( unsigned int i = 0; i < m_taps.size(); ++i )
		{
			TRACE1("Moving %s\n",(char*)m_taps[i].filename.c_str());
			progress.StepProgress( stepSize, "Preparing " + m_taps[i].name );
			HDD_Move( (char*)m_taps[i].filename.c_str(), "/ProgramFiles/Auto Start", "/ProgramFiles/Temp Auto Start" );
			if ( !TAP_Hdd_Exist( (char*)m_taps[i].filename.c_str() ) )
			{
				// TAP wasn't moved, flag the failure and
				if ( warnings.size() > 0 )
					warnings += ", ";
				warnings += m_taps[i].name;
				m_taps[i].spare = false;
				progress.StepProgress( stepSize );
			}
			else
				m_taps[i].spare = true;
		}
		TRACE("Done moving\n");

		// Now move them back into Auto Start in the order the user has requested
		if ( !TAP_Hdd_ChangeDir( "/ProgramFiles/Auto Start" ) )
		{
			progress.OnClose();
			MessageBox::Show("Reorder TAPs", "Failed to find Auto Start folder", "OK");
			return;
		}
		int count = 0;
		for ( unsigned int i = 0; i < m_taps.size(); ++i )
		{
			if ( m_taps[i].spare )
			{
				TRACE1("Moving TAP %s\n",(char*)m_taps[i].filename.c_str());
				progress.StepProgress( stepSize, "Reordering " + m_taps[i].name );
				HDD_Move((char*)m_taps[i].filename.c_str(), "/ProgramFiles/Temp Auto Start", "/ProgramFiles/Auto Start");
				if ( !TAP_Hdd_Exist( (char*)m_taps[i].filename.c_str() ) )
				{
					if ( errors.size() > 0 )
						errors += ", ";
					errors += m_taps[i].name;
				}
				++count;
			}
		}
	}

	// Finally, deal with enabled and disabled
	for ( unsigned int i = 0; i < m_taps.size(); ++i )
	{
		int extOffset = m_taps[i].filename.size()-3;
		string tapName = m_taps[i].filename;
		tapName[extOffset] = 't';
		string napName = m_taps[i].filename;
		napName[extOffset] = 'n';
		if ( m_taps[i].enabled )
		{
			// ensure TAP is enabled
			if ( TAP_Hdd_Exist( (char*)tapName.c_str() ) )
				TAP_Hdd_Delete( (char*)napName.c_str() );
			else
				TAP_Hdd_Rename( (char*)napName.c_str(), (char*)tapName.c_str() );
		}
		else
		{
			// ensure TAP is disabled
			if ( TAP_Hdd_Exist( (char*)tapName.c_str() ) )
			{
				TAP_Hdd_Delete( (char*)napName.c_str() );
				TAP_Hdd_Rename( (char*)tapName.c_str(), (char*)napName.c_str() );
			}
		}
	}

	// Delete Temp Auto Start if the directory is empty
	if ( TAP_Hdd_ChangeDir( "/ProgramFiles/Temp Auto Start" ) )
	{
		TYPE_File file;
		dword totalEntry = TAP_Hdd_FindFirst( &file );
		while ( totalEntry-- )
		{
			// ignore ., .. and deleted files
			if ( file.attr != ATTR_PARENTFOLDER && file.attr != ATTR_THISFOLDER && file.attr != ATTR_DELETEDFILE )
				break;
			if ( totalEntry==0 || TAP_Hdd_FindNext( &file ) == 0 )
			{
				TAP_Hdd_ChangeDir("..");
				TAP_Hdd_Delete( tempAutoStartName );
				break;
			}
		}
	}

	progress.OnClose();

	// report any errors
	if ( errors.size() || warnings.size() )
	{
		if ( errors.size() > 0 )
			MessageBox::Show("Reorder TAPs", "The following TAPs could not be reordered:\n" + errors + "Check the Temp Auto Start folder", "OK");
		else if ( warnings.size() > 0 )
			MessageBox::Show("Reorder TAPs", "The following TAPs could not be reordered:\n" + warnings, "OK");
	}
	else
	{
		if ( MessageBox::Show("Reorder TAPs", "Finished", HDD_isAnyRecording() ? "OK" : "OK\nReboot" ) == 2 )
			Reboot(false);
	}
}
