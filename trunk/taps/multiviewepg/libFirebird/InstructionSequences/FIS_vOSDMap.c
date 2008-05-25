#include "../main/FBLib_main.h"

dword FIS_vOSDMap (void)
{
  dword                 *p;
  static dword          vOSDMap = 0;

  if (!vOSDMap)
  {
    if (!LibInitialized) InitTAPex();
    if (!LibInitialized) return 0;

    p = (dword*)FindInstructionSequence //("0000c012 3332ffff 33d67fff 3c118033 0218c021 93a40067 2631d574 0016c900",
                                        // "ffff07ff fc00ffff fc00ffff ffe00000 fc0007ff ffe00000 fc000000 ffe007ff",

                                        //(c)R2-D2 :-)
                                        ("33d98000 26108bd0 0000c012 3332ffff 33d67fff 3c118033 0218c021",
                                         "fc00ffff ffe00000 ffff07ff fc00ffff fc00ffff ffe00000 fc0007ff",
                                         0x80090000, 0x80280000, 3, FALSE);

    //The TF6xxx series seems to use a completely different mechanism.
    if (!p) return 0;

    vOSDMap = (p[0] << 16) + (short)(p[3] & 0xffff);
  }

  return vOSDMap;
}
