#include "../libFireBird.h"

dword FIS_fwMoveOld (void)
{
  static dword          fwMoveOld = 0;

  if (!fwMoveOld)
    fwMoveOld = FindInstructionSequence ("27a40010 00002825 0c0252d9 00000000 27a40014 00002825 0c0252d9 00000000 8fa40010 8f859ac4 24060080 0c03d528 00000000",
                                         "ffffffff ffffffff fc000000 ffffffff ffffffff ffffffff fc000000 ffffffff ffffffff ffff0000 ffffffff fc000000 ffffffff",
                                          0x80008000, 0x80280000, 0, TRUE);

  return fwMoveOld;
}
