/******************************************************************************/
/*                                                                            */
/* Name:       imem                                                           */
/* Author:     DeJe                                                           */
/*                                                                            */
/* Function:  -memory manager based on "segregated freelists"                 */
/*                                                                            */
/******************************************************************************/

#include "imem.h"

bool IMEM_isInitialized( void )
{
  return isInitialized;
}
