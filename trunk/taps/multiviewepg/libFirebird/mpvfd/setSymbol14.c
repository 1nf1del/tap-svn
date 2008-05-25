#include "FBLib_mpvfd.h"

//-----------------------------------------------------------------------------
// This function sets the symbol represented by the 14-bit bitmap at specified
// position.
void setSymbol14 (dword position, dword symbol)
{
  dword                 i, mask = 0x2000;

  // test all segment bits of the supplied symbol and
  // map them to the appropriate bits in the data array
  for(i = 0; i < 14; i++)
  {
    if(symbol & mask)
    {
      // set the appropriate bit in the data array
      VFDDisplay.Memory.tapData[segmentMap14[position][i][0]] |= segmentMap14[position][i][1];
    }

    // shift the mask to the right
    mask >>= 1;
  }
}
