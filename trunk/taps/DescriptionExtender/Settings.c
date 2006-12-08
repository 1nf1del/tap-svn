#include <tap.h>
#include <model.h>
#include "Settings.h"


#define optionsFile "DescriptionExtender.dat"
#define INI_VERSION 1

Options options;

static TYPE_Window window;
static bool windowActive = FALSE;
static dword rgn = 0;
static char* optionTextBuffer = 0;
static int optionCount = 2;

static void OptionsMenu_UpdateText();


void OptionsMenu_Show()
{
	int i, count;
	char* p;

	if ( !rgn )
		rgn = TAP_Osd_Create( 0, 0, 720, 576, 0, 0 );		// create rgn-handle

	TAP_Win_Delete( &window );
	windowActive = TRUE;

	TAP_ExitNormal();

	TAP_Win_SetDefaultColor( &window );
	TAP_Win_Create( &window, rgn, 160, 70, 330, 150, FALSE, FALSE );
	TAP_Win_SetTitle( &window, "Description Extender Options", 0, FNT_Size_1622 );

	// allocate memory for option text
	optionTextBuffer = (char*)malloc(0x40*(optionCount+5));
	p = optionTextBuffer;
	for ( i = 0; i < optionCount+5; ++i )
	{
		*p = 0;
		TAP_Win_AddItem( &window, p );
		window.item[i] = p;
		p += 0x40;
	}
	OptionsMenu_UpdateText();

	TAP_Win_SetSelection( &window, 0 );
}


void OptionsMenu_Close()
{
	free(optionTextBuffer);
	optionTextBuffer = 0;
	TAP_Win_Delete( &window );
	TAP_Osd_Delete( rgn );
	rgn = 0;
	windowActive = FALSE;
	TAP_EnterNormal();
}


dword OptionsMenu_HandleKey( dword key, dword keyHW )
{
	if ( windowActive )
	{
		int selection = TAP_Win_GetSelection( &window );

		switch ( key )
		{
		case RKEY_Ok:
			if ( selection == optionCount+1 )
			{
				// Cancel - Reload original settings
				OptionsMenu_Close();
				LoadSettings();
			}
			return 0;

		case RKEY_0:
			if ( selection == optionCount )
			{
				// Reset to defaults
				ResetSettings();
				OptionsMenu_UpdateText();
			}
			return 0;

		case RKEY_Exit:
			OptionsMenu_Close();
			SaveSettings();
			return 0;

		case RKEY_VolUp:
			if ( selection == 0 )
				options.insertSpace = !options.insertSpace;
			else if ( selection == 1 )
				options.addGenre = !options.addGenre;
			OptionsMenu_UpdateText();
			return 0;

		case RKEY_VolDown:
			if ( selection == 0 )
				options.insertSpace = !options.insertSpace;
			else if ( selection == 1 )
				options.addGenre = !options.addGenre;
			OptionsMenu_UpdateText();
			return 0;

		case RKEY_ChDown:
			if ( selection < optionCount+1 )
			{
				TAP_Win_Action( &window, key );
				OptionsMenu_UpdateText();
			}
			return 0;
		case RKEY_ChUp:
			if ( selection > 0 )
			{
				TAP_Win_Action( &window, key );
				OptionsMenu_UpdateText();
			}
			return 0;

		default:
			TAP_Win_Action( &window, key ); // send all other key's to menu-Win
			return 0;
		}
	}

	return key;
}


void OptionsMenu_UpdateText()
{
	int i;
	char* p;

	p = optionTextBuffer;

	strcpy( p, "Extra space@|" );
	strcat( p, options.insertSpace ? "Yes" : "No" );
	p += 0x40;

	strcpy( p, "Genre@|" );
	strcat( p, options.addGenre ? "Yes" : "No" );
	window.check[1] = (GetModel() == TF5800t && _appl_version >= 0x1288) ? 0 : 2;
	p += 0x40;

	strcpy( p, "Reset to defaults" ); p += 0x40;
	strcpy( p, "Cancel" ); p += 0x40;

	p += 0x40;

	// Display context sensitive help
	switch (TAP_Win_GetSelection( &window ))
	{
	case 0:
		strcpy( p, "Inserts a space between short description" ); p += 0x40;
		strcpy( p, "and extended information"); p += 0x40;
		break;
	case 1:
		strcpy( p, "Appends the genre to the description" ); p += 0x40;
		strcpy( p, " " ); p += 0x40;
		break;
	case 2:
		strcpy( p, "Press 0 to reset options to their defaults" ); p += 0x40;
		strcpy( p, " " ); p += 0x40;
		break;
	default:
		strcpy( p, " " ); p += 0x40;
		strcpy( p, " " ); p += 0x40;
		break;
	}

	TAP_Win_SetSelection( &window, TAP_Win_GetSelection( &window ) );
}


void SaveSettings()
{
	TYPE_File* fp;

	TAP_Hdd_ChangeDir( ".." );
	TAP_Hdd_ChangeDir( ".." );
	TAP_Hdd_ChangeDir( "ProgramFiles" );
	if ( !TAP_Hdd_Exist( "Settings" ) )
		TAP_Hdd_Create( "Settings", ATTR_FOLDER );
	TAP_Hdd_ChangeDir( "Settings" );

	if ( !TAP_Hdd_Exist( optionsFile ) )
		TAP_Hdd_Create( optionsFile, ATTR_NORMAL );

	if ( fp = TAP_Hdd_Fopen(optionsFile) )
	{
		int version = INI_VERSION;
		int zero[64];
		memset( zero, 0, sizeof(zero) );
		TAP_Hdd_Fwrite( &version, 1, sizeof(version), fp );
		TAP_Hdd_Fwrite( &options, 1, sizeof(options), fp );
		TAP_Hdd_Fwrite( zero, 1, sizeof(zero), fp );
		TAP_Hdd_Fclose( fp );
	}
}


void LoadSettings()
{
	TYPE_File* fp;
	bool validSettings = FALSE;

	TAP_Hdd_ChangeDir( ".." );
	TAP_Hdd_ChangeDir( ".." );
	TAP_Hdd_ChangeDir( "ProgramFiles" );
	TAP_Hdd_ChangeDir( "Settings" );

	if ( fp = TAP_Hdd_Fopen(optionsFile) )
	{
		int version = 0;
		TAP_Hdd_Fread( &version, 1, sizeof(version), fp );
		if ( version == INI_VERSION )
		{
			TAP_Hdd_Fread( &options, 1, sizeof(options), fp );
			validSettings = TRUE;
		}
		TAP_Hdd_Fclose( fp );
	}

	if ( !validSettings )
	{
		ResetSettings();
		SaveSettings();
	}
}


void ResetSettings()
{
	if ( GetModel() == TF5800t )
	{
		options.insertSpace = TRUE;
		options.addGenre = _appl_version >= 0x1288;
	}
	else
	{
		options.insertSpace = FALSE;
		options.addGenre = FALSE;
	}
}
