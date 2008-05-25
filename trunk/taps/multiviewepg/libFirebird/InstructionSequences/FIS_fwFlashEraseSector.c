#include "../main/FBLib_main.h"

dword FIS_fwFlashEraseSector (void)
{
  static dword          fwFlashEraseSector = 0;
  dword                *p, i;

  if (!fwFlashEraseSector)
  {
    if (!LibInitialized) InitTAPex ();
    if (!LibInitialized) return FALSE;

    //Locate FlashEraseSector
    fwFlashEraseSector = FindInstructionSequence ("00000000 03c02025 240500d0 0c001bf9 00000000 304200ff 14400004 00000000",
                                                  "ffffffff ffff07ff ffffffff fc000000 ffffffff ffffffff ffff0000 ffffffff",
                                                   0x80005000, 0x800c0000, 0, TRUE);

    //A structure, referenced via $gp, contains 4 pointers to the needed functions
    p = (dword*)(FWgp - 32768);
    for (i = 0; i < 0x4000; i++)
    {
      if (*p == fwFlashEraseSector) break;
      p++;
    }

    if (i >= 0x4000) return 0;

    //Are these valid pointers?
    if (!isValidRAMPtr (p [0]) || !isValidRAMPtr (p [1]) || !isValidRAMPtr (p [2]) || !isValidRAMPtr (p [3])) return 0;
  }

  return fwFlashEraseSector;
}
