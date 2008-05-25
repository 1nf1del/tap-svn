#include "../libFireBird.h"

dword FIS_fwSetPlaybackMode (void)
{
  static dword          fwSetPlaybackMode = 0;

  if (!fwSetPlaybackMode)
  {
    //Locate the JAL to fwSetPlaybackMode
    fwSetPlaybackMode = FindInstructionSequence ("24040002 24050001 0c043aae 24060001 8fbf0010",
                                                 "ffffffff ffffffff fc000000 ffffffff ffff0000",
                                                 0x800F0000, 0x80180000, 2, FALSE);

    if (fwSetPlaybackMode) fwSetPlaybackMode = ABS_ADDR (*(dword*)fwSetPlaybackMode);
  }

  return fwSetPlaybackMode;
}
