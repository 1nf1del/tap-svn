#include                "FBLib_dialog.h"

void DialogMsgBoxExit (void)
{
  if (!FBDialogMsgBox || !FBDialogMsgBox->isVisible) return;

  TAP_Osd_Delete (FBDialogMsgBox->MemOSDRgn);

  if (FBDialogMsgBox->OSDSaveBox)
  {
    TAP_Osd_RestoreBox (FBDialogMsgBox->OSDRgn, 0, 0, GetOSDRegionWidth(FBDialogMsgBox->OSDRgn), GetOSDRegionHeight(FBDialogMsgBox->OSDRgn), FBDialogMsgBox->OSDSaveBox);
    TAP_MemFree (FBDialogMsgBox->OSDSaveBox);
    FBDialogMsgBox->OSDSaveBox = NULL;
  }

  FreeOSDRegion(FBDialogMsgBox->OSDRgn);   // doesn't wipe out

  FBDialogMsgBox->isVisible = FALSE;

  if (FBDialogMsgBox->isNormalMode) TAP_EnterNormal();

  FBDialogMsgBox = NULL;
}
