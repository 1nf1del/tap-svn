#include "../libFireBird.h"

dword FIS_vHDDShutdown (void)
{
  dword                 vHDDShutdown;

  vHDDShutdown = FindInstructionSequence ("3c18b001 240f00e0 af0f2d3c 24040028 0c02b661 00000000 3c19b001 8f352d58 24040028 0c02b661 00000000 24042710 0c024dc8 00000000",
                                           "ffe0ffff ffe0ffff 00000000 ffffffff fc000000 ffffffff ffe0ffff fc000000 ffffffff fc000000 ffffffff ffffffff fc000000 ffffffff",
                                           0x80080000, 0x80300000, 1, FALSE);

  return vHDDShutdown;
}
