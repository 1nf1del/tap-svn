#include "FBLib_rec.h"

tRECSlot                *RECSlot [2] = {NULL, NULL};

bool getRECSlotAddress (void)
{
  RECSlot [0] = (tRECSlot*) FIS_vRECSlotAddress(0);
  RECSlot [1] = (tRECSlot*) FIS_vRECSlotAddress(1);

  return TRUE;
}
