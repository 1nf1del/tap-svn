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

#include "StdAfx.h"
#include ".\regions.h"
#include "OSDRegion.h"

DWORD Regions::m_lut[256];
BYTE Regions::m_LenLut = 0;
FrameBuffer Regions::m_FrameBuffer;

const int UserRegionStartNum =  (SYSOSD_ServiceStatus + 1);

Regions::Regions(void) 
{
	CDC* pDC = GetDC();
	pDC->FillSolidRect(0,0,720,576, TRANSPARENT_COLOUR);
	ReleaseDC();
	m_FrameBuffer.SetBackingBitmap(GetBitmap());
	m_bHasDrawnViaDC = false;
	m_pGetWRegion = new OSDRegion(0,0,720,576,0,0,0, this, true);
}

Regions::~Regions(void)
{
	delete m_pGetWRegion;
}

bool Regions::AreAnyWindowsUpdated()
{
	for (unsigned int i=0; i<m_Regions.size(); i++)
	{
		if (m_Regions[i])
		{
			if (m_Regions[i]->AreAnyWindowsUpdated())
				return true;
		}
	}

	return false;
}

bool Regions::IsDirty()
{
	return m_bHasDrawnViaDC || m_FrameBuffer.IsDirty() || AreAnyWindowsUpdated() ;
}

bool Regions::Draw(CDC* pDC)
{
	if (!IsDirty())
		return false;

	LogInfo("Drawing OSD Regions...");

	if (AreAnyWindowsUpdated())
	{
		for (unsigned int i=0; i<m_Regions.size(); i++)
		{
			if (m_Regions[i])
			{
				m_Regions[i]->DrawToBitmap();
			}
		}
	}

	m_FrameBuffer.CopyBackModifiedPages();

	CDC* pThisDC = GetDC();	

	pDC->BitBlt(0,0, 720, 576, pThisDC, 0, 0, SRCCOPY);
	ReleaseDC();
	m_bHasDrawnViaDC = false;
	return true;
}

int Regions::Create( DWORD x, DWORD y, DWORD w, DWORD h, BYTE lutIdx, int flag )
{
	if ((flag & OSD_Flag_256) || (flag & OSD_Flag_Plane2))
	{
		LogUnimplemented("Cannot Create OSD Region with 2 or 256 colours");
		return 0;
	}

	m_Regions.push_back(new OSDRegion(x,y,w,h,lutIdx,flag, m_Regions.size() + UserRegionStartNum, this,
		flag == OSD_Flag_MemRgn));
	return (int)(m_Regions.size() - 1 + UserRegionStartNum);
}

OSDRegion* Regions::Find(WORD index)
{
	return m_Regions[index - UserRegionStartNum];
}

void Regions::Destroy(WORD index)
{
	index -= UserRegionStartNum;
	delete m_Regions[index];
	m_Regions[index] = 0;
}

int Regions::SetLut(BYTE lutIdx, BYTE lut[][4] )
{
	m_LenLut = lutIdx;
	memcpy(m_lut, lut, 1024);
	return 0;
}

int Regions::Zoom(int xzoom, int yzoom )
{
	return 0;
}

int Regions::GetBaseInfo(TYPE_OsdBaseInfo *info )
{
	return m_FrameBuffer.GetBaseInfo(info);
}

DWORD* Regions::GetLut()
{
	return &m_lut[0];
}

int Regions::GetLutLen()
{
	return m_LenLut;
}

FrameBuffer& Regions::GetFrameBuffer()
{
	return m_FrameBuffer;
}


int Regions::GetW(const char *str, BYTE fntType, BYTE fntSize )
{
	return m_pGetWRegion->GetW(str, fntType, fntSize);
}

void Regions::InvalidateFrameBuffer()
{
	m_FrameBuffer.Invalidate();
	m_bHasDrawnViaDC = true;
}