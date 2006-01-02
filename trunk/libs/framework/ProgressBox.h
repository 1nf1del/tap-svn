#pragma once
#include "dialogbox.h"

class ProgressBox :
	public DialogBox
{
public:
	ProgressBox(const char* title, const char* line1, const char* line2);
	virtual ~ProgressBox(void);
	virtual void CreateDialog();

	void UpdateProgress(int percent, const string& sStep = "");
};
