#include "FBLib_rec.h"

//---------------------------------------  HDD_RECSlotGetAddress --------------------------------
//
tRECSlot *HDD_RECSlotGetAddress (byte Slot)
{
  //Sanity check of the parameters
  if (Slot > 1) return NULL;

  //Find the address of the two RECslot structures
  if (RECSlot [0] == NULL) getRECSlotAddress();
  if (RECSlot [0] == NULL) return NULL;

  return (RECSlot [Slot]);
}
