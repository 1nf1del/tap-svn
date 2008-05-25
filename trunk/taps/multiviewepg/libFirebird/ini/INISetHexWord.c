#include "FBLib_ini.h"
#include "../libFireBird.h"

//---------------------------------------  INISetHexWord --------------------------------
//
void INISetHexWord (char *Key, word Value)
{
  char                  s[6];

  sprintf(s, "%04x", Value);
  INISetString (Key, s);
}
