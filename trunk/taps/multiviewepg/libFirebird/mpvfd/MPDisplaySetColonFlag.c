#include "FBLib_mpvfd.h"

//-----------------------------------------------------------------------------
void MPDisplaySetColonFlag (bool active)
{
  VFDDisplay.Segments.Colon = active;
}
