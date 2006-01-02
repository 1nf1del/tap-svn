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

#include <tap.h>
#include <vtable.h>
#include "Tapplication.h"
#include "Page.h"
#include "logger.h"
#include "dialog.h"

Tapplication* Tapplication::tap = NULL;

Tapplication::Tapplication()
{
	TAP_Print("Tapplication\n");
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
	// Notify open pages that the TAP is closing
	for ( int i = 0; i < pageCount; ++i )
		pageStack[i]->OnClose();

	if ( pageCount > 0 )
		TAP_ExitNormal();

	DiscardTheApplication();
	TAP_Exit(); // this won't return - I think...
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
	if ( pageCount > 0 )
	{
		if ( pageCount == 1 )
			TAP_EnterNormal();

		TAP_Osd_FillBox(screenRgn, 0, 0, MAX_SCREEN_X, MAX_SCREEN_Y, COLOR_None);
		if (pageCount>1)
			pageStack[pageCount-2]->Redraw();

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


// accessors
Tapplication* Tapplication::GetTheApplication()
{
	return tap;
}

void Tapplication::SetTheApplication(Tapplication* theApp)
{
	tap = theApp;
}

word Tapplication::GetScreenRegion()
{
	return screenRgn;
}

word GetTAPScreenRegion()
{
	return Tapplication::GetTheApplication()->GetScreenRegion();
}

void Tapplication::DiscardTheApplication()
{
	delete GetTheApplication();
	SetTheApplication(NULL);
}

int Tapplication::CreateTheApplication()
{
	SetTheApplication(CreateTapplication());
	if ( !GetTheApplication() )
	{
		TRACE("Failed to create application object\n");
		return 0;
	}

	TRACE("Created Application Object OK\n");

	// If start returns false then this is not a TSR
	if ( !GetTheApplication()->Start() )
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