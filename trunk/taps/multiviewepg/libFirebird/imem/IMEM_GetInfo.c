/******************************************************************************/
/*                                                                            */
/* Name:       imem                                                           */
/* Author:     DeJe                                                           */
/*                                                                            */
/* Function:  -memory manager based on "segregated freelists"                 */
/*                                                                            */
/******************************************************************************/

#include "imem.h"

void IMEM_GetInfo( dword *pTotal, dword *pFree )
{
	if( isInitialized ) {
		*pTotal	= imem.dwTotal;
		*pFree	= imem.dwFree;
	}
}
