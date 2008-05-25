/******************************************************************************/
/*                                                                            */
/* Name:       imem                                                           */
/* Author:     DeJe                                                           */
/*                                                                            */
/* Function:  -memory manager based on "segregated freelists"                 */
/*                                                                            */
/******************************************************************************/

#include "imem.h"

void IMEM_Compact( void )
{
	if( isInitialized ) {
		imem.pFree = compact( imem.pHeap, imem.dwTotal - 2 * sizeof(UNIT) );
	}
}
