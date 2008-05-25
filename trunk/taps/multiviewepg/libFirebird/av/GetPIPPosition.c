#include "FBLib_av.h"
#include "../libFireBird.h"

volatile tScalerSettings    *ScalerSettings;

// ------------------------------ GetPIPPosition ------------------------------------
//
bool GetPIPPosition (int *North, int *South, int *East, int *West)
{
  if (!LibInitialized) InitTAPex ();
  if (!LibInitialized) return FALSE;

  if (!pScalerSettingsPIP->Active) return FALSE;

  //Let the scaler structure point to the scaler settings of the PIP
  ScalerSettings = (tScalerSettings *) (pScalerSettingsPIP->Address | 0x80000000);

  if (North) *North = ScalerSettings->WindowY - 1;
  if (South) *South = ScalerSettings->WindowY + ScalerSettings->WindowHeight + 2;
  if (East)  *East = ScalerSettings->WindowX + ScalerSettings->WindowWidth + 1;
  if (West)  *West = ScalerSettings->WindowX - 1;

  return TRUE;
}
