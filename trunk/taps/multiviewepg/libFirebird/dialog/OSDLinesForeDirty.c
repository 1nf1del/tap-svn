#include                "FBLib_dialog.h"

bool OSDLinesForeDirty (bool All)
{
  int                   i;

  if (!FBDialogWindow) return FALSE;

  for (i = 0; i < FBDialogWindow->NrItemLines; i++)
    if (!FBDialogWindow->OSDLineForeDirty[i] == All) return !All;

  return All;
}
