#include                "FBLib_main.h"

void CallTraceExit (dword *Magic)
{
  char                  Spaces [101];

  Spaces [0] = '\0';

  if (CallLevel > 0) CallLevel--;
                else TAP_Print ("\n\nCallLevel Underflow!\n\n");


  if (CallTraceEnabled || Magic)
  {
    memset (Spaces, ' ', CallLevel < 50 ? CallLevel << 1 : 100);
    Spaces [CallLevel < 50 ? CallLevel << 1 : 100] = '\0';
  }

  if (Magic && *Magic != DEFAULTMAGIC)
  {
    TAP_Print ("\n\n%sINVALID MAGIC!\n\n", Spaces);
    *Magic = DEFAULTMAGIC;
  }

  //if (CallTraceEnabled) TAP_Print ("%s/\n", Spaces);
}
