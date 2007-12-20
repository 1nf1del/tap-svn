/*
	Copyright (C) 2006 Simon Capewell

	This file is part of the TAPs for Topfield PVRs project.
		http://tap.berlios.de/

	This is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	The software is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this software; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef __LOADEDTAPPAGE_H
#define __LOADEDTAPPAGE_H

#include "ListPage.h"


class LoadedTAPPage : public ListPage
{
public:
	LoadedTAPPage();
	~LoadedTAPPage();

	virtual void OnIdle();
	virtual dword OnKey( dword key, dword extKey );

private:
	static dword ExitTapKeyPress(ListPage* page, ListItem* item, dword key, dword extKey);
	static dword CommitOrder(ListPage* page, ListItem* item, dword key, dword extKey);

	// List item for displaying a running TAP
	class TAPListItem :
		public ListItem
	{
	public:
		TAPListItem(ListPage* pParentList, unsigned int index) :
			ListItem(pParentList),
			m_index(index),
			m_closing(false)
		{
		}

		void EnableTAP( bool enable );
		bool IsEmpty();
		int GetIndex() { return m_index; }

		virtual dword OnKey( dword key, dword extKey );
		virtual string GetFooterText();
		virtual void DrawSubItem(short int iColumn, Rect rcBounds);

	private:
		unsigned int m_index;
		bool m_closing;
	};
};


#endif
