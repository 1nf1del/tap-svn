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
#include ".\framebuffer.h"

FrameBuffer::FrameBuffer(void)
{
	m_pEvenLines = NULL;
	m_pOddLines = NULL;
	m_pBackingBitmap = NULL;
	m_bBufferUpToDate = false;

	SYSTEM_INFO si;
	ZeroMemory(&si, sizeof(SYSTEM_INFO));
	GetSystemInfo(&si);
	m_iPageSize = si.dwPageSize;
}

FrameBuffer::~FrameBuffer(void)
{
	VirtualFree(m_pEvenLines, 0, MEM_RELEASE);
	VirtualFree(m_pOddLines, 0, MEM_RELEASE);
}

void FrameBuffer::SetBackingBitmap(CBitmap* backingBitmap)
{
	m_pBackingBitmap = backingBitmap;
}

int FrameBuffer::GetBaseInfo(TYPE_OsdBaseInfo* info)
{
	if (m_pEvenLines == NULL)
		Initialize();

	info->bytePerPixel = 2;
	info->dataFormat = OSD_1555;
	info->height = 576;
	info->width = 720;
	info->eAddr = m_pEvenLines;
	info->oAddr = m_pOddLines;

	return 1;
}

void FrameBuffer::Initialize()
{
	m_pEvenLines = (char*) VirtualAlloc(NULL, 720*576, MEM_COMMIT, PAGE_NOACCESS);
	m_pOddLines = (char*) VirtualAlloc(NULL, 720*576, MEM_COMMIT, PAGE_NOACCESS);
}

bool FrameBuffer::IsInBlock(char* pBase, int iSize, char* pCheckPtr)
{
	if (pCheckPtr<pBase)
		return false;
	if (pCheckPtr>pBase + iSize)
		return false;

	return true;
}

int FrameBuffer::ExceptionFilter(unsigned int code, struct _EXCEPTION_POINTERS *ep)
{
	if (ep->ExceptionRecord->ExceptionCode != EXCEPTION_ACCESS_VIOLATION)
		return EXCEPTION_CONTINUE_SEARCH;

	if ((m_pEvenLines == NULL) || (m_pOddLines == NULL))
		return EXCEPTION_CONTINUE_SEARCH;

	char* pMemAddr = (char*) ep->ExceptionRecord->ExceptionInformation[1];
	if (IsInBlock(m_pEvenLines, 720*576, pMemAddr))
		return HandleAccess(m_pEvenLines, pMemAddr, ep->ExceptionRecord->ExceptionInformation[0]);

	if (IsInBlock(m_pOddLines, 720*576, pMemAddr))
		return HandleAccess(m_pOddLines, pMemAddr, ep->ExceptionRecord->ExceptionInformation[0]);

	return EXCEPTION_CONTINUE_SEARCH;
}

char* FrameBuffer::GetPageStart(char* pMemPtr)
{
	return (char*)(((DWORD)pMemPtr / m_iPageSize) * m_iPageSize);
}

void FrameBuffer::OffSetToXY(int iOffset, bool bEvenLines, int& iX, int& iY)
{
	iY = (iOffset / (720 * 2)) * 2 + (bEvenLines ? 0 : 1); // assumption -- even 0 before odd 1
	iX = (iOffset % (720 * 2)) / 2;
}

char* FrameBuffer::XYToPtr(int iX, int iY)
{
	char* pBase = (iY % 2) ? m_pOddLines : m_pEvenLines;
	pBase += (720 * 2) * (iY / 2);
	pBase += iX * 2;
	return pBase;

}

char* FrameBuffer::GetBitData()
{
	if (m_pBackingBitmap == NULL)
		return NULL;

	char* pBuffer = (char*) malloc(720 * 576 * 2);

	BITMAPINFO bmInfo;
	ZeroMemory(&bmInfo, sizeof(BITMAPINFO));
	bmInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmInfo.bmiHeader.biWidth = 720;
	bmInfo.bmiHeader.biHeight = -576;
	bmInfo.bmiHeader.biPlanes = 1;
	bmInfo.bmiHeader.biBitCount = 16;
	bmInfo.bmiHeader.biCompression = BI_RGB;

	CDC dc;
	dc.CreateCompatibleDC(NULL);

	if (GetDIBits(dc, (HBITMAP)m_pBackingBitmap->m_hObject, 0, 576, pBuffer, &bmInfo, DIB_RGB_COLORS) != 576)
	{
		free(pBuffer);
		return NULL;
	}

	return pBuffer;
}


void FrameBuffer::CopyInBitmapData()
{
	char* pData = GetBitData();
	if (pData == NULL)
		return;

	char* pBase = pData;
	DWORD dwOld;
	VirtualProtect(m_pEvenLines, 720*576, PAGE_READWRITE, &dwOld);
	VirtualProtect(m_pOddLines, 720*576, PAGE_READWRITE, &dwOld);
	for (int j=0; j<576; j++)
	{
		char* pDest = XYToPtr(0, j);
		memcpy(pDest, pData, 720 * 2);
		pData += 720*2;
	}
	VirtualProtect(m_pEvenLines, 720*576, PAGE_READONLY, &dwOld);
	VirtualProtect(m_pOddLines, 720*576, PAGE_READONLY, &dwOld);

	m_bBufferUpToDate = true;
	free (pBase);
}


int FrameBuffer::HandleAccess(char* pBase, char* pAddr, DWORD dwReadWrite)
{
	DWORD dwOldAccess;
	char* pPage = GetPageStart(pAddr);
	if (dwReadWrite == 0)
	{
		CopyInBitmapData();
	}
	else
	{
		if (!IsBufferUpToDate())
			CopyInBitmapData();

		VirtualProtect(pPage, m_iPageSize, PAGE_READWRITE, &dwOldAccess);
		m_setModifiedPages.insert(pPage);
	}
	
	return EXCEPTION_CONTINUE_EXECUTION;
}

void FrameBuffer::CopyBackScanLine(int j)
{
	char* pData = XYToPtr(0, j);

	BITMAPINFO bmInfo;
	ZeroMemory(&bmInfo, sizeof(BITMAPINFO));
	bmInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmInfo.bmiHeader.biWidth = 720;
	bmInfo.bmiHeader.biHeight = -576;
	bmInfo.bmiHeader.biPlanes = 1;
	bmInfo.bmiHeader.biBitCount = 16;
	bmInfo.bmiHeader.biCompression = BI_RGB;

	CDC dc;
	dc.CreateCompatibleDC(NULL);

	if (SetDIBits(dc, (HBITMAP)m_pBackingBitmap->m_hObject, 575-j, 1, pData, &bmInfo, DIB_RGB_COLORS) != 1)
		return; // argh

	return;
}

void FrameBuffer::CopyBackModifiedPage(char* pPage)
{
	bool bEvenLines = (pPage - m_pEvenLines) < 720 * 576;
	
	int iOffset = pPage - (bEvenLines ? m_pEvenLines : m_pOddLines);
	int iXStart, iYStart;
	OffSetToXY(iOffset, bEvenLines, iXStart, iYStart);

	int iXEnd, iYEnd;
	OffSetToXY(iOffset + m_iPageSize, bEvenLines, iXEnd, iYEnd);

	if (iXEnd == 0)
		iYEnd-=2;

	for (int j = iYStart; j<=iYEnd; j+=2)
	{
		CopyBackScanLine(j);
	}
}

void FrameBuffer::CopyBackModifiedPages()
{
	std::set<char*>::iterator it;
	for (it = m_setModifiedPages.begin(); it!= m_setModifiedPages.end(); it++)
	{
		CopyBackModifiedPage(*it);
	}
	m_setModifiedPages.clear();
}

void FrameBuffer::Invalidate()
{
	CopyBackModifiedPages();
	DWORD dwOld;
	VirtualProtect(m_pEvenLines, 720*576, PAGE_NOACCESS, &dwOld);
	VirtualProtect(m_pOddLines, 720*576, PAGE_NOACCESS, &dwOld);
	m_bBufferUpToDate = false;
}

bool FrameBuffer::IsDirty()
{
	return !m_setModifiedPages.empty();
}

bool FrameBuffer::IsBufferUpToDate()
{
	return m_bBufferUpToDate;
}