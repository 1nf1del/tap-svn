#include "../libFireBird.h"

tPlaySlot *HDD_PlaySlotGetAddress (void)
{
  return (tPlaySlot *)FIS_vPlaySlot();
}
