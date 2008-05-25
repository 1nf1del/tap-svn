#include                "../libFireBird.h"

// adapted from bdb's exTAP library

//
//this will enable/disable interactive immediately, and permanently (i.e. updates the flash)
//it acts the same as using the menus
//
void InteractiveSetStatus (bool enable)
{
  dword                 addr;
  byte                  *petc_mheg;
  bool                  disable;
  static dword          fw_mheg_disable = 0;

  if (!(addr = GetEEPROMAddress())) return;

  // offset +2 to type_etc_structure due to initial CRC in EEPROM
  petc_mheg = (byte *) (addr + 0x1e);

  disable = !enable && ((*petc_mheg & 0x20) == 0);

  if (disable & !(fw_mheg_disable = FIS_fwMHEGDisable())) return;

  if ((enable && (*petc_mheg & 0x20)) || disable)
  {
    *petc_mheg ^= 0x20;
    TAP_WriteSystemVar();

    if (disable) CallFirmware(fw_mheg_disable, 0, 0, 0, 0);
  }
}
