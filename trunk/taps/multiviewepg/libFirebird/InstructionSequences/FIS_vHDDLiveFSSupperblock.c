#include "../libFireBird.h"

dword FIS_vHDDLiveFSSupperblock (void)
{
  dword                 *vHDDLiveFSSupperblock = NULL;

  if (!vHDDLiveFSSupperblock)
  {
    vHDDLiveFSSupperblock = (dword*)FindInstructionSequence ("27bdffe8 afbf0014 3c0481dd 24843de4 8f8f97ec 000f2a40 0c000c36 00000000 3c1981dd af221afc",
                                                      "ffff0000 ffff0000 ffff0000 ffff0000 ffe00000 ffe0ffff fc000000 ffffffff ffe00000 fc1f0000",
                                                      0x80080000, 0x800b0000, 0, FALSE);

    if (vHDDLiveFSSupperblock) vHDDLiveFSSupperblock = (dword*)((vHDDLiveFSSupperblock[10] << 16) + (short)(vHDDLiveFSSupperblock[11] & 0xffff));
  }

  return (dword)vHDDLiveFSSupperblock;
}
