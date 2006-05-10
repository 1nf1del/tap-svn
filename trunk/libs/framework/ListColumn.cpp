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

#include "listcolumn.h"
#include "ListPage.h"

ListColumn::ListColumn(ListPage* pParentList, short int percentWidth, dword dwFlags, const char* pHeaderText)
{
	m_theList = pParentList;
	m_width = percentWidth;
	m_dwFlags = dwFlags;
	m_text = pHeaderText;
}


short int ListColumn::DrawBackground(Rect rcBounds, enum colorSets whatColors)
{
	// need to know if selected!
	ListColors theColors = m_theList->GetColorDef(whatColors);

//	if (whatColors != normalColors)
	rcBounds.Fill(m_theList->GetRegionIndex(), theColors.bgColor);

	if (m_theList->GetFlags() & LF_ROW_SEPARATORS)
	{
		rcBounds.DrawLowerBorder(m_theList->GetRegionIndex(), 2, theColors.frameColor);
	}
	if (m_theList->GetFlags() & LF_COLUMN_SEPARATORS)
	{
		rcBounds.DrawRightBorder(m_theList->GetRegionIndex(), 2, theColors.frameColor);
	}

	return 1;
}


const string& ListColumn::GetText() const
{
	return m_text;
}

short int ListColumn::GetWidth()
{
	return m_width;
}

void ListColumn::SetWidth(short int iWidth)
{
	m_width = iWidth;
}

dword ListColumn::GetFlags() const
{
	return m_dwFlags;
}