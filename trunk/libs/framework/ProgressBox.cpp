#include ".\progressbox.h"
#include "Tapplication.h"

ProgressBox::ProgressBox(const char* title, const char* line1, const char* line2)
: DialogBox(title, line1, line2)
{
}

ProgressBox::~ProgressBox(void)
{
}

void ProgressBox::CreateDialog()
{
	DialogBox::CreateDialog();
	UpdateProgress(0);

}

void ProgressBox::UpdateProgress(int percent, const string& sStep)
{
	if (!sStep.empty())
	{
		line1=sStep;
		Draw();
	}

	if (percent>100)
		percent = 100;

	if (percent<0)
		percent = 0;

	TAP_Osd_FillBox( GetTAPScreenRegion(), YESNO_WINDOW_X + 30, YESNO_WINDOW_Y + 130,
		percent*3, 15,  HEADING_TEXT_COLOUR);
	TAP_Osd_FillBox ( GetTAPScreenRegion(), YESNO_WINDOW_X + 30 + percent * 3, YESNO_WINDOW_Y + 130,
		(100-percent)*3, 15,  TITLE_COLOUR);

#ifdef WIN32
	TAP_SystemProc(); // repaint on emulator so we can see what we have
#endif
}
