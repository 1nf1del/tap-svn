#include <string.h>
#include "FBLib_flash.h"

//--------------------------------------- FlashRemoveServiceByName --------------------------------
//
dword FlashRemoveServiceByName (char* ChannelName, bool TVService)
{
  TYPE_Service          *TvSvc, *TvSvcSource, *TvSvcDest;
  dword                 RemovedCount = 0;
  int                   ServiceIndex = 0;
  char                  ServiceName [MAX_SvcName];

  if (!LibInitialized) InitTAPex ();
  if (!LibInitialized || (FlashOffset == 0 || SystemType == ST_UNKNOWN)) return 0;

  if (TVService) TvSvc = (TYPE_Service *) (FlashBlockOffset[TVServices] + 4);
  else TvSvc = (TYPE_Service *) (FlashBlockOffset[RadioServices] + 4);

  while (TvSvc->TunerNum != 0)
  {
    strncpy (ServiceName, (char *) (FlashBlockOffset [ServiceNames] + 4 + TvSvc->NameOffset), MAX_SvcName);
    ServiceName[MAX_SvcName - 1] = '\0';

    if (strcasecmp (ChannelName, ServiceName) == 0 || strcasecmp (ChannelName, ValidFileName (ServiceName, NonPrintableChars)) == 0)
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

      //A channel has been removed, reindex the timer table
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
