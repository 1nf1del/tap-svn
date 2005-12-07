//Tapulator - An emulator for the Topfield PVR TAP API
//Copyright (C) 2005  Robin Glover
//
//This program is free software; you can redistribute it and/or
//modify it under the terms of the GNU General Public License
//as published by the Free Software Foundation; either version 2
//of the License, or (at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program; if not, write to the Free Software
//Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
//
// You can contact the author via email at robin.w.glover@gmail.com

#pragma once

typedef void (*TYPE_DrawItemFunc)( TYPE_Window *win, dword itemNo );

#include "InfoLogger.h"

class OSDRegion;
class Window : public InfoLogger
{
public:
	Window(TYPE_Window *win, OSDRegion* region, DWORD x, DWORD y, DWORD w, DWORD h, BYTE save, BYTE bScr);
	~Window(void);

	void SetTitle(CString sTitle, int font, int size);
	void Draw();
	void SetFont(int font, int size);
	void AddItem(CString sItem);
	DWORD GetSelection();
	void SetSelection(DWORD pos);
	void Action(DWORD key );
	void SetActive(DWORD itemNo, BYTE active );
	void SetDrawItemFunc(TYPE_DrawItemFunc func );

	void DrawOnRegion();

	static void SetDefaultColor(TYPE_Window* win);
	static void SetColor(TYPE_Window* win, WORD titleBack, WORD titleText, WORD bodyBack, WORD bodyText, WORD border, WORD shadow, WORD dark, WORD light);

private:
	void SetupTitleAndBody();
	int CalcItemHeight();
	int CalcItemsOnScreen();

	TYPE_Window* m_pWinData;
	CString m_sTitle;
	OSDRegion* m_pRegion;
	std::vector<CString> m_sItems;
	bool m_bVisible;
};
