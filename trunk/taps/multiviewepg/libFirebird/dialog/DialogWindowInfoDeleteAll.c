#include                "FBLib_dialog.h"

void DialogWindowInfoDeleteAll (void)
{
  tDialogInfoItem       *item, *next;

  if (!FBDialogWindow) return;

  item = FBDialogWindow->InfoItems;

  while (item)
  {
    next = item->next;
    TAP_MemFree(item);
    item = next;
  }

  FBDialogWindow->InfoItems = NULL;
  FBDialogWindow->InfoOwnForeColor = TRUE;
  FBDialogWindow->OSDInfoForeDirty = TRUE;
}
