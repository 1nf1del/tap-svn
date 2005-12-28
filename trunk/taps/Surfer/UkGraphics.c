/************************************************************
	    Part of the ukEPG project
	This module displays our graphics

Name	: UkGraphics.c
Author	: Darkmatter
Version	: 0.0
For		: Topfield TF5x00 series PVRs
Descr.	:
Licence	:
Usage	:
History	: v0.0 Darkmatter: 02-08-05 	Inception date

	Last change:  USE   4 Aug 105    3:59 pm
**************************************************************/

#include "graphics/top.GD"
#include "graphics/side.GD"

#include "graphics/Highlight.GD"
#include "graphics/rowA.GD"

#include "graphics/MenuRow.GD"
#include "graphics/MenuRow_Highlight.GD"
#include "graphics/Menu_Title.GD"

#include "graphics/smallGreenBar.GD"
#include "graphics/NotAvailableBar.GD"
#include "graphics/RadioServiceBar.GD"
#include "graphics/PlainRedBar.GD"

#include "graphics/Highlight448x34.GD"
#include "graphics/NarrowHighlight.GD"


#define FILL_COLOUR COLOR_Black
#define MAIN_TEXT_COLOUR RGB(29,29,29)
#define TITLE_COLOUR RGB(18,18,18)
#define INFO_COLOUR RGB(18,18,18)
//#define EVENT_FILL_COLOUR ARGB(1, 0, 0, 16)  // Original colour defined by darkmatter.
#define EVENT_FILL_COLOUR COLOR_User3          // Different colour chosen by kidhazy to improve readability.


//------------
//
void DrawGraphicBoarders(void)
{
    TAP_Osd_FillBox( rgn, 0, 0, 720, 576, FILL_COLOUR );				// clear the screen
    TAP_Osd_PutGd( rgn, 0, 0, &_topGd, TRUE );							// draw top graphics
    TAP_Osd_PutGd( rgn, 0, 0, &_sideGd, TRUE );							// draw left side graphics
	TAP_Osd_PutGd( rgn, 672, 0, &_sideGd, TRUE );						// draw right side graphics
}


//------------
//
void PrintCenter( word windowRgn, dword x, dword y, dword maxX, const char *str, dword fcolor, dword bcolor, byte fntSize)
{
	dword width, offset;

	width = TAP_Osd_GetW( str, 0, fntSize );

	if ( width <= (maxX-x) ) offset = (maxX - x - width)/2;				// centralise text
	else offset = 5;													// too wide - fill width

	switch ( fntSize )
	{
		case FNT_Size_1419 : if ( bcolor > 0 ) TAP_Osd_FillBox( windowRgn, x, y-1, maxX-x, 21, bcolor );
							 TAP_Osd_PutStringAf1419( windowRgn, x+offset, y, maxX, str, fcolor, bcolor);
							 break;
							
		case FNT_Size_1622 : if ( bcolor > 0 ) TAP_Osd_FillBox( windowRgn, x, y-1, maxX-x, 24, bcolor );
							 TAP_Osd_PutStringAf1622( windowRgn, x+offset, y, maxX, str, fcolor, bcolor);
							 break;
							 
		case FNT_Size_1926 : if ( bcolor > 0 ) TAP_Osd_FillBox( windowRgn, x, y-1, maxX-x, 28, bcolor );
							 TAP_Osd_PutStringAf1926( windowRgn, x+offset, y, maxX, str, fcolor, bcolor);
							 break;
							 
		default : break;
	}

	
}


