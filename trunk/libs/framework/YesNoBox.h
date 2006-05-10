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

#ifndef __YESNOBOX_H
#define __YESNOBOX_H

#include "tap.h"
#include "DialogBox.h"
class Decorator;

class YesNoBox : public DialogBox
{
public:
	static int Show(const char* title, const char* line1, const char* line2, const char* button1, const char* button2 = 0, char defaultOption = 0, Decorator* pDecorator = 0);
	template <class D> static int Show(const char* title, const char* line1, const char* line2, const char* button1, const char* button2 = 0, char defaultOption = 0)
	{
		Show(title, line1, line2, button1, button2, defaultOption, new D());
	}


	YesNoBox(const char* title, const char* line1, const char* line2, const char* button1, const char* button2, char defaultOption, Decorator* pDecorator = NULL);
	virtual ~YesNoBox();

	virtual dword OnKey( dword key, dword extKey );
	virtual void CreateDialog();

private:
	void DisplayLine();

private:
	char  option;
	bool  result;
	string  button1;
	string  button2;
};


#endif
