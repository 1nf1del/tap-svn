#include "FBLib_mpvfd.h"

//-----------------------------------------------------------------------------
void MPDisplaySetPmFlag (bool active)
{
  VFDDisplay.Segments.PostMeridiem = active;
}
