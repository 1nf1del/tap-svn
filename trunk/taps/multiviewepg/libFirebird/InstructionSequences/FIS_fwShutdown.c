#include "../libFireBird.h"

dword FIS_fwShutdown (void)
{
  static dword          fwShutdown = 0;

  if (!fwShutdown)
    fwShutdown = FindInstructionSequence ("24840f44 240f0001 a38f85f2 0c05dd59 240400ff 304200ff 10400044 24040001 27a4002c 3c068011 24c6d358 0c04da75",
                                          "ffff0000 fc00ffff ffe00000 fc000000 ffffffff ffffffff ffff0000 ffffff00 ffff0000 ffff0000 ffff0000 fc000000",
                                           0x800F0000, 0x80160000, 0, TRUE);

  return fwShutdown;
}
