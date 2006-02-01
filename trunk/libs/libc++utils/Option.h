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
#ifndef libcpputils_option_h
#define libcpputils_option_h
#include <stdlib.h>
#include "tap.h"
#include "tapstring.h"
#include "taparray.h"
class Option;
class Options;
class OptionUpdateNotifier;
class KeyDef;

class Option
{
public:
	Option(Options* pContainer, const string& key, const string& defaultvalue, const string& name, const string& description, OptionUpdateNotifier* pNotifier = NULL);
	virtual ~Option(void);

	const string& GetValue() const;
	const string& GetName() const;
	const string& GetDescription() const;
	bool SetValue(const string& value);
	virtual void OnUpdate() const;
	virtual int ValueAsInteger() const;
	virtual bool ValueAsBoolean() const;
	virtual KeyDef ValueAsKeyDef() const;
	const array<string>& GetChoices() const;
	string GetUniqueId() const;
	virtual bool ImmediateUpdateNeeded() const;

protected:
	array<string> m_choices;

private:
	void ReadValue() const;
	void WriteValue() const;

	OptionUpdateNotifier* m_pNotifier;
	string m_key;
	mutable string m_value;
	string m_defaultvalue;
	string m_name;
	string m_description;
	Options* m_pContainer;
};

#endif