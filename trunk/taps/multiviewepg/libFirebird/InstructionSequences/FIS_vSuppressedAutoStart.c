#include "../libFireBird.h"
#include "../main/FBLib_main.h"

dword FIS_vSuppressedAutoStart (void)
{
  static dword          vSuppressedAutoStart = 0;

  if (!vSuppressedAutoStart)
  {
    if (!LibInitialized) InitTAPex();
    if (!LibInitialized) return 0;

    vSuppressedAutoStart = FindInstructionSequence ("83998911 57200043 8fbf0014 0c052c4d 8f848504 3c048026 0c0528ba 2484fc24 00021600 00021603 14400005 3c058026 0c052c4d 8f8484f4 10000035 00000000",
                                                    "ffe00000 fc1f0000 ffff0000 fc000000 ffff0000 ffff0000 fc000000 ffff0000 ffffffff ffffffff ffff0000 ffff0000 fc000000 ffff0000 ffff0000 ffffffff",
                                                     0x80150000, 0x80220000, 0, FALSE);

    if (vSuppressedAutoStart) vSuppressedAutoStart = FWgp + (short) (*(dword*)(vSuppressedAutoStart) & 0xffff);
  }

  return vSuppressedAutoStart;
}
