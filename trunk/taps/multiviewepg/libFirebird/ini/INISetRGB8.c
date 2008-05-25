#include "FBLib_ini.h"
#include "../libFireBird.h"

void INISetRGB8 (char *Key, byte Red, byte Green, byte Blue)
{
  static char rgb[12];

  sprintf(rgb, "%3.3d,%3.3d,%3.3d", (Red & 0x1f) * 255 / 31, (Green & 0x1f) * 255 / 31, (Blue & 0x1f) * 255 / 31);
  INISetString(Key, rgb);
}
