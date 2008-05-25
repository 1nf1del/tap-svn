#include "FBLib_flash.h"

//--------------------------------------- FlashFindServiceAddress --------------------------------
//
dword FlashFindServiceAddress (word ServiceID, word PMTPID, word PCRPID, bool TVService)
{
  TYPE_Service          *TvSvc;

  if (!LibInitialized) InitTAPex ();
  if (!LibInitialized || (FlashOffset == 0)) return 0;

#ifdef DEBUG_FIREBIRDLIB
  TAP_Print ("FireBirdLib: FindService   : SVDID=%5d, PMTPID=%5d, PCRPID=%5d ", ServiceID, PMTPID, PCRPID);
#endif

  if (TVService) (dword) TvSvc=FlashBlockOffset [TVServices] + 4;
            else (dword) TvSvc=FlashBlockOffset [RadioServices] + 4;
  do
  {
    if ((TvSvc->SVCID == ServiceID) && (TvSvc->PMTPID == PMTPID) && (TvSvc->PCRPID == PCRPID)) break;
    TvSvc++;
  } while (TvSvc->TunerNum != 0);

  if (TvSvc->TunerNum == 0)
  {
    (dword) TvSvc = 0;

#ifdef DEBUG_FIREBIRDLIB
    TAP_Print ("not found.\n");
#endif

  }

#ifdef DEBUG_FIREBIRDLIB
  else TAP_Print ("@ 0x%8.8x\n", (dword) (dword) TvSvc);
#endif

  return (dword) TvSvc;
}
