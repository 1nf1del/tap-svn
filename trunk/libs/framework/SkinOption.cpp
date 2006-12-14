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
#include "skinoption.h"
#include "Tapplication.h"
#include "Logger.h"
#include "DirectoryUtils.h"

SkinOption::SkinOption(Options* pContainer) :
Option(pContainer, "Skin", "Default", "Select Skin Colours (*.mcf)", "", NULL)
{
	m_choices.push_back("Default");
	m_bOldFolder = false;

	array<string> skins = GetFilesInFolder("/ProgramFiles/Settings/Skins", ".mcf");
	if (skins.size() == 0)
	{
		skins = GetFilesInFolder("/ProgramFiles", ".mcf");
		m_bOldFolder = true;
	}

	for (unsigned int i=0; i<skins.size(); i++)
	{
		m_choices.push_back(skins[i].substr(0,skins[i].size()-4));
	}
}

SkinOption::~SkinOption(void)
{
}

void SkinOption::OnUpdate() const
{
	string value = GetValue();
	Tapplication* pTap = Tapplication::GetTheApplication();

	// load the default colors first, so that a failure to load the skin will
	// leave the system viewable at least...
	pTap->SetDefaultColors();
	if (value == "Default")
	{
//		pTap->SetDefaultColors();
	}
	else
	{
		if (m_bOldFolder)
			pTap->LoadSkin("/ProgramFiles/" + value + ".mcf");
		else
			pTap->LoadSkin("/ProgramFiles/Settings/Skins/" + value + ".mcf");
	}
}

bool SkinOption::ImmediateUpdateNeeded() const
{
	return true;
}