#include "../libFireBird.h"

dword FIS_vHDDLiveFSRootDir (void)
{
  dword                *pHDDLiveFSRootDir = 0;

  if (!pHDDLiveFSRootDir)
  {
    pHDDLiveFSRootDir = (dword*)FindInstructionSequence ("3c1981dd 3c1881dd 8f181af8 af3808dc 3c0581dd 24a508cc 3c0481dd 8c84084c 24060080 0c03dc51 00000000",
                                                         "fc000000 fc000000 fc000000 fc000000 ffff0000 ffff0000 ffff0000 ffff0000 ffffffff fc000000 ffffffff",
                                                          0x80080000, 0x80100000, 0, FALSE);

    if (pHDDLiveFSRootDir) pHDDLiveFSRootDir = (dword*)((pHDDLiveFSRootDir[4] << 16) + (short) (pHDDLiveFSRootDir[5] & 0xffff) - 0x00000030);
  }

  return (dword)pHDDLiveFSRootDir;
}
