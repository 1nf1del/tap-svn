#include "FBLib_mpvfd.h"

void MPDisplayToggleSegments (byte MemoryAddress, byte MemoryData)
{
  VFDDisplay.Memory.tapData [MemoryAddress] ^= MemoryData;
}
