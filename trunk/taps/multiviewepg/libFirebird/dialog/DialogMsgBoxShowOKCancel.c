#include                "FBLib_dialog.h"

void DialogMsgBoxShowOKCancel (dword DefaultButton)
{
  if (!FBDialogMsgBox) return;

  strcpy (FBDialogMsgBox->ButtonText [0], "OK");

  switch (TAP_GetSystemVar (SYSVAR_OsdLan))
  {
    case LAN_German:
      strcpy (FBDialogMsgBox->ButtonText [1], "Abbrechen");
      break;

    default:
      strcpy (FBDialogMsgBox->ButtonText [1], "Cancel");
  }
  FBDialogMsgBox->ButtonText [2][0] = '\0';
  FBDialogMsgBox->DefaultButton = DefaultButton;
  DialogMsgBoxShow();
}
