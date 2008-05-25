#include "../libFireBird.h"

dword FIS_fwMemMonitor (void)
{
  static dword          fwMemMonitor = 0;

  if (!fwMemMonitor)
    fwMemMonitor = FindInstructionSequence ("8f260000 3c048027 2484aed0 03c02825 0c02b716 00000000 27de0001 1000ffef 00000000 0c02aa2d 00000000 8fbf001c 8fb00020 8fb10024",
                                            "fc00ffff ffff0000 ffff0000 fc0007ff fc000000 ffffffff fc00ffff ffff0000 ffffffff fc000000 ffffffff ffe00000 ffe00000 ffe00000",
                                             0x80000000, 0x80008000, 0, TRUE);

  return fwMemMonitor;
}
