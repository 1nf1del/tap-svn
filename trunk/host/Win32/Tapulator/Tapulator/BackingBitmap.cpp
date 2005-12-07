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
#include ".\backingbitmap.h"

BackingBitmap::BackingBitmap(void)
{
	m_pDC = NULL;
	m_pBitmap = NULL;
	m_iDCCount = 0;
}

BackingBitmap::~BackingBitmap(void)
{
	delete m_pDC;
	delete m_pBitmap;
}

CDC* BackingBitmap::GetDC()
{
	if (m_pDC == 0)
	{
		HDC hScreenDC = ::GetDC(NULL);
		CDC screenDC;
		screenDC.Attach(hScreenDC);

		m_pDC = new CDC();
		m_pDC->CreateCompatibleDC(&screenDC);

		if (m_pBitmap == NULL)
		{
			m_pBitmap = new CBitmap();
			m_pBitmap->CreateCompatibleBitmap(&screenDC, 720, 576);
		}

		m_pDC->SelectObject(*m_pBitmap);
		InvalidateFrameBuffer();
	}
	m_iDCCount++;

	return m_pDC;
}

void BackingBitmap::ReleaseDC()
{
	m_iDCCount--;
	if (m_iDCCount ==0)
	{
		delete m_pDC;
		m_pDC = 0;
	}
}

CBitmap* BackingBitmap::GetBitmap()
{
	return m_pBitmap;
}