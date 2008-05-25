#include "FBLib_mpvfd.h"

//-----------------------------------------------------------------------------
void MPDisplaySetAmFlag (bool active)
{
  VFDDisplay.Segments.AnteMeridiem = active;
}
