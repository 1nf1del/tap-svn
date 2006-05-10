#include ".\skindecorator.h"
#include "Tapplication.h"
#include "Rect.h"
#include "colors.h"

SkinDecorator::SkinDecorator(void)
{
}

SkinDecorator::~SkinDecorator(void)
{
}

void SkinDecorator::DrawDialogFrame(short int x, short int y, short int w, short int h)
{
	Rect rect(x,y,w,h);
	Tapplication* pApp = Tapplication::GetTheApplication();

	ListColors cols = pApp->GetColorDef(normalColors);
	ListColors hcols = pApp->GetColorDef(headerColors);

	rect.Fill(m_wRegion, cols.bgColor);
	Rect r2 = rect;
	r2.h = 50;
	r2.Fill(m_wRegion, hcols.bgColor);
	rect.DrawBox(m_wRegion, 2, cols.frameColor);

	//DrawCorner(x+2,y+2, cols.frameColor,3);
	//DrawCorner(x+w-4,y+2, cols.frameColor,2);
	//DrawCorner(x+2,y+h-4, cols.frameColor,1);
	//DrawCorner(x+w-2,y+h-2, cols.frameColor,0);
}

void SkinDecorator::DrawCorner(short int x, short int y, word colour, byte direction)
{
	for (int i=0; i<4; i++)
	{
		if (i==direction)
			continue;
		TAP_Osd_PutPixel(m_wRegion, x+i%2,y+i/2,colour);
	}
}

void SkinDecorator::DrawButtonBackground(short int x, short int y, short int w, short int h, bool bSel)
{
	Tapplication* pApp = Tapplication::GetTheApplication();
	ListColors cols = pApp->GetColorDef(bSel ? highlightColors : normalColors);
	Rect rect(x,y,w,h);
	rect.Fill(m_wRegion, cols.bgColor);
	rect.DrawBox(m_wRegion, 2, cols.frameColor);
}

void SkinDecorator::PrintText(short int x, short int y, short int maxX, const char* text, byte fontSize, colorSets whichColors)
{
	Tapplication* pApp = Tapplication::GetTheApplication();
	ListColors cols = pApp->GetColorDef(whichColors);
	PrintTextImpl(x+2,y,maxX-2, text, fontSize, cols.textColor, cols.bgColor);

}

void SkinDecorator::DrawProgress(short int x, short int y, short int w, short int h, short int percent)
{
	Tapplication* pApp = Tapplication::GetTheApplication();
	ListColors hcols = pApp->GetColorDef(highlightColors);
	ListColors cols = pApp->GetColorDef(normalColors);

	Rect r(x-2,y-2, w+4, h+4);
	r.DrawBox(m_wRegion, 2, cols.frameColor);
	DrawProgressImpl(x,y,w,h,percent, hcols.bgColor, cols.bgColor);
}
