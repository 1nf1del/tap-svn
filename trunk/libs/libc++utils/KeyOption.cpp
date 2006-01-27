#include ".\keyoption.h"
#include "..\..\..\trunk\libs\Firmware\RawKey.h"

KeyOption::KeyOption(Options* pContainer, const string& key, const string& defaultValue, const string& name, const string& description,OptionUpdateNotifier* pNotifier) :
Option(pContainer, key, defaultValue, name, description, pNotifier)
{
	m_choices.push_back("Menu");
	m_choices.push_back("List");
	m_choices.push_back("White");
	m_choices.push_back("Recall");
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

	return KeyDef(-1,-1);
}
