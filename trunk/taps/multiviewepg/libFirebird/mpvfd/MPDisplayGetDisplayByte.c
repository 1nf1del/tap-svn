#include "FBLib_mpvfd.h"

byte MPDisplayGetDisplayByte (byte MemoryAddress)
{
  return VFDDisplay.Memory.tapData [MemoryAddress];
}
