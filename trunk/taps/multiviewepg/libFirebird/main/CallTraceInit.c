#include                "FBLib_main.h"

dword       CallLevel        = 0;
bool        CallTraceEnabled = FALSE;

void CallTraceInit (void)
{
  CallLevel = 0;
  CallTraceEnabled = TRUE;
}
