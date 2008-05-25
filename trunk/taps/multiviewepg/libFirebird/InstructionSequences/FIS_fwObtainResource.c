#include "../libFireBird.h"

dword FIS_fwObtainResource (void)
{
  static dword         *fwObtainResource = 0;

  if (!fwObtainResource)
  {
    fwObtainResource = (dword*)FindInstructionSequence ("24040003 2405ffff 0c00037c 00000000",
                                                        "ffffffff ffffffff fc000000 ffffffff",
                                                         0x80000000, 0x80010000, 0, FALSE);

    if (fwObtainResource) fwObtainResource = (dword*)ABS_ADDR(fwObtainResource[2]);
  }

  return (dword)fwObtainResource;
}
