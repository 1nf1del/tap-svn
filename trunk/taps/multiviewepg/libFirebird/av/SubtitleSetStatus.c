#include                "../libFireBird.h"

// adapted from bdb's exTAP library

void SubtitleSetStatus (bool enable)
{
  dword                 addr;
  byte                  *petc_subt;

  if (!(addr = GetEEPROMAddress())) return;

  // offset +2 to type_etc_structure due to initial CRC in EEPROM
  petc_subt = (byte *) (addr + 0x0f);

  if ((enable && (*petc_subt & 0x10)) || (!enable && ((*petc_subt & 0x10) == 0)))
  {
    *petc_subt ^= 0x10;
    TAP_WriteSystemVar();
  }
}
