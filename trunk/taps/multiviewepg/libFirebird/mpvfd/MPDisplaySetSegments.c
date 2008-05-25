#include "FBLib_mpvfd.h"

void MPDisplaySetSegments (byte MemoryAddress, byte MemoryData)
{
  VFDDisplay.Memory.tapData [MemoryAddress] |= MemoryData;
}
