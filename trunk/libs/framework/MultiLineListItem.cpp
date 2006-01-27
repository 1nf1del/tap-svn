/*
Copyright (C) 2006 Robin Glover

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
#include ".\multilinelistitem.h"
#include "ListPage.h"

MultiLineListItem::MultiLineListItem(const string& sText, ListPage* pPage) : m_sText(sText), ListItem(pPage, 0)
{
}

MultiLineListItem::~MultiLineListItem(void)
{
}

void MultiLineListItem::DrawSubItem(int iColumn, Rect rcBounds)
{
	if (iColumn == 0)
	{
		ListColors theColors = m_theList->GetColorDef(normalColors);
		rcBounds.PutMultiLineString(m_theList->GetRegionIndex(), m_sText, theColors.textColor, theColors.bgColor, FNT_Size_1622);
	}

}
