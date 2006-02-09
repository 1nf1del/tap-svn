#pragma once
#include "decorator.h"

class SkinDecorator :
	public Decorator
{
public:
	SkinDecorator(void);
	virtual ~SkinDecorator(void);

	virtual void DrawButtonBackground(int x, int y, int w, int h, bool bSel);
	virtual void DrawDialogFrame(int x, int y, int w, int h);
	virtual void PrintText(int x, int y, int maxX, const char* text, int fontSize, colorSets whichColors);
	virtual void DrawProgress(int x, int y, int w, int h, int percent);

private:

	void DrawCorner(int x, int y, word colour, byte direction);
};
