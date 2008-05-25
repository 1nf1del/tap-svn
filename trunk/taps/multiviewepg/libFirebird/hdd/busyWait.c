#include "FBLib_hdd.h"
#include "../libFireBird.h"

volatile byte *HDD_pAltStatus   = (byte*) 0xb0012d5b;   // alternate status register

void busyWait (void)
{
  if (!LibInitialized) InitTAPex ();
  if (!LibInitialized) return;

  while ((*HDD_pAltStatus & 0x80) != 0);
}
