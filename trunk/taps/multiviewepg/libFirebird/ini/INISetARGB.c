#include "FBLib_ini.h"
#include "../libFireBird.h"

//---------------------------------------  INISetARGB --------------------------------
//
void INISetARGB (char *Key, byte Alpha, byte Red, byte Green, byte Blue)
{
  static char argb[12];

  sprintf(argb, "%1.1d,%2.2d,%2.2d,%2.2d", Alpha & 1, Red & 0x1f, Green & 0x1f, Blue & 0x1f);
  INISetString(Key, argb);
}
