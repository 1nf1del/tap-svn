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