#include "FBLib_flash.h"

//--------------------------------------- FlashFindEndOfServiceTableAddress --------------------------------
//
dword FlashFindEndOfServiceTableAddress (bool TVService)
{
  TYPE_Service          *TvSvc;

  if (!LibInitialized) InitTAPex ();
  if (!LibInitialized || (FlashOffset == 0)) return 0;

  if (TVService)(dword) TvSvc = FlashBlockOffset [TVServices] + 4;
  else (dword) TvSvc = FlashBlockOffset [RadioServices] + 4;
  while (TvSvc->TunerNum != 0) TvSvc++;

#ifdef DEBUG_FIREBIRDLIB
  TAP_Print ("FireBirdLib: FindEndOfServiceTable @ 0x%8.8x\n", (dword) TvSvc);
#endif

  return (dword) TvSvc;
}
