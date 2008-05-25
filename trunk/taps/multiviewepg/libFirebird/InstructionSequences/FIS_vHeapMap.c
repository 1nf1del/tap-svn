#include "../libFireBird.h"
#include "../main/FBLib_main.h"

dword FIS_vHeapMap (void)
{
  dword                 *p;
  static dword          vHeapMap = 0;

  if (!vHeapMap)
  {
    if (!LibInitialized) InitTAPex();
    if (!LibInitialized) return 0;

    p = (dword*)FindInstructionSequence ("02b80018 00000000 0000c012 00000000 0338c821 3c0f81bf 25ef22c0 32b5ffff 240e0006 00000000 00000000 02ae0018 00000000 00007012 00000000 01ee7821 95ef0000 35ef8000 a72f0000 3c1981ea 27396d70 32b5ffff 24180500 00000000 00000000 02b80018",
                                         "fc00ffff ffffffff ffff07ff ffffffff fc0007ff ffe00000 fc000000 fc00ffff ffe0ffff ffffffff ffffffff fc00ffff ffffffff ffff07ff ffffffff fc0007ff fc00ffff fc00ffff fc00ffff ffe00000 fc000000 fc00ffff ffe0ffff ffffffff ffffffff fc00ffff",
                                         0x80002000, 0x80080000, 5, FALSE);

    //The TF6xxx series seems to use a completely different mechanism.
    if (!p) return 0;

    vHeapMap = (p[0] << 16) + (short)(p[1] & 0xffff);
  }

  return vHeapMap;
}
