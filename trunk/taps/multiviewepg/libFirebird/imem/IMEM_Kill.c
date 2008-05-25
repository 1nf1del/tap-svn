/******************************************************************************/
/*                                                                            */
/* Name:       imem                                                           */
/* Author:     DeJe                                                           */
/*                                                                            */
/* Function:  -memory manager based on "segregated freelists"                 */
/*                                                                            */
/******************************************************************************/

#include "imem.h"

void IMEM_Kill( void )
{
	if( isInitialized && imem.pHeap ) {
		TAP_MemFree( imem.pHeap );
		memset( &imem, 0, sizeof(imem) );
		isInitialized = FALSE;
	}
}
