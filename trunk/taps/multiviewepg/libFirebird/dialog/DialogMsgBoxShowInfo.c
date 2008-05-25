#include                "FBLib_dialog.h"

void DialogMsgBoxShowInfo (dword ms)
{
  if (!FBDialogMsgBox) return;

  FBDialogMsgBox->InfoBoxTimeout = (ms ? TAP_GetTick() + ms / 10 : 0xffffffff);

  FBDialogMsgBox->ButtonText [0][0] = '\0';
  DialogMsgBoxShow ();
}
