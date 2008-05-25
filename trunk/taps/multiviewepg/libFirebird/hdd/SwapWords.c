#include "FBLib_hdd.h"

void SwapWords (word *DataBuf)
{
  int                   i;

  for (i = 0; i < 256; i++) DataBuf [i] = ((DataBuf [i] & 0x00FF) << 8) | ((DataBuf [i] & 0xFF00) >> 8);
}
