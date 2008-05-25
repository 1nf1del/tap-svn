#include "FBLib_mpvfd.h"

void MPDisplaySetDisplayByte (byte MemoryAddress, byte MemoryData)
{
  VFDDisplay.Memory.tapData [MemoryAddress]=MemoryData;
}
