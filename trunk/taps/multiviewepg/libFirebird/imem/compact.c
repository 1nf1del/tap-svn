/******************************************************************************/
/*                                                                            */
/* Name:       imem                                                           */
/* Author:     DeJe                                                           */
/*                                                                            */
/* Function:  -memory manager based on "segregated freelists"                 */
/*                                                                            */
/******************************************************************************/

#include "imem.h"

UNIT* compact( UNIT *pPtr, dword nsize )
{
	dword	bsize;
	dword	psize;
	UNIT	*pBest;

	pBest	= pPtr;
	bsize	= 0;

	if( !isInitialized || (nsize>imem.dwTotal) || !imem.pHeap ) {
		return 0;
	}

	while( psize = pPtr->dwSize, psize ) {
		if( psize & USED ) {
			if( bsize != 0 ) {
				pBest->dwSize = bsize;
				if( bsize >= nsize ) {
					return pBest;
				}
			}
			bsize = 0;
			pBest = pPtr = (UNIT*)( (dword)pPtr + (psize & ~USED) );
		}
		else {
			bsize += psize;
			pPtr = (UNIT*)( (dword)pPtr + psize );
		}
	}

	if( bsize != 0 ) {
		pBest->dwSize = bsize;
		if( bsize >= nsize ) {
			return pBest;
		}
	}

	return 0;
}
