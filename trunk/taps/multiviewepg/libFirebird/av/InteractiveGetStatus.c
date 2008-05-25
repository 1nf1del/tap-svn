#include                "../libFireBird.h"

// adapted from bdb's exTAP library

//
//this will return the current interactive setting, 0=off, 1=on, -1=error
//
int InteractiveGetStatus (void)
{
  dword                 addr;

  if (!(addr = GetEEPROMAddress())) return -1;

  // offset +2 to type_etc_structure due to initial CRC in EEPROM
  return (*(byte *) (addr + 0x1e) & 0x20 ? 0 : 1);
}
