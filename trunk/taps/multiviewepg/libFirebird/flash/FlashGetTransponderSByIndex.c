#include "FBLib_flash.h"

//--------------------------------------- FlashGetTransponderSByIndex --------------------------------
//
TYPE_TpInfoS *FlashGetTransponderSByIndex (word TpIdx, byte SatIdx)
{
  dword                 offset, tpCount;
  TYPE_TpInfoS          *transponder;

  if (!LibInitialized) InitTAPex ();
  if (!LibInitialized || (FlashOffset == 0 || SystemType != ST_DVBS)) return NULL;

  offset = FlashBlockOffset[Transponders] + 4;
  tpCount = *(dword *) offset;
  transponder = (TYPE_TpInfoS *) (offset + 4);

  // search start of transponders for satellite
  while (tpCount)
  {
    if (transponder->SatIdx == SatIdx) break;

    transponder++;
    tpCount--;
  }

  if (tpCount == 0 || TpIdx >= tpCount) return NULL;
  else
  {
    transponder += TpIdx;
    return transponder;
  }
}
