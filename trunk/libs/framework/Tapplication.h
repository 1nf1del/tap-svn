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

#ifndef __TAPPLICATION_H
#define __TAPPLICATION_H

#include "colors.h"

class Page;
class Dialog;

class Tapplication
{
public:
	Tapplication();
	virtual ~Tapplication();

	// Overridable events
	virtual bool Start();
	virtual void OnIdle();
	virtual dword OnKey( dword key, dword extKey );
	dword EventHandler( word event, dword param1, dword param2 );

	// Operations
	void PushPage( Page* page );
	Page* PopPage();
	dword Close();
	void SetActiveDialog(Dialog* pDialog);
	bool IsTopPage( Page* page);

	// Skinning support
	bool LoadSkin( const char* filename );
	ListColors GetColorDef( colorSets whichSet ) const		{ return m_colors[whichSet]; }

	// Application creation
	static Tapplication* GetTheApplication();
	static int CreateTheApplication();
	static void DiscardTheApplication();

	// Accessors
	word GetScreenRegion();

protected:
	int pageCount;
	word screenRgn;

private:
	static Tapplication* CreateTapplication();
	static void SetTheApplication(Tapplication*);

	Page* pageStack[10];
	Dialog* m_activeDialog;

	// colours
	ListColors m_colors[4];

	static Tapplication* tap;
};

word GetTAPScreenRegion();

// Screen declarations
const int MAX_SCREEN_X = 720;
const int MAX_SCREEN_Y = 576;

const word FILL_COLOUR = COLOR_Black;
const word MAIN_TEXT_COLOUR = RGB(29,29,29);
const word HEADING_TEXT_COLOUR = RGB8888(0,152,192);
const word INFO_FILL_COLOUR = COLOR_User3;
const word TITLE_COLOUR = RGB(18,18,18);
const word INFO_COLOUR = COLOR_White;

// Constructor on globals are not called, so application object needs to be created on the heap
#define DEFINE_APPLICATION(T) Tapplication* Tapplication::CreateTapplication() { return new T; }


#endif
