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

#include "StdAfx.h"
#include ".\window.h"
#include "osdregion.h"



Window::Window(TYPE_Window *win, OSDRegion* region, DWORD x, DWORD y, DWORD w, DWORD h, BYTE save, BYTE bScr)
{
	m_pWinData = win;
	m_pWinData->win.x = x;
	m_pWinData->win.y = y;
	m_pWinData->win.w = w;
	m_pWinData->win.h = h;
	m_pWinData->win.save = save;
	m_pWinData->win.rgn = region->GetIndex();
	m_pWinData->bScr = bScr;
	m_pRegion = region;
	m_pRegion->AddWindow(this);

	m_pWinData->itemNum = 0;
	m_pWinData->startPos = 0;
	m_pWinData->currentPos = 0;
	m_pWinData->maxItemView = 0;

	m_bVisible = true;

	SetupTitleAndBody();
}

Window::~Window(void)
{
	m_pRegion->RemoveWindow(this);
}

void Window::SetTitle(CString sTitle, int font, int size)
{
	m_sTitle = sTitle;
	// TODO: font
}

void Window::SetupTitleAndBody()
{
	m_pWinData->win.titleW = m_pWinData->win.bodyW = m_pWinData->win.w;
	m_pWinData->win.titleX = m_pWinData->win.bodyX = m_pWinData->win.x;
	m_pWinData->win.titleY = m_pWinData->win.y;
	m_pWinData->win.titleH = min(m_pWinData->win.h, 30);
	m_pWinData->win.bodyY = m_pWinData->win.y + m_pWinData->win.titleH;
	m_pWinData->win.bodyH = m_pWinData->win.h - m_pWinData->win.titleH;

}

void Window::SetDefaultColor(TYPE_Window* win)
{
	SetColor(win, COLOR_Yellow, COLOR_Black, COLOR_Gray, COLOR_White, COLOR_Cyan, COLOR_Black,
		COLOR_DarkGray, COLOR_White);
}

void Window::SetColor(TYPE_Window* win, WORD titleBack, WORD titleText, WORD bodyBack, WORD bodyText,
					  WORD border, WORD shadow, WORD dark, WORD light)
{
	win->win.color.titleBack = titleBack;
	win->win.color.titleText = titleText;
	win->win.color.bodyBack = bodyBack;
	win->win.color.bodyText = bodyText;
	win->win.color.border = border;
	win->win.color.shadow = shadow;
	win->win.color.dark = dark;
	win->win.color.light = light;
}

void Window::Draw()
{
	m_bVisible = true;
}

void Window::SetFont(int font, int size)
{
	m_pWinData->fntSize = size;
	m_pWinData->fntSize = font;
}

void Window::AddItem(CString sItem)
{
	m_sItems.push_back(sItem);
	int iItemNum = m_pWinData->itemNum;
	m_pWinData->idx[iItemNum] = iItemNum;
	m_pWinData->check[iItemNum] = 0;
	m_pWinData->item[iItemNum] = (char*)m_sItems[iItemNum].GetString();
	m_pWinData->itemNum++;
}

DWORD Window::GetSelection()
{
	return m_pWinData->currentPos;
}

void Window::SetSelection(DWORD pos)
{
	m_pWinData->currentPos = pos;
}

void Window::Action(DWORD key )
{
	switch (key)
	{
	case RKEY_ChUp:
		if (m_pWinData->currentPos>0)
			m_pWinData->currentPos--;
		break;
	case RKEY_ChDown:
		if (m_pWinData->currentPos<m_pWinData->itemNum-1)
			m_pWinData->currentPos++;
		break;
	}

	if (m_pWinData->startPos > m_pWinData->currentPos)
		m_pWinData->startPos = m_pWinData->currentPos;

	if (m_pWinData->currentPos >= m_pWinData->startPos + CalcItemsOnScreen())
		m_pWinData->startPos = m_pWinData->currentPos - CalcItemsOnScreen() + 1;

}

void Window::SetActive(DWORD itemNo, BYTE active )
{
	LogUnimplemented(__FUNCTION__ "(" __FUNCSIG__ ")");
}

void Window::SetDrawItemFunc(TYPE_DrawItemFunc func )
{
	LogUnimplemented(__FUNCTION__ "(" __FUNCSIG__ ")");
}


int Window::CalcItemHeight()
{
	return 20;
}

int Window::CalcItemsOnScreen()
{
	int itemHeight = CalcItemHeight();
	int itemsOnScreen = m_pWinData->win.bodyH / itemHeight;
	if (itemsOnScreen < 1)
		itemsOnScreen = 1;

	return itemsOnScreen;
}

void Window::DrawOnRegion()
{
	if (!m_bVisible)
		return;

	TYPE_WindowColor& colors = m_pWinData->win.color;

	CRect rcTitle(CPoint(m_pWinData->win.titleX, m_pWinData->win.titleY), CSize(m_pWinData->win.titleW, m_pWinData->win.titleH));
	m_pRegion->DrawTextInBox(rcTitle, m_sTitle, colors.titleText, colors.titleBack, colors.border, 0, 1, DT_CENTER|DT_VCENTER);

	CRect rcBody(CPoint(m_pWinData->win.bodyX, m_pWinData->win.bodyY), CSize(m_pWinData->win.bodyW, m_pWinData->win.bodyH));
	m_pRegion->DrawFilledBox(rcBody, m_pWinData->win.color.bodyBack, m_pWinData->win.color.border);

	int itemsOnScreen = CalcItemsOnScreen();
	int itemHeight = CalcItemHeight();

	for (int i=0 ; i<itemsOnScreen ; i++)
	{	
		int iIndex = i + m_pWinData->startPos;
		if (iIndex >= m_pWinData->itemNum)
			break;
		int yOffs = i * itemHeight;
		CRect rcBox(CPoint(m_pWinData->win.bodyX, m_pWinData->win.bodyY + yOffs),
			CSize(m_pWinData->win.bodyW, itemHeight));
			
		m_pRegion->DrawTextInBox(rcBox, m_sItems[iIndex], colors.bodyText,
			iIndex == m_pWinData->currentPos ? colors.check : colors.bodyBack, colors.border, 0,  1, DT_LEFT|DT_VCENTER);
	}
}