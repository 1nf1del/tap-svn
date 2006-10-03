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
#include "options.h"
#include "option.h"
#include "inifile.h"

Options::Options(const string& sIniFile)
{
	m_sIniFile = sIniFile;
	m_pIniFile = new IniFile(m_sIniFile);
}

Options::~Options(void)
{
	for (unsigned int i=0; i<m_options.size(); i++)
		delete m_options[i];

	SaveValues();
	delete m_pIniFile;
}

const string& Options::GetIniFile() const
{
	return m_sIniFile;
}

bool Options::Add(Option* pOption)
{
	string name = pOption->GetUniqueId();

	if (m_names.contains(name))
		return false;

	m_names.push_back(name);
	m_options.push_back(pOption);

	return true;
}

Option* Options::GetOption(const string& name)
{
	int iIndex = m_names.find(name);
	if (iIndex == -1)
		return NULL;

	return m_options[iIndex];
}

array<Option*> Options::GetAllOptions()
{
	return m_options;
}

void Options::BroadcastDefaultValues()
{
	for (unsigned int i=0; i<m_options.size(); i++)
		m_options[i]->OnUpdate();

}

void Options::WriteValue(const string& sKey, const string& sValue)
{
	m_pIniFile->SetValue(sKey, sValue);
}

string Options::ReadValue(const string& sKey)
{
	string result;
	m_pIniFile->GetValue(sKey, result);
	return result;
}

void Options::SaveValues()
{
	m_pIniFile->Save(m_sIniFile);
}