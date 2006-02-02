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
#include ".\epgsourceoption.h"
#include "EPGdata.h"

EPGSourceOption::EPGSourceOption(Options* pContainer, const string& key, const string& name, const string& description, OptionUpdateNotifier* pNotifier) :
Option(pContainer, key, "MEI", name, description, pNotifier)
{
	m_choices.push_back("MEI");
	m_choices.push_back("Jags CSV");
	m_choices.push_back("Builtin");
	m_choices.push_back("Builtin+");
	m_choices.push_back("Auto");
}

EPGSourceOption::~EPGSourceOption()
{

}


int EPGSourceOption::ValueAsInteger() const
{
	const string& value = GetValue();

	if (value == "MEI")
		return Mei;

	if (value == "Builtin")
		return BuiltinEPG;

	if (value == "Jags CSV")
		return JagsCSV;

	if (value == "Auto")
		return Auto;

	if (value == "Builtin+")
		return BuiltinExtendedEPG;

	return Mei;


}

bool EPGSourceOption::ImmediateUpdateNeeded() const
{
	return true;
}
