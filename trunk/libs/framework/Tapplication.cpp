/*
	Copyright (C) 2005 Simon Capewell

	This file is part of the TAPs for Topfield PVRs project.
		http://tap.berlios.de/

	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <stdlib.h>
#include <tap.h>
#include <vtable.h>
#include <inifile.h>
#include "Tapplication.h"
#include "Page.h"
#include "logger.h"
#include "dialog.h"

Tapplication* Tapplication::tap = NULL;

Tapplication::Tapplication() :
	m_screenOffsetX(0),
	m_screenOffsetY(0),
	m_isClosing(false)
{
	// Setup skin colours
	m_colors[normalColors].frameColor = RGB8888(8,8,8);
	m_colors[normalColors].bgColor = RGB8888(81,90,195);
	m_colors[normalColors].textColor = RGB8888(240,240,240);

	m_colors[highlightColors].frameColor = m_colors[normalColors].frameColor;
	m_colors[highlightColors].bgColor = RGB8888(8,176,8);
	m_colors[highlightColors].textColor = RGB8888(240,240,8);

	m_colors[headerColors].frameColor = m_colors[normalColors].frameColor;
	m_colors[headerColors].bgColor = RGB8888(8,8,8);
	m_colors[headerColors].textColor = RGB8888(240,240,240);

	m_colors[footerColors].frameColor = m_colors[normalColors].frameColor;
	m_colors[footerColors].bgColor = m_colors[normalColors].bgColor;
	m_colors[footerColors].textColor = m_colors[normalColors].textColor;

	m_colors[scrollBarColors].frameColor = m_colors[headerColors].frameColor;
	m_colors[scrollBarColors].bgColor = m_colors[headerColors].bgColor;
	m_colors[scrollBarColors].textColor = m_colors[headerColors].textColor;

#ifdef DEBUG
	if ( tap != 0 )
		TAP_Print( "Tapplication already exists\n" );
#endif

	pageCount = 0;

	screenRgn = TAP_Osd_Create( 0, 0, 720, 576, 0, FALSE );
	m_activeDialog = NULL;
}


Tapplication::~Tapplication()
{
	TAP_Print("~Tapplication\n");

	TAP_Osd_Delete( screenRgn );
	screenRgn = 0;

	// Clear the global TAP object pointer
	tap = 0;
}


bool Tapplication::Start()
{
	TAP_Print("Tapplication::Start\n");

	// Load application defaults
	IniFile file;
	if ( file.Load("default.ini") )
	{
		string skinName;
		if ( file.GetValue( "Skin", skinName ) )
			LoadSkin( skinName );
		file.GetValue( "ScreenOffsetX", m_screenOffsetX );
		file.GetValue( "ScreenOffsetY", m_screenOffsetY );
	}
	return false;
}

dword Tapplication::EventHandler( word event, dword param1, dword param2 )
{
    switch ( event )
	{
	case EVT_IDLE:
		OnIdle();
		return 0;
	case EVT_KEY:
		return OnKey( param1, param2 );
	}

	return param1;
}

void Tapplication::OnIdle()
{
	// Dispatch idle messages to all pages
	for ( int i = 0; i < pageCount; ++i )
		pageStack[i]->OnIdle();
}


dword Tapplication::OnKey( dword key, dword extKey )
{
	if (m_activeDialog != NULL)
		return m_activeDialog->OnKey(key,extKey);

	if ( pageCount == 0 )
		return key;

	// Dispatch keypress to the currently active page
	return pageStack[pageCount-1]->OnKey( key, extKey );
}


dword Tapplication::Close()
{
	// Close all open pages starting from the topmost
	while ( pageCount )
	{
		Page* page = pageStack[pageCount-1];
		page->Close();
	}

	// flag that we're exiting
	m_isClosing = true;

	return 0;
}


// Page stack management
void Tapplication::PushPage( Page* page )
{
	if ( pageCount == 0 )
		TAP_ExitNormal();
	pageStack[ pageCount++ ] = page;
}

Page* Tapplication::PopPage()
{
	TRACE("Popping Page \n");
	if ( pageCount > 0 )
	{
		if ( pageCount == 1 )
			TAP_EnterNormal();

		TRACE("Blanking screen\n");
		TAP_Osd_FillBox(screenRgn, 0, 0, MAX_SCREEN_X, MAX_SCREEN_Y, COLOR_None);
		if (pageCount>1)
		{
			TRACE("Redrawing underlying page\n");
			pageStack[pageCount-2]->Redraw();
			TRACE("Redraw done\n");
		}

		return pageStack[ --pageCount ];
	}
	return NULL;
}

bool Tapplication::IsTopPage( Page* page)
{
	if (pageCount == 0)
		return false;

	return page == pageStack[pageCount-1];
}

int Tapplication::CreateTheApplication()
{
	tap = CreateTapplication();
	if ( !tap )
	{
		TRACE("Failed to create application object\n");
		return 0;
	}

	// If start returns false then this is not a TSR
	if ( !tap->Start() )
	{
		DiscardTheApplication();
		TRACE("Exiting TAP - no TSR requested\n");
		return 0;
	}

	TRACE("TAP running in TSR mode\n");
	return 1;
}

void Tapplication::SetActiveDialog(Dialog* pDialog)
{
	if (pDialog && m_activeDialog)
	{
		TRACE("Ooops - not allowed more than 1 active dialog");
	}
	m_activeDialog = pDialog;
}


// Skin management
// This could do with being moved into IniFile, but we could do with a Color type first
void GetColour( IniFile& file, const char* key, word& color )
{
	string value;
	if ( file.GetValue(key, value) )
	{
		array<string> rgb;
		value.split( ",", rgb );
		if ( rgb.size() == 3 )
			color = RGB8888( atoi(rgb[0]), atoi(rgb[1]), atoi(rgb[2]) );
	}
}


bool Tapplication::LoadSkin( const char* filename )
{
	IniFile file;
	if ( !file.Load( filename ) )
		return false;

	GetColour( file, "Frame", m_colors[normalColors].frameColor );
	GetColour( file, "Background", m_colors[normalColors].bgColor );
	GetColour( file, "TextForeground", m_colors[normalColors].textColor );

	m_colors[highlightColors].frameColor = m_colors[normalColors].frameColor;
	GetColour( file, "Highlight", m_colors[highlightColors].bgColor );
	GetColour( file, "TextHighlightedForeground", m_colors[highlightColors].textColor );

	m_colors[headerColors].frameColor = m_colors[normalColors].frameColor;
	GetColour( file, "TitleBackground", m_colors[headerColors].bgColor );
	GetColour( file, "TitleForeground", m_colors[headerColors].textColor);

	// default footer colours to the normal colours in case the ini file doesn't contain them
	m_colors[footerColors].frameColor = m_colors[normalColors].frameColor;
	m_colors[footerColors].bgColor = m_colors[normalColors].bgColor;
	m_colors[footerColors].textColor = m_colors[normalColors].textColor;
	GetColour( file, "FooterBackground", m_colors[headerColors].bgColor );
	GetColour( file, "FooterForeground", m_colors[headerColors].textColor);

	m_colors[scrollBarColors].frameColor = m_colors[headerColors].frameColor;
	m_colors[scrollBarColors].bgColor = m_colors[headerColors].bgColor;
	m_colors[scrollBarColors].textColor = m_colors[headerColors].textColor;

	return true;
}
