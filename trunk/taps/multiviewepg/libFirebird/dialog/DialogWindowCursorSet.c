#include                "FBLib_dialog.h"

void DialogWindowCursorSet (int ItemIndex)
{
  int                   selected;
  int                   direction;

  if (!FBDialogWindow || !FBDialogWindow->DialogItems || ItemIndex < 0 || ItemIndex >= FBDialogWindow->NrItems) return;

  selected = FBDialogWindow->SelectedItem;

  if (ItemIndex == 0) direction = +1;
  else if (ItemIndex == FBDialogWindow->NrItems - 1) direction = -1;
  else direction = 0;

  CalcPrepare();
  FBDialogWindow->TopItem = FBDialogWindow->SelectedItem = ItemIndex;
  CalcTopIndex(selected, direction);
  DialogWindowRefresh();
}
