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

#ifndef __AUTOSTARTPAGE_H
#define __AUTOSTARTPAGE_H

#include "ListPage.h"


class AutoStartPage : public ListPage
{
public:
	AutoStartPage();
	~AutoStartPage();

	virtual void OnOpen();
	virtual dword OnKey( dword key, dword extKey );

private:
	void PopulateList();
	void EnableTAP( unsigned int index, bool enable );

	static dword Commit(ListPage* page, ListItem* item, dword key, dword extKey);
	static dword Backup(ListPage* page, ListItem* item, dword key, dword extKey);
	static dword Restore(ListPage* page, ListItem* item, dword key, dword extKey);

	class TAPListItem :
		public ListItem
	{
	public:
		TAPListItem(ListPage* pParentList, unsigned int index) :
			ListItem(pParentList),
			m_index(index)
		{
		}
		virtual string GetFooterText();
		virtual void DrawSubItem(short int iColumn, Rect rcBounds);

	private:
		unsigned int m_index;
	};

	struct AutoStartTAP
	{
		int index;
		int spare;
		string filename;
		dword id;
		string name;
		string footer;
		bool enabled;

		bool operator==(const AutoStartTAP& right) const
		{
			return spare == right.spare;
		}
		bool operator<(const AutoStartTAP& right) const
		{
			return spare < right.spare;
		}
	};

	array<AutoStartTAP> m_taps;
	int m_dirty;

	friend class TAPListItem;
};


#endif
