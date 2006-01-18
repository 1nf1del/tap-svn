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

#ifndef __PAGE_H
#define __PAGE_H

#define HandleKey(keyCode, action)\
	if (key == keyCode) \
	{\
		action;\
		return 0;\
	}

#define HandleExtKey(keyCode, rawKeyCode, action)\
	if (((extKey == 0) && (key == keyCode)) || (extKey == rawKeyCode)) \
	{\
		action;\
		return 0;\
	}

#define HandleExtKeyDef(keyDef, action)\
	if (((extKey == 0) && (key == keyDef.key)) || (extKey == keyDef.extKey)) \
	{\
		action;\
		return 0;\
	}

class Page
{
public:
	Page();
	virtual ~Page();

	void Open();
	void Close();

	virtual void OnIdle();
	virtual dword OnKey( dword key, dword extKey );
	virtual void OnOpen();
	virtual void OnClose();
	virtual void Redraw();
};


#endif
