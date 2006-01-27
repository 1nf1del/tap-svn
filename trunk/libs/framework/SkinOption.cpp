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
#include ".\skinoption.h"
#include "Tapplication.h"
#include "Logger.h"

SkinOption::SkinOption(Options* pContainer) :
Option(pContainer, "Skin", "Default", "Select Skin Colours (*.mcf)", "", NULL)
{
	m_choices.push_back("Default");

	TYPE_File file;
	int iCount = TAP_Hdd_FindFirst(&file);

	do
	{
		string sName = file.name;
		sName = sName.tolower();
		if (sName.size()<=4)
			continue;
		if (sName.find(".mcf")!=sName.size()-4)
			continue;


		sName = file.name;
		sName = sName.substr(0,sName.size()-4);
		m_choices.push_back(sName);
	} while (TAP_Hdd_FindNext(&file)<(dword)iCount);

}

SkinOption::~SkinOption(void)
{
}

void SkinOption::OnUpdate() const
{
	string value = GetValue();
	Tapplication* pTap = Tapplication::GetTheApplication();

	if (value == "Default")
	{
		pTap->SetDefaultColors();
	}
	else
	{
		pTap->LoadSkin(value + ".mcf");
	}
}

bool SkinOption::ImmediateUpdateNeeded() const
{
	return true;
}