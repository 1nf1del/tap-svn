#include "../libFireBird.h"
#include "FBLib_rec.h"

//---------------------------------------  HDD_RECSlotIsPaused --------------------------------
//
bool HDD_RECSlotIsPaused (byte Slot)
{
  //Sanity check of the parameters
  if (Slot > 1) return FALSE;

  //Find the address of the two RECslot structures
  if (RECSlot [0] == NULL) getRECSlotAddress();
  if (RECSlot [0] == NULL) return FALSE;

  //Check if the selected REC-slot is currently in use
  if (RECSlot [Slot]->Duration == 0) return FALSE;

  return (RECSlot [Slot]->Pause == 1);
}
