#include ".\decorator.h"
#include "graphics.c"
#include "TextTools.h"
#include "Tapplication.h"
#include "Rect.h"

Decorator::Decorator()
{
	m_wRegion = GetTAPScreenRegion();
}

Decorator::~Decorator()
{
}

void Decorator::DrawButtonBackground(short int  x, short int  y, short int  w, short int  h, bool bSel)
{
	(h);
	(w);
	TAP_Osd_PutGd( m_wRegion, x, y, bSel ? &_bigkeygreenGd : &_bigkeyblueGd, FALSE );
}

void Decorator::DrawDialogFrame(short int  x, short int  y, short int  w, short int  h)
{
	(h);
	(w);
    TAP_Osd_PutGd( m_wRegion, x, y, &_popup360x180Gd, TRUE );
}

word GetTextColor(colorSets whichArea)
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

	return textColor;

}

void Decorator::PrintText(short int  x, short int  y, short int  maxX, const char* text, byte  fontSize, colorSets whichArea)
{
	PrintTextImpl(x,y,maxX, text, fontSize, GetTextColor(whichArea), 0);
}

void Decorator::PrintTextLeft(short int x, short int y, short int maxX, const char* text, byte fontSize, colorSets whichArea)
{
	PrintTextLeftImpl( x, y, maxX, text, fontSize, GetTextColor(whichArea), 0 );
}

void Decorator::DrawColoredButon(short int x, short int y, short int w, short int h, word fillColor)
{
	Rect r(x,y,w,h);
	r.Fill(m_wRegion, fillColor);
	r.DrawBox(m_wRegion, 1, COLOR_White);
}

void Decorator::DrawProgress(short int  x, short int  y, short int  w, short int  h, short int  percent)
{
	DrawProgressImpl(x,y,w,h, percent, HEADING_TEXT_COLOUR, TITLE_COLOUR);
}

void Decorator::PrintTextImpl(short int  x, short int  y, short int  maxX, const char* text, byte  fontSize, word textColor, word bgColor)
{
	PrintCenter( m_wRegion, x, y, maxX, text, textColor, bgColor, fontSize );
}

void Decorator::PrintTextLeftImpl(short int  x, short int  y, short int  maxX, const char* text, byte  fontSize, word textColor, word bgColor)
{
	PrintLeft( m_wRegion, x, y, maxX, text, textColor, bgColor, fontSize );
}

void Decorator::DrawProgressImpl(short int  x, short int  y, short int  w, short int  h, short int  percent, word doneColor, word todoColor)
{
	short int  split = (w * percent) / 100;

	if (split>0)
		TAP_Osd_FillBox( m_wRegion, x, y, split, h,  doneColor);
	if (split<w)
		TAP_Osd_FillBox ( m_wRegion, x+split, y, w-split, h,  todoColor);
}
