#include "FBLib_flash.h"

//--------------------------------------- FlashRemoveServiceByLCN --------------------------------
//
dword FlashRemoveServiceByLCN (char* LCN, bool TVService)
{
  TYPE_Service          *TvSvc, *TvSvcSource, *TvSvcDest;
  dword                 RemovedCount = 0;
  int                   ServiceIndex = 0;
  char                  LCNs [16];

  if (!LibInitialized) InitTAPex ();
  if (!LibInitialized || (FlashOffset == 0 || !(SystemType == ST_DVBT || SystemType == ST_DVBC))) return 0;

  if (TVService) TvSvc = (TYPE_Service *) (FlashBlockOffset[TVServices] + 4);
  else TvSvc = (TYPE_Service *) (FlashBlockOffset[RadioServices] + 4);

  while (TvSvc->TunerNum != 0)
  {
    TAP_SPrint (LCNs, "%d", TvSvc->LCN);
    if (strcmp (LCN, LCNs) == 0)
    {
      RemovedCount++;
      TvSvcDest = TvSvc;
      TvSvcSource = TvSvc;
      TvSvcSource++;
      do
      {
        memcpy(TvSvcDest, TvSvcSource, sizeof (TYPE_Service));
        TvSvcSource++;
        TvSvcDest++;
      } while (TvSvcDest->TunerNum != 0);

      //A channel has been removed, re-index the timer table
      FlashReindexTimers(ServiceIndex, (TVService ? SVC_TYPE_Tv :SVC_TYPE_Radio), delete);

      //Also re-index the favourites tables
      FlashReindexFavourites(ServiceIndex, (TVService ? SVC_TYPE_Tv :SVC_TYPE_Radio), delete);
    }
    else
    {
      TvSvc++;
      ServiceIndex++;
    }
  }

  return RemovedCount;
}
