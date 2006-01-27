#include ".\optionlistitem.h"
#include "option.h"
#include "..\..\..\trunk\libs\Firmware\RawKey.h"
#include "../../../trunk/libs/framework/Page.h"

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
	if (m_index<0)
		return key;

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

