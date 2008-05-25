#include "FBLib_flash.h"

//--------------------------------------- GetEEPROMPin --------------------------------
//
word GetEEPROMPin (void)
{
  dword                 addr = GetEEPROMAddress();

  return (addr ? *(word*) (addr + 0x24) : 0);
}
