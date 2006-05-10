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
#ifndef _listitem_18CE3693_FC3F_4d4b_A9E0_66CD4F632AE1_h
#define _listitem_18CE3693_FC3F_4d4b_A9E0_66CD4F632AE1_h

#include <stdlib.h>
#include "tap.h"
#include "Rect.h"
#include "tapstring.h"
#define LIST_MAX_COLUMNS 5


class ListPage;
class ListItem;
class string;

class ListItem
{
public:
	ListItem(ListPage* pParentList, dword dwFlags); 
	virtual ~ListItem();

	virtual dword OnKey( dword key, dword extKey );
	virtual int MeasureSubItem(short int iColumn, Rect rcBounds);
	virtual void DrawSubItem(short int iColumn, Rect rcBounds);
	virtual string GetFooterText();
	virtual byte GetFontSize(short int iColumn);

protected:
	void DrawSubItemString(short int iColumn, Rect rcBounds, const char* text);
	ListPage* m_theList;

private:
	dword m_dwFlags;
};

#endif