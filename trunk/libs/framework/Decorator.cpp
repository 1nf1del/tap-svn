#include ".\decorator.h"
#include "graphics.c"
#include "TextTools.h"
#include "Tapplication.h"

Decorator::Decorator()
{
	m_wRegion = GetTAPScreenRegion();
}

Decorator::~Decorator()
{
}

void Decorator::DrawButtonBackground(int x, int y, int w, int h, bool bSel)
{
	TAP_Osd_PutGd( m_wRegion, x, y, bSel ? &_bigkeygreenGd : &_bigkeyblueGd, FALSE );
}

void Decorator::DrawDialogFrame(int x, int y, int w, int h)
{
    TAP_Osd_PutGd( m_wRegion, x, y, &_popup360x180Gd, TRUE );
}

void Decorator::PrintText(int x, int y, int maxX, const char* text, int fontSize, colorSets whichArea)
{
	word textColor = 0;
	switch (whichArea)
	{
	case headerColors:
	case normalColors:
		textColor = MAIN_TEXT_COLOUR;
		break;
	case highlightColors:
		textColor = COLOR_Yellow;
		break;
	}

	PrintTextImpl(x,y,maxX, text, fontSize, textColor, 0);
}

void Decorator::DrawProgress(int x, int y, int w, int h, int percent)
{
	DrawProgressImpl(x,y,w,h, percent, HEADING_TEXT_COLOUR, TITLE_COLOUR);
}

void Decorator::PrintTextImpl(int x, int y, int maxX, const char* text, int fontSize, word textColor, word bgColor)
{
	PrintCenter( m_wRegion, x, y, maxX, text, textColor, bgColor, fontSize );
}

void Decorator::DrawProgressImpl(int x, int y, int w, int h, int percent, word doneColor, word todoColor)
{
	int split = (w * percent) / 100;

	if (split>0)
		TAP_Osd_FillBox( m_wRegion, x, y, split, h,  doneColor);
	if (split<w)
		TAP_Osd_FillBox ( m_wRegion, x+split, y, w-split, h,  todoColor);
}
