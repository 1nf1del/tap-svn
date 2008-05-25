#include "../libFireBird.h"

dword FIS_fwBIOS (void)
{
  static dword          fwBIOS = 0;

  if (!fwBIOS)
    fwBIOS = FindInstructionSequence ("0080f025 00a0b025 2fc10014 14200004 00000000 24020090 1000005e 00000000",
                                      "ffffffff ffffffff ffffff00 ffffffff ffffffff ffffffff ffff0000 ffffffff",
                                       0x80000000, 0x80400000, 0, TRUE);

  return fwBIOS;
}
