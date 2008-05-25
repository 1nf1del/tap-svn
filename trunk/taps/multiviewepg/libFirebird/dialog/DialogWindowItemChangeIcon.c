#include                <string.h>
#include                "FBLib_dialog.h"

void DialogWindowItemChangeIcon (int ItemIndex, eDialogItemIcons IconType, TYPE_GrData *IconGd)
{
  if (!FBDialogWindow || !FBDialogWindow->DialogItems || ItemIndex < 0 || ItemIndex >= FBDialogWindow->NrItems) return;

  FBDialogWindow->DialogItems[ItemIndex].Icon = IconType;
  FBDialogWindow->DialogItems[ItemIndex].UserIcon = (IconType == DialogItemIcon_User ? IconGd : NULL);

  if (ItemIndex >= FBDialogWindow->TopItem && ItemIndex <= FBDialogWindow->TopItem + FBDialogWindow->NrItemLines - 1) FBDialogWindow->OSDLineForeDirty[ItemIndex - FBDialogWindow->TopItem] = TRUE;
}
