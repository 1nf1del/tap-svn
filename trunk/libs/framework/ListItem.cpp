/*
Copyright (C) 2005 Robin Glover

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

#include "ListItem.h"
#include "ListPage.h"
#include "Logger.h"
#include "ListColumn.h"
ListItem::ListItem(ListPage* pParentList, dword dwFlags)
{
	m_dwFlags = dwFlags;
	m_theList = pParentList;
}

ListItem::~ListItem()
{
}

void ListItem::DrawSubItem(int iColumn, Rect rcBounds)
{
}

void ListItem::DrawSubItemString(int iColumn, Rect rcBounds, const char* text)
{
	ListColors theColors = m_theList->GetColorDef( 
		m_theList->GetSelectedItem() == this ? highlightColors : normalColors);
	rcBounds.y-=1;
	rcBounds.PutString(m_theList->GetRegionIndex(), text, theColors.textColor, theColors.bgColor
		, GetFontSize(iColumn), m_theList->GetColumnFlags(iColumn) & LCF_CENTER_TEXT);
}

int ListItem::GetFontSize(int iColumn)
{
	return m_theList->GetBodyFontSize();
}

dword ListItem::OnKey( dword key, dword extKey )
{
	return key;
}

int ListItem::MeasureSubItem(int iColumn, Rect rcBounds)
{
	return m_theList->GetDefaultItemHeight();
}

string ListItem::GetFooterText()
{
	return "";
}