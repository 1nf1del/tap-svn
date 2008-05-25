#include                "FBLib_dialog.h"

void CheckSelectable (int OldItem, int Direction)
{
  if (!FBDialogWindow || !FBDialogWindow->DialogItems) return;

  while ((FBDialogWindow->SelectedItem < 0 || FBDialogWindow->SelectedItem >= FBDialogWindow->NrItems) || !FBDialogWindow->DialogItems[FBDialogWindow->SelectedItem].Selectable)
  {
    if (FBDialogWindow->SelectedItem == OldItem) break;
    else FBDialogWindow->SelectedItem += Direction;
  }
}
