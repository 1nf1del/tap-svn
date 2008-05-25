#include                "FBLib_dialog.h"

void DialogWindowInfoAddIcon (dword X, dword Y, TYPE_GrData *IconGd)
{
  tDialogInfoItem       *item, *last = NULL;

  if (!FBDialogWindow) return;

  item = FBDialogWindow->InfoItems;

  while (item)
  {
    last = item;
    item = item->next;
  }

  if ((item = TAP_MemAlloc(sizeof(tDialogInfoItem))))
  {
    memset(item, 0, sizeof(tDialogInfoItem));

    item->ItemType = InfoIcon;
    item->X = X;
    item->Y = Y;
    item->data = IconGd;
    item->next = NULL;

    if (last) last->next = item;
    else FBDialogWindow->InfoItems = item;

    FBDialogWindow->OSDInfoForeDirty = TRUE;
  }
}
