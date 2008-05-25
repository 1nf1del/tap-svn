#include                "FBLib_main.h"

void CallTraceEnter (char *ProcName)
{
  char                  Spaces [101];

  if (CallLevel >= 50) TAP_Print ("\n\nCallLevel Overflow!\n\n");
  if (CallTraceEnabled)
  {
    memset (Spaces, ' ', CallLevel < 50 ? CallLevel << 1 : 100);
    Spaces [CallLevel < 50 ? CallLevel << 1 : 100] = '\0';
    TAP_Print ("%s%s\n", Spaces, ProcName);
  }
  CallLevel++;
}
