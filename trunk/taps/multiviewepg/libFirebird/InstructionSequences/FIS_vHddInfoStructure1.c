#include "../libFireBird.h"
#include "../main/FBLib_main.h"

dword FIS_vHddInfoStructure1 (void)
{
  dword                 vHddInfoStructure1 = 0;

  if (!vHddInfoStructure1)
  {
    if (!LibInitialized) InitTAPex ();
    if (!LibInitialized) return 0;

    vHddInfoStructure1 = FindInstructionSequence("3402fe00 10000019 00000000 8f998380 8f9897d4 00000000 00000000 03380019 00000000 0000c812 00000000 0019cac2",
                                                 "ffffffff ffff0000 ffffffff ffe00000 ffe00000 ffffffff ffffffff fc00ffff ffffffff ffff07ff ffffffff ffc007ff",
                                                  0x80090000, 0x80180000, 3, FALSE);
    if (vHddInfoStructure1)
    {
      vHddInfoStructure1 += sizeof (dword);
      vHddInfoStructure1 = FWgp + (short)(*(dword*)vHddInfoStructure1 & 0xffff);
    }
  }

  return vHddInfoStructure1;
}
