#include "../libFireBird.h"
#include "../main/FBLib_main.h"

dword FIS_vHddInfoStructure2 (void)
{
  dword                 vHddInfoStructure2 = 0;

  if (!vHddInfoStructure2)
  {
    if (!LibInitialized) InitTAPex ();
    if (!LibInitialized) return 0;

    vHddInfoStructure2 = FindInstructionSequence("3402fe00 10000019 00000000 8f998380 8f9897d4 00000000 00000000 03380019 00000000 0000c812 00000000 0019cac2",
                                                 "ffffffff ffff0000 ffffffff ffe00000 ffe00000 ffffffff ffffffff fc00ffff ffffffff ffff07ff ffffffff ffc007ff",
                                                  0x80090000, 0x80180000, 3, FALSE);

    if (vHddInfoStructure2) vHddInfoStructure2 = FWgp + (short)(*(dword*)vHddInfoStructure2 & 0xffff);
  }

  return vHddInfoStructure2;
}
