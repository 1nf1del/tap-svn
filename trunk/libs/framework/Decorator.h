#pragma once

#include "tap.h"
#include "colors.h"
class Decorator
{
public:
	Decorator();
	virtual ~Decorator(void);
	virtual void DrawButtonBackground(short int x, short int y, short int w, short int h, bool bSel);
	virtual void DrawDialogFrame(short int x, short int y, short int w, short int h);
	virtual void PrintText(short int x, short int y, short int maxX, const char* text, byte fontSize, colorSets whichArea);
	virtual void DrawProgress(short int x, short int y, short int w, short int h, short int percent);
	virtual void PrintTextLeft(short int x, short int y, short int maxX, const char* text, byte fontSize, colorSets whichArea);
	virtual void DrawColoredButon(short int x, short int y, short int w, short int h, word fillColor);

protected:
	void PrintTextImpl(short int x, short int y, short int maxX, const char* text, byte fontSize, word textColor, word bgColor);
	void DrawProgressImpl(short int x, short int y, short int w, short int h, short int percent, word doneColor, word todoColor);
	void PrintTextLeftImpl(short int  x, short int  y, short int  maxX, const char* text, byte  fontSize, word textColor, word bgColor);

	word m_wRegion;
};
