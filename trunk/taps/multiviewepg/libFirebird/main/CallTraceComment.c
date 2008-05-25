#include                "FBLib_main.h"

void CallTraceComment (char *Comment)
{
  char                  Spaces [101];

  Spaces [0] = '\0';

  if (CallTraceEnabled)
  {
    memset (Spaces, ' ', CallLevel < 50 ? CallLevel << 1 : 100);
    Spaces [CallLevel < 50 ? CallLevel << 1 : 100] = '\0';
    if (Comment && *Comment) TAP_Print ("%s# %s\n", Spaces, Comment);
  }
}
