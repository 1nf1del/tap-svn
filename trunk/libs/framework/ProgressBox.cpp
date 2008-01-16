/*
Copyright (C) 2005 Robin Glover

This file is part of the TAPs for Topfield PVRs project.
http://tap.berlios.de/

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "progressbox.h"
#include "Tapplication.h"
#include "Decorator.h"

ProgressBox::ProgressBox(const char* title, const char* line1, const char* line2, Decorator* pDecorator)
: DialogBox(title, line1, line2, pDecorator)
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

void ProgressBox::UpdateProgress(short int percent, const char* step1, const char* step2 )
{
	if (step1 || step2)
	{
		if (step1)
			line1 = step1;
		if (step2)
			line2 = step2;
		Draw();
	}

	if (percent>100)
		percent = 100;

	if (percent<0)
		percent = 0;

	m_pDecorator->DrawProgress(YESNO_WINDOW_X + 30, YESNO_WINDOW_Y + 130, 300, 15, percent);

#ifdef WIN32
	TAP_SystemProc(); // repaint on emulator so we can see what we have
#endif
}
