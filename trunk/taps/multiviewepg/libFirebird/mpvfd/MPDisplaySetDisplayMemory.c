#include "FBLib_mpvfd.h"

//-----------------------------------------------------------------------------
// This function copies a 48 byte data array into the VFD display memory
void MPDisplaySetDisplayMemory (byte *pData)
{
  dword                 i;

  // merge the FW data and the selected TAP data
  for(i = 0; i < 48; i++)
    VFDDisplay.Memory.tapData [i] = pData [i];
}
