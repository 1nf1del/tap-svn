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

#include "listpage.h"
#include "ListColumn.h"
#include "Tapplication.h"
#include "../Firmware/RawKey.h"
#include "YesNoBox.h"
#include "Logger.h"

word ListPage::m_osdRegionIndex = 0;
int ListPage::m_iPageCount = 0;

ListPage::ListPage(dword dwFlags, Rect rcPosition, int itemHeight,  int headerHeight, int footerHeight)
{
	TRACE_MEMORY();
	m_dwFlags = dwFlags;
	m_rcList = rcPosition;
	m_itemHeight = itemHeight;
	m_headerHeight = headerHeight;
	m_footerHeight = footerHeight;

	m_fsBody = FNT_Size_1622;
	m_fsHeader = FNT_Size_1926;
	m_fsFooter = FNT_Size_1419;

	m_firstItemInView = 0;
	m_columnCount = 0;

	m_selectedItem = 0;
	m_bWidthsCalculated = false;

	m_iPageCount++;
}

ListPage::~ListPage()
{
	TRACE("Started destroying list page\n");
	for (unsigned int i = 0; i<m_items.size(); i++)
		delete m_items[i];

	TRACE("Deleted items\n");

	for (unsigned int i = 0; i<m_columnCount; i++)
		delete m_columnInfo[i];

	TRACE("Deleted columns\n");

	m_iPageCount--;
	if (m_iPageCount == 0)
	{
		if (m_osdRegionIndex != 0)
			TAP_Osd_Delete(m_osdRegionIndex);
		m_osdRegionIndex = 0;
	}

	TRACE("Finished destroying list page\n");
}

int ListPage::DrawFrame(Rect drawRect, colorSets whichColors)
{
	if (m_osdRegionIndex == 0)
		return 0;

	ListColors theColors = GetColorDef(whichColors);

	drawRect.Fill(m_osdRegionIndex, theColors.bgColor);
	drawRect.DrawBox(m_osdRegionIndex, 2, theColors.frameColor);

	return 2;
}

void ListPage::DrawHeaderItem(int iColumn, Rect drawRect)
{
	if (m_osdRegionIndex == 0)
		return;

	ListColors theColors = GetColorDef(headerColors);

	drawRect.PutString(m_osdRegionIndex, m_columnInfo[iColumn]->GetText(), theColors.textColor, theColors.bgColor,
		m_fsHeader, (m_columnInfo[iColumn]->GetFlags() & LCF_CENTER_HEADER_TEXT)!=0);
}

void ListPage::DrawFooter()
{
	Rect drawRect = FooterRect();

	if (m_osdRegionIndex == 0)
		return;

	int iWidth = DrawFooterFrame(drawRect);
	drawRect.Shrink(iWidth);

	ListItem* pSelItem = ListPage::GetSelectedItem();
	if (pSelItem == 0)
		return;

	string sFooterText = pSelItem->GetFooterText();
	if (sFooterText.empty())
		return;

	ListColors theColors = GetColorDef(normalColors);

	drawRect.PutMultiLineString(m_osdRegionIndex, sFooterText, theColors.textColor, theColors.bgColor, m_fsFooter);
}

ListColors ListPage::GetColorDef(enum colorSets whichSet)
{
	return Tapplication::GetTheApplication()->GetColorDef(whichSet);
}


bool ListPage::AddColumn(ListColumn* pNewColumn)
{
	int i=m_columnCount;

	if (i == LIST_MAX_COLUMNS)
		return false;

	if (m_items.size() > 0)
		return false;

	m_columnInfo[i] = pNewColumn;
	m_columnCount++;

	return true;
}

void ListPage::AddItem(ListItem* pNewItem)
{
	unsigned int i=m_items.size();
	
	m_items.push_back(pNewItem);

}

void ListPage::CalcRealColumnWidths()
{
	int iTotPercent = 0;
	int iWidth = ItemsRect().w;
	int iRemain = iWidth;

	for (int i=0; i<m_columnCount; i++)
	{
		iTotPercent += m_columnInfo[i]->GetWidth();
	}

	for (int i=0; i<m_columnCount-1; i++)
	{
		m_columnInfo[i]->SetWidth((m_columnInfo[i]->GetWidth() * (iWidth)) / iTotPercent);
		iRemain -= m_columnInfo[i]->GetWidth();
	}
	
	m_columnInfo[m_columnCount-1]->SetWidth(iRemain);
	m_bWidthsCalculated = true;
}


bool ListPage::Show()
{
	if (m_osdRegionIndex == 0)
	{
		Rect rcScreen(0,0,720, 576);
		m_osdRegionIndex = rcScreen.CreateRegion(true);
		TRACE1("Created OSD region, index = %d\n", m_osdRegionIndex);
	}

	if (m_osdRegionIndex == 0)
		return false;

	return Draw();
}

int ListPage::CountItemsInView()
{
	int screenHeight = ItemsRect().h;

	return screenHeight / m_itemHeight;
}

void ListPage::DrawItem(Rect rcItems, unsigned int iIndex)
{
	// crashes in here
	int i = iIndex - m_firstItemInView;
	unsigned int iThisItem = i + m_firstItemInView;
	ListItem* pThisItem = 0;
	if (iThisItem < m_items.size())
		pThisItem = m_items[iThisItem];

	rcItems.h = m_itemHeight;
	Rect rcSubItem = rcItems;
	rcSubItem.y += i * m_itemHeight;

	for (int j = 0; j< m_columnCount; j++)
	{
		enum colorSets whatColors = iThisItem == m_selectedItem ? highlightColors : normalColors;
		rcSubItem.w = m_columnInfo[j]->GetWidth();
		int iBorderW = m_columnInfo[j]->DrawBackground(rcSubItem, whatColors);
		if (pThisItem)
		{
			Rect rcContent = rcSubItem;
			rcContent.Shrink(iBorderW);
			pThisItem->DrawSubItem(j, rcContent);
		}
		rcSubItem.x += m_columnInfo[j]->GetWidth();
	}

	rcItems.y += m_itemHeight;
}


void ListPage::DrawVisibleItems()
{
	Rect rcItems = ItemsRect();
	int itemCount = ListPage::CountItemsInView();

//	TRACE("Got items count\n");
	for (int i=0; i<itemCount; i++)
	{
		DrawItem(rcItems, i + m_firstItemInView);
//		TRACE1("Drawn item %d\n", i); 
	}
}

void ListPage::DrawHeader()
{
	Rect r = HeaderRect();
	int iFrameWidth = DrawHeaderFrame(r);
	r.Shrink(iFrameWidth);
	if (m_dwFlags & LF_COLUMNS_IN_HEADER)
	{
		Rect rcSubItem = r;
		for (int j = 0; j< m_columnCount; j++)
		{
			rcSubItem.w = m_columnInfo[j]->GetWidth();
			DrawHeaderItem(j, rcSubItem);
			rcSubItem.x += m_columnInfo[j]->GetWidth();
		}
	}
	else
	{
		DrawHeaderItem(0, r);
	}
}

Rect ListPage::PageRect()
{
	Rect r = m_rcList;
	r.x = r.y = 0; // relative to region

	return r;
}
Rect ListPage::FooterRect()
{
	Rect r = PageRect();
	r.y = r.h - m_footerHeight;
	r.h = m_footerHeight;
	return r;
}

Rect ListPage::HeaderRect()
{
	Rect r = PageRect();
	r.h = m_headerHeight;
	return r;
}

Rect ListPage::ScrollBarRect()
{
	Rect r = ItemsRect();
	r.x=0;
	r.w = SCROLLBAR_WIDTH;
	return r;
}

Rect ListPage::ItemsRect()
{
	Rect r = PageRect();

	if (m_dwFlags & LF_SHOW_HEADER)
	{
		r.y += m_headerHeight;
		r.h -= m_headerHeight;
	}
	else
	{
		r.y +=2;
		r.h -=2;
	}

	if (m_dwFlags & LF_SHOW_FOOTER)
		r.h -= m_footerHeight;
	
	if (m_dwFlags & LF_SCROLLBAR)
	{
		r.x += SCROLLBAR_WIDTH;
		r.w -= SCROLLBAR_WIDTH;
	}
	else
	{
		r.x += 2;
		r.w -= 2;
	}

	return r;
}

void ListPage::DrawFooterAndScrollBar()
{
	if (m_dwFlags & LF_SCROLLBAR)
		DrawScrollBar();

	if (m_dwFlags & LF_SHOW_FOOTER)
		DrawFooter();
}

bool ListPage::Draw()
{
	TRACE("Drawing page\n");
	if (!m_bWidthsCalculated)
		CalcRealColumnWidths();

	TRACE("Got column widths\n");
	DrawFrame(PageRect());
	TRACE("Drawn Frames\n");

	if (m_dwFlags & LF_SHOW_HEADER)
		DrawHeader();
	TRACE("Drawn Header\n");

	DrawVisibleItems();
	TRACE("Drawn Items\n");

	DrawFooterAndScrollBar();
	TRACE("Drawn footer/scroll bar\n");

	TAP_Osd_Copy(m_osdRegionIndex, GetTAPScreenRegion(), 0, 0, m_rcList.w, m_rcList.h, m_rcList.x, m_rcList.y, false);
	TRACE("Copied memory image to screen\n");

	return true;
}

ListItem* ListPage::GetSelectedItem()
{
	if (m_selectedItem >= (int)m_items.size())
		return 0;

	return m_items[m_selectedItem];
}

void ListPage::CalcFirstInView(int iOffset)
{
	int iItemsInView = CountItemsInView();

	if (m_selectedItem < m_firstItemInView)
	{
		if (abs(iOffset) == 1)
			m_firstItemInView = m_selectedItem;
		else
		{
			m_firstItemInView = max(0,m_firstItemInView + iOffset);
		}
	}
	else if (m_selectedItem >= iItemsInView + m_firstItemInView)
	{
		if (abs(iOffset) == 1)
		{
			if (m_selectedItem >= m_firstItemInView + iItemsInView)
				m_firstItemInView = m_selectedItem - iItemsInView + 1;
		}
		else
		{
			m_firstItemInView = min((int)m_items.size()-1, m_firstItemInView + iOffset);
		}
	}

}

dword ListPage::MoveSelection(int iOffset, bool bWrap)
{
	int iOldSel = m_selectedItem;
	int iOldFirst = m_firstItemInView;

	if (bWrap)
	{
		m_selectedItem = (m_selectedItem + m_items.size() + iOffset) % m_items.size();
	}
	else
	{
		int newPos = m_selectedItem + iOffset;
		m_selectedItem = (unsigned int) min(max((newPos),0),(int)(m_items.size()-1));
	}

	CalcFirstInView(iOffset);

	if (abs(iOffset) == 1)
	{
		PartialDraw(iOldSel, iOldFirst);
	}
	else
	{
		Draw();
	}

	return 0;
}

void ListPage::OnIdle()
{

}

dword ListPage::OnKey( dword key, dword extKey )
{
	HandleKey(RKEY_Exit, Close());
	HandleExtKey(RKEY_ChUp, RAWKEY_Up, MoveSelection(-1));
	HandleExtKey(RKEY_ChDown, RAWKEY_Down, MoveSelection(1));
	HandleKey(RKEY_Forward, MoveSelection(CountItemsInView(), false));
	HandleKey(RKEY_Rewind, MoveSelection(-CountItemsInView(), false));

	ListItem* pSelItem = ListPage::GetSelectedItem();
	if (pSelItem)
	{
		dword dwRet = pSelItem->OnKey(key, extKey);
		if ((dwRet == 0) && Tapplication::GetTheApplication()->IsTopPage(this))
			Draw();
		return dwRet;
	}

	return key;
}

void ListPage::OnOpen()
{
	TRACE("ListPage::OnOpen About to show the list page\n");
	if(!Show())
	{
		TRACE("Failed to show the page\n");
		YesNoBox ynb("Sorry!", "Failed to create the page", "Please Report this bug", "Ok", "Ok", 0);
		ynb.DoModal();
		Close();
	}
	TRACE("Page displayed OK.\n");
}

void ListPage::OnClose()
{
	TRACE("ListPage::OnClose\n");
}


void ListPage::SetFontSizes(byte fsHeader, byte fsBody, byte fsFooter)
{
	m_fsBody = fsBody;
	m_fsHeader = fsHeader;
	m_fsFooter = fsFooter;

}

int ListPage::DrawHeaderFrame(Rect drawRect)
{
	if (m_osdRegionIndex == 0)
		return 0;

	ListColors theColors = GetColorDef(headerColors);

	drawRect.Fill(m_osdRegionIndex, theColors.bgColor);
	drawRect.DrawBox(m_osdRegionIndex, 2, theColors.frameColor);

	return 2;
}

int ListPage::DrawFooterFrame(Rect drawRect)
{
	return DrawFrame(drawRect);
}

int ListPage::DrawBodyFrame(Rect drawRect)
{
	return DrawFrame(drawRect);
}

word ListPage::GetRegionIndex()
{
	return m_osdRegionIndex;
}
byte ListPage::GetBodyFontSize()
{
	return m_fsBody;
}

unsigned short int ListPage::GetDefaultItemHeight()
{
	return m_itemHeight;
}

dword ListPage::GetFlags()
{
	return m_dwFlags;
}

void ListPage::Redraw()
{
	Draw();
}

void ListPage::DrawScrollBar()
{
	Rect r = ScrollBarRect();
	DrawFrame(r, scrollBarColors);

	int iItemCount = m_items.size() ? m_items.size() : 1;
	int iBarHeight = r.h;
	int iBarSize = (CountItemsInView() * iBarHeight)/iItemCount;
	iBarSize = max(iBarSize,6);
	iBarSize = min(iBarSize,iBarHeight);
	int iBarPos = (m_firstItemInView * iBarHeight) / iItemCount;

	r.y += iBarPos;
	r.h = iBarSize;
	r.x += 2;
	r.w = SCROLLBAR_WIDTH - 4;

	r.Fill(m_osdRegionIndex, GetColorDef(scrollBarColors).textColor);
}

void ListPage::PartialDraw(int iOldSel, int iOldFirstInView)
{
	Rect r = ItemsRect();

	int iDiff = m_firstItemInView - iOldFirstInView;
	if (iDiff !=0)
	{
		if (abs(iDiff)>1)
		{
			Draw();
			return;
		}

		int iH = (CountItemsInView()-1) * m_itemHeight;
		int ySrc = r.y;
		int yDst = r.y;

		if (iDiff == 1)
			ySrc += m_itemHeight;
		else
			yDst += m_itemHeight;
		
		TAP_Osd_Copy(m_osdRegionIndex, m_osdRegionIndex, r.x, ySrc, r.w, iH, r.x, yDst, false);
	}

	DrawItem(r, m_selectedItem);
	DrawItem(r, iOldSel);

	DrawFooterAndScrollBar();

	TAP_Osd_Copy(m_osdRegionIndex, GetTAPScreenRegion(), 0, 0, m_rcList.w, m_rcList.h, m_rcList.x, m_rcList.y, false);
}

dword ListPage::GetColumnFlags(int i) const
{
	if (i<0) 
		return 0;
	if (i>=m_columnCount)
		return 0;

	return m_columnInfo[i]->GetFlags();
}