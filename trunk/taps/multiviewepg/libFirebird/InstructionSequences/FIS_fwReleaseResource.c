#include "../libFireBird.h"

dword FIS_fwReleaseResource (void)
{
  static dword         *fwReleaseResource = 0;

  if (!fwReleaseResource)
  {
    fwReleaseResource = (dword*)FindInstructionSequence ("24040003 0c0003d9 00000000 24040001 0c0003d9 00000000 00002025 0c0003d9 00000000 24040002 0c0003d9 00000000",
                                                         "ffffffff fc000000 ffffffff ffffffff fc000000 ffffffff ffffffff fc000000 ffffffff ffffffff fc000000 ffffffff",
                                                         0x80000000, 0x80010000, 0, FALSE);

    if (fwReleaseResource) fwReleaseResource = (dword*)ABS_ADDR(fwReleaseResource[1]);
  }

  return (dword)fwReleaseResource;
}
