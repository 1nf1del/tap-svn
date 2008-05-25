#include "FBLib_mpvfd.h"

//-----------------------------------------------------------------------------
void MPDisplaySetDisplayMode (DISPLAY_MODE mode)
{
  if (((int) mode >= 0) && (mode < DISPLAY_LAST))
  {
    VFDDisplay.Memory.displayMode = mode;

    // select the applicable mask
    switch(mode)
    {
      case DISPLAY_SHORT:
        memcpy (&StringsMask [0], &shortStringMask [0], 48);
        break;

      case DISPLAY_LONG:
        memcpy (&StringsMask [0], &longStringMask [0], 48);
        break;

      case DISPLAY_FULL:
        memcpy (&StringsMask [0], &fullStringsMask [0], 48);
        break;

      default:
        memcpy (&StringsMask [0], &bothStringsMask [0], 48);
        break;
    }
  }
}
