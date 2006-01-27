#include ".\optionspage.h"
#include "ListColumn.h"
#include "OptionListItem.h"
#include "Options.h"


OptionsPage::OptionsPage(const string& title, Options* pOptions) : ListPage(LF_SHOW_HEADER | LF_ROW_SEPARATORS | LF_SCROLLBAR|  LF_COLUMN_SEPARATORS 
																	  , Rect(50,75,620,450), 50, 50, 0)
{
	m_pOptions = pOptions;
	SetFontSizes(FNT_Size_1926, FNT_Size_1926, FNT_Size_1622);
	AddColumn(new ListColumn(this, 20, LCF_CENTER_HEADER_TEXT|LCF_CENTER_TEXT, title));
	AddColumn(new ListColumn(this, 80, 0, ""));

	if (pOptions)
	{
		const array<Option*>& options = pOptions->GetAllOptions();
		for (unsigned int i=0; i<options.size(); i++)
		{
			AddItem(new OptionListItem(this, options[i]));
		}
	}
}

OptionsPage::~OptionsPage(void)
{
	DiscardItems(); // forces the optionlistitems to write their values to the ini file
	// and write out the inifile
	m_pOptions->SaveValues();
}

dword OptionsPage::OnKey(dword key, dword extKey)
{
	return ListPage::OnKey(key, extKey);
}

