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
#ifndef libcpputils_options_h
#define libcpputils_options_h
#include <stdlib.h>
#include "tap.h"
#include "tapstring.h"
#include "taparray.h"
class Option;
#include "OptionUpdateNotifier.h"
class IniFile;

class Options
{
public:
	Options(const string& sIniFile);
	~Options(void);

	const string& GetIniFile() const;
	bool Add(Option* pOption);
	// query by 'section:key'
	Option* GetOption(const string& name);
	array<Option*> GetAllOptions();
	void BroadcastDefaultValues();
	string ReadValue(const string& sKey);
	void WriteValue(const string& sKey, const string& sValue);
	void SaveValues();

private:

	string m_sIniFile;
	array<Option*> m_options;
	array<string> m_names;
	IniFile* m_pIniFile;
};
#endif