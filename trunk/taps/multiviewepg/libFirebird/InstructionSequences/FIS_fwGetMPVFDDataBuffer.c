#include "../libFireBird.h"
#include "../main/FBLib_main.h"

dword FIS_fwGetMPVFDDataBuffer (void)
{
  static dword          fwGetMPVFDDataBuffer = 0;

  if (!fwGetMPVFDDataBuffer)
  {
    fwGetMPVFDDataBuffer = FindInstructionSequence ("27bdffe8 afbe0014 0080f025 3c198057 27399a94 afd90000 00001025 8fbe0014 27bd0018 03e00008 00000000",
                                                    "ffff0000 fc000000 ffff07ff fc000000 fc000000 fc000000 ffffffff fc000000 ffff0000 ffffffff ffffffff",
                                                     0x80004800, 0x80005800, 0, FALSE);
  }

  return fwGetMPVFDDataBuffer;
}
