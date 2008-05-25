#include                "FBLib_dialog.h"

void DrawMsgBoxButtons (void)
{
  int                   i;
  dword                 ButtonOffset;

  if (!FBDialogMsgBox || FBDialogMsgBox->InfoBoxTimeout || !FBDialogProfile) return;

  ButtonOffset = (FBDialogMsgBox->ExpandedBox ? MsgBox_C_Title_Gd->width : 0);

  for (i = 0; i < FBDialogMsgBox->NrButtons; i++)
  {
    TAP_Osd_PutFreeColorGd (FBDialogMsgBox->OSDRgn, MsgBox_W_Title_Gd->width + i * MsgBox_C_Title_Gd->width + 10 + ButtonOffset, 105, (i == FBDialogMsgBox->SelectedButton && FBDialogMsgBox->NrButtons != 1 ? MsgBox_In_Gd : MsgBox_Out_Gd), TRUE, (i == FBDialogMsgBox->SelectedButton && FBDialogMsgBox->NrButtons != 1 ? FBDialogProfile->ButtonIn : FBDialogProfile->ButtonOut));
    TAP_Osd_PutS (FBDialogMsgBox->OSDRgn, MsgBox_W_Title_Gd->width + i * MsgBox_C_Title_Gd->width + 10 + ButtonOffset, 115, MsgBox_W_Title_Gd->width + i * MsgBox_C_Title_Gd->width + 10 + MsgBox_In_Gd->width + ButtonOffset, FBDialogMsgBox->ButtonText [i], FBDialogProfile->ItemsTextColor, 0, 0, FNT_Size_1419, TRUE, ALIGN_CENTER);
  }
}
