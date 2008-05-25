#include                "FBLib_dialog.h"

bool HasEnoughItemMemory (void)
{
  tDialogItem           *TempDialogItems  = NULL;

  if (FBDialogWindow && (FBDialogWindow->NrItems == FBDialogWindow->ReservedItems))
  {
    //Add additional entries
    TempDialogItems = TAP_MemAlloc ((FBDialogWindow->ReservedItems + ITEMS_TO_ADD) * sizeof (tDialogItem));

    if (!TempDialogItems) return FALSE;

    if (FBDialogWindow->DialogItems) memcpy (TempDialogItems, FBDialogWindow->DialogItems, FBDialogWindow->ReservedItems * sizeof (tDialogItem));
    memset (TempDialogItems + FBDialogWindow->ReservedItems, 0, ITEMS_TO_ADD * sizeof (tDialogItem));
    if (FBDialogWindow->DialogItems) TAP_MemFree (FBDialogWindow->DialogItems);

    FBDialogWindow->ReservedItems += ITEMS_TO_ADD;
    FBDialogWindow->DialogItems = TempDialogItems;
  }

  return TRUE;
}
