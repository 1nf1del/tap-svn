#include                <string.h>
#include                "FBLib_dialog.h"

void DialogWindowItemChangeValue (int ItemIndex, char *Value, dword ValueColor)
{
  if (!FBDialogWindow || !FBDialogWindow->DialogItems || ItemIndex < 0 || ItemIndex >= FBDialogWindow->NrItems) return;

  if (Value)
  {
    strncpy (FBDialogWindow->DialogItems[ItemIndex].Value, Value, sizeof(FBDialogWindow->DialogItems->Value) - 1);
    FBDialogWindow->DialogItems[ItemIndex].Value[sizeof(FBDialogWindow->DialogItems->Value) - 1] = '\0';
  }

  FBDialogWindow->DialogItems[ItemIndex].ValueColor = ValueColor;

  if (ItemIndex >= FBDialogWindow->TopItem && ItemIndex <= FBDialogWindow->TopItem + FBDialogWindow->NrItemLines - 1) FBDialogWindow->OSDLineForeDirty[ItemIndex - FBDialogWindow->TopItem] = TRUE;
}
