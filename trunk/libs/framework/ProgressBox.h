#pragma once
#include "dialogbox.h"

class ProgressBox :
	public DialogBox
{
public:
	ProgressBox(char* title, char* line1, char* line2);
	virtual ~ProgressBox(void);
	virtual void CreateDialog();

	void UpdateProgress(int percent);
};
