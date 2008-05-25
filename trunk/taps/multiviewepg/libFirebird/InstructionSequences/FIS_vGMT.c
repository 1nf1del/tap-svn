#include "../libFireBird.h"
#include "../main/FBLib_main.h"

dword FIS_vGMT (void)
{
  static dword          vGMT = 0;

  if (!vGMT)
  {
    if (!LibInitialized) InitTAPex();
    if (!LibInitialized) return 0;

    vGMT = FindInstructionSequence ("3c04801e 0c02b6eb 248418c0 8f9884b4 0018c402 0c04539b 3304ffff 8f9e84b4",
                                    "ffff0000 fc000000 fc1f0000 ffe00000 ffe007ff fc000000 fc1fffff fc000000",
                                     0x80100000, 0x80200000, 3, FALSE);

    if (vGMT) vGMT = FWgp + (short)(*(dword*)(vGMT) & 0x0000ffff);
  }

  return vGMT;
}
