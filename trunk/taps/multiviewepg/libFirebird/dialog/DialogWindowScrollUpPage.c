#include                "FBLib_dialog.h"

void DialogWindowScrollUpPage (void)
{
  int                   selected;

  if (!FBDialogWindow || !FBDialogWindow->NrItems || !FBDialogProfile) return;

  CalcPrepare();
  selected = FBDialogWindow->SelectedItem;

  //Check if the first item of the current window has been selected
  if (FBDialogWindow->SelectedItem == FBDialogWindow->TopItem)
  {
    //If yes, scroll up one page
    if (FBDialogWindow->hasCursor && FBDialogProfile->ScrollWrapAround && (FBDialogWindow->SelectedItem == 0)) FBDialogWindow->SelectedItem = FBDialogWindow->NrItems - 1;
    else
    {
      FBDialogWindow->SelectedItem -= FBDialogWindow->NrItemLines;
      CheckSelectable(selected, +1);
    }
  }
  else
  {
    //If no, then jump up to the top of the current window
    FBDialogWindow->SelectedItem = FBDialogWindow->TopItem;
    CheckSelectable(selected, +1);
  }

  CalcTopIndex(selected, -1);
  DialogWindowRefresh();
}
