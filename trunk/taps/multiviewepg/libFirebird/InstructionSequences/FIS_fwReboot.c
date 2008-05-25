#include "../libFireBird.h"

dword FIS_fwReboot (void)
{
  static dword          fwReboot = 0;

  if (!fwReboot)
    fwReboot = FindInstructionSequence ("3c04b402 00002825 0c0007fe 00000000",
                                        "ffffffff ffffffff fc000000 ffffffff",
                                         0x80000000, 0x80200000, 0, TRUE);

  return fwReboot;
}
