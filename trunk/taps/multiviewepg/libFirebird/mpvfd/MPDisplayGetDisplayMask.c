#include "FBLib_mpvfd.h"

byte MPDisplayGetDisplayMask (byte MaskAddress)
{
  return StringsMask [MaskAddress];
}
