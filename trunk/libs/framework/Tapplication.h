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
class Decorator;

extern "C" const dword  __tap_ud__;
extern "C" void cpp_TAP_Exit();
class Tapplication
{
	friend void cpp_TAP_Exit();
public:

	// Overridable events
	virtual bool Start();
	virtual void OnIdle();
	virtual dword OnKey( dword key, dword extKey );
	dword EventHandler( word event, dword param1, dword param2 );
	virtual void BeforeExitNormal();
	virtual void AfterEnterNormal();

	// Operations
	void PushPage( Page* page );
	Page* PopPage(bool bRedrawUnerlying = true);
	dword Close();
	bool IsClosing() const;
	void SetActiveDialog(Dialog* pDialog);
	bool IsTopPage( Page* page);

	// Skinning support
	bool LoadSkin( const char* filename );
	ListColors GetColorDef( colorSets whichSet ) const		{ return m_colors[whichSet]; }
	virtual void SetDefaultColors();
	virtual Decorator* GetDefaultDialogDecorator();

	// Application creation
	static Tapplication* GetTheApplication();
	static int CreateTheApplication();
	static void DiscardTheApplication();

	// Accessors
	word GetScreenRegion();
	dword GetID() const;

protected:
	Tapplication();
	virtual ~Tapplication();
	int pageCount;
	word screenRgn;
	// colours - protected so a tap can override the defaults
	ListColors m_colors[5];

private:
	static Tapplication* CreateTapplication();
	static void SetTheApplication(Tapplication*);

	Page* pageStack[10];
	Dialog* m_activeDialog;

	// colours
	int m_screenOffsetX, m_screenOffsetY;

	bool m_isClosing;
	static Tapplication* tap;
};

// inline accessors
inline Tapplication* Tapplication::GetTheApplication()
{
	return tap;
}

inline word Tapplication::GetScreenRegion()
{
	return screenRgn;
}

inline void Tapplication::DiscardTheApplication()
{
	tap->Close();
	delete tap;
	tap = NULL;
}

inline bool Tapplication::IsClosing() const
{
	return m_isClosing;
}

inline word GetTAPScreenRegion()
{
	return Tapplication::GetTheApplication()->GetScreenRegion();
}


inline dword Tapplication::GetID() const
{
	return __tap_ud__;
}


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
