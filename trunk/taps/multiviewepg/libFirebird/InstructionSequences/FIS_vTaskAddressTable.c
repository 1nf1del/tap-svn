#include "../libFireBird.h"

dword FIS_vTaskAddressTable (void)
{
  dword                 *p;
  static dword          vTaskAddressTable = 0;

  if (!vTaskAddressTable)
  {
    p = (dword*) FindInstructionSequence ("3c028027 8c428c6c",
                                          "ffff0000 ffff0000",
                                          0x80000180, 0x80001000, 0, FALSE);

    if (p) vTaskAddressTable = (((p[0] & 0x0000ffff) << 16) + (short)(p[1] & 0x0000ffff));
  }

  return vTaskAddressTable;
}
