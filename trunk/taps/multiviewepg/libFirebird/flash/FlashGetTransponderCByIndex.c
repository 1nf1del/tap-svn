#include "FBLib_flash.h"

//--------------------------------------- FlashGetTransponderCByIndex --------------------------------
//
TYPE_TpInfoC *FlashGetTransponderCByIndex (word TpIdx)
{
  dword                 offset;

  if (!LibInitialized) InitTAPex ();
  if (!LibInitialized || (FlashOffset == 0 || SystemType != ST_DVBC)) return NULL;

  offset = FlashBlockOffset[Transponders] + 4;

  // sanity check of the parameter
  if ((dword) TpIdx >= *(dword *) offset) return NULL;

  return (TYPE_TpInfoC *) (offset + 4 + TpIdx * sizeof(TYPE_TpInfoC));
}
