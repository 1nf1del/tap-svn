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
#include "infologger.h"

struct BlockInfo
{
	void *pData;
	int iAllocNum;
	int iSize;

	bool operator<(const BlockInfo& other) const
	{
		return this->iAllocNum > other.iAllocNum; // deliberate > so higher nums come first in sort
	}
};

class Heap :
	public InfoLogger
{
	friend class CInfoDialog;
public:
	Heap(void);
	~Heap(void);

	static Heap* GetTheHeap();
	static void DoneWithHeap();

	void* Allocate(int iSize);
	void Release(void* pData);
	void MemInfo(DWORD *heapSize, DWORD *freeHeapSize, DWORD *availHeapSize);
	void DumpLeaks();

private:

	unsigned int m_iTotalBytes;
	unsigned int m_iMaxBytes;
	std::map<void*, BlockInfo> m_mapLiveBlocks;
	unsigned int m_iAllocNum;
	unsigned int m_iOutstandingAllocs;
	int m_iLargestAlloc;
	unsigned int m_iHeapSize;

	static Heap* m_spTheHeap;
};
