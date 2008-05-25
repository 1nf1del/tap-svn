#include                "FBLib_dialog.h"

void CalcTopIndex (int OldItem, int Direction)
{
  bool                  scrolled = FALSE, alternate;
  int                   LastItem = 0, i;

  if (!FBDialogWindow || !FBDialogWindow->DialogItems || !FBDialogWindow->NrItems) return;

  if ((alternate = (Direction == 0)))
  {
    LastItem = FBDialogWindow->SelectedItem;
    Direction = -1;
  }

  // skip non-selectable items
  while ((FBDialogWindow->SelectedItem < 0 || FBDialogWindow->SelectedItem >= FBDialogWindow->NrItems) || !FBDialogWindow->DialogItems[FBDialogWindow->SelectedItem].Selectable)
  {
    if (FBDialogWindow->SelectedItem < 0 || FBDialogWindow->SelectedItem >= FBDialogWindow->NrItems || (scrolled && (FBDialogWindow->SelectedItem == OldItem)))
    {
      FBDialogWindow->SelectedItem = OldItem;
      break;
    }

    if (alternate)
    {
      i = FBDialogWindow->SelectedItem;
      FBDialogWindow->SelectedItem = LastItem;
      LastItem = i;
      Direction = -Direction;
    }

    if (Direction == +1)
    {
      // scroll down
      if (FBDialogWindow->hasCursor && FBDialogProfile->ScrollWrapAround && (FBDialogWindow->SelectedItem == FBDialogWindow->NrItems - 1)) FBDialogWindow->SelectedItem = 0;
      else FBDialogWindow->SelectedItem++;
    }
    else if (Direction == -1)
    {
      // scroll up
      if (FBDialogWindow->hasCursor && FBDialogProfile->ScrollWrapAround && (FBDialogWindow->SelectedItem == 0)) FBDialogWindow->SelectedItem = FBDialogWindow->NrItems - 1;
      else FBDialogWindow->SelectedItem--;
    }

    scrolled = TRUE;
  }

  if (!FBDialogWindow->hasCursor)
  {
    FBDialogWindow->TopItem = FBDialogWindow->SelectedItem;

    if (FBDialogWindow->NrItemLines + FBDialogWindow->TopItem > FBDialogWindow->NrItems)
    {
      FBDialogWindow->TopItem = FBDialogWindow->NrItems - FBDialogWindow->NrItemLines;

      if (FBDialogWindow->TopItem < 0) FBDialogWindow->TopItem = 0;

      FBDialogWindow->SelectedItem = FBDialogWindow->TopItem;
    }
  }

  if (FBDialogWindow->SelectedItem >= FBDialogWindow->NrItems) FBDialogWindow->SelectedItem = FBDialogWindow->NrItems - 1;

  if (FBDialogWindow->SelectedItem < 0) FBDialogWindow->SelectedItem = 0;

  if (FBDialogWindow->SelectedItem < FBDialogWindow->TopItem) FBDialogWindow->TopItem = FBDialogWindow->SelectedItem;

  if (FBDialogWindow->SelectedItem >= FBDialogWindow->TopItem + FBDialogWindow->NrItemLines) FBDialogWindow->TopItem = FBDialogWindow->SelectedItem - FBDialogWindow->NrItemLines + 1;

  if (FBDialogWindow->NrItems > FBDialogWindow->NrItemLines)
  {
    if (FBDialogWindow->TopItem < 0) FBDialogWindow->TopItem = 0;
    if (FBDialogWindow->TopItem > FBDialogWindow->NrItems - FBDialogWindow->NrItemLines) FBDialogWindow->TopItem = FBDialogWindow->NrItems - FBDialogWindow->NrItemLines;
  }
  else FBDialogWindow->TopItem = 0;

  if (FBDialogWindow->TopItem != PrevTopItem)
  {
    for (i = 0; i < FBDialogWindow->NrItemLines; i++) FBDialogWindow->OSDLineForeDirty[i] = TRUE;
  }
  else if (FBDialogWindow->SelectedItem != PrevSelItem)
  {
    if (PrevSelItem >= FBDialogWindow->TopItem && PrevSelItem <= FBDialogWindow->TopItem + FBDialogWindow->NrItemLines - 1) FBDialogWindow->OSDLineForeDirty[PrevSelItem - FBDialogWindow->TopItem] = TRUE;
    if (FBDialogWindow->SelectedItem >= FBDialogWindow->TopItem && FBDialogWindow->SelectedItem <= FBDialogWindow->TopItem + FBDialogWindow->NrItemLines - 1) FBDialogWindow->OSDLineForeDirty[FBDialogWindow->SelectedItem - FBDialogWindow->TopItem] = TRUE;
  }
}
