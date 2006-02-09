#pragma once

#include "tap.h"
#include "colors.h"
class Decorator
{
public:
	Decorator();
	virtual ~Decorator(void);
	virtual void DrawButtonBackground(int x, int y, int w, int h, bool bSel);
	virtual void DrawDialogFrame(int x, int y, int w, int h);
	virtual void PrintText(int x, int y, int maxX, const char* text, int fontSize, colorSets whichArea);
	virtual void DrawProgress(int x, int y, int w, int h, int percent);

protected:
	void PrintTextImpl(int x, int y, int maxX, const char* text, int fontSize, word textColor, word bgColor);
	void DrawProgressImpl(int x, int y, int w, int h, int percent, word doneColor, word todoColor);

	word m_wRegion;
};
