#include                <string.h>
#include                "FBLib_dialog.h"

void DialogWindowItemChangeParameter (int ItemIndex, char *Parameter, dword ParameterColor)
{
  if (!FBDialogWindow || !FBDialogWindow->DialogItems || ItemIndex < 0 || ItemIndex >= FBDialogWindow->NrItems) return;

  if (Parameter)
  {
    strncpy (FBDialogWindow->DialogItems[ItemIndex].Parameter, Parameter, sizeof(FBDialogWindow->DialogItems->Parameter) - 1);
    FBDialogWindow->DialogItems[ItemIndex].Parameter[sizeof(FBDialogWindow->DialogItems->Parameter) - 1] = '\0';
  }

  FBDialogWindow->DialogItems[ItemIndex].ParameterColor = ParameterColor;

  if (ItemIndex >= FBDialogWindow->TopItem && ItemIndex <= FBDialogWindow->TopItem + FBDialogWindow->NrItemLines - 1) FBDialogWindow->OSDLineForeDirty[ItemIndex - FBDialogWindow->TopItem] = TRUE;
}
