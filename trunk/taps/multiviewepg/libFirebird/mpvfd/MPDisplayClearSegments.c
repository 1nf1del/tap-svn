#include "FBLib_mpvfd.h"

void MPDisplayClearSegments (byte MemoryAddress, byte MemoryData)
{
  VFDDisplay.Memory.tapData [MemoryAddress] &= ~MemoryData;
}
