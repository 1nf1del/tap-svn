/*
Copyright (C) 2005 Robin Glover

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

#include "dialog.h"
#include "Tapplication.h"

Dialog::Dialog(void)
{
	m_iModalResult = Dialog_Running;
}

Dialog::~Dialog(void)
{
}

int Dialog::DoModal()
{
	CreateDialog();
	Tapplication::GetTheApplication()->SetActiveDialog(this);
	do
	{
		TAP_SystemProc();
	} while (m_iModalResult == Dialog_Running);
	Tapplication::GetTheApplication()->SetActiveDialog(NULL);
	DestroyDialog();
	return m_iModalResult;
}

dword Dialog::OnKey( dword key, dword extKey )
{
	(extKey);
	if (key == RKEY_Exit)
		m_iModalResult = Dialog_Exited;
	return key;
}

void Dialog::EndDialog(unsigned int iResult)
{
	m_iModalResult = iResult;
	return;
}

void Dialog::CreateDialog()
{

}

void Dialog::DestroyDialog()
{

}