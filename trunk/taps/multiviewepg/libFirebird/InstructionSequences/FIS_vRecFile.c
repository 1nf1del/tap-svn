#include "../libFireBird.h"

dword FIS_vRecFile (byte Slot)
{
  dword                 *p;
  static dword          vRecFile = 0;
  dword                 RecFile = 0;

  if (Slot > 1) return 0;

  if (!vRecFile)
  {
    p = (dword*) FindInstructionSequence ("3c0f81dd 25ef1ccc 33deffff 001ec0c0 01f87821 172f0035 00000000",
                                          "ffe00000 fc000000 ffffffff ffe007ff fc0007ff fc000000 ffffffff",
                                          0x80080000, 0x80100000, 0, FALSE);

    if (p) vRecFile = ((p[0] & 0x0000ffff) << 16) + (short)(p[1] & 0x0000ffff);
  }

  if (vRecFile) RecFile = *(dword*)(vRecFile + (Slot << 3));

  return RecFile;
}
