#include "../main/FBLib_main.h"

dword FIS_fwFlashFindSectorAddressIndex (void)
{
  static dword          fwFlashFindSectorAddressIndex = 0;
  dword                 *p, i;

  if (!fwFlashFindSectorAddressIndex)
  {
    fwFlashFindSectorAddressIndex = FIS_fwFlashEraseSector();
    if (!fwFlashFindSectorAddressIndex) return 0;

    //A structure, referenced via $gp, contains 4 pointers to the needed functions
    p = (dword*)(FWgp - 32768);
    for (i = 0; i < 0x4000; i++)
    {
      if (*p == fwFlashFindSectorAddressIndex) break;
      p++;
    }

    if (i >= 0x4000)
    {
      fwFlashFindSectorAddressIndex = 0;
      return 0;
    }

    //Are these valid pointers?
    if (!isValidRAMPtr (p [0]) || !isValidRAMPtr (p [1]) || !isValidRAMPtr (p [2]) || !isValidRAMPtr (p [3]))
    {
      fwFlashFindSectorAddressIndex = 0;
      return 0;
    }

    fwFlashFindSectorAddressIndex = p [2];
  }

  return fwFlashFindSectorAddressIndex;
}
