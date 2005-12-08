//Tapulator - An emulator for the Topfield PVR TAP API
//Copyright (C) 2005  Robin Glover
//
//This program is free software; you can redistribute it and/or
//modify it under the terms of the GNU General Public License
//as published by the Free Software Foundation; either version 2
//of the License, or (at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program; if not, write to the Free Software
//Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
//
// You can contact the author via email at robin.w.glover@gmail.com

#pragma once

class Window;
class OSDRegion;

class Windows
{
public:
	Window* Create(TYPE_Window *win, OSDRegion* region, DWORD x, DWORD y, DWORD w, DWORD h, BYTE save, BYTE bScr);
	Window* Find(TYPE_Window *win);
	void Destroy(TYPE_Window *win);

	Windows();
	~Windows(void);


	std::map<TYPE_Window*, Window*> m_mapWindows;
};