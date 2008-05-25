/******************************************************************************/
/*                                                                            */
/* Name:       imem                                                           */
/* Author:     DeJe                                                           */
/*                                                                            */
/* Function:  -memory manager based on "segregated freelists"                 */
/*                                                                            */
/******************************************************************************/

#include "imem.h"

void *IMEM_Alloc( dword dwSize )
{
	dword	fsize;
	UNIT	*pTmp;

	if( !isInitialized || (dwSize == 0) || (dwSize>=imem.dwTotal) || !imem.pHeap ) {
		return 0;
	}

	dwSize  += (3 + sizeof(UNIT));
	dwSize >>= 2;
	dwSize <<= 2;

	if( imem.pFree == 0 || dwSize > imem.pFree->dwSize ) {
		imem.pFree = compact( imem.pHeap, dwSize );
		if( imem.pFree == 0 ) {
			return 0;
		}
	}

	pTmp = imem.pFree;
	fsize = imem.pFree->dwSize;

	if( fsize >= dwSize + sizeof(UNIT) ) {
		imem.pFree = (UNIT*)( (dword)pTmp + dwSize );
		imem.pFree->dwSize = fsize - dwSize;
	}
	else {
		imem.pFree = 0;
		dwSize = fsize;
	}

	pTmp->dwSize = dwSize | USED;
	imem.dwFree -= dwSize;

	return (void*)( (dword)pTmp + sizeof(UNIT) );
}
