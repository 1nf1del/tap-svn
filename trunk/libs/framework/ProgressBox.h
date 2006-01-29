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

#ifndef __PROGRESSBOX_H
#define __PROGRESSBOX_H


#include "dialogbox.h"

class ProgressBox :
	public DialogBox
{
public:
	ProgressBox(const char* title, const char* line1, const char* line2);
	virtual ~ProgressBox(void);
	virtual void CreateDialog();

	void UpdateProgress(int percent, const char* step1 = NULL, const char* step2 = NULL);
};


#endif
