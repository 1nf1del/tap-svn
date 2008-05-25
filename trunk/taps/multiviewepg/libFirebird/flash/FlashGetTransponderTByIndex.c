#include "FBLib_flash.h"

//--------------------------------------- FlashGetTransponderTByIndex --------------------------------
//
TYPE_TpInfoT *FlashGetTransponderTByIndex (word TpIdx)
{
  dword                 offset;

  if (!LibInitialized) InitTAPex ();
  if (!LibInitialized || (FlashOffset == 0 || SystemType != ST_DVBT)) return NULL;

  offset = FlashBlockOffset[Transponders] + 4;

  // sanity check of the parameter
  if ((dword) TpIdx >= *(dword *) offset) return NULL;

  return (TYPE_TpInfoT *) (offset + 4 + TpIdx * sizeof(TYPE_TpInfoT));
}
