#include                "FBLib_dialog.h"

void DialogWindowCursorChange (bool visible)
{
  if (!FBDialogWindow) return;

  if (visible != FBDialogWindow->hasCursor)
  {
    if (FBDialogWindow->SelectedItem >= FBDialogWindow->TopItem && FBDialogWindow->SelectedItem <= FBDialogWindow->TopItem + FBDialogWindow->NrItemLines - 1) FBDialogWindow->OSDLineForeDirty[FBDialogWindow->SelectedItem - FBDialogWindow->TopItem] = TRUE;

    if (!visible) FBDialogWindow->SelectedItem = FBDialogWindow->TopItem;

    FBDialogWindow->hasCursor = visible;
  }
}
