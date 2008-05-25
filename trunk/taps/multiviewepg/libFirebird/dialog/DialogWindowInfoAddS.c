#include                <string.h>
#include                "FBLib_dialog.h"

void DialogWindowInfoAddS (dword X, dword Y, dword MaxX, char *Text, word FColor, word BColor, byte FontSize, byte bDot, byte Align)
{
  tDialogInfoItem       *item, *last = NULL;

  if (!FBDialogWindow || !Text) return;

  item = FBDialogWindow->InfoItems;

  while (item)
  {
    last = item;
    item = item->next;
  }

  if ((item = TAP_MemAlloc(sizeof(tDialogInfoItem) + strlen(Text) + 1)))
  {
    item->ItemType = InfoString;
    item->X = X;
    item->Y = Y;
    item->MaxX = MaxX;
    item->FColor = FColor;
    item->BColor = BColor;
    item->FontSize = FontSize;
    item->bDot = bDot;
    item->Align = Align;
    item->data = strcpy((char *) (item + 1), Text);
    item->next = NULL;

    if (last) last->next = item;
    else FBDialogWindow->InfoItems = item;

    if (!FColor) FBDialogWindow->InfoOwnForeColor = FALSE;

    FBDialogWindow->OSDInfoForeDirty = TRUE;
  }
}
