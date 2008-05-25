#include "../libFireBird.h"

dword FIS_fwMHEGDisable (void)
{
  static dword          fw_mheg_disable = 0;

  if (fw_mheg_disable == 0)
  {
    fw_mheg_disable = FindInstructionSequence("27bdffc8 afbf0034 83998b3f 5320002a a3808b40 2419000e afb90024 27a50024 0c00031a 24040013",
                                              "ffffffff ffffffff ffff0000 ffffff00 ffff0000 ffffffff ffffffff ffffffff ffff0000 ffffffff",
                                              0x80190000, 0x80220000, 0, FALSE );
  }

  return fw_mheg_disable;
}
