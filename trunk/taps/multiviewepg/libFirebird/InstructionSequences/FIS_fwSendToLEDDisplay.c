#include "../libFireBird.h"

dword FIS_fwSendToLEDDisplay (void)
{
  static dword          fwSendToLEDDisplay = 0;

  if (!fwSendToLEDDisplay)
  {
    fwSendToLEDDisplay = FindInstructionSequence ("27bdffd8 afb60020 afbe0024 afbf001c 0080f025 00a0b025 33d90001 13200003 00000000 92d90000 a39992e8 33d90002 13200003 00000000",
                                                  "ffff0000 fffc0000 ffe00000 ffe00000 ffff07ff ffff07ff ffe0ffff fc1f0000 ffffffff fc00ffff ffe00000 ffe0ffff fc1f0000 ffffffff",
                                                   0x80004000, 0x80010000, 0, FALSE);
  }

  return fwSendToLEDDisplay;
}
