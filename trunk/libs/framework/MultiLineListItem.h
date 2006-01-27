#ifndef framework_multilinelistitem_h
#define framework_multilinelistitem_h
#include "listitem.h"

class MultiLineListItem :
	public ListItem
{
public:
	MultiLineListItem(const string& sText, ListPage* pPage);
	virtual ~MultiLineListItem(void);
	virtual void DrawSubItem(int iColumn, Rect rcBounds);

private:
	string m_sText;

};
#endif