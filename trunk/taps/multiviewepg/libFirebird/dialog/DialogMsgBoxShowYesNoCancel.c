#include                "FBLib_dialog.h"

void DialogMsgBoxShowYesNoCancel (dword DefaultButton)
{
  if (!FBDialogMsgBox) return;

  switch (TAP_GetSystemVar (SYSVAR_OsdLan))
  {
    case LAN_German:
      strcpy (FBDialogMsgBox->ButtonText [0], "Ja");
      strcpy (FBDialogMsgBox->ButtonText [1], "Nein");
      strcpy (FBDialogMsgBox->ButtonText [2], "Abbrechen");
      break;

    default:
      strcpy (FBDialogMsgBox->ButtonText [0], "Yes");
      strcpy (FBDialogMsgBox->ButtonText [1], "No");
      strcpy (FBDialogMsgBox->ButtonText [2], "Cancel");
  }
  FBDialogMsgBox->ButtonText [3][0] = '\0';
  FBDialogMsgBox->DefaultButton = DefaultButton;
  DialogMsgBoxShow();
}
