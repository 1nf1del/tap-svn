#include "FBLib_flash.h"

//--------------------------------------- FlashRemoveCASServices --------------------------------
//
dword FlashRemoveCASServices (bool TVService)
{

  TYPE_Service          *TvSvc, *TvSvcSource, *TvSvcDest;
  dword                 RemovedCount = 0;
  int                   ServiceIndex = 0;

  if (!LibInitialized) InitTAPex ();
  if (!LibInitialized || (FlashOffset == 0 || SystemType == ST_UNKNOWN)) return 0;

  if (TVService) TvSvc = (TYPE_Service *) (FlashBlockOffset[TVServices] + 4);
  else TvSvc = (TYPE_Service *) (FlashBlockOffset[RadioServices] + 4);

  while (TvSvc->TunerNum != 0)
  {
    if (TvSvc->CASFlag)
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

      //A service has been removed; reindex the timer table
      FlashReindexTimers(ServiceIndex, (TVService ? SVC_TYPE_Tv :SVC_TYPE_Radio), delete);

      //Also reindex the favourites tables
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
