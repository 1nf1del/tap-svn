#include "FBLib_flash.h"

//--------------------------------------- FlashGetServiceByIndex --------------------------------
//
TYPE_Service *FlashGetServiceByIndex (word ServiceIdx, bool TVService)
{
  TYPE_Service          *Svc;

  if (!LibInitialized) InitTAPex ();
  if (!LibInitialized || (FlashOffset == 0)) return NULL;

  if (TVService) Svc = (TYPE_Service *) (FlashBlockOffset[TVServices] + 4 + ServiceIdx * sizeof(TYPE_Service));
            else Svc = (TYPE_Service *) (FlashBlockOffset[RadioServices] + 4 + ServiceIdx * sizeof(TYPE_Service));

  return ((dword) Svc >= FlashFindEndOfServiceTableAddress(TVService) ? NULL : Svc);
}
