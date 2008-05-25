#include "FBLib_mpvfd.h"

//-----------------------------------------------------------------------------
// This function performs a pattern match to identify the FW display function.
dword * findSendToVfdDisplay (dword *pData, dword size)
{
  dword                 rc = 0xffffffff;
  dword                 i = 0, start = 0;

  while(i < size)
  {
    if((pData[i] & ADD_SP_MASK) == ADD_SP_CMD)
    {
      start = i;
      rc = 0;
    }
    else
    {
      // The display function contains a "move $a0,48" instruction and
      // a couple of instructions later a "move $a2,48" instruction.
      // This sequence seems to be unique in all MP firmware versions.
      switch(pData[i])
      {
        case MOVE_A0_48:
          rc = 1;
          break;

        case MOVE_A2_48:
          rc |= 2;
      }

      if(rc == 3) return (pData + start);

      if(i > (start + 50))
      {
        // segment too long, reset
        rc = 0;
      }
    }
    i++;
  }
  return NULL;
}
