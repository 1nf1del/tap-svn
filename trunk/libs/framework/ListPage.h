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
#ifndef _ListPage_h
#define _ListPage_h

#include <stdlib.h>
#include "tap.h"
#include "listitem.h"
#include "colors.h"
#include "taparray.h"
#include "Page.h"

class ListColumn;

#define LF_SHOW_HEADER			0x00000001
#define LF_SHOW_FOOTER			0x00000002
#define LF_COLUMNS_IN_HEADER	0x00000004
#define LF_ROW_SEPARATORS		0x00000010
#define LF_SCROLLBAR			0x00000020
#define LF_COLUMN_SEPARATORS	0x00000040
#define LF_ONE_ITEM_ONLY		0x00000080 // pass up/down keys to item

#define SCROLLBAR_WIDTH 8

#define FIXINT2SHORTWARNING(x) ((x)*1)

class ListPage : public Page
{
public:

	ListPage(dword dwFlags, Rect rcPosition, short itemHeight, short headerHeight, short footerHeight);
	~ListPage();

// Page interface overrides
	virtual void OnIdle();
	virtual dword OnKey( dword key, dword extKey );
	virtual void OnOpen();
	virtual void OnClose();
	void DiscardItems();


	bool AddColumn(ListColumn* pNewColumn);
	void AddItem(ListItem* pNewItem);
	void RemoveItem(ListItem* pItem);
	ListItem* GetSelectedItem() const;
	int GetSelectedIndex() const;
	dword MoveSelection(short int iOffset, bool bWrap = true);

	bool Show();

	virtual ListColors GetColorDef(enum colorSets whichSet);
	virtual void Redraw();
	virtual string GetFooterText();
	virtual void OnItemAboutToDelete(ListItem* pItem);

	word GetRegionIndex();
	byte GetBodyFontSize();
	void SetFontSizes(byte fsHeader, byte fsBody, byte fsFooter);
	unsigned short int GetDefaultItemHeight();
	dword GetFlags();
	dword GetColumnFlags(int i) const;

	bool MoveItem(int index, bool down);
	void FlashHighlight();

protected:
	array<ListItem*> m_items;
	virtual void DrawFooterContent(Rect drawRect);

private:
	void CalcRealColumnWidths();
	void IncreaseItemArraySize();
	bool Draw();
	void Destroy();
	void DrawVisibleItems();
	void DrawHeader();
	void DrawFooter();
	void PartialDraw(short int iOldSel, short int iOldFirstInView);
	short int CountItemsInView();

	short int DrawFrame(Rect drawRect, colorSets whichColors = normalColors);
	short int DrawHeaderFrame(Rect drawRect);
	short int DrawFooterFrame(Rect drawRect);
	short int DrawBodyFrame(Rect drawRect);
	void DrawHeaderItem(int iColumn, Rect drawRect);
	void DrawScrollBar();
	Rect ItemsRect();
	Rect PageRect();
	Rect FooterRect();
	Rect HeaderRect();
	Rect ScrollBarRect();
	void DrawItem(Rect rcItems, unsigned short int iIndex);
	void DrawFooterAndScrollBar();
	void CalcFirstInView(short int iOffset);

	dword m_dwFlags;
	Rect m_rcList;
	unsigned short int m_itemHeight;
	unsigned short int m_selectedItem;
	unsigned short int m_firstItemInView;
	byte m_columnCount;
	unsigned short m_headerHeight;
	unsigned short m_footerHeight;
	ListColumn* m_columnInfo[LIST_MAX_COLUMNS];

	static word m_osdRegionIndex;
	static int m_iPageCount;
	byte m_fsHeader;
	byte m_fsBody;
	byte m_fsFooter;
	bool m_bWidthsCalculated;
};


inline int ListPage::GetSelectedIndex() const
{
	return m_selectedItem;
}


#endif