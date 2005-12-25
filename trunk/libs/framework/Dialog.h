#pragma once
#include "tap.h"

#define Dialog_Running -1
#define Dialog_Exited 0
#define Dialog_Ok 1
#define Dialog_Cancel 2

class Dialog
{
public:
	Dialog(void);
	virtual ~Dialog(void);

	int DoModal();
	virtual void CreateDialog();
	virtual dword OnKey( dword key, dword extKey );
	virtual void DestroyDialog();

protected:
	void EndDialog(unsigned int iResult);

private:
	int m_iModalResult;
};
