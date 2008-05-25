#include "FBLib_flash.h"

//--------------------------------------- GetEEPROMAddress --------------------------------
//
dword GetEEPROMAddress (void)
{
  return FIS_vEEPROM();
}
