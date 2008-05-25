#include "../libFireBird.h"

dword FIS_fwEnqueueEvent (void)
{
  static dword          fwEnqueueEvent = 0;

  if (!fwEnqueueEvent)
  {
    fwEnqueueEvent = FindInstructionSequence ("0c04d8d6 8fa40020 3c0f2000 004f2825 0c056450",
                                              "fc000000 ffffffff ffe0ffff fc00ffff fc000000",
                                               0x80120000, 0x80200000, 4, FALSE);
    if (fwEnqueueEvent) fwEnqueueEvent = ABS_ADDR(*(dword*)fwEnqueueEvent);
  }

  return fwEnqueueEvent;
}
