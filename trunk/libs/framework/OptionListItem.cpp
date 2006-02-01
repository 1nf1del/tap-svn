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
#include ".\optionlistitem.h"
#include "option.h"
#include "morekeys.h"
#include "Page.h"

OptionListItem::OptionListItem(ListPage* pParentList, Option* pOption) : m_option(pOption), ListItem(pParentList, 0)
{
	m_index = pOption->GetChoices().find(pOption->GetValue());
	m_startIndex = m_index;
}

OptionListItem::~OptionListItem(void)
{
	UpdateOption();
}

void OptionListItem::ChangeValue(int iOffset)
{
	if (m_option->GetChoices().size() == 0)
		return;

	m_index = ( m_index + iOffset + m_option->GetChoices().size() ) % m_option->GetChoices().size();

	if (m_option->ImmediateUpdateNeeded())
		UpdateOption();
}

void OptionListItem::UpdateOption()
{
	if (m_startIndex != m_index)
		m_option->SetValue(m_option->GetChoices()[m_index]);

	m_startIndex = m_index;
}


dword OptionListItem::OnKey( dword key, dword extKey )
{
	HandleExtKey(RKEY_VolUp, RAWKEY_Right, ChangeValue(1));
	HandleExtKey(RKEY_VolDown, RAWKEY_Left, ChangeValue(-1));
	return key;
}

void OptionListItem::DrawSubItem(int iColumn, Rect rcBounds)
{
	if (iColumn == 0)
		DrawSubItemString(0, rcBounds, m_index>-1 ? m_option->GetChoices()[m_index] : string("????") );

	if (iColumn == 1)
		DrawSubItemString(1, rcBounds, m_option->GetName());
}

