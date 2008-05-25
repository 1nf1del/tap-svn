#include "../main/FBLib_main.h"

dword FIS_fwFWFlashProgram (void)
{
  static dword          fwFWFlashProgram = 0;
  dword                 *p, i;

  if (!fwFWFlashProgram)
  {
    fwFWFlashProgram = FIS_fwFlashEraseSector();
    if (!fwFWFlashProgram) return 0;

    //A structure, referenced via $gp, contains 4 pointers to the needed functions
    p = (dword*)(FWgp - 32768);
    for (i = 0; i < 0x4000; i++)
    {
      if (*p == fwFWFlashProgram) break;
      p++;
    }

    if (i >= 0x4000)
    {
      fwFWFlashProgram = 0;
      return 0;
    }

    //Are these valid pointers?
    if (!isValidRAMPtr (p [0]) || !isValidRAMPtr (p [1]) || !isValidRAMPtr (p [2]) || !isValidRAMPtr (p [3]))
    {
      fwFWFlashProgram = 0;
      return 0;
    }

    fwFWFlashProgram = p [1];
  }

  return fwFWFlashProgram;
}
