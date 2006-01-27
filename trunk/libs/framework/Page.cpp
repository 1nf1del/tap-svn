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
#include "Tapplication.h"
#include "Page.h"
#include "logger.h"


Page::Page()
{
}

Page::~Page()
{
}

void Page::Open()
{
	Tapplication::GetTheApplication()->PushPage( this );
	OnOpen();
}


void Page::Close(bool bRedrawUnderlyingPage)
{
	OnClose();
	if (Tapplication::GetTheApplication()->PopPage(bRedrawUnderlyingPage) != this)
		TRACE("Ooops, the page closing isn't the top page - this'll crash!\n");
	TRACE("About to delete popped page\n");
	delete this;
	TRACE("Page deleted\n");
}


void Page::OnIdle()
{
}


dword Page::OnKey( dword key, dword extKey )
{
	return key;
}


void Page::OnOpen()
{
}

void Page::OnClose()
{
}

void Page::Redraw()
{
}