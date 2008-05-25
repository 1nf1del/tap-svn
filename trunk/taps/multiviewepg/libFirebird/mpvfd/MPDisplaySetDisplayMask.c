#include "FBLib_mpvfd.h"

void MPDisplaySetDisplayMask (byte MaskAddress, byte MaskData)
{
  StringsMask [MaskAddress] = MaskData;
}
