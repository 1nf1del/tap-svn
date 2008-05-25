#include                "FBLib_dialog.h"

void DialogWindowScrollDown (void)
{
  int                   selected;

  if (!FBDialogWindow || !FBDialogWindow->NrItems || !FBDialogProfile) return;

  CalcPrepare();
  selected = FBDialogWindow->SelectedItem;

  if (FBDialogWindow->hasCursor && FBDialogProfile->ScrollWrapAround && (FBDialogWindow->SelectedItem == FBDialogWindow->NrItems - 1)) FBDialogWindow->SelectedItem = 0;
  else FBDialogWindow->SelectedItem++;

  if (FBDialogWindow->hasCursor && FBDialogProfile->ScrollScreen && (FBDialogWindow->SelectedItem >= FBDialogWindow->TopItem + FBDialogWindow->NrItemLines)) FBDialogWindow->TopItem = FBDialogWindow->SelectedItem;

  CalcTopIndex(selected, +1);
  DialogWindowRefresh();
}
