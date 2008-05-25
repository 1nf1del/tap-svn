#include "../libFireBird.h"
#include "../main/FBLib_main.h"

dword FIS_vFlash (void)
{
  static dword          vFlash = 0;
  dword                 *pFlash;

  if (!vFlash)
  {
    //This one is used by InitTAPex. Do not check LibInitialized!

    pFlash = (dword*)FindInstructionSequence ("8f999ebc 0016c340 03382021 0c000c56 24052000",
                                              "ff800000 ffe007ff fc00ffff fc000000 ffffffff",
                                               0x80160000, 0x80200000, 0, FALSE);

    if (pFlash)
    {
      pFlash = (dword*)(FWgp + (short)(*pFlash & 0xffff));
      vFlash = *pFlash;
    }
  }

  return vFlash;
}
