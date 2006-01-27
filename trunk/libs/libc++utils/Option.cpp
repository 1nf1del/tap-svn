#include ".\option.h"
#include <ctype.h>
#include "ini.h"
#include "Options.h"
#include "OptionUpdateNotifier.h"
#include "KeyDef.h"
#ifdef WIN32
#include <crtdbg.h>
#else
#define _ASSERT(x)
#endif



Option::Option(Options* pContainer, const string& key, const string& defaultvalue, const string& name, const string& description, OptionUpdateNotifier* pNotifier)
{
	m_key = key;
	m_defaultvalue = defaultvalue;
	m_value = "<UNKNOWN>";
	m_pContainer = pContainer;
	m_name = name;
	m_description = description;
	m_pNotifier = pNotifier;
}

Option::~Option(void)
{
	delete m_pNotifier;
}

const string& Option::GetValue() const
{
	if (m_value == "<UNKNOWN>")
		ReadValue();

	return m_value;
}

bool Option::SetValue(const string& value)
{
	if (m_choices.contains(value))
	{
		m_value = value;
		WriteValue();
		OnUpdate();

		return true;
	}
	return false;
}

void Option::OnUpdate() const
{
	if (m_pNotifier)
		m_pNotifier->OnUpdate(this);
}

int Option::ValueAsInteger() const
{
	if (GetValue().size() && isdigit(GetValue()[0]))
		return atoi(GetValue());

	_ASSERT(false);
	return 0;
}

bool Option::ValueAsBoolean() const
{
	if (GetValue().compareNoCase("yes")==0)
		return true;
	if (GetValue().compareNoCase("no")==0)
		return false;

	_ASSERT(false);
	return false;
}

const array<string>& Option::GetChoices() const
{
	return m_choices;
}

void Option::ReadValue() const
{
	m_value = m_pContainer->ReadValue(m_key);
	if (m_value.empty())
		m_value = m_defaultvalue;
}

void Option::WriteValue() const
{
	m_pContainer->WriteValue(m_key, m_value);
}


const string& Option::GetName() const
{
	return m_name;
}

const string& Option::GetDescription() const
{
	return m_description;
}

string Option::GetUniqueId() const
{
	return m_key;
}

bool Option::ImmediateUpdateNeeded() const
{
	return false;
}

KeyDef Option::ValueAsKeyDef() const
{
	return KeyDef();
}