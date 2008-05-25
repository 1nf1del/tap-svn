#include "../libFireBird.h"

dword FIS_fwWriteSectorsDMA (void)
{
  dword                 fwWriteSectorsDMA = 0;

  if (!fwWriteSectorsDMA) fwWriteSectorsDMA = FindInstructionSequence ("3c19b001 240e00ca af2e2d3c 24040028 0c02b661 00000000",
                                                                     "ffe0ffff ffe0ffff fc00ffff ffffffff fc000000 ffffffff",
                                                                      0x80080000, 0x800B0000, 0, TRUE);

  return fwWriteSectorsDMA;
}
