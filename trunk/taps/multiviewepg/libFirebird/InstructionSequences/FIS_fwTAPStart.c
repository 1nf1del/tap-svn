#include "../libFireBird.h"

dword FIS_fwTAPStart (void)
{
  static dword          fwTAPStart = 0;

  if (!fwTAPStart)
    fwTAPStart = FindInstructionSequence ("2411001a 1300000d 00009025 03c0a825 82b40000 2418000d 12980009 0092c821 24190020 12990006",
                                          "ffe00000 fc01f000 ffe007ff 00000000 fc00ffff fc00ffff fc000000 fc0007ff fc00ffff fc000000",
                                           0x80140000, 0x80200000, 0, TRUE);

  return fwTAPStart;
}
