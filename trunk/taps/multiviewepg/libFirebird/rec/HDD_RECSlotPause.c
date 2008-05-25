#include "../libFireBird.h"
#include "FBLib_rec.h"

//---------------------------------------  HDD_RECSlotPause --------------------------------
//
bool HDD_RECSlotPause (byte Slot, bool Pause)
{
  //Sanity check of the parameters
  if (Slot > 1) return FALSE;

  //Find the address of the two RECslot structures
  if (RECSlot [0] == NULL) getRECSlotAddress();
  if (RECSlot [0] == NULL) return FALSE;

  //Check if the selected REC-slot is currently in use
  if (RECSlot [Slot]->Duration == 0) return FALSE;

  RECSlot [Slot]->Pause = Pause ? 1 : 0;

  return TRUE;
}
