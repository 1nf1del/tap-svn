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
#include "../../../trunk/libs/framework/Dialog.h"

class YesNoBox : public Dialog
{
public:
	YesNoBox(char* title, char* line1, char* line2, char* button1, char* button2, int defaultOption);
	virtual ~YesNoBox();

	virtual dword OnKey( dword key, dword extKey );
	virtual void CreateDialog();
	virtual void DestroyDialog();

private:
	void DisplayLine();

private:
	byte* windowCopy;
	char* title;
	char* line1;
	char* line2;
	char  option;
	bool  result;
	char  button1[10];
	char  button2[10];
//	YesNoCallback _callback;
};


#endif
