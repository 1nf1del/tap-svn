#include <stdlib.h>
#include "FBLib_flash.h"

//--------------------------------------- FlashRemoveServiceByIndexString --------------------------------
//
dword FlashRemoveServiceByIndexString (char* ChannelIndex, bool TVService)
{
  dword                 lChannelIndex;

  if (!LibInitialized) InitTAPex ();
  if (!LibInitialized || (FlashOffset == 0)) return 0;

  lChannelIndex=atol (ChannelIndex);
  return FlashRemoveServiceByIndex (lChannelIndex, TVService);
}
