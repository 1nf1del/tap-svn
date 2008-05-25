#include "FBLib_ini.h"
#include "../libFireBird.h"

char *LangGetString (dword StringID)
{
  if (!LangStrings || !LangStringPtr || (StringID >= LangNrStrings)) return "";

  return &LangStrings [LangStringPtr [StringID]];
}
