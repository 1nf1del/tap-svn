#include                "FBLib_dialog.h"

void DialogMsgBoxShowYesNo (dword DefaultButton)
{
  if (!FBDialogMsgBox) return;

  switch (TAP_GetSystemVar (SYSVAR_OsdLan))
  {
    case LAN_German:
      strcpy (FBDialogMsgBox->ButtonText [0], "Ja");
      strcpy (FBDialogMsgBox->ButtonText [1], "Nein");
      break;

    default:
      strcpy (FBDialogMsgBox->ButtonText [0], "Yes");
      strcpy (FBDialogMsgBox->ButtonText [1], "No");
  }
  FBDialogMsgBox->ButtonText [2][0] = '\0';
  FBDialogMsgBox->DefaultButton = DefaultButton;
  DialogMsgBoxShow();
}
