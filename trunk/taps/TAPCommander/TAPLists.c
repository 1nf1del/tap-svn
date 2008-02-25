#include <tap.h>
#include "libFireBird.h"
#include "TAPLists.h"


word *pMalloc, *pMem, *pFile;


#define BLOCKSIZE	(0x500)

typedef struct {
	word inUse:1;
	word blocks:15;
	word prev;
	word next;
} ListElt;


static void *extractPtr(word *p, int hi, int lo)
{
	return (void*)((p[hi]<<16)+(short)p[lo]);
}


bool InitTAPMonitor()
{
	if (pMalloc == NULL)
	{
		pMalloc = (word*)FindInstructionSequence(
			"3c198214 27395840 32b5ffff 24180500 00000000 00000000 02b80018 00000000 0000c012 00000000 03389021 10000010 00000000 3c1981da 2739c098 27390004 32b5ffff 24180006",
			"FFFF0000 FFFF0000 FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFF0000 FFFF0000 FFFFFFFF FFFFFFFF FFFFFFFF",
			0x80000000, 0x80010000, 0, FALSE);
	}
	if (pMem == NULL)
	{
		pMem = (word*)FindInstructionSequence(
			"3c1e8042 27de7384 000f9880 03d3c821 8f390000 1320004b 00808825 3c158042 26b573c4 02b3c021 8f120000 52400046 8fbf0010 3c168042 26d67344",
			"FFFF0000 FFFF0000 FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFF0000 FFFF0000 FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFF0000 FFFF0000",
			0x80150000, 0x80200000, 0, FALSE);
	}
/*
	if (pFile == NULL)
	{
		pFile = (word*)FindInstructionSequence(
			"3c1e8042 27de7444 000f9880 03d3c821 8f390000 1320004a 00808825 3c158042 26b57484 02b3c021 8f120000 52400045 8fbf0010 3c168042 26d67404",
			"FFFF0000 FFFF0000 FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFF0000 FFFF0000 FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFF0000 FFFF0000",
			0x80150000, 0x80200000, 0, FALSE);
	}
*/
	return pMalloc && pMem;
}


bool GetTAPMemoryUse(int tapIndex, dword *pTotal, dword *pBiggest, dword *pFreed, dword *pDanger)
{
	tTAPTable *tapTable;
	dword heap;
	ListElt *blocklist;
	dword **memLists, *memListMaxs, *memListCounts;
	void *fileLists, *fileListMaxs, *fileListCounts;
	dword total = 0, biggest = 0, freed = 0, danger = 0;

	if (!InitTAPMonitor())
		return FALSE;

	tapTable = (tTAPTable*)TAP_TableAddress;
	heap = (dword)extractPtr(pMalloc,1,3);
	blocklist = (ListElt*)extractPtr(pMalloc,27,29);
	memLists = extractPtr(pMem,1,3);
	memListMaxs = extractPtr(pMem,15,17);
	memListCounts = extractPtr(pMem,27,29);

	if (memLists)
	{
		dword *mems = memLists[tapIndex];
		if (mems)
		{
			int i;
			dword mx = memListMaxs[tapIndex];
			dword count = memListCounts[tapIndex];
			word index = (tapTable[tapIndex].HeapAlloc - heap) / BLOCKSIZE;
			ListElt *b = &blocklist[index];
			if (b)
				total += b->blocks * BLOCKSIZE;

			for (i = 0; i < mx; ++i)
			{
				dword m = mems[i];
				if (m)
				{
					word index = (m - heap) / BLOCKSIZE;
					ListElt *b = &blocklist[index];
					dword size;
					if (b)
					{
						size = b->blocks * BLOCKSIZE;
						if (b->inUse)
						{
							if (i >= count && danger)
							{
								// TAP_Print("\t* %p %d\n",m,size);
								danger++;
							}
							total += size;
							if (size > biggest)
								biggest = size;
						}
						else
						{
							// TAP_Print("\t%c %p %d ERROR - free!\n",i>=count?'*':' ',m,size);
							freed++;
						}
					}
				}
			}

		}
	}

	if (pTotal)
		*pTotal = total;
	if (pBiggest)
		*pBiggest = biggest;
	if (pFreed)
		*pFreed = freed;
	if (pDanger)
		*pDanger = danger;

	return TRUE;
}
