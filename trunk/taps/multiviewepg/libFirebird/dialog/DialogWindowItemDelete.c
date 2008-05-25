#include                <string.h>
#include                "FBLib_dialog.h"

void DialogWindowItemDelete (int ItemIndex)
{
  int                   i, j;

  if (!FBDialogWindow || !FBDialogWindow->DialogItems || !FBDialogWindow->NrItems || ItemIndex < 0 || ItemIndex >= FBDialogWindow->NrItems) return;

  if (FBDialogWindow->NrItems > 1 && ItemIndex != FBDialogWindow->NrItems - 1) memmove(&FBDialogWindow->DialogItems[ItemIndex], &FBDialogWindow->DialogItems[ItemIndex + 1], (FBDialogWindow->NrItems - 1 - ItemIndex) * sizeof(tDialogItem));

  FBDialogWindow->NrItems--;

  memset(&FBDialogWindow->DialogItems[FBDialogWindow->NrItems], 0, sizeof(tDialogItem));

  if (ItemIndex <= FBDialogWindow->TopItem + FBDialogWindow->NrItemLines - 1)
  {
    i = (ItemIndex < FBDialogWindow->TopItem ? 0 : ItemIndex - FBDialogWindow->TopItem);

    for (j = i; j < FBDialogWindow->NrItemLines; j++) FBDialogWindow->OSDLineForeDirty[j] = TRUE;
  }
}
