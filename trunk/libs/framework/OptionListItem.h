#ifndef framework_optionlistitem_h
#define framework_optionlistitem_h
#include "listitem.h"
class Option;

class OptionListItem :
	public ListItem
{
public:
	OptionListItem(ListPage* pParentList, Option* pOption);
	virtual ~OptionListItem(void);

	virtual dword OnKey( dword key, dword extKey );
	virtual void DrawSubItem(int iColumn, Rect rcBounds);

private:
	void ChangeValue(int iOffset);
	void UpdateOption();

	Option* m_option;
	int m_index;
	int m_startIndex;
};
#endif