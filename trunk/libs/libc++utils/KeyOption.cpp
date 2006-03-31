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
#include ".\keyoption.h"
#include "..\..\..\trunk\libs\Firmware\RawKey.h"

KeyOption::KeyOption(Options* pContainer, const string& key, const string& defaultValue, const string& name, const string& description,OptionUpdateNotifier* pNotifier) :
Option(pContainer, key, defaultValue, name, description, pNotifier)
{
	m_choices.push_back("Menu");
	m_choices.push_back("List");
	m_choices.push_back("White");
	m_choices.push_back("Recall");
	m_choices.push_back("Guide");
}

KeyOption::~KeyOption(void)
{
}

KeyDef KeyOption::ValueAsKeyDef() const 
{
	return GetKeyCodes(GetValue());
}

KeyDef KeyOption::GetKeyCodes(const string& sDesc)
{
	if (sDesc == "Menu")
		return KeyDef(RKEY_Menu, RAWKEY_Menu);

	if (sDesc == "List")
		return KeyDef(RKEY_TvRadio, RAWKEY_List_TVRad);

	if (sDesc == "White")
		return KeyDef(RKEY_Ab, RAWKEY_White_Ab);

	if (sDesc == "Recall")
		return KeyDef(RKEY_Recall, RAWKEY_Recall);

	if (sDesc == "Guide")
		return KeyDef(RKEY_Guide, RAWKEY_Guide);

	return KeyDef(0xFFFFFFFF,0xFFFFFFFF);
}
