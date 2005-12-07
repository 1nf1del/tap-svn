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

class OSDRegion;
#include "FrameBuffer.h"
#include "InfoLogger.h"
#include "BackingBitmap.h"

class Regions : public InfoLogger, public BackingBitmap
{
public:
	Regions(void);
	~Regions(void);

	bool IsDirty();

	int Create( DWORD x, DWORD y, DWORD w, DWORD h, BYTE lutIdx, int flag );
	OSDRegion* Find(WORD index);
	void Destroy(WORD index);
	bool Draw(CDC* pDC);

	int GetW(const char *str, BYTE fntType, BYTE fntSize );


	static int SetLut(BYTE lutIdx, BYTE lut[][4] );
	static int Zoom(int xzoom, int yzoom );
	static int GetBaseInfo(TYPE_OsdBaseInfo *info );
	static DWORD* GetLut();
	static int GetLutLen();
	static FrameBuffer& GetFrameBuffer();

protected:
	virtual void InvalidateFrameBuffer();
private:

	bool AreAnyWindowsUpdated();

	static DWORD m_lut[256];
	static BYTE m_LenLut;
	static FrameBuffer m_FrameBuffer;
	std::vector<OSDRegion*> m_Regions;
	OSDRegion* m_pGetWRegion;

	bool m_bHasDrawnViaDC;
};
