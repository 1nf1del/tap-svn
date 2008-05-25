#include "../libFireBird.h"

//--------------------------------------- ReadEEPROM --------------------------------
//
bool ReadEEPROM (dword EEPROMStart, dword NrOfBytes, char *DestinationAddress)
{
  bool                  ret;

  ret = ReadIICRegister (0, 0xa0, EEPROMStart, NrOfBytes, DestinationAddress);

  return ret;
}
