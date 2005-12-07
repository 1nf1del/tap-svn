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
#include ".\heap.h"
#include <algorithm>

Heap* Heap::m_spTheHeap = NULL;

Heap::Heap(void)
{
	m_iMaxBytes = 0;
	m_iTotalBytes = 0;
	m_iAllocNum = 0;
	m_iOutstandingAllocs = 0;
	m_iLargestAlloc = 0;
	m_iHeapSize = 32 * 1024 * 1024;
}

Heap::~Heap(void)
{
}

Heap* Heap::GetTheHeap()
{
	if (m_spTheHeap == NULL)
		m_spTheHeap = new Heap();

	return m_spTheHeap;
}

void Heap::DoneWithHeap()
{
	delete m_spTheHeap;
}

void* Heap::Allocate(int iSize)
{
	void* pData = malloc(iSize);
	m_iAllocNum++;

	BlockInfo bi;
	bi.iAllocNum = m_iAllocNum;
	bi.pData = pData;
	bi.iSize = iSize;

	m_mapLiveBlocks[pData] = bi;
	m_iOutstandingAllocs ++;
	m_iTotalBytes += iSize;
	if (m_iTotalBytes>m_iMaxBytes)
		m_iMaxBytes = m_iTotalBytes;
	m_iLargestAlloc = max(m_iLargestAlloc, iSize);

	LogVerbose("MemAlloc - allocated %d bytes at 0x%x alloc #%d", iSize, pData, m_iAllocNum);
	if (m_iTotalBytes > m_iHeapSize)
		LogError("MemAlloc - over maximum heap size by %d bytes", m_iTotalBytes - m_iHeapSize);

	return pData;
}

void Heap::Release(void *pData)
{
	if (pData == NULL)
		return;

	if (m_mapLiveBlocks.find(pData) == m_mapLiveBlocks.end())
	{
		LogError("MemFree - released pointer at 0x%x that wasn't allocated!", pData);
		ASSERT(false);
		return;
	}

	BlockInfo bi = m_mapLiveBlocks[pData];
	m_mapLiveBlocks.erase(pData);
	m_iTotalBytes-=bi.iSize;
	m_iOutstandingAllocs --;

	LogVerbose("MemFree - released block at 0x%x, size %d bytes, alloc #%d", bi.pData, bi.iSize, bi.iAllocNum);

	free(pData);
}

void Heap::MemInfo(DWORD *heapSize, DWORD *freeHeapSize, DWORD *availHeapSize)
{
	*heapSize = m_iHeapSize;
	*freeHeapSize = m_iHeapSize - m_iTotalBytes;
	*availHeapSize = *freeHeapSize; // should be largest block?

}

void Heap::DumpLeaks()
{
	std::vector<BlockInfo> vecBlocks;

	for (std::map<void*, BlockInfo>::iterator it = m_mapLiveBlocks.begin(); it!=m_mapLiveBlocks.end(); it++)
	{
		vecBlocks.push_back(it->second);
	}

	std::sort(vecBlocks.begin(), vecBlocks.end());

	LogInfo("Begin Dump of Memory leaks");

	for (unsigned int i = 0; i<vecBlocks.size(); i++)
	{
		BlockInfo& bi = vecBlocks[i];
		LogError("MemLeak - block at 0x%x, size %d bytes, alloc #%d", bi.pData, bi.iSize, bi.iAllocNum);
	}

	LogInfo("End Dump of Memory leaks");


}