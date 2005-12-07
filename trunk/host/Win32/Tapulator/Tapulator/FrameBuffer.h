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

class FrameBuffer
{
public:
	FrameBuffer();
	~FrameBuffer(void);

	int GetBaseInfo(TYPE_OsdBaseInfo* info);
	void SetBackingBitmap(CBitmap* backingBitmap);
	int ExceptionFilter(unsigned int code, struct _EXCEPTION_POINTERS *ep);
	void CopyBackModifiedPages();
	void Invalidate();
	bool IsDirty();

private:
	bool IsBufferUpToDate();
	void Initialize();
	bool IsInBlock(char* pBase, int iSize, char* pCheckPtr);
	int HandleAccess(char* pBase, char* pAddr, DWORD dwReadWrite);
	char* GetPageStart(char* pMemPtr);
	void CopyInBitmapData();
	void OffSetToXY(int iOffset, bool bEvenLines, int& iX, int& iY);
	char* XYToPtr(int iX, int iY);
	char* GetBitData();
	void CopyBackModifiedPage(char* pPage);
	void CopyBackScanLine(int j);

	bool m_bBufferUpToDate;
	CBitmap* m_pBackingBitmap;
	char* m_pEvenLines;
	char* m_pOddLines;
	int m_iPageSize;
	std::set<char*> m_setModifiedPages;
};
