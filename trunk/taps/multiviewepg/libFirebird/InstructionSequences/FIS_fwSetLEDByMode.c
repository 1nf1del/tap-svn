#include "../libFireBird.h"

dword FIS_fwSetLEDByMode (void)
{
  static dword          fw_SetLEDByMode = 0;

  if (fw_SetLEDByMode == 0)
  {
    fw_SetLEDByMode = FindInstructionSequence("240f003f 240e0073 2419006e a3cf0001 a3ce0002 a3d90003 27859d90 0c001aa6",
                                              "ffe0ffff ffe0ffff ffe0ffff ffe0ffff ffe0ffff ffe0ffff ffff0000 fc000000",
                                              0x80150000, 0x80280000, 0, TRUE );
  }

  return fw_SetLEDByMode;
}
