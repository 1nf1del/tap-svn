#include ".\dialogbox.h"

#include "../../../trunk/libs/framework/Tapplication.h"
#include "TextTools.h"
#include "graphics.c"


DialogBox::DialogBox(char* title, char* line1, char* line2)
{
	this->title		= title;
	this->line1		= line1;
	this->line2		= line2;
}

DialogBox::~DialogBox(void)
{
}

void DialogBox::CreateDialog()
{
	// Store the currently displayed screen area where we're about to put our pop-up window on.
    windowCopy = TAP_Osd_SaveBox(GetTAPScreenRegion(), YESNO_WINDOW_X, YESNO_WINDOW_Y, YESNO_WINDOW_W, YESNO_WINDOW_H);

    // Display the pop-up window.
    TAP_Osd_PutGd( GetTAPScreenRegion(), YESNO_WINDOW_X, YESNO_WINDOW_Y, &_popup360x180Gd, TRUE );

    // Display Title and information in pop-up window
	PrintCenter( GetTAPScreenRegion(), YESNO_WINDOW_X+5, YESNO_WINDOW_Y +  13, YESNO_WINDOW_X+YESNO_WINDOW_W-5, title, MAIN_TEXT_COLOUR, 0, FNT_Size_1926 );
	PrintCenter( GetTAPScreenRegion(), YESNO_WINDOW_X+5, YESNO_WINDOW_Y +  56, YESNO_WINDOW_X+YESNO_WINDOW_W-5, line1, MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );
	PrintCenter( GetTAPScreenRegion(), YESNO_WINDOW_X+5, YESNO_WINDOW_Y +  89, YESNO_WINDOW_X+YESNO_WINDOW_W-5, line2, MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );
}


//-----------------------------------------------------------------------
//
void DialogBox::DestroyDialog()
{
	TAP_Osd_RestoreBox(GetTAPScreenRegion(), YESNO_WINDOW_X, YESNO_WINDOW_Y, YESNO_WINDOW_W, YESNO_WINDOW_H, windowCopy);
	TAP_MemFree(windowCopy);
}

