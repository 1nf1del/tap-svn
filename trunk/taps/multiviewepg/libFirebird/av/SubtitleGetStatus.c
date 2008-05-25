#include                "../libFireBird.h"

// adapted from bdb's exTAP library

int SubtitleGetStatus (void)
{
  dword                 addr;

  if (!(addr = GetEEPROMAddress())) return -1;

  // offset +2 to type_etc_structure due to initial CRC in EEPROM
  return (*(byte *) (addr + 0x0f) & 0x10 ? 0 : 1);
}
