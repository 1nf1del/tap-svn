/******************************************************************************/
/*                                                                            */
/* Name:       imem                                                           */
/* Author:     DeJe                                                           */
/*                                                                            */
/* Function:  -memory manager based on "segregated freelists"                 */
/*                                                                            */
/******************************************************************************/

#include "imem.h"

IMEM	imem;
bool    isInitialized = FALSE;

bool IMEM_Init( dword dwSize )
{
	byte	*pHeap;
	dword	Heap;
	dword	FreeHeap;
	dword	AvailHeap;

	if( isInitialized ) return FALSE;

	memset( &imem, 0, sizeof(imem) );

	TAP_MemInfo( &Heap, &FreeHeap, &AvailHeap );
	//
	// check if there is enough memory
	//
	if( AvailHeap < 3 * 1024 * 1024 ) return FALSE;
	//
	// let at least 1 MByte free
	//
	imem.dwTotal = min( dwSize, AvailHeap - (1 * 1024 * 1024) );
	//
	// allocate internal pool memory
	//
	pHeap = (byte*) TAP_MemAlloc( imem.dwTotal );
	if( !pHeap ) return FALSE;

	imem.pHeap		= (UNIT*) pHeap;
	imem.pFree		= (UNIT*) pHeap;
	imem.pHeap->dwSize	= imem.dwTotal - sizeof(UNIT);
	imem.pFree->dwSize	= imem.dwTotal - sizeof(UNIT);
	imem.dwFree		= imem.pHeap->dwSize;

	*(dword*)(pHeap + imem.dwTotal - 4) = 0;

	isInitialized = TRUE;

	return TRUE;
}
