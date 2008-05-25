#include "../libFireBird.h"

bool isOSDRegionAlive (word Region)
{
  return (GetOSDRegionWidth(Region) != 0 && GetOSDRegionHeight(Region) != 0);
}
