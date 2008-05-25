#include "FBLib_ini.h"
#include "../libFireBird.h"

//---------------------------------------  INISetHexDWord --------------------------------
//
void INISetHexDWord (char *Key, dword Value)
{
  char                  s[10];

  sprintf(s, "%08x", Value);
  INISetString (Key, s);
}
