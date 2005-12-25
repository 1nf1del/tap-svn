#include ".\dialog.h"
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