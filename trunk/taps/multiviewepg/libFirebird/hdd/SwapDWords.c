#include "FBLib_hdd.h"

void SwapDWords (dword *DataBuf)
{
  int                   i;

  for (i = 0; i < 128; i++) DataBuf [i] = ((DataBuf [i] & 0x000000FF) << 24) | ((DataBuf [i] & 0xFF000000) >> 24) |
  ((DataBuf [i] & 0x0000FF00) << 8) | ((DataBuf [i] & 0x00FF0000) >> 8);
}
