#include "../libFireBird.h"

dword FIS_fwPIC2_ISR18 (void)
{
  static dword          fwPIC2_ISR18 = 0;

  if (!fwPIC2_ISR18) fwPIC2_ISR18 = FindInstructionSequence ("3c19b200 8f393060 33390004 1320000a 00000000 3c19b200 8f393050 333e00ff 3c18b200 3c0fb200",
                                                             "fc00ffff fc00ffff fc00ffff fc00ffff ffffffff fc00ffff fc00ffff fc00ffff fc00ffff fc00ffff",
                                                              0x80000000, 0x80010000, 0, TRUE);

  return fwPIC2_ISR18;
}
