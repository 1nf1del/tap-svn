#include "FBLib_av.h"
#include "../libFireBird.h"

int GetPinStatus (void)
{
  int                   *p = (int*)FIS_vPinStatus();

  return (int)(p ? *p : -2);
}
