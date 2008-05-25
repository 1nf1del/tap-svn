#include "FBLib_mpvfd.h"

//-----------------------------------------------------------------------------
void setSymbol17 (dword position, dword symbol)
{
  dword                 i, mask = 0x10000;

  // test all segment bits of the supplied symbol and
  // map them to the appropriate bits in the data array
  for(i = 0; i < 17; i++)
  {
    if(symbol & mask)
    {
      // set the appropriate bit in the data array
      VFDDisplay.Memory.tapData[segmentMap17[position][i][0]] |= segmentMap17[position][i][1];
    }

    // shift the mask to the right
    mask >>= 1;
  }
}
