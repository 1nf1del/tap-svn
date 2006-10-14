/*
	Copyright (C) 2005 Simon Capewell

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

#ifndef __DIALOGBOX_H
#define __DIALOGBOX_H

#include "tap.h"
#include "Dialog.h"
#include "tapstring.h"
class Decorator;
// Position of the Delete Confirmation Screen
#define YESNO_WINDOW_W 360
#define YESNO_WINDOW_H 180
#define YESNO_WINDOW_X ((MAX_SCREEN_X-YESNO_WINDOW_W)/2)   // Centre the yes/no window
#define YESNO_WINDOW_Y ((MAX_SCREEN_Y-YESNO_WINDOW_H)/2)   // Centre the yes/no window

#define YESNO_OPTION_W 102                                  // Width  of yes/no Options Buttons
#define YESNO_OPTION_H 43                                   // Height of yes/no Options Buttons
#define YESNO_OPTION_X  (YESNO_WINDOW_X + 52)              // Starting x-position for first Option button
#define YESNO_OPTION_Y  (YESNO_WINDOW_Y + YESNO_WINDOW_H - YESNO_OPTION_H - 15)             // Starting y-position for Option buttons.
#define YESNO_OPTION_X_SPACE   (YESNO_OPTION_W+50)         // Space between options on yes/no window.


class DialogBox : public Dialog
{
public:
	DialogBox(const char* title, const char* line1, const char* line2, Decorator* pDecorator = NULL);
	virtual ~DialogBox();

	virtual void CreateDialog();
	virtual void DestroyDialog();

	void SetLine1( const string& line ) { line1 = line; }
	void SetLine2( const string& line ) { line2 = line; }

protected:
	void Draw();
	void DisplayLine();

protected:
	string title;
	string line1;
	string line2;
	Decorator* m_pDecorator;

private:
	byte* windowCopy;
};

#endif