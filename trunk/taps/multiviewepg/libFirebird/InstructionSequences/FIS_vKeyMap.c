#include "../libFireBird.h"
#include "../main/FBLib_main.h"

dword FIS_vKeyMap (void)
{
  static dword          vKeyMap = 0;

  if (!vKeyMap)
  {
    vKeyMap = FindInstructionSequence ("0d0e0f70", "ffffffff", 0x80200000, 0x80300000, 0, FALSE);

    if (vKeyMap) vKeyMap -= 9 * sizeof(dword);
  }

  return vKeyMap;
}
