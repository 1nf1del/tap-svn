#include                "FBLib_dialog.h"

void DialogWindowScrollDownPage (void)
{
  int                   selected;

  if (!FBDialogWindow || !FBDialogWindow->NrItems || !FBDialogProfile) return;

  CalcPrepare();
  selected = FBDialogWindow->SelectedItem;

  //Check if the last item of the current window has been selected
  if ((FBDialogWindow->SelectedItem == FBDialogWindow->TopItem + FBDialogWindow->NrItemLines - 1) || (FBDialogWindow->SelectedItem == FBDialogWindow->NrItems - 1) || !FBDialogWindow->hasCursor)
  {
    //If yes, advance one page
    if (FBDialogWindow->hasCursor && FBDialogProfile->ScrollWrapAround && (FBDialogWindow->SelectedItem == FBDialogWindow->NrItems - 1)) FBDialogWindow->SelectedItem = 0;
    else
    {
      FBDialogWindow->SelectedItem += FBDialogWindow->NrItemLines;
      CheckSelectable(selected, -1);
    }
  }
  else
  {
    //If no, then jump down to the bottom of the current window
    FBDialogWindow->SelectedItem = FBDialogWindow->TopItem + FBDialogWindow->NrItemLines - 1;
    CheckSelectable(selected, -1);
  }

  CalcTopIndex(selected, +1);
  DialogWindowRefresh();
}
