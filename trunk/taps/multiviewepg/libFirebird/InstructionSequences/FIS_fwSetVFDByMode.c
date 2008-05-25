#include "../libFireBird.h"

dword FIS_fwSetVFDByMode (void)
{
  static dword          fw_SetVFDByMode = 0;

  if (fw_SetVFDByMode == 0)
  {
    fw_SetVFDByMode = FindInstructionSequence("3c1e81b6 27de766c 03c02025 00002825 0c04df13 24060010 27c40010 00002825 0c04df13 24060010",
                                              "ffff0000 ffff0000 fc1fffff ffffffff fc000000 ffffffff fc1fffff ffffffff fc000000 ffffffff",
                                              0x80180000, 0x80280000, 0, TRUE );

    //If still NULL, retry with a different pattern to locate the TF5500 display routines
    if(!fw_SetVFDByMode) fw_SetVFDByMode = FindInstructionSequence("a38089c0 24190001 af999e68 839889c0 a3989e64 a38089c0 0c067c56 24040001 0c0681e9 00000000 278489c3",
                                                                   "ffff0000 ffe0ffff ffe00000 ffe00000 ffe00000 ffff0000 fc000000 ffffffff fc000000 ffffffff ffff0000",
                                                                   0x80180000, 0x80280000, 0, TRUE );
  }

  return fw_SetVFDByMode;
}
