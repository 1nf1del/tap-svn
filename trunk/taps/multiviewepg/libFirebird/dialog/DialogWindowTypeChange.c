#include                "FBLib_dialog.h"

void DialogWindowTypeChange (eDialogWindowTypes WindowType)
{
  int                   i;

  if (!FBDialogWindow || (int) WindowType < 0 || WindowType >= DialogWindowType_NR_OF_TYPES) return;

  FBDialogWindow->Type = WindowType;

  for (i = 0; i < FBDialogWindow->NrItemLines; i++) FBDialogWindow->OSDLineForeDirty[i] = TRUE;
  FBDialogWindow->OSDLineBackDirty         = TRUE;
  FBDialogWindow->OSDLineSelectedBackDirty = TRUE;
}
