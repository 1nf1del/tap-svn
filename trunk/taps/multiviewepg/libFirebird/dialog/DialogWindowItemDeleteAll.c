#include                "FBLib_dialog.h"

void DialogWindowItemDeleteAll (void)
{
  int                   i;

  if (!FBDialogWindow || !FBDialogWindow->DialogItems) return;

  memset (FBDialogWindow->DialogItems, 0, FBDialogWindow->ReservedItems * sizeof(tDialogItem));

  FBDialogWindow->NrItems = 0;
  FBDialogWindow->TopItem = 0;
  FBDialogWindow->SelectedItem = 0;

  for (i = 0; i < FBDialogWindow->NrItemLines; i++) FBDialogWindow->OSDLineForeDirty[i] = TRUE;
}
