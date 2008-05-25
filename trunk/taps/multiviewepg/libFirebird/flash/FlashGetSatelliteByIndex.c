#include "FBLib_flash.h"

//--------------------------------------- FlashGetSatelliteByIndex --------------------------------
//
TYPE_SatInfoS *FlashGetSatelliteByIndex (byte SatIdx)
{
  if (!LibInitialized) InitTAPex ();
  if (!LibInitialized || (FlashOffset == 0 || SystemType != ST_DVBS)) return NULL;

  // sanity check of the parameter
  if (SatIdx > MAXSAT) return NULL;

  return (TYPE_SatInfoS *) (FlashBlockOffset[Satellites] + 4 + SatIdx * sizeof(TYPE_SatInfoS));
}
