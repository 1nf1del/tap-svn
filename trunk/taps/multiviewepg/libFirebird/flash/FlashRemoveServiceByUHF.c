#include "FBLib_flash.h"

//--------------------------------------- FlashRemoveServiceByUHF --------------------------------
//
dword FlashRemoveServiceByUHF (char* UHFChannel, bool TVService, bool UseAUSChannelCoding)
{
  TYPE_Service          *TvSvc, *TvSvcSource, *TvSvcDest;
  dword                 RemovedCount = 0;
  int                   ServiceIndex = 0;
  char                  UHFs [100];
  TYPE_TpInfoT          *TPInfo;

  if (!LibInitialized) InitTAPex ();
  if (!LibInitialized || (FlashOffset == 0 || SystemType != ST_DVBT)) return 0;

  if (TVService) TvSvc = (TYPE_Service *) (FlashBlockOffset[TVServices] + 4);
  else (dword) TvSvc = (TYPE_Service *) (FlashBlockOffset[RadioServices] + 4);

  while (TvSvc->TunerNum != 0)
  {
    TPInfo = (TYPE_TpInfoT *) (FlashBlockOffset[Transponders] + 4 + 4 + sizeof(TYPE_TpInfoT) * TvSvc->TPIdx);
    if (UseAUSChannelCoding)
    {
      memset (UHFs, 0, sizeof(UHFs));
      if (TPInfo->ChannelNr > 0x0f) UHFs [0] = 48 + (TPInfo->ChannelNr >> 4);
      if ((TPInfo->ChannelNr & 0x0f) == 0x0a) UHFs [strlen (UHFs)] = 0x41;
      else UHFs [strlen (UHFs)] = 48 + (TPInfo->ChannelNr & 0x0f);
    }
    else TAP_SPrint (UHFs, "%d", TPInfo->ChannelNr);

    if (strcmp (UHFChannel, UHFs) == 0)
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

      //A channel has been removed, re-index the timer tables
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
