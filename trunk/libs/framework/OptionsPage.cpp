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
#include "optionspage.h"
#include "ListColumn.h"
#include "OptionListItem.h"
#include "Options.h"


OptionsPage::OptionsPage(const string& title, Options* pOptions, short int itemHeight, short int headerHeight) : ListPage(LF_SHOW_HEADER | LF_ROW_SEPARATORS | LF_SCROLLBAR|  LF_COLUMN_SEPARATORS 
																	  , Rect(50,75,620,450), itemHeight, headerHeight, 0)
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

