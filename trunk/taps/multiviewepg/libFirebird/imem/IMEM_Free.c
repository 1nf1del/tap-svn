/******************************************************************************/
/*                                                                            */
/* Name:       imem                                                           */
/* Author:     DeJe                                                           */
/*                                                                            */
/* Function:  -memory manager based on "segregated freelists"                 */
/*                                                                            */
/******************************************************************************/

#include "imem.h"

void IMEM_Free( void *pPtr )
{
	if( isInitialized && pPtr ) {
		UNIT *pTmp;

		pTmp = (UNIT*)( (dword)pPtr - sizeof(UNIT) );
		if( pTmp->dwSize & USED ) {
			pTmp->dwSize &= (~USED);
			imem.dwFree += pTmp->dwSize;
		}
	}
}
