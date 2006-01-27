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
